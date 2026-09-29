// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Sanborn Cliff', see ../tasks.md.

#include <iostream>
#include <array>
#include <string>
#include <cstdlib>

using std::cout, std::array, std::string;

// `const&` - printing needs neither a copy nor a change.
void print(const string& name, const array<double, 3>& values) {
    cout << name << ":";
    for (const double x : values) {
        cout << ' ' << x;
    }
    cout << '\n';
}

int main() {
    array<double, 3> a{1.0, 2.0};           // the third element is 0.0
    a[2] = a[0] + a[1];
    print("a", a);

    array<double, 3> b{};
    b = a;                                  // copies all elements
    b[0] = 10.0;
    print("a", a);
    print("b", b);

    // Two arrays: different addresses, 24 bytes each.
    cout << "&a[0]=" << &a[0] << ", &b[0]=" << &b[0] << '\n';

    // Extension: `=` does not compile for plain C arrays. For historical reasons (C), a C array is not a value that can
    // be assigned - in most expressions its name turns into the address of its first element (more on that with
    // pointers). `std::array` wraps the C array in a struct, and a struct can be copied member by member.

    return EXIT_SUCCESS;
}
