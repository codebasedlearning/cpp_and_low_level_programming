// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Wintervale', see ../tasks.md.

#include <iostream>
#include <array>
#include <cstdlib>

using std::cout, std::array;

// B^N with a loop - `constexpr`, so the compiler may run it while compiling.
constexpr long long power(const long long b, const unsigned n) {
    long long result{1};
    for (unsigned i{0}; i < n; ++i) {
        result *= b;
    }
    return result;
}

// The same, but the compiler must run it: every call needs constant arguments.
consteval long long power_now(const long long b, const unsigned n) {
    return power(b, n);
}

// Extension: the old way, with templates - a variable template, defined by itself for N - 1, and a partial
// specialization for N = 0 that stops the recursion.
template <long long B, unsigned N>
constexpr long long power_v{B * power_v<B, N - 1>};

template <long long B>
constexpr long long power_v<B, 0>{1};

int main() {
    static_assert(power(2, 10) == 1024);
    static_assert(power_now(3, 4) == 81);
    static_assert(power_v<2, 10> == 1024);

    const array<int, power(2, 4)> slots{};          // the compiler needs the size - and computes it
    cout << "slots.size()=" << slots.size() << '\n';

    unsigned n{0};
    cout << "n? ";
    if (std::cin >> n) {
        cout << "2^" << n << "=" << power(2, n) << " (at runtime)\n";
        // cout << power_now(2, n);         // error: 'n' is not a constant expression
    }

    // constexpr long long big{power(10, 19)};  // error: overflow in constant expression - 10^19 > 2^63 - 1.
    // At runtime, the same overflow is undefined behavior and goes unnoticed (unit 0x01). In a constant expression,
    // UB is not allowed: the compiler has to check - and refuses.

    // Extension, `nm -C` on the object file: `power` does not appear as long as it is only used while compiling. A
    // `constexpr` function is implicitly `inline`, and nobody needs its code. With the call at runtime above, it is
    // there, as a `W` - as Debug; as Release, it is inlined into `main`. The template version leaves more behind: as
    // Debug, gcc keeps every instance, `power_v<2, 0>` to `power_v<2, 10>`, as a constant in read-only data (`r`) -
    // eleven of them for one `static_assert`. As Release, none.
    return EXIT_SUCCESS;
}
