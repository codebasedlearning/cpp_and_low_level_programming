// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - ThreadSanitizer (TSan): the compiler adds a check to every memory access, and a runtime library remembers which
 *   thread touched which bytes, and what ordered the accesses - a mutex, an atomic, a `join`. Two accesses without
 *   order, one of them a write: a report.
 * - It finds the races of the session - even those that did not give a wrong result in this run.
 * - `volatile` does not fool it; an atomic or a mutex makes the report go away.
 * - The price: five to fifteen times slower, and much more memory - a tool for testing.
 * - Platforms: gcc and clang on Linux, Apple clang on macOS. Not with MinGW, not with MSVC. Not together with
 *   AddressSanitizer.
 */

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl, std::atomic, std::mutex, std::lock_guard, std::thread;

// gcc defines `__SANITIZE_THREAD__` when TSan is on, clang reports it via `__has_feature`.
#if defined(__SANITIZE_THREAD__)
#define CBL_TSAN 1
#elif defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define CBL_TSAN 1
#endif
#endif
#ifndef CBL_TSAN
#define CBL_TSAN 0
#endif


/* ---- Content ---- */

/* -- .Switched on in the CMakeLists. --
 * The unit's `CMakeLists.txt` builds this snippet with `-fsanitize=thread` if the toolchain can link it
 * (`check_linker_flag`) - and without it otherwise. Then this program only says so, and does nothing racy.
 */

namespace {

    /* --- Which experiment? ---
     * TSan reports every race it sees and goes on - but one experiment per run is easier to read. Set `experiment` to
     * 1, 2, 3 or 4, build, run, and read the report. 3 and 4 are correct: no report.
     */
    constexpr int experiment{1};

    /* --- `plain`, `flag`, `counter` and `guard` --- The variables of the session. */
    int plain{0};
    volatile bool flag{false};
    atomic<int> counter{0};
    mutex guard;
    int guarded{0};

    /* --- `race_on_a_counter` ---
     * Report: `WARNING: ThreadSanitizer: data race`, then an access "by thread T2" - "Read of size 4" or "Write of
     * size 4" - and the "Previous write of size 4 ... by thread T1", both in `count_plain`, and what the bytes are:
     * "Location is global '(anonymous namespace)::plain'". And where the two threads were created. The count itself
     * may even be right in this run - TSan reports the race, not the result.
     */
    void count_plain() {
        for (int i{0}; i < 100'000; ++i) {
            ++plain;
        }
    }

    void race_on_a_counter() {
        thread a{count_plain};
        thread b{count_plain};
        a.join();
        b.join();
        cout << " 1| plain=" << plain << endl;
    }

    /* --- `race_on_a_volatile_flag` ---
     * The waiting thread of the session, with `volatile`: it waits, and on x86-64 it even sees the right value. TSan
     * reports it anyway, twice: the race on `flag` - `volatile` orders nothing between threads - and the race on
     * `data`, "Location is stack of main thread".
     */
    void race_on_a_volatile_flag() {
        int data{0};
        thread waiter{[&data] {
            while (!flag) {}
            cout << " 1| data=" << data << endl;
        }};
        std::this_thread::sleep_for(std::chrono::milliseconds{10});
        data = 42;
        flag = true;
        waiter.join();
    }

    /* --- `count_atomically` and `count_locked` --- Correct - no report. */
    void count_atomic() {
        for (int i{0}; i < 100'000; ++i) {
            ++counter;
        }
    }

    void count_atomically() {
        thread a{count_atomic};
        thread b{count_atomic};
        a.join();
        b.join();
        cout << " 1| counter=" << counter << endl;
    }

    void count_under_the_lock() {
        for (int i{0}; i < 100'000; ++i) {
            const lock_guard lock{guard};
            ++guarded;
        }
    }

    void count_locked() {
        thread a{count_under_the_lock};
        thread b{count_under_the_lock};
        a.join();
        b.join();
        cout << " 1| guarded=" << guarded << endl;
    }

    /* --- `run_the_experiment` --- */
    void run_the_experiment() {
        print_function_header();

        if (!CBL_TSAN) {
            cout << " 1| built without ThreadSanitizer - the toolchain cannot link it. Nothing to see here.\n";
            return;
        }
        switch (experiment) {
            case 1: race_on_a_counter(); break;
            case 2: race_on_a_volatile_flag(); break;
            case 3: count_atomically(); break;
            case 4: count_locked(); break;
            default: break;
        }
    }

}

/* --- What TSan can and cannot do ---
 * It sees only what runs: a race in code that did not run in this test is not reported. It needs no luck with the
 * timing, though - two accesses without order are reported even when they were far apart in time. It knows
 * `std::mutex`, `std::atomic`, `join` and the condition variables, and hand-made synchronization with plain variables
 * not at all - which is the point. On Linux, older compilers may fail at start with "unexpected memory mapping" - a
 * newer compiler, or `setarch -R` to run without address randomization, helps.
 */

/* --- `main` --- */
int main() {
    run_the_experiment();

    return EXIT_SUCCESS;
}
