// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Hollow Creek', see ../tasks.md.

#include <iostream>
#include <vector>
#include <deque>
#include <memory>
#include <functional>
#include <cstdlib>

using std::cout, std::vector, std::deque, std::function, std::unique_ptr, std::make_unique;

// The task's version - `count` dies at the `}`, the lambda keeps its address. Our runs, x86-64 Linux: gcc 13 prints
// garbage as Debug (32768 21979 21979), and 1 2 3 as Release - right by accident - with the warning "'count' is used
// uninitialized" (`-O2 -Wall`). clang 18 warns "address of stack memory associated with local variable 'count'
// returned", and prints garbage as Debug and as Release. AddressSanitizer with gcc reports "stack-use-after-return".
// Nothing of it is reliable - least of all the run that looks right.
// auto make_dangling_counter() {
//     int count{0};
//     return [&count] { return ++count; };
// }

// Fixed: the lambda has its own `count`, a member, and `mutable` lets it count.
auto make_counter() {
    return [count = 0]() mutable { return ++count; };
}

class sensor {
public:
    explicit sensor(const double value) : value_{value} {}

    function<double()> reader() const { return [this] { return value_; }; }

    // Fix 3: a reader that does not need `this` - a copy of the value. It no longer follows changes of the sensor,
    // it reads what the value was when the reader was made.
    function<double()> snapshot() const { return [value = value_] { return value; }; }

    const void* address() const { return this; }

private:
    double value_;
};

// Extension: a sensor that can be neither copied nor moved.
class fixed_sensor {
public:
    explicit fixed_sensor(const double value) : value_{value} {}
    fixed_sensor(const fixed_sensor&) = delete;
    fixed_sensor& operator=(const fixed_sensor&) = delete;
    fixed_sensor(fixed_sensor&&) = delete;
    fixed_sensor& operator=(fixed_sensor&&) = delete;

    function<double()> reader() const { return [this] { return value_; }; }

private:
    double value_;
};

int main() {
    const auto c = make_counter();
    auto counter = c;                       // a copy - `c` itself is `const`, and `operator()` changes the member
    cout << "counter: " << counter() << ' ' << counter() << ' ' << counter() << '\n';

    vector<function<int()>> counters;
    for (int i{0}; i < 5; ++i) {
        counters.push_back(make_counter());
    }
    counters[0]();
    counters[0]();
    counters[3]();
    cout << "five counters, next values:";
    for (function<int()>& next : counters) {
        cout << ' ' << next();
    }
    cout << '\n';

    // The dangling readers: the vector grows from 2 to 4, and moves the sensors into a new block.
    {
        vector<sensor> sensors;
        sensors.emplace_back(1.5);
        sensors.emplace_back(2.5);
        vector<function<double()>> readers{sensors[0].reader(), sensors[1].reader()};
        const void* old_address{sensors[0].address()};
        cout << "readers: " << readers[0]() << ", " << readers[1]() << '\n';
        sensors.emplace_back(3.5);
        cout << "sensor 0 was at " << old_address << ", is at " << sensors[0].address() << '\n';
        // cout << readers[0]();            // undefined behavior: reads the freed block
        // Our runs, x86-64 Linux: a tiny number like 4.7e-310 instead of 1.5. The freed block went back to the heap,
        // and glibc keeps its own list of free blocks in them: the first 8 bytes of the old sensor are an address now,
        // read as a `double`. With AddressSanitizer: "heap-use-after-free", the read in the lambda, the block freed by
        // the vector when it grew, and where it had been allocated.
    }

    // Fix 1: `reserve` - no reallocation while there is room. It promises nothing once the capacity is used up.
    {
        vector<sensor> sensors;
        sensors.reserve(10);
        sensors.emplace_back(1.5);
        const function<double()> first{sensors[0].reader()};
        sensors.emplace_back(2.5);
        cout << "fix 1, reserve: " << first() << '\n';
    }

    // Fix 2: the vector holds `unique_ptr`s - it moves the pointers when it grows, the sensors stay where they are. One
    // allocation per sensor, and a pointer to follow.
    {
        vector<unique_ptr<sensor>> sensors;
        sensors.push_back(make_unique<sensor>(1.5));
        const function<double()> first{sensors[0]->reader()};
        for (int i{0}; i < 100; ++i) {
            sensors.push_back(make_unique<sensor>(0.0));
        }
        cout << "fix 2, unique_ptr: " << first() << '\n';
    }

    // Fix 3: a snapshot - a copy of the value, no `this`.
    {
        vector<sensor> sensors;
        sensors.emplace_back(1.5);
        const function<double()> first{sensors[0].snapshot()};
        sensors.emplace_back(2.5);
        cout << "fix 3, snapshot: " << first() << '\n';
    }

    // Extension: `vector<fixed_sensor>` with `emplace_back` does not compile - when the vector grows, it must move or
    // copy its elements, and neither is allowed. A `deque` never moves its elements when it grows at the ends: it
    // allocates a new chunk and leaves the old ones where they are. So `emplace_back` compiles - and the readers stay
    // valid.
    // vector<fixed_sensor> in_a_vector;
    // in_a_vector.emplace_back(1.5);       // compiler error: no move constructor, no copy constructor
    deque<fixed_sensor> in_a_deque;
    in_a_deque.emplace_back(1.5);
    const function<double()> first{in_a_deque[0].reader()};
    for (int i{0}; i < 1000; ++i) {
        in_a_deque.emplace_back(0.0);
    }
    cout << "extension, deque: " << first() << '\n';

    return EXIT_SUCCESS;
}
