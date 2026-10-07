// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::async` runs a function in another thread and returns a `std::future` - a ticket for the result.
 *   `get()` waits for it.
 * - An exception in the function travels through the future: `get()` throws it in the thread that asks.
 * - `std::promise` is the other end of a future: a thread keeps the promise, another one waits for it.
 * - `launch::async` starts a thread; `launch::deferred` runs the function in the thread that calls `get()`.
 * - Between the two ends is a shared state, on the heap - the result, or the exception, and a flag.
 * - A trap: the destructor of a future from `std::async` waits for the thread.
 */

#include <iostream>
#include <future>                           // for async, future, promise
#include <thread>
#include <chrono>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::future, std::promise, std::thread;
using std::chrono::milliseconds;


/* ---- Content ---- */

namespace {

    /* --- `sum_up_to` --- The sum 1 + 2 + ... + n, slowly - it pretends to be work. */
    long long sum_up_to(const int n) {
        std::this_thread::sleep_for(milliseconds{100});
        long long sum{0};
        for (int i{1}; i <= n; ++i) {
            sum += i;
        }
        return sum;
    }

    /* --- `get_a_result` ---
     * `std::async(std::launch::async, f, args...)` starts `f(args...)` in a new thread, and returns at once - with a
     * `future<long long>`. `main` does something else meanwhile, and `get()` waits for the result: 100 ms in all, not
     * 200. No `std::ref`, no variable for the result, no `join` - the future is all of it.
     * `get()` can be called once: it moves the result out.
     * - !![#async]
     */
    void get_a_result() {
        print_function_header();

        const stopwatch watch{};
        future<long long> result{std::async(std::launch::async, sum_up_to, 1000)};
        const long long mine{sum_up_to(10)};
        cout << " 1| mine=" << mine << ", the other one: " << result.get() << ", took "
             << static_cast<int>(watch.elapsed_ms()) << " ms\n";
    }

    /* --- `pass_an_exception` ---
     * The function throws in its thread; the future stores the exception, and `get()` throws it again - in `main`,
     * where it can be caught. What `exception_ptr` did by hand in the other follow-up.
     */
    void pass_an_exception() {
        print_function_header();

        future<int> result{std::async(std::launch::async, []() -> int {
            throw std::runtime_error{"no result today"};
        })};
        try {
            cout << result.get();
        } catch (const std::exception& e) {
            cout << " 1| caught: " << e.what() << '\n';
        }
    }

    /* --- `keep_a_promise` ---
     * A `promise` and its `future` are two ends of one shared state. The thread gets the promise - moved, it cannot be
     * copied - and sets the value when it has it; `main` waits at the other end. The thread may go on working after
     * `set_value` - the value is there as soon as it is set, not when the thread ends.
     */
    void keep_a_promise() {
        print_function_header();

        promise<int> answer;
        future<int> result{answer.get_future()};
        thread worker{[p = std::move(answer)]() mutable {
            std::this_thread::sleep_for(milliseconds{50});
            p.set_value(42);
            std::this_thread::sleep_for(milliseconds{50});      // still working
        }};
        cout << " 1| the answer: " << result.get() << '\n';
        worker.join();
    }

    /* --- `choose_the_launch` ---
     * `launch::deferred` starts no thread: the function runs when somebody calls `get()` - in that thread. Without a
     * policy, `std::async` may choose either - in practice, the libraries start a thread. Say `launch::async` if you
     * want one.
     */
    void choose_the_launch() {
        print_function_header();

        const auto id_of_the_runner = [] { return std::this_thread::get_id(); };
        future<thread::id> eager{std::async(std::launch::async, id_of_the_runner)};
        future<thread::id> lazy{std::async(std::launch::deferred, id_of_the_runner)};
        const thread::id main_id{std::this_thread::get_id()};
        cout << " 1| async ran in main's thread: " << (eager.get() == main_id) << ", deferred: "
             << (lazy.get() == main_id) << '\n';
    }

    /* --- `count_the_shared_state` ---
     * The shared state between promise and future is a block on the heap: the value (or the exception), a flag "ready",
     * and what it takes to wait - a mutex and a condition variable, or an atomic. A `future` only points to it: a
     * `shared_ptr` with libstdc++ - 16 bytes, see previous snippets -, a plain pointer with libc++.
     * A `promise` allocates the state when it is created - libstdc++ in two blocks, the state and the place for the
     * result, libc++ in one. `std::async` allocates the state, and the block for the thread. Count as Debug.
     */
    void count_the_shared_state() {
        print_function_header();

        heap_watch heap{};
        promise<int> p;
        const std::size_t for_the_promise{heap.allocations()};
        future<int> f{std::async(std::launch::async, [] { return 1; })};
        const int value{f.get()};
        cout << " 1| sizeof(future<int>)=" << sizeof(future<int>) << ", a promise: allocations=" << for_the_promise
             << ", async: allocations=" << heap.allocations() - for_the_promise << ", value " << value << '\n';
    }

    /* --- `wait_in_the_destructor` ---
     * The future of `std::async` is special: its destructor waits until the thread is done. So a future that is not
     * kept - the return value of `std::async`, thrown away - waits at the `;`. Two such calls in a row run one after
     * the other, not at the same time: 200 ms, not 100. Keep the futures. (`std::async` is `[[nodiscard]]` in some
     * libraries - `static_cast<void>` says that throwing the result away is meant.)
     */
    void wait_in_the_destructor() {
        print_function_header();

        const stopwatch watch{};
        static_cast<void>(std::async(std::launch::async, sum_up_to, 10));     // waits at the `;`
        static_cast<void>(std::async(std::launch::async, sum_up_to, 10));
        cout << " 1| thrown away: " << static_cast<int>(watch.elapsed_ms()) << " ms\n";
    }

}

/* --- `main` --- */
int main() {
    get_a_result();
    pass_an_exception();
    keep_a_promise();
    choose_the_launch();
    count_the_shared_state();
    wait_in_the_destructor();

    return EXIT_SUCCESS;
}
