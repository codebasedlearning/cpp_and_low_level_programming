// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Westwheat', see ../tasks.md. Measure as Release.

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`

using std::cout, std::vector, std::thread, std::mutex, std::lock_guard;

vector<int> numbers;
mutex numbers_mutex;

// The unprotected version - a data race on `numbers`. Our runs, gcc 13 on x86-64 Linux, five runs each:
// - n = 3: 6 of 6 elements - or 3, or 4. Both threads read the same size, both write their element to the same place,
//   both store size + 1: an element is lost.
// - n = 1,000 and 100,000: "double free or corruption", "free(): invalid size", "munmap_chunk(): invalid pointer", a
//   segmentation fault - or, now and then, all elements. When the vector grows, both threads may allocate a new block,
//   copy, and free the old one: the old block is freed twice, or one thread writes into a block the other has already
//   freed. glibc finds its bookkeeping broken, and aborts.
// Debug and Release differ only in how often each happens. A result that looks right proves nothing.
// void append_unprotected(const int n) {
//     for (int i{0}; i < n; ++i) {
//         numbers.push_back(i);
//     }
// }

void append(const int n) {
    for (int i{0}; i < n; ++i) {
        const lock_guard lock{numbers_mutex};
        numbers.push_back(i);
    }
}

int main() {
    for (const int n : {3, 1'000, 100'000}) {
        numbers.clear();
        thread a{append, n};
        thread b{append, n};
        a.join();
        b.join();
        cout << "n=" << n << ": " << numbers.size() << " of " << 2 * n << " elements\n";
    }

    // Extension: one thread with 2n against two threads with n each.
    constexpr int n{1'000'000};
    numbers.clear();
    stopwatch watch{};
    thread alone{append, 2 * n};
    alone.join();
    const double alone_ms{watch.elapsed_ms()};

    numbers.clear();
    watch.reset();
    thread a{append, n};
    thread b{append, n};
    a.join();
    b.join();
    const double two_ms{watch.elapsed_ms()};
    cout << "one thread: " << alone_ms << " ms, two threads: " << two_ms << " ms\n";

    // Our runs, Release, x86-64 Linux: one thread about 45 ms, two threads 115 to 175 ms. The work cannot run in
    // parallel - only one thread at a time may hold the lock - so two threads only add the fight for the mutex: the
    // cache line with the mutex and the vector's size moves between the cores, and a thread that finds the mutex taken
    // goes to sleep. Parallel work needs parts that do not share: each thread fills its own vector, and they are joined
    // at the end.
    // ThreadSanitizer (extension) reports the race even for n = 3, in every run: "data race", a "Read of size 8" by
    // one thread and a "Previous write of size 8" by the other - the pointers inside the vector. It does not need the
    // unlucky timing.

    return EXIT_SUCCESS;
}
