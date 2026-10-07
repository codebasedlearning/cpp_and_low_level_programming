// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: the debugger, with several threads. It lists every thread of the program - and each of
 *   them has its own call stack, its own frames, its own local variables.
 * - When a breakpoint is hit, the debugger stops all threads and shows the one that hit it. You can switch to any
 *   other.
 * - `main` is a thread, too - here, it waits in `join`.
 * - A thread's call stack does not start in `main`: it starts with the function the thread was given.
 * - Every platform, every debugger: gdb, lldb, the one of Visual Studio.
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::to_string, std::vector, std::thread;


/* ---- Content ---- */

namespace {

    /* --- `count_down` ---
     * The work of a worker: count down from `start`, slowly. `id`, `start` and `left` are its local variables - one set
     * per thread. See below for the breakpoint.
     */
    void count_down(const int id, const int start) {
        int left{start};
        while (left > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{50});
            --left;                         // the breakpoint
        }
        const string line{" a|   worker " + to_string(id) + " done\n"};
        cout << line;
    }

    /* --- `run_three_workers` --- Three workers, the first counts from 10, the second from 20, the third from 30. */
    void run_three_workers() {
        print_function_header();

        vector<thread> workers;
        for (int id{1}; id <= 3; ++id) {
            workers.emplace_back(count_down, id, 10 * id);
        }
        for (thread& t : workers) {
            t.join();
        }
        cout << " 1| all workers done\n";
    }

}

/* --- In the debugger ---
 * Run it once without the debugger: it takes one and a half seconds - the last worker counts 30 times 50 ms. Then set a
 * breakpoint on `--left;` and start it in the debugger.
 * - The threads. When the program stops, the debugger lists all of its threads - in CLion in the Debug tool window,
 *   next to or above the frames, depending on the layout. There are four: `main` and three workers - maybe one or two
 *   more that the system started. Their names depend on the debugger: an id, a number, the address of the thread.
 * - `main`. Select it: its frames are `main`, `run_three_workers`, `std::thread::join`, and on top functions of the
 *   system that wait - on Linux `__pthread_clockjoin_ex` and `__futex_abstimed_wait_common64`, on macOS something like
 *   `_pthread_join` and `__ulock_wait`. `main` does not run while it waits: the operating system wakes it when the
 *   worker has ended.
 * - A worker. Its frames are `count_down`, a few functions of the library that called it - `__invoke` and `_M_run`
 *   with libstdc++, `__thread_proxy` with libc++ - and at the bottom the start of the thread in the system:
 *   `start_thread` and `clone3` on Linux, `_pthread_start` on macOS. No `main`: the stack of a thread starts with the
 *   function it was given.
 * - The locals. Each worker has its own `id`, `start` and `left`. Look at the address of `left` in two workers - add
 *   `&left` to the watches, or look in the memory view: the addresses are megabytes apart. Each thread has a stack of
 *   its own - the session measures it.
 * - Resume. The next stop may be in another worker - whichever reaches `--left` first. By default, a breakpoint stops
 *   all threads; some debuggers can stop only the one that hit it.
 * - Step. While you step through one thread, the others may run on - and hit the breakpoint in between. Then the
 *   debugger shows that thread instead. It is not lost, it just was not the only one.
 */

/* --- `main` --- */
int main() {
    run_three_workers();

    return EXIT_SUCCESS;
}
