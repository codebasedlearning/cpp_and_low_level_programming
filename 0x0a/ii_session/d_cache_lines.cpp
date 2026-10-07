// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Cores do not share memory byte by byte: they share cache lines, blocks of 64 bytes. A core that writes must have
 *   the line to itself; another core that wants it must wait until it has travelled over.
 * - Two threads, two counters, no shared variable - and yet they slow each other down, if the two counters are in
 *   the same line: false sharing.
 * - 64 bytes apart, and each core keeps its line. `alignas(64)`, or `std::hardware_destructive_interference_size`
 *   where the library has it.
 * - What a thread writes often belongs in a local - a register - or in a line of its own.
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <new>                              // for hardware_destructive_interference_size
#include <version>
#include <cstdint>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::atomic, std::thread, std::uintptr_t;


/* ---- Content ---- */

namespace {

    /* --- `together` and `apart` ---
     * Two counters, next to each other - 16 bytes, one cache line. And the same two counters, each at the start of a
     * block of 64 bytes: `alignas(64)` makes the address of a member a multiple of 64, so the second one starts in the
     * next line. 128 bytes, of which 112 are padding - bought on purpose.
     * - !![#false-sharing]
     */
    struct together {
        atomic<long long> first{0};
        atomic<long long> second{0};
    };

    struct apart {
        alignas(64) atomic<long long> first{0};
        alignas(64) atomic<long long> second{0};
    };

    /* --- `line_of` --- The number of the 64-byte line an address is in. */
    uintptr_t line_of(const void* p) {
        return reinterpret_cast<uintptr_t>(p) / 64;
    }

    /* --- `show_the_lines` ---
     * The addresses of the counters, and whether they are in the same line - the same address divided by 64.
     * `together` has both counters in one line, `apart` in two. The addresses in `apart` are multiples of 64: in hex,
     * they end in `00`, `40`, `80` or `c0`.
     */
    void show_the_lines() {
        print_function_header();

        const together t;
        const apart a;
        cout << " 1| together: sizeof=" << sizeof(t) << ", &first=" << &t.first << ", &second=" << &t.second
             << ", same line: " << (line_of(&t.first) == line_of(&t.second)) << '\n';
        cout << " 2| apart:    sizeof=" << sizeof(a) << ", &first=" << &a.first << ", &second=" << &a.second
             << ", same line: " << (line_of(&a.first) == line_of(&a.second)) << '\n';
#if defined(__cpp_lib_hardware_interference_size)
        cout << " 3| hardware_destructive_interference_size=" << std::hardware_destructive_interference_size << '\n';
#else
        cout << " 3| no hardware_destructive_interference_size in this library\n";
#endif
    }

    /* --- `count_both` ---
     * Two threads, each counts its own counter up `n` times - relaxed: only the count matters (see the previous
     * snippet). Returns the milliseconds.
     */
    template <typename Counters>
    double count_both(Counters& counters, const int n) {
        const stopwatch watch{};
        thread a{[&counters, n] {
            for (int i{0}; i < n; ++i) {
                counters.first.fetch_add(1, std::memory_order_relaxed);
            }
        }};
        thread b{[&counters, n] {
            for (int i{0}; i < n; ++i) {
                counters.second.fetch_add(1, std::memory_order_relaxed);
            }
        }};
        a.join();
        b.join();
        return watch.elapsed_ms();
    }

    /* --- `measure_false_sharing` ---
     * Ten million increments per thread, for `together` and for `apart`, and one thread that does both counts for
     * comparison. Run it as Release.
     * - `apart`: about half the time of one thread - two cores, two lines, each core keeps its line in its own cache.
     * - `together`: four times as slow as `apart`, and slower than one thread alone. The threads never touch the same
     *   byte - but every increment needs the line exclusively, so it moves from one core to the other and back, ten
     *   million times each way.
     * Ours: x86-64 Linux 280, 70 and 135 ms; ARM64 Linux on an Apple silicon Mac 75, 20 and 38 ms. On both, 64 bytes
     * apart is enough. Apple reports 128 as its line size (`sysctl hw.cachelinesize`) - some libraries pad to 128 to
     * be safe everywhere.
     * The same happens without atomics - to any variable that is written in memory in every step. The optimizer
     * usually keeps a plain local counter in a register; an atomic, or a variable it cannot keep, goes to memory.
     */
    void measure_false_sharing() {
        print_function_header();

        constexpr int n{10'000'000};
        together t;
        apart a;
        const double together_ms{count_both(t, n)};
        const double apart_ms{count_both(a, n)};

        atomic<long long> alone{0};
        const stopwatch watch{};
        for (int i{0}; i < 2 * n; ++i) {
            alone.fetch_add(1, std::memory_order_relaxed);
        }
        const double alone_ms{watch.elapsed_ms()};

        cout << " 1| together: " << together_ms << " ms, " << t.first << " + " << t.second << '\n';
        cout << " 2| apart:    " << apart_ms << " ms, " << a.first << " + " << a.second << '\n';
        cout << " 3| alone:    " << alone_ms << " ms, " << alone << '\n';

        /* -- .Q&A -- !![Would `struct alignas(64) together { ... }` be enough?](#a-1005) */
    }

}

/* --- Cache lines ---
 * Between a core and the memory are caches - small, fast copies of what the core used recently. They do not copy
 * bytes, they copy lines: on the processors of this course, 64 bytes, aligned to 64. When a core writes to a line,
 * the copies of that line in the other cores' caches become invalid; before another core can write to it, the line
 * must come over - a protocol between the caches (MESI and its relatives) takes care of that. A read-only line can be
 * in every cache at once; a written line belongs to one core at a time.
 * So what counts is not whether two threads share a variable, but whether they write to the same line. Layouts that
 * keep one thread's hot data together, and different threads' hot data apart, are a big part of making parallel code
 * fast. The row-by-row sum of unit 0x05 was the first look at the same machine: memory comes in lines.
 */

/* --- `main` --- */
int main() {
    show_the_lines();
    measure_false_sharing();

    return EXIT_SUCCESS;
}
