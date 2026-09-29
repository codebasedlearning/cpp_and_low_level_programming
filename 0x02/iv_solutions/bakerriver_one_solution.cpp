// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Baker River', see ../tasks.md.

#include <iostream>
#include <string>
#include <cmath>                            // for `std::floor`
#include <cstdlib>

using std::cout, std::cin, std::string;

void cut(double& a, double& b);
void rotate(string& x, string& y, string& z);

int main() {
    double a{};
    double b{};
    cout << "Enter two numbers: ";
    cin >> a >> b;
    cout << "before cut: a=" << a << ", b=" << b << ", &a=" << &a << '\n';
    cut(a, b);
    cout << "after cut:  a=" << a << ", b=" << b << '\n';

    string x{};
    string y{};
    string z{};
    cout << "Enter three words: ";
    cin >> x >> y >> z;
    cout << "before rotate: " << x << ' ' << y << ' ' << z << '\n';
    rotate(x, y, z);
    cout << "after rotate:  " << x << ' ' << y << ' ' << z << '\n';

    return EXIT_SUCCESS;
}

// References: `cut` changes the variables of the caller. Extension: `&a` here is the same address as `&a` in `main` - a
// reference is not a copy.
void cut(double& a, double& b) {
    cout << "  in cut:   &a=" << &a << '\n';
    a = std::floor(a);                      // rounds down: -2.5 becomes -3; `std::trunc` cuts towards 0
    b = std::floor(b);
}

void rotate(string& x, string& y, string& z) {
    const string first{x};                  // one copy is needed, otherwise `x` is lost
    x = y;
    y = z;
    z = first;
}
