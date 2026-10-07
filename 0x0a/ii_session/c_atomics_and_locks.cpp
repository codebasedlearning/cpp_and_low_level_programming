// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::atomic<int>` is an `int` - 4 bytes, no lock inside - whose operations are single steps for all cores:
 *   `++a` is `lock add` on x86-64, `ldadd` on ARM64. Two threads count to two million.
 * - An `atomic<bool>` is a flag another thread can wait for: every load is a real load, and everything written before
 *   the flag is visible after it.
 * - A `mutex` protects more than one variable, for as long as it is locked. A free mutex costs one atomic instruction;
 *   a taken one puts the waiting thread to sleep - a system call.
 * - Measured with one and with two threads: an atomic gets slower when two cores compete for it, a mutex much slower.
 * - The order of memory accesses: x86-64 keeps almost all of it, ARM64 needs extra instructions - the ones of the
 *   guard of a static local, again.
 */

#include <iostream>
#include <atomic>                           // for atomic
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::atomic, std::mutex, std::lock_guard, std::thread;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The variables and functions up to `increment_locked` are for Compiler Explorer, below.
 */

/* --- `atomic_counter`, `increment_atomic` and `increment_relaxed` ---
 * An atomic `int`, counted up with `++` - and with `fetch_add` and an explicit memory order, see below.
 * - !![#atomic]
 */
atomic<int> atomic_counter{0};

void increment_atomic() {
    ++atomic_counter;
}

void increment_relaxed() {
    atomic_counter.fetch_add(1, std::memory_order_relaxed);
}

/* --- `flag`, `set_flag` and `read_flag` --- An atomic `bool`, written and read. */
atomic<bool> flag{false};

void set_flag() {
    flag = true;
}

bool read_flag() {
    return flag;
}

/* --- `guard`, `guarded` and `increment_locked` --- A plain `int`, protected by a mutex. */
mutex guard;
int guarded{0};

void increment_locked() {
    const lock_guard lock{guard};
    ++guarded;
}

namespace {

    /* --- `count_up_atomically` and `count_up_locked` --- A million increments, atomic or under the lock. */
    void count_up_atomically(const int n) {
        for (int i{0}; i < n; ++i) {
            increment_atomic();
        }
    }

    void count_up_locked(const int n) {
        for (int i{0}; i < n; ++i) {
            increment_locked();
        }
    }

    /* --- `count_atomically` ---
     * `sizeof(atomic<int>)` is 4 - the `int`, nothing else. `is_always_lock_free` says that the processor can do its
     * operations directly, without a lock; for an `int`, every current processor can. For a struct of 32 bytes it
     * cannot: then the library protects it with a lock of its own - still correct, and slower.
     * Two threads, a million increments each: two million, every time, as Debug and as Release.
     */
    void count_atomically() {
        print_function_header();

        struct four_doubles {
            double values[4];
        };
        cout << " 1| sizeof(atomic<int>)=" << sizeof(atomic<int>) << ", lock-free: "
             << atomic<int>::is_always_lock_free << ", for 32 bytes: " << atomic<four_doubles>::is_always_lock_free
             << '\n';

        atomic_counter = 0;
        thread a{count_up_atomically, 1'000'000};
        thread b{count_up_atomically, 1'000'000};
        a.join();
        b.join();
        cout << " 2| atomic_counter=" << atomic_counter << '\n';
    }

    /* --- `payload` and `ready` --- The value and the flag of the previous snippet - now the flag is atomic. */
    int payload{0};
    atomic<bool> ready{false};

    /* --- `wait_for_an_atomic_flag` ---
     * The waiting thread of the previous snippet, with an atomic flag: `ready.load()` is a real load in every round -
     * the compiler may not keep the value in a register - and the store of `true` in `main` comes after the write of
     * `payload`, for every thread that sees it: whoever sees `ready` as `true`, sees `payload` as 42. `payload` itself
     * stays a plain `int`: the flag orders the accesses to it.
     * The waiting thread spins - it checks the flag as fast as it can, and keeps a core busy for nothing. How to wait
     * without that: the follow-up, condition variables.
     */
    void wait_for_an_atomic_flag() {
        print_function_header();

        int seen{-1};
        thread waiter{[&seen] {
            while (!ready.load()) {}
            seen = payload;
        }};
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        payload = 42;
        ready.store(true);
        waiter.join();
        cout << " 1| the waiting thread saw payload=" << seen << '\n';
    }

    /* --- `lock_a_mutex` ---
     * A `mutex` is 40 bytes with glibc on x86-64, 48 on ARM64 - a counter of who holds it, and who waits. It
     * protects whatever the code between lock and unlock touches: here one `int`, in practice often a whole structure -
     * a `vector` and its size, a map and a count. `lock_guard` locks it, and unlocks at the `}`, also when an exception
     * leaves the block (see previous snippets: RAII).
     * - !![#mutex]
     */
    void lock_a_mutex() {
        print_function_header();

        guarded = 0;
        thread a{count_up_locked, 1'000'000};
        thread b{count_up_locked, 1'000'000};
        a.join();
        b.join();
        cout << " 1| sizeof(mutex)=" << sizeof(mutex) << ", guarded=" << guarded << '\n';
    }

}

