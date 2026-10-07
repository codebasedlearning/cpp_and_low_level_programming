// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A thread runs a function at the same time as the rest of the program. `std::thread t{f};` starts it, `t.join()`
 *   waits until it is done.
 * - A thread runs a function or a lambda, with arguments - which are copied. `std::ref` passes a reference instead.
 * - Several threads in a `vector`, started in a loop, joined in a loop. `hardware_concurrency()` says how many can
 *   really run at the same time.
 * - Threads that print at the same time mix their output - so each prints a finished line, or waits for its turn.
 * - Two threads that change the same data must take turns: a `mutex`, locked by a `lock_guard`.
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>                           // for thread, this_thread
#include <mutex>                            // for mutex, lock_guard
#include <chrono>                           // for milliseconds
#include <functional>                       // for ref
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::string, std::to_string, std::vector, std::thread, std::mutex, std::lock_guard;
using std::chrono::milliseconds;


/* ---- Content ---- */

namespace {

    /* --- `say_hello` --- A function for a thread: it prints one line. */
    void say_hello() {
        cout << " a|   hello from a thread\n";
    }

    /* --- `start_a_thread` ---
     * `thread t{say_hello}` starts a new thread, which runs `say_hello` - while `main` goes on with the next line.
     * `join` waits until the thread has finished. Every thread must be joined before its `thread` object is destroyed;
     * otherwise the program ends right there, with `std::terminate` (see the follow-up for the alternatives).
     * - !![#thread]
     */
    void start_a_thread() {
        print_function_header();

        cout << " 1| main starts a thread\n";
        thread t{say_hello};
        t.join();
        cout << " 2| main: the thread is done\n";
        // thread forgotten{say_hello};     // no join: at the `}`, the program is terminated - exit status 134
    }

    /* --- `add_up` --- Adds the numbers from 1 to `n`, and writes the result to `result`. */
    void add_up(const int n, long long& result) {
        long long sum{0};
        for (int i{1}; i <= n; ++i) {
            sum += i;
        }
        result = sum;
    }

    /* --- `pass_arguments` ---
     * The arguments after the function are copied into the new thread - all of them, even where the function takes a
     * reference. `add_up` would get a reference to the copy, and `result` would stay 0 - the compiler sees that coming
     * and refuses: "std::thread arguments must be invocable after conversion to rvalues". `std::ref(result)` passes
     * the variable itself. A lambda does the same with `[&result]`.
     * `result` is read after `join` - not before: until then, the thread may still be writing.
     */
    void pass_arguments() {
        print_function_header();

        long long result{0};
        thread t{add_up, 1000, std::ref(result)};
        // thread u{add_up, 1000, result};  // compiler error: a copy cannot bind to `long long&`
        t.join();
        cout << " 1| add_up: " << result << '\n';

        long long other{0};
        thread u{[&other] { add_up(100, other); }};
        u.join();
        cout << " 2| with a lambda: " << other << '\n';

        /* -- .Q&A -- !![Why must `result` not be read before `join`?](#a-1001) */
    }

    /* --- `work` --- Pretends to work for 200 ms, then writes a finished line. */
    void work(const int id) {
        std::this_thread::sleep_for(milliseconds{200});
        const string line{" b|   worker " + to_string(id) + " done\n"};
        cout << line;
    }

    /* --- `start_several_threads` ---
     * Four threads, started in a loop - `emplace_back` constructs each `thread` in the `vector` - and joined in a loop.
     * All four sleep 200 ms, at the same time: the whole thing takes 200 ms, not 800. In which order they finish is
     * up to the operating system - run it a few times.
     * Each thread prints a finished line with one `<<`. With several `<<` per line, the pieces of different threads
     * could mix.
     * `hardware_concurrency()` is the number of threads the processor can run at the same time - cores, or hardware
     * threads. You can start more; then they take turns.
     */
    void start_several_threads() {
        print_function_header();

        cout << " 1| hardware_concurrency: " << thread::hardware_concurrency() << '\n';
        const stopwatch watch{};
        vector<thread> workers;
        for (int id{1}; id <= 4; ++id) {
            workers.emplace_back(work, id);
        }
        for (thread& t : workers) {
            t.join();
        }
        cout << " 2| four times 200 ms took " << static_cast<int>(watch.elapsed_ms()) << " ms\n";
    }

    /* --- `fill` ---
     * Appends `n` numbers to a shared `vector` - one at a time, and each time it locks `m` first. `lock_guard` locks
     * the mutex in its constructor and unlocks it in its destructor, at the `}` - RAII (see previous snippets). While
     * one thread holds the lock, the other waits in its `lock_guard`.
     * - !![#mutex]
     */
    void fill(vector<int>& numbers, mutex& m, const int value, const int n) {
        for (int i{0}; i < n; ++i) {
            const lock_guard lock{m};
            numbers.push_back(value);
        }
    }

    /* --- `take_turns` ---
     * Two threads append to the same `vector`. `push_back` reads the size, maybe moves everything to a bigger block,
     * writes, and increases the size - if two threads did that at the same time, the result would be anything. With the
     * mutex, they take turns, and all 20,000 numbers arrive. What happens without it: see the session.
     */
    void take_turns() {
        print_function_header();

        vector<int> numbers;
        mutex m;
        thread a{fill, std::ref(numbers), std::ref(m), 1, 10'000};
        thread b{fill, std::ref(numbers), std::ref(m), 2, 10'000};
        a.join();
        b.join();
        cout << " 1| size=" << numbers.size() << '\n';
    }

}

/* --- `main` --- */
int main() {
    start_a_thread();
    pass_arguments();
    start_several_threads();
    take_turns();

    return EXIT_SUCCESS;
}
