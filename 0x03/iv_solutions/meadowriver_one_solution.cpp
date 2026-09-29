// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Meadow River', see ../tasks.md.

#include <iostream>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <cstdlib>

using std::cout, std::array, std::size_t, std::out_of_range, std::to_string;

// Extension: 4 instead of 3. In the split, it goes into the header - every file that uses `polynom` needs it.
constexpr size_t dim{4};

class polynom {
public:
    explicit polynom(const array<double, dim>& coeffs) : coeffs_{coeffs} {}

    // `const`: evaluating does not change the polynomial. Horner's method, as in 'Elkford'. The index is a `size_t`,
    // like the one of `array` - so it counts down from `dim` to 1 and uses `i - 1`: a `size_t` is never negative, a
    // condition `i >= 0` would always be true.
    double eval(const double x) const {
        double result{0.0};
        for (size_t i{dim}; i > 0; --i) {
            result = result * x + coeffs_[i - 1];
        }
        return result;
    }

    double at(const size_t i) const {
        if (i >= dim) {
            throw out_of_range{"index " + to_string(i) + " not in [0, " + to_string(dim) + ")"};
        }
        return coeffs_[i];
    }

private:
    array<double, dim> coeffs_;             // c0 + c1*x + c2*x^2 + ...
};

// Extension: a free function has no access to the private coefficients - it uses `at`, the public interface.
polynom add(const polynom& p, const polynom& q) {
    array<double, dim> sum{};
    for (size_t i{0}; i < dim; ++i) {
        sum[i] = p.at(i) + q.at(i);
    }
    return polynom{sum};
}

int main() {
    const polynom p{{2.0, 3.0, 4.0, 0.0}};  // the 'Elkford' polynomial, 2 + 3x + 4x^2
    cout << "p(0)=" << p.eval(0.0) << " (expected 2)\n";
    cout << "p(1)=" << p.eval(1.0) << " (expected 9)\n";
    cout << "p(2)=" << p.eval(2.0) << " (expected 24)\n";

    const polynom q{{1.0, 0.0, 0.0, 1.0}};  // 1 + x^3
    cout << "q(2)=" << q.eval(2.0) << " (expected 9)\n";
    cout << "(p+q)(2)=" << add(p, q).eval(2.0) << " (expected 33)\n";

    cout << "p.at(1)=" << p.at(1) << " (expected 3)\n";
    try {
        const double c{p.at(4)};
        cout << "p.at(4)=" << c << " - never printed\n";
    } catch (const out_of_range& e) {
        cout << "p.at(4): " << e.what() << " (expected)\n";
    }

    return EXIT_SUCCESS;
}
