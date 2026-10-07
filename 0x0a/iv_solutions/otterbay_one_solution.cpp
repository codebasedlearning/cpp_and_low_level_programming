// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Otter Bay', see ../tasks.md.

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstddef>
#include <cstdlib>

using std::cout, std::queue, std::thread, std::mutex, std::lock_guard, std::unique_lock, std::condition_variable;

// A queue for several threads: one mutex, and two condition variables - one says "not empty", one "not full". With
// `capacity == 0`, the queue has no limit and `push` never waits.
class number_queue {
public:
    explicit number_queue(const std::size_t capacity) : capacity_{capacity} {}

    void push(const int n) {
        {
            unique_lock lock{m_};
            not_full_.wait(lock, [this] {
                const bool full{capacity_ > 0 && numbers_.size() >= capacity_};
                if (full) {
                    ++producer_waits_;
                }
                return !full;
            });
            numbers_.push(n);
        }
        not_empty_.notify_one();            // outside the lock: the woken thread can take the mutex at once
    }

    int pop(int& waits) {
        int n{0};
        {
            unique_lock lock{m_};
            not_empty_.wait(lock, [this, &waits] {
                if (numbers_.empty()) {
                    ++waits;
                }
                return !numbers_.empty();
            });
            n = numbers_.front();
            numbers_.pop();
        }
        not_full_.notify_one();
        return n;
    }

    int producer_waits() const {
        const lock_guard lock{m_};
        return producer_waits_;
    }

private:
    mutable mutex m_;
    condition_variable not_empty_;
    condition_variable not_full_;
    queue<int> numbers_;
    std::size_t capacity_;
    int producer_waits_{0};
};

struct result {
    long long sum{0};
    int count{0};
    int waits{0};
};

void consume(number_queue& q, result& r) {
    while (true) {
        const int n{q.pop(r.waits)};
        if (n < 0) {
            return;
        }
        r.sum += n;
        ++r.count;
    }
}

void run(const std::size_t capacity) {
    number_queue q{capacity};
    result first;
    result second;
    thread c1{consume, std::ref(q), std::ref(first)};
    thread c2{consume, std::ref(q), std::ref(second)};
    thread producer{[&q] {
        for (int n{1}; n <= 10'000; ++n) {
            q.push(n);
        }
        q.push(-1);                         // one sentinel per consumer
        q.push(-1);
    }};
    producer.join();
    c1.join();
    c2.join();

    cout << (capacity == 0 ? "unbounded" : "at most 10") << ":\n";
    cout << "  consumer 1: " << first.count << " numbers, sum " << first.sum << ", found the queue empty "
         << first.waits << " times\n";
    cout << "  consumer 2: " << second.count << " numbers, sum " << second.sum << ", found the queue empty "
         << second.waits << " times\n";
    cout << "  total " << first.sum + second.sum << ", the producer found the queue full " << q.producer_waits()
         << " times\n";
}

int main() {
    run(0);
    run(10);

    // Our runs, x86-64 Linux with two cores: the total is always 50,005,000; the split between the consumers changes
    // from run to run - between 45 and 55 percent here. Unbounded, the producer never waits - it cannot - and the
    // consumers find the queue empty a few dozen times. Bounded to 10, all three wait about a thousand
    // times: the producer whenever the consumers fall behind, the consumers whenever the producer does.
    // Each consumer needs its own sentinel: a consumer that takes the -1 stops, and the other one would wait forever
    // for a number that never comes. `notify_all` instead of `notify_one` is still correct - every woken thread checks
    // its predicate - but it wakes both consumers for one number, and one of them goes back to sleep: more wakeups for
    // nothing.

    return EXIT_SUCCESS;
}
