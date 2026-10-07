// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A thread is a stack of its own and a set of registers, and the operating system decides when it runs - on which
 *   core, and for how long.
 * - Everything else is shared: the global variables, the heap, the code. Every thread sees the same addresses.
 * - `thread_local` is the exception: one variable per thread, found through a register that points to the current
 *   thread's block.
 * - A `std::thread` is a handle, 8 bytes. What the thread runs - the callable and a copy of its arguments - is copied
 *   into a block on the heap.
 * - `std::ref` and `[&]` hand a thread an address - valid only as long as the variable lives.
 * - Starting a thread costs tens of microseconds - thousands of function calls.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `thread t{add_up, 1000, std::ref(result)}` - why the `std::ref`, and what happened without it?
 * - Four threads, each sleeping 200 ms: how long did it take - and why not 800 ms?
 * - What happens to a program that destroys a `std::thread` it has not joined?
 * - In the debugger: where was `main` while the workers counted down - which frames did it have?
 */

#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
#include <cstdint>                          // for uintptr_t
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::array, std::vector, std::thread, std::unique_ptr, std::make_unique, std::uintptr_t;
using std::chrono::milliseconds;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `per_thread` and `count_per_thread` are for Compiler Explorer, below.
 */

/* --- `per_thread` and `count_per_thread` ---
 * A `thread_local` variable: every thread has its own `per_thread`, created when the thread starts, destroyed when it
 * ends.
 * - !![#thread-local]
 */
thread_local int per_thread{0};

void count_per_thread() {
    ++per_thread;
}

namespace {

    /* --- `where` --- What a thread reports: the addresses of a local, of `per_thread`, and its count. */
    struct where {
        const void* local{nullptr};
        const void* thread_local_variable{nullptr};
        int count{0};
    };

    /* --- `report` ---
     * Runs in a thread: counts `per_thread` up `n` times, notes the addresses, and sleeps a little - so that all
     * threads are alive at the same time, and none can reuse the stack of another. The addresses go to `out`, and
     * `main` prints them after `join`: threads that print at the same time mix their lines.
     */
    void report(where& out, const int n) {
        const int local{n};
        for (int i{0}; i < n; ++i) {
            count_per_thread();
        }
        out = where{&local, &per_thread, per_thread};
        std::this_thread::sleep_for(milliseconds{100});
    }

    /* --- `distance_kib` --- How far apart two addresses are, in KiB. */
    long long distance_kib(const void* a, const void* b) {
        return (static_cast<long long>(reinterpret_cast<uintptr_t>(a)) -
                static_cast<long long>(reinterpret_cast<uintptr_t>(b))) / 1024;
    }

    /* --- `show_the_stacks` ---
     * `main` and three threads, and the address of a local variable in each. `main`'s stack is where it always was,
     * near the top of the address space. The threads' stacks are somewhere else - blocks that the thread library got
     * from the operating system when it created the thread - and a little more than 8 MiB apart on Linux: each stack is
     * 8 MiB, and between two of them is a guard that nobody may touch - 4 KiB on our x86-64 machine, 64 KiB on ARM64.
     * On macOS, a thread's stack is 512 KiB - `main` has 8 MiB there, too.
     * The stacks are the same kind of memory as the heap - only the addresses and the size are fixed when the thread
     * starts. What does not fit, overflows (task 'Juniper Gate').
     * - !![#thread]
     */
    void show_the_stacks() {
        print_function_header();

        const int local_in_main{0};
        array<where, 3> reports{};
        vector<thread> threads;
        for (where& r : reports) {
            threads.emplace_back(report, std::ref(r), 10);
        }
        for (thread& t : threads) {
            t.join();
        }
        cout << " 1| main:     &local=" << &local_in_main << '\n';
        for (std::size_t i{0}; i < reports.size(); ++i) {
            cout << ' ' << i + 2 << "| thread " << i + 1 << ": &local=" << reports[i].local;
            if (i > 0) {
                cout << ", " << distance_kib(reports[i - 1].local, reports[i].local) << " KiB below the one before";
            }
            cout << '\n';
        }

        /* -- .Q&A -- !![What would happen if two stacks were right next to each other, without a gap?](#a-1002) */
    }

    /* --- `global_value` --- One for all threads. */
    int global_value{0};

    /* --- `show_what_is_shared` ---
     * Every thread sees `global_value` and the block on the heap at the same address - there is one of each, for the
     * whole program. `per_thread` has another address in every thread, and every thread counted only its own: 10, 20
     * and 30, where one shared variable would have ended at 60 (or less, see the next snippet). `main` has its own,
     * too - still 0.
     * The `thread_local` block of a thread sits next to its stack, a few KiB above the local variables - glibc puts
     * both into the same block.
     */
    void show_what_is_shared() {
        print_function_header();

        const unique_ptr<int> on_the_heap{make_unique<int>(0)};
        array<where, 3> reports{};
        vector<thread> threads;
        for (std::size_t i{0}; i < reports.size(); ++i) {
            threads.emplace_back(report, std::ref(reports[i]), 10 * static_cast<int>(i + 1));
        }
        for (thread& t : threads) {
            t.join();
        }
        cout << " 1| shared: &global_value=" << &global_value << ", heap: " << on_the_heap.get() << '\n';
        cout << " 2| main:     &per_thread=" << &per_thread << ", per_thread=" << per_thread << '\n';
        for (std::size_t i{0}; i < reports.size(); ++i) {
            cout << ' ' << i + 3 << "| thread " << i + 1 << ": &per_thread=" << reports[i].thread_local_variable
                 << ", per_thread=" << reports[i].count << '\n';
        }
    }

}

