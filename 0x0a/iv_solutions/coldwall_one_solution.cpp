// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Coldwall', see ../tasks.md. Measure as Release - and version A also as Debug.

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <numeric>
#include <functional>
#include <version>
#include <cstddef>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`

#if defined(__cpp_lib_parallel_algorithm)
#include <execution>
#endif

using std::cout, std::vector, std::thread, std::atomic, std::size_t;

// (A) every digit goes straight into `sums[i]`.
void add_up_shared(const vector<int>& digits, const size_t from, const size_t to, vector<long long>& sums,
                   const size_t i) {
    for (size_t k{from}; k < to; ++k) {
        sums[i] += digits[k];
    }
}

// (B) a local sum, written once.
void add_up_local(const vector<int>& digits, const size_t from, const size_t to, vector<long long>& sums,
                  const size_t i) {
    long long sum{0};
    for (size_t k{from}; k < to; ++k) {
        sum += digits[k];
    }
    sums[i] = sum;
}

// Extension: one atomic for all.
void add_up_atomic(const vector<int>& digits, const size_t from, const size_t to, atomic<long long>& total) {
    for (size_t k{from}; k < to; ++k) {
        total += digits[k];
    }
}

using part_function = void (*)(const vector<int>&, size_t, size_t, vector<long long>&, size_t);

// Splits the digits into `k` parts, one thread per part; the last part takes what is left over.
long long add_up(const vector<int>& digits, const size_t k, const part_function f) {
    vector<long long> sums(k);
    vector<thread> threads;
    const size_t part{digits.size() / k};
    for (size_t i{0}; i < k; ++i) {
        const size_t to{i + 1 == k ? digits.size() : (i + 1) * part};
        threads.emplace_back(f, std::cref(digits), i * part, to, std::ref(sums), i);
    }
    for (thread& t : threads) {
        t.join();
    }
    return std::accumulate(sums.begin(), sums.end(), 0LL);
}

int main() {
    vector<int> digits(50'000'000);
    std::mt19937 generator{23};
    std::uniform_int_distribution<int> pick{0, 9};
    for (int& d : digits) {
        d = pick(generator);
    }
    const unsigned cores{thread::hardware_concurrency()};
    cout << "hardware_concurrency: " << cores << '\n';

    for (size_t k{1}; k <= 2 * static_cast<size_t>(cores); ++k) {
        stopwatch watch{};
        const long long local{add_up(digits, k, add_up_local)};
        const double local_ms{watch.elapsed_ms()};
        watch.reset();
        const long long shared{add_up(digits, k, add_up_shared)};
        const double shared_ms{watch.elapsed_ms()};
        cout << "k=" << k << ": (B) local " << local_ms << " ms, (A) shared " << shared_ms << " ms, sums " << local
             << ' ' << shared << '\n';
    }

    // Extension: one atomic for all, with 2 threads.
    atomic<long long> total{0};
    stopwatch watch{};
    thread a{add_up_atomic, std::cref(digits), 0, digits.size() / 2, std::ref(total)};
    thread b{add_up_atomic, std::cref(digits), digits.size() / 2, digits.size(), std::ref(total)};
    a.join();
    b.join();
    cout << "one atomic, 2 threads: " << watch.elapsed_ms() << " ms, sum " << total << '\n';

#if defined(__cpp_lib_parallel_algorithm)
    watch.reset();
    const long long sequential{std::reduce(std::execution::seq, digits.begin(), digits.end(), 0LL)};
    const double seq_ms{watch.elapsed_ms()};
    watch.reset();
    const long long parallel{std::reduce(std::execution::par, digits.begin(), digits.end(), 0LL)};
    const double par_ms{watch.elapsed_ms()};
    cout << "reduce: seq " << seq_ms << " ms, par " << par_ms << " ms, sums " << sequential << ' ' << parallel << '\n';
#endif

    // Our runs, x86-64 Linux with 2 cores, gcc 13:
    // - Release, (B): about 30 ms with one thread, 15 to 25 with two, and no faster with more - two cores. On a machine
    //   with more cores, it goes on for a while, then stops: adding up is so little work per byte that the memory
    //   cannot deliver the digits fast enough - the threads wait for the memory, not for each other.
    // - Release, (A) is as fast as (B): the optimizer keeps `sums[i]` in a register for the whole loop and writes it
    //   once at the end - it made (B) out of (A). A `vector<int>` and a `vector<long long>` cannot overlap (the
    //   aliasing rule, see unit 0x05), so it may.
    // - Debug, (A) with two threads is two to three times slower than (B) - slower even than one thread: every step
    //   writes `sums[i]` to memory, and `sums[0]` and `sums[1]` are in the same cache line - false sharing, as in the
    //   session. (B) writes once per thread.
    // - One atomic for all: every digit a `lock add` on the same line, from both cores - a second, thirty to forty
    //   times slower.
    // - `reduce` with `par`: as fast as `seq` with libstdc++ without TBB (see the follow-up).
    // The old version of the task added up 1 to n in a loop per part. clang turns that loop into Gauss's formula -
    // `n * (n + 1) / 2`, a few instructions, no loop - so it needs no time, whatever n is. gcc keeps the loop. A
    // measurement of work the compiler can do at compile time measures nothing.

    return EXIT_SUCCESS;
}
