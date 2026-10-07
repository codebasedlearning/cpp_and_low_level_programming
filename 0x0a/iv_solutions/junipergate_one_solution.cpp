// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Juniper Gate', see ../tasks.md.
// Run it without arguments for the addresses. The extension crashes on purpose: run it with the argument `main`, then
// with `thread` (in CLion: Run | Edit Configurations | Program arguments).

#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <string_view>
#include <cstdint>
#include <cstdlib>

using std::cout, std::endl, std::array, std::vector, std::thread, std::unique_ptr, std::make_unique;
using std::string_view, std::uintptr_t;

int global_value{0};
thread_local int per_thread{0};

struct addresses {
    const void* local{nullptr};
    const void* tls{nullptr};
    const void* global{nullptr};
    const void* heap{nullptr};
};

// The heap block is freed at the end of the function - its address stays a number we can print.
void report(addresses& out) {
    const int local{0};
    const unique_ptr<int> block{make_unique<int>(0)};
    out = addresses{&local, &per_thread, &global_value, block.get()};
    std::this_thread::sleep_for(std::chrono::milliseconds{50});    // all threads alive at the same time
}

long long distance(const void* a, const void* b) {
    return static_cast<long long>(reinterpret_cast<uintptr_t>(a)) -
           static_cast<long long>(reinterpret_cast<uintptr_t>(b));
}

// Extension: 1 KiB per level. The array is used, so that the compiler must keep it - and the result of the recursive
// call is used after the call, so that it cannot turn the recursion into a loop. A million levels would be a GiB of
// stack - the end that is never reached, but that keeps the compiler from warning about an endless recursion.
int dive(const int depth) {
    if (depth == 1'000'000) {
        return 0;
    }
    array<char, 1024> buffer{};
    buffer[static_cast<std::size_t>(depth) % buffer.size()] = static_cast<char>(depth);
    if (depth % 100 == 0) {
        cout << "depth " << depth << endl;  // `endl`: the program crashes, what is in the buffer would be lost
    }
    return dive(depth + 1) + buffer[static_cast<std::size_t>(depth + 1) % buffer.size()];
}

int main(const int argc, const char* argv[]) {
    if (argc > 1 && string_view{argv[1]} == "main") {
        return dive(0);
    }
    if (argc > 1 && string_view{argv[1]} == "thread") {
        thread t{[] { dive(0); }};
        t.join();
        return EXIT_SUCCESS;
    }

    addresses in_main;
    report(in_main);
    array<addresses, 4> in_threads{};
    vector<thread> threads;
    for (addresses& a : in_threads) {
        threads.emplace_back(report, std::ref(a));
    }
    for (thread& t : threads) {
        t.join();
    }

    cout << "main:     local " << in_main.local << ", tls " << in_main.tls << ", global " << in_main.global << ", heap "
         << in_main.heap << '\n';
    for (std::size_t i{0}; i < in_threads.size(); ++i) {
        const addresses& a{in_threads[i]};
        cout << "thread " << i + 1 << ": local " << a.local << ", tls " << a.tls << ", global " << a.global << ", heap "
             << a.heap << ", tls - local: " << distance(a.tls, a.local) << " bytes\n";
    }
    for (std::size_t i{1}; i < in_threads.size(); ++i) {
        cout << "stack " << i << " - stack " << i + 1 << ": "
             << distance(in_threads[i - 1].local, in_threads[i].local) / 1024 << " KiB\n";
    }

    // Our runs, x86-64 Linux (glibc):
    // - The stacks of the threads are 8196 KiB apart - 8 MiB and a guard page of 4 KiB - in the order the threads were
    //   created, going down. `main`'s stack is far away, at the top of the address space.
    // - The `thread_local`s are a little above the locals of their thread, 2 KiB or so: glibc puts the thread's
    //   `thread_local` block and its control block at the top of the thread's stack block.
    // - The global is the same for all.
    // - The heap blocks of the threads are not near `main`'s: glibc gives every thread (up to a limit) an arena of its
    //   own - a separate heap, from `mmap`, near the stacks - so that the threads do not fight for one lock in
    //   `malloc`. `main`'s block comes from the classic heap after the program's data. On macOS, it looks different
    //   again.
    // Extension, x86-64 Linux: both `main` and a thread get to about 7,800 levels - both have 8 MiB (`ulimit -s`),
    // then exit status 139, a segmentation fault: the guard page. On macOS, a thread has 512 KiB: about 500 levels.

    return EXIT_SUCCESS;
}