/* --- The machine code of `thread_local` ---
 * Paste `per_thread` and `count_per_thread` into Compiler Explorer, `-O2`.
 * - x86-64 gcc: `add DWORD PTR fs:per_thread@tpoff, 1`. `fs` is a segment register - a relic of 16-bit times - that
 *   the operating system sets to the address of the current thread's block, at every switch between threads. The
 *   linker fills in the offset of `per_thread` in that block (`tpoff`). So the same instruction reaches another
 *   variable in every thread.
 * - ARM64 gcc: `mrs x0, tpidr_el0` - read the "thread pointer" register - then two `add`s for the offset, and `ldr`,
 *   `add`, `str`.
 * - Compare with `++global_value`: `add DWORD PTR global_value[rip], 1` - an address relative to the code, the same for
 *   all threads.
 * A `thread_local` with a constructor, a `std::string` say, also gets a guard, as a static local (see previous
 * snippets) - it is created on the first use in each thread.
 */

namespace {

    /* --- `show_the_thread_object` ---
     * `sizeof(thread)` is 8: the handle of the operating system's thread - a number on Linux, a pointer on macOS - and
     * nothing else. Where are the function and its arguments? `heap_watch` tells: starting a thread allocates. The
     * callable - here a lambda with 400 bytes of captures - and the arguments are copied into a block on the heap,
     * which the new thread gets, and deletes when it is done. libstdc++ needs one block, libc++ three. The stack of
     * the thread is not in these numbers: the thread library gets it from the operating system directly.
     * Count as Debug.
     */
    void show_the_thread_object() {
        print_function_header();

        const array<int, 100> values{};
        heap_watch heap{};
        thread small{[] {}};
        const std::size_t small_allocations{heap.allocations()};
        const std::size_t small_bytes{heap.bytes()};
        heap.reset();
        thread large{[values] { return values[0]; }};
        const std::size_t large_allocations{heap.allocations()};
        const std::size_t large_bytes{heap.bytes()};
        small.join();
        large.join();

        cout << " 1| sizeof(thread)=" << sizeof(thread) << '\n';
        cout << " 2| []:       allocations=" << small_allocations << ", bytes=" << small_bytes << '\n';
        cout << " 3| [values]: allocations=" << large_allocations << ", bytes=" << large_bytes << '\n';
    }

    /* --- `fill_squares` --- Writes the squares into the vector it gets. */
    void fill_squares(vector<long long>& squares) {
        for (std::size_t i{0}; i < squares.size(); ++i) {
            squares[i] = static_cast<long long>(i * i);
        }
    }

    /* --- `pass_an_address` ---
     * A thread gets copies - `std::ref(squares)` makes the copy an address, and so does `[&squares]` in a lambda:
     * the thread writes into the vector of this function. That is fine as long as `squares` lives - so `join` comes
     * before the `}`, not after.
     * `detach` lets a thread run on, on its own: nobody waits for it. With an address of a local, that is the
     * dangling pointer of unit 0x05, in another thread - commented out.
     */
    void pass_an_address() {
        print_function_header();

        vector<long long> squares(5);
        thread t{fill_squares, std::ref(squares)};
        t.join();
        cout << " 1| squares:";
        for (const long long s : squares) {
            cout << ' ' << s;
        }
        cout << '\n';

        // vector<long long> gone(1000);
        // thread u{[&gone] { std::this_thread::sleep_for(milliseconds{10}); fill_squares(gone); }};
        // u.detach();                      // undefined behavior: `gone` dies at the `}`, the thread writes later
    }

    /* --- `do_nothing` --- The work of a thread that has nothing to do. */
    void do_nothing() {}

    /* --- `measure_a_thread` ---
     * A thousand threads, one after the other, each started and joined - and a thousand calls of the same function.
     * The operating system must create each thread: a stack, a control block, a place in its schedule - and a system
     * call or two. Tens of microseconds per thread. A function call takes a nanosecond or less - if it is not inlined
     * away anyway. So a thread is worth it for work of milliseconds, not of microseconds; programs that need many small
     * tasks keep a few threads alive and hand them work - a thread pool.
     */
    void measure_a_thread() {
        print_function_header();

        constexpr int n{1000};
        stopwatch watch{};
        for (int i{0}; i < n; ++i) {
            thread t{do_nothing};
            t.join();
        }
        const double threads_ms{watch.elapsed_ms()};
        watch.reset();
        for (int i{0}; i < n; ++i) {
            do_nothing();
        }
        const double calls_ms{watch.elapsed_ms()};
        cout << " 1| per thread: " << threads_ms * 1000.0 / n << " us, per call: " << calls_ms * 1000.0 / n << " us\n";
    }

}

/* --- `main` --- */
int main() {
    show_the_stacks();
    show_what_is_shared();
    show_the_thread_object();
    pass_an_address();
    measure_a_thread();

    return EXIT_SUCCESS;
}