/* --- The machine code of atomics and locks ---
 * Paste `atomic_counter` to `increment_locked` into Compiler Explorer, with `#include <atomic>` and `#include <mutex>`,
 * `-O2`.
 * - `increment_atomic`, x86-64 gcc: `lock add DWORD PTR atomic_counter[rip], 1` (clang: `lock inc`). The `add` of the
 *   plain counter - with a `lock` in front: the core keeps the cache line with the counter for itself until the add is
 *   done, and no other core can come in between. The same instruction as for the count of a `shared_ptr` (see
 *   previous snippets).
 * - ARM64 gcc: `bl __aarch64_ldadd4_acq_rel` - a call. Not every ARM64 processor has an atomic add (`ldaddal`, since
 *   ARMv8.1); the helper function checks once, at the start of the program, and then uses it, or a loop of
 *   `ldaxr`/`stlxr` otherwise. With `-march=armv8.1-a`, gcc writes `ldaddal w1, w1, [x0]` directly.
 * - `set_flag`: x86-64 `xchg` - a store with a full barrier: nothing that comes after it happens before it is
 *   visible to all cores. `read_flag`: `movzx`, a plain load. ARM64: `stlrb` and `ldarb`, a store-release and a
 *   load-acquire - the `ldar` of the guard of a static local (see previous snippets).
 * - `increment_locked`: `call pthread_mutex_lock`, the `add`, `call pthread_mutex_unlock`. Inside the lock, when the
 *   mutex is free: one `lock cmpxchg` - compare and exchange: "if it is 0, make it 1". When it is taken: a system call
 *   (`futex` on Linux), and the thread sleeps until the owner's unlock wakes it up.
 */

/* --- Memory order ---
 * Every atomic operation takes a memory order - by default `std::memory_order_seq_cst`, the strictest: all threads see
 * all atomic operations in one order. That is what you want, unless you have measured and understood.
 * - `memory_order_relaxed` makes only the operation itself atomic, and orders nothing else: fine for a counter that is
 *   read at the end. `increment_relaxed` is the same `lock add` on x86-64 - x86 has no cheaper one - and `ldadd`
 *   instead of `ldaddal` on ARM64.
 * - `memory_order_release` for the store of a flag, and `memory_order_acquire` for the load: what was written before
 *   the release is visible after the acquire. On x86-64, a release store is a plain `mov` - and a seq_cst one the
 *   `xchg`.
 * More in the `tinker_` file of the follow-up. The rule for now: default order, or a mutex.
 */

namespace {

    /* --- `run` --- Starts `f(n)` in one thread, or `f(n / 2)` in two, and returns the milliseconds. */
    double run(void (*const f)(int), const int threads, const int n) {
        const stopwatch watch{};
        if (threads == 1) {
            thread a{f, n};
            a.join();
        } else {
            thread a{f, n / 2};
            thread b{f, n / 2};
            a.join();
            b.join();
        }
        return watch.elapsed_ms();
    }

    /* --- `measure_the_counters` ---
     * Two million increments, atomic and under the lock - in one thread, and split between two. Run it as Release.
     * - One thread: both are cheap - a few nanoseconds per increment, a few more for the mutex: it costs two atomic
     *   instructions, lock and unlock, and a call each - and nobody waits.
     * - Two threads: the atomic is several times slower than alone - both cores want the same cache line, and it
     *   travels between them at every increment. Two threads, and slower than one! The mutex is worse: when a thread
     *   finds it taken, it spins a little, then sleeps - and must be woken by a system call.
     * Ours, x86-64 Linux: one thread 14 ms (atomic) and 55 ms (mutex), two threads 35 ms and 120 to 240 ms; ARM64
     * Linux on an Apple silicon Mac: 4 and 9 ms, 8 and 55 ms. The lesson: a shared counter is a bottleneck, however it
     * is protected. Better: every thread counts on its own, and the results are added at the end - see the next
     * snippet, and task 'Granite Falls'.
     */
    void measure_the_counters() {
        print_function_header();

        constexpr int n{2'000'000};
        for (const int threads : {1, 2}) {
            atomic_counter = 0;
            guarded = 0;
            const double atomic_ms{run(count_up_atomically, threads, n)};
            const double mutex_ms{run(count_up_locked, threads, n)};
            cout << ' ' << threads << "| " << threads << " thread(s): atomic " << atomic_ms << " ms, mutex " << mutex_ms
                 << " ms, counts " << atomic_counter << " and " << guarded << '\n';
        }

        /* -- .Q&A -- !![`if (atomic_counter == 0) { atomic_counter = 1; }` - is that atomic?](#a-1004) */
    }

}

/* --- `main` --- */
int main() {
    count_atomically();
    wait_for_an_atomic_flag();
    lock_a_mutex();
    measure_the_counters();

    return EXIT_SUCCESS;
}
