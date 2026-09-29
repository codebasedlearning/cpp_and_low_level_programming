// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Elkford', see ../tasks.md.

#include <iostream>
#include <array>
#include <cstdlib>

using std::cout, std::array, std::ostream;

constexpr int dim{3};

struct polynom {
    array<double, dim> coeffs;              // c0 + c1*x + c2*x^2
};

// `const&`: the polynomial is neither copied nor changed.
double eval(const polynom& p, const double x) {
    // Horner's method, c0 + x*(c1 + x*c2): 2 multiplications instead of 3 - in general n instead of about n^2/2 for a
    // polynomial of degree n.
    double result{0.0};
    for (int i{dim - 1}; i >= 0; --i) {
        result = result * x + p.coeffs[i];
    }
    return result;
}

// Extension: `cout << p` calls this function; it returns the stream so that `<<` can be chained.
ostream& operator<<(ostream& os, const polynom& p) {
    os << p.coeffs[0] << " + " << p.coeffs[1] << "x + " << p.coeffs[2] << "x^2";
    return os;
}

int main() {
    const polynom p{{2.0, 3.0, 4.0}};
    cout << "p(x) = " << p << '\n';
    cout << "p(0)=" << eval(p, 0.0) << " (expected 2)\n";
    cout << "p(1)=" << eval(p, 1.0) << " (expected 9)\n";
    cout << "p(2)=" << eval(p, 2.0) << " (expected 24)\n";

    return EXIT_SUCCESS;
}
