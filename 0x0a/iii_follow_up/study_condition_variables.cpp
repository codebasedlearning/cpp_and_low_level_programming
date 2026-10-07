// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Waiting by spinning - checking a flag in a loop - keeps a core busy for nothing: millions of checks.
 * - A condition variable lets a thread sleep until another one says that something changed. `wait` unlocks the mutex,
 *   sleeps, and locks it again when woken.
 * - Always wait with a condition - a predicate: a wakeup can come without a notification (spurious), and a notification
 *   can come before the wait (lost). The predicate catches both.
 * - The flag is protected by the mutex - no `volatile`, no atomic needed.
 * - A queue with a producer and a consumer - the classic.
 * - From the old course: the idea of the old snippet, without its `volatile`.
 */

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>               // for condition_variable, cv_status
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::queue, std::thread, std::mutex, std::lock_guard, std::unique_lock;
using std::condition_variable, std::atomic;
using std::chrono::milliseconds;


/* ---- Content ---- */

namespace {

    /* --- `spin` ---
     * Waits for the atomic flag of the session, and counts how often it looked. 100 ms of waiting.
     */
    void spin() {
        print_function_header();

        atomic<bool> ready{false};
        long long checks{0};
        thread waiter{[&ready, &checks] {
            while (!ready.load()) {
                ++checks;
            }
        }};
        std::this_thread::sleep_for(milliseconds{100});
        ready = true;
        waiter.join();
        cout << " 1| checked the flag " << checks << " times in 100 ms - a core, busy with nothing\n";
    }

    /* --- `wait_on_a_condition` ---
     * The same wait, with a condition variable. The waiting thread locks the mutex, and calls `wait` with a predicate:
     * if `ready` is `true`, it goes on at once; otherwise `wait` unlocks the mutex and puts the thread to sleep. `main`
     * sets `ready` - under the lock - and calls `notify_one`: the waiting thread wakes up, locks the mutex again,
     * checks the predicate, and goes on. The predicate was called once or twice - not millions of times.
     * `wait` needs a `unique_lock`: it must unlock and lock again.
     * - !![#condition-variable]
     */
    void wait_on_a_condition() {
        print_function_header();

        mutex m;
        condition_variable changed;
        bool ready{false};
        int checks{0};
        thread waiter{[&] {
            unique_lock lock{m};
            changed.wait(lock, [&] {
                ++checks;
                return ready;
            });
        }};
        std::this_thread::sleep_for(milliseconds{100});
        {
            const lock_guard lock{m};
            ready = true;
        }
        changed.notify_one();
        waiter.join();
        cout << " 1| checked the condition: " << checks << '\n';
    }

    /* --- `lose_a_notification` ---
     * Without the predicate: `wait(lock)` sleeps until the next notification - but the notification came before, while
     * the thread was busy with something else. A condition variable has no memory: the notification is lost, and the
     * thread would sleep forever. `wait_for` sleeps at most 200 ms, so we can see it: `timeout`.
     * With the predicate, the waiting thread first looks at `ready` - and does not sleep at all. That is why the old
     * snippet had a `while (!done)` around its `wait`: the same thing by hand.
     */
    void lose_a_notification() {
        print_function_header();

        mutex m;
        condition_variable changed;
        bool ready{false};
        bool timed_out{false};
        thread waiter{[&] {
            std::this_thread::sleep_for(milliseconds{100});     // busy with something else
            unique_lock lock{m};
            timed_out = changed.wait_for(lock, milliseconds{200}) == std::cv_status::timeout;
        }};
        {
            const lock_guard lock{m};
            ready = true;
        }
        changed.notify_one();               // nobody is waiting yet
        waiter.join();
        cout << " 1| without a predicate: timed out=" << timed_out << ", although ready=" << ready << '\n';
    }

    /* --- `produce_and_consume` ---
     * A producer puts the numbers 1 to 100 into a queue, and -1 at the end; a consumer takes them out and adds them
     * up, until it gets -1. One mutex protects the queue, one condition variable says "there is something in it". The
     * consumer sleeps while the queue is empty; every `push` wakes it. It takes the element under the lock and works
     * outside of it. How often it found the queue empty - and slept - depends on who was faster: run it a few times.
     * The -1 is a sentinel: a value that means "no more" - simpler than a second flag.
     */
    void produce_and_consume() {
        print_function_header();

        mutex m;
        condition_variable not_empty;
        queue<int> numbers;
        long long sum{0};
        int empty{0};

        thread consumer{[&] {
            while (true) {
                unique_lock lock{m};
                not_empty.wait(lock, [&] {
                    if (numbers.empty()) {
                        ++empty;
                    }
                    return !numbers.empty();
                });
                const int n{numbers.front()};
                numbers.pop();
                lock.unlock();
                if (n < 0) {
                    break;
                }
                sum += n;
            }
        }};
        thread producer{[&] {
            for (int n{1}; n <= 100; ++n) {
                {
                    const lock_guard lock{m};
                    numbers.push(n);
                }
                not_empty.notify_one();
            }
            {
                const lock_guard lock{m};
                numbers.push(-1);
            }
            not_empty.notify_one();
        }};
        producer.join();
        consumer.join();
        cout << " 1| sum=" << sum << ", the consumer found the queue empty: " << empty << '\n';
    }

}

/* --- Spurious wakeups ---
 * A waiting thread may wake up although nobody notified it - the standard allows it, and some systems do it, rarely.
 * The predicate turns that into "check, and sleep again". So: never `wait(lock)` alone, always `wait(lock, predicate)`,
 * or a loop around it. And change what the predicate reads only under the mutex - otherwise the change can fall between
 * the check and the sleep, and the notification is lost after all.
 */

/* --- `main` --- */
int main() {
    spin();
    wait_on_a_condition();
    lose_a_notification();
    produce_and_consume();

    return EXIT_SUCCESS;
}
