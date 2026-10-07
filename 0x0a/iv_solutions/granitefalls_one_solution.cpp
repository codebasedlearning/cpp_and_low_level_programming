// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Granite Falls', see ../tasks.md. Measure as Release.

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`

using std::cout, std::vector, std::thread, std::atomic, std::mutex, std::lock_guard;

constexpr int n{10'000'000};

// (a) a plain `int` - a data race. Our runs, x86-64 Linux: as Debug somewhere between 10 and 11 million of 20
// million; as Release 20 million, because the loop became one `add` of n (see the session) - the race is still there.
// int plain{0};
// void count_plain() {
//     for (int i{0}; i < n; ++i) {
//         ++plain;
//     }
// }

// (b)
atomic<int> counter{0};

void count_atomic() {
    for (int i{0}; i < n; ++i) {
        ++counter;
    }
}

// (c)
mutex counter_mutex;
int guarded{0};

void count_locked() {
    for (int i{0}; i < n; ++i) {
        const lock_guard lock{counter_mutex};
        ++guarded;
    }
}

// (d)
atomic<int> total{0};

void count_local() {
    int mine{0};
    for (int i{0}; i < n; ++i) {
        ++mine;
    }
    total += mine;
}

// (e), extension
atomic<int> relaxed{0};

void count_relaxed() {
    for (int i{0}; i < n; ++i) {
        relaxed.fetch_add(1, std::memory_order_relaxed);
    }
}

double run(void (*const f)(), const int threads) {
    const stopwatch watch{};
    vector<thread> workers;
    for (int i{0}; i < threads; ++i) {
        workers.emplace_back(f);
    }
    for (thread& t : workers) {
        t.join();
    }
    return watch.elapsed_ms();
}

int main() {
    for (const int threads : {2, 4, 8}) {
        counter = 0;
        guarded = 0;
        total = 0;
        relaxed = 0;
        const double atomic_ms{run(count_atomic, threads)};
        const double locked_ms{run(count_locked, threads)};
        const double local_ms{run(count_local, threads)};
        const double relaxed_ms{run(count_relaxed, threads)};
        cout << threads << " threads:\n";
        cout << "  (b) atomic:  " << atomic_ms << " ms, " << counter << '\n';
        cout << "  (c) mutex:   " << locked_ms << " ms, " << guarded << '\n';
        cout << "  (d) local:   " << local_ms << " ms, " << total << '\n';
        cout << "  (e) relaxed: " << relaxed_ms << " ms, " << relaxed << '\n';
    }

    // The order: (d) - nothing shared during the loop, and as Release the loop is one `add` of n - then (b) and (e),
    // then (c), far behind. Our runs, two threads, x86-64 Linux: (d) well below a millisecond, (b) and (e) about
    // 275 ms, (c) about 1.2 s. Every doubling of the threads doubles (b), (c) and (e) - the same increments, and more
    // fighting for one line. (d) does not care.
    // The instructions, `-O2`:
    // - (a) x86-64: `add DWORD PTR plain[rip], ...` once, outside a loop; ARM64: `ldr`, `add`, `str` - no `lock`,
    //   nothing atomic.
    // - (b) x86-64 gcc: `lock add DWORD PTR counter[rip], 1` in the loop (clang: `lock inc`); ARM64 gcc: a call of
    //   `__aarch64_ldadd4_acq_rel` in the loop - with `-march=armv8.1-a`, `ldaddal`.
    // - (c) `call pthread_mutex_lock`, the `add`, `call pthread_mutex_unlock` in the loop - the `lock cmpxchg` is
    //   inside the library.
    // - (d) no loop at all: the local count is `n`, and one `lock add` of it at the end.
    // - (e) x86-64: the same `lock add` as (b) - x86 has no cheaper atomic add, so relaxed is not faster there. ARM64:
    //   `__aarch64_ldadd4_relax`, or `ldadd` without the `al` - no acquire, no release. It may be a little faster
    //   there; the cache line still travels between the cores.

    return EXIT_SUCCESS;
}
