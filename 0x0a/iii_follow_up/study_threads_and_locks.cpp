// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A thread must be joined or detached before its `std::thread` is destroyed. A detached thread runs on, on its own -
 *   and nobody knows when it ends.
 * - An exception that leaves a thread's function ends the program. Catch it in the thread and hand it over, with a
 *   `std::exception_ptr`.
 * - `std::jthread` (C++20) joins in its destructor, and can be asked to stop - where the library has it.
 * - A class that protects its own data: a `mutable` mutex, locked in every member function - even the `const` ones.
 * - `unique_lock` can unlock early and lock again; `scoped_lock` locks several mutexes at once, without deadlock.
 * - A deadlock: two threads, two mutexes, locked in opposite order.
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <exception>                        // for exception_ptr, current_exception, rethrow_exception
#include <stdexcept>
#include <version>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector, std::thread, std::mutex, std::lock_guard, std::unique_lock;
using std::scoped_lock;
using std::chrono::milliseconds;


/* ---- Content ---- */

namespace {

    /* --- `join_or_detach` ---
     * `joinable()` says whether a `std::thread` still stands for a thread that must be joined or detached. After
     * `join` or `detach`, it does not. A detached thread runs on, and the `std::thread` object is free to go - but
     * nobody waits for the thread: when `main` returns, the program ends, and the thread with it, wherever it was. And
     * it must not use anything of the function that started it - see the session. Detach rarely, if ever.
     * - !![#thread]
     */
    void join_or_detach() {
        print_function_header();

        thread t{[] { std::this_thread::sleep_for(milliseconds{10}); }};
        cout << " 1| joinable before join: " << t.joinable();
        t.join();
        cout << ", after: " << t.joinable() << '\n';

        thread d{[] { std::this_thread::sleep_for(milliseconds{10}); }};
        d.detach();
        cout << " 2| after detach: " << d.joinable() << " - the thread runs on, unwatched\n";
    }

    /* --- `catch_in_the_thread` ---
     * An exception that leaves the function of a thread cannot reach `main` - `main` is on another stack. The program
     * calls `std::terminate`. So the thread catches it, and stores it in a `std::exception_ptr` - a pointer to the
     * exception object, which lives on. After `join`, `main` throws it again with `rethrow_exception`. That is what
     * `std::async` does for you, see the other follow-up.
     */
    void catch_in_the_thread() {
        print_function_header();

        std::exception_ptr error{nullptr};
        thread t{[&error] {
            try {
                throw std::runtime_error{"something broke in the thread"};
            } catch (...) {
                error = std::current_exception();
            }
        }};
        t.join();
        try {
            if (error) {
                std::rethrow_exception(error);
            }
        } catch (const std::exception& e) {
            cout << " 1| caught in main: " << e.what() << '\n';
        }
        // thread u{[] { throw std::runtime_error{"lost"}; }};   // terminate, exit status 134
    }

    /* --- `use_jthread` ---
     * `std::jthread` joins in its destructor - RAII, no forgotten `join`. And it has a stop token: the function can
     * take a `std::stop_token` as its first parameter and ask it whether somebody wants it to stop. `request_stop()`
     * asks; the destructor asks, too, and then joins. Cooperative: the thread must look, nobody kills it.
     * libstdc++ has it since gcc 10, MSVC since VS 2019; libc++ needed `-fexperimental-library` up to version 18 - the
     * snippet asks the library first.
     */
    void use_jthread() {
        print_function_header();

#if defined(__cpp_lib_jthread)
        int rounds{0};
        {
            std::jthread worker{[&rounds](const std::stop_token& token) {
                while (!token.stop_requested()) {
                    ++rounds;
                    std::this_thread::sleep_for(milliseconds{10});
                }
            }};
            std::this_thread::sleep_for(milliseconds{55});
            worker.request_stop();
        }                                   // joins here
        cout << " 1| the worker stopped after " << rounds << " rounds\n";
#else
        cout << " 1| no std::jthread in this library\n";
#endif
    }

