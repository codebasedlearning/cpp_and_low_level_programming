// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Ravencastle', see ../tasks.md.

#include <iostream>
#include <numeric>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::gcd, std::invalid_argument;

class fraction {
public:
    // Throws if `denom` is 0 - such a fraction is never born. Extension: reduces the fraction, the sign goes to the
    // numerator.
    fraction(const int num, const int denom) : num_{num}, denom_{denom} {
        if (denom_ == 0) {
            throw invalid_argument{"denominator 0"};
        }
        const int divisor{gcd(num_, denom_)};   // of the absolute values; gcd(0, d) is d, so 0/5 becomes 0/1
        num_ /= divisor;
        denom_ /= divisor;
        if (denom_ < 0) {
            num_ = -num_;
            denom_ = -denom_;
        }
    }

    // All three only read - `const`, so they work on `const fraction`s, too.
    int num() const { return num_; }
    int denom() const { return denom_; }

    double value() const {
        // `num_ / denom_` would be an integer division, 1/2 gives 0. `1.0 *` makes it a `double` first.
        return 1.0 * num_ / denom_;
    }

private:
    int num_;
    int denom_;
};

/*
 * Extension, the split:
 * - `fraction.hpp`: `#pragma once` and the class with the declarations, e.g. `fraction(int num, int denom);` and
 *   `int num() const;` - and the data members.
 * - `fraction.cpp`: `#include "fraction.hpp"` and the definitions, e.g. `int fraction::num() const { return num_; }`.
 * - `main.cpp`: `#include "fraction.hpp"` and `main`.
 * - `add_executable(ravencastle main.cpp fraction.cpp)`.
 */

int main() {
    const fraction half{1, 2};
    cout << "half: " << half.num() << "/" << half.denom() << " = " << half.value() << " (expected 1/2 = 0.5)\n";

    const fraction f{6, -8};
    cout << "6/-8: " << f.num() << "/" << f.denom() << " = " << f.value() << " (expected -3/4 = -0.75)\n";

    try {
        const fraction broken{1, 0};
        cout << "never printed\n";
    } catch (const invalid_argument& e) {
        cout << "1/0: " << e.what() << " (expected)\n";
    }

    // Extension: two `int`s, no padding, nothing else - 8 bytes.
    cout << "sizeof(fraction)=" << sizeof(fraction) << '\n';

    return EXIT_SUCCESS;
}
