// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Roarport', see ../tasks.md. Count the allocations as Debug.

#include <iostream>
#include <functional>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::function;

// Extension: the class of the second lambda, by hand. C++ Insights shows the same - two `int` members, a constructor
// that takes them, and `bool operator()(int value) const`.
struct in_interval {
    int a;
    int b;

    bool operator()(const int value) const { return a <= value && value <= b; }
};

int main() {
    const auto add = [](const double x, const double y, const double z) { return x + y + z; };
    cout << "add(1, 2, 3.5)=" << add(1.0, 2.0, 3.5) << '\n';

    int a{1};                               // not `const`: a constant would not need to be captured
    int b{5};
    const auto is_within = [a, b](const int value) { return a <= value && value <= b; };
    cout << "3 in [1, 5]: " << is_within(3) << ", 7 in [1, 5]: " << is_within(7) << '\n';

    int z{3};
    const auto negate_z = [&z] { z = -z; };
    cout << "z=" << z;
    negate_z();
    cout << ", after negate_z: z=" << z << '\n';

    // 1 byte (no member, but an object takes at least one byte), 8 bytes (two `int`s), 8 bytes (an address).
    cout << "sizeof: add " << sizeof(add) << ", is_within " << sizeof(is_within) << ", negate_z " << sizeof(negate_z)
         << '\n';

    // Extension: `[z] { z = -z; }` does not compile - "assignment of read-only variable": the copy is a member, and
    // `operator()` is `const`. With `mutable`, it compiles, and negates the copy inside the lambda - `z` outside stays
    // as it was. The lambda is 4 bytes now, the `int`.
    auto negate_copy = [z]() mutable { z = -z; return z; };
    cout << "mutable copy: returns " << negate_copy() << ", z=" << z << ", sizeof " << sizeof(negate_copy) << '\n';

    const in_interval by_hand{a, b};
    cout << "by hand: 3 in [1, 5]: " << by_hand(3) << ", sizeof " << sizeof(by_hand) << '\n';

    // Extension: 32 bytes with libstdc++, 48 with libc++. The lambda is 8 bytes and can be copied byte by byte: it
    // fits into the buffer inside the `std::function` - no allocation.
    heap_watch heap{};
    const function<bool(int)> wrapped{is_within};
    cout << "sizeof(function<bool(int)>)=" << sizeof(wrapped) << ", allocations=" << heap.allocations()
         << ", wrapped(4)=" << wrapped(4) << '\n';

    a = 10;                                 // changes nothing inside the lambdas: they have copies
    cout << "after a = 10: is_within(3)=" << is_within(3) << '\n';

    return EXIT_SUCCESS;
}