    /* --- `safe_counter` ---
     * A class that protects itself: every member function locks the mutex first. `value` is `const` - it does not
     * change the counter - but it must lock, and locking changes the mutex. So the mutex is `mutable` (see previous
     * snippets): it is not part of what the object means. The users of the class need no lock of their own.
     */
    class safe_counter {
    public:
        void add(const int n) {
            const lock_guard lock{m_};
            value_ += n;
        }

        int value() const {
            const lock_guard lock{m_};
            return value_;
        }

    private:
        mutable mutex m_;
        int value_{0};
    };

    /* --- `protect_inside_the_class` --- Four threads, one `safe_counter`. */
    void protect_inside_the_class() {
        print_function_header();

        safe_counter counter;
        vector<thread> threads;
        for (int i{0}; i < 4; ++i) {
            threads.emplace_back([&counter] {
                for (int k{0}; k < 10'000; ++k) {
                    counter.add(1);
                }
            });
        }
        for (thread& t : threads) {
            t.join();
        }
        cout << " 1| value=" << counter.value() << '\n';
    }

    /* --- `unlock_early` ---
     * `unique_lock` is a `lock_guard` that can do more: `unlock()` before the `}`, `lock()` again, or start unlocked
     * with `std::defer_lock`. It costs a `bool` more - it must know whether it holds the lock. Condition variables need
     * it, see the other follow-up. Hold a lock as briefly as possible: copy what you need, unlock, then work.
     */
    void unlock_early() {
        print_function_header();

        mutex m;
        vector<int> shared{1, 2, 3};
        unique_lock lock{m};
        const vector<int> copy{shared};
        lock.unlock();                      // the others may go on - we work on the copy
        int sum{0};
        for (const int n : copy) {
            sum += n;
        }
        lock.lock();
        shared.push_back(sum);
        cout << " 1| owns the lock: " << lock.owns_lock() << ", shared.back()=" << shared.back() << '\n';
    }

    /* --- `account` and `transfer` ---
     * A transfer locks both accounts - the money must leave one and arrive in the other in one step. `scoped_lock`
     * takes both mutexes at once, with an algorithm that cannot deadlock, whatever the order of the arguments.
     */
    struct account {
        mutex m;
        int balance{100};
    };

    void transfer(account& from, account& to, const int amount) {
        const scoped_lock lock{from.m, to.m};
        from.balance -= amount;
        to.balance += amount;
    }

    /* --- `transfer_by_hand` ---
     * The same with two `lock_guard`s: `from` first, then `to`. Thread 1 transfers from `a` to `b`, and holds `a`;
     * thread 2 transfers from `b` to `a`, and holds `b`. Each waits for the mutex the other holds - forever: a
     * deadlock. Not undefined behavior - the program simply stops moving. The cure: one order for all (by address, by
     * number), or `scoped_lock`.
     * - !![#deadlock]
     */
    // void transfer_by_hand(account& from, account& to, const int amount) {
    //     const lock_guard first{from.m};
    //     std::this_thread::sleep_for(milliseconds{1});  // makes the deadlock likely
    //     const lock_guard second{to.m};
    //     from.balance -= amount;
    //     to.balance += amount;
    // }

    /* --- `avoid_a_deadlock` ---
     * A thousand transfers each way, with `scoped_lock`. Remove the `//`s here and in `transfer_by_hand`, and the
     * program hangs - stop it by hand.
     */
    void avoid_a_deadlock() {
        print_function_header();

        account a;
        account b;
        thread t1{[&a, &b] {
            for (int i{0}; i < 1000; ++i) {
                transfer(a, b, 1);
            }
        }};
        thread t2{[&a, &b] {
            for (int i{0}; i < 1000; ++i) {
                transfer(b, a, 1);
            }
        }};
        t1.join();
        t2.join();
        cout << " 1| a=" << a.balance << ", b=" << b.balance << '\n';
        // thread d1{[&a, &b] { transfer_by_hand(a, b, 1); }};
        // thread d2{[&a, &b] { transfer_by_hand(b, a, 1); }};
        // d1.join();                       // never returns: a deadlock
        // d2.join();
    }

}

/* --- `main` --- */
int main() {
    join_or_detach();
    catch_in_the_thread();
    use_jthread();
    protect_inside_the_class();
    unlock_early();
    avoid_a_deadlock();

    return EXIT_SUCCESS;
}
