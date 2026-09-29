// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Kengate', see ../tasks.md.

#include <iostream>
#include <cstdlib>

using std::cout;

// Each result is written only if the caller wants it - a `nullptr` means "not needed".
bool divmod(const int n, const int d, int* quotient, int* remainder) {
    const bool ok{d != 0};
    if (quotient != nullptr) {
        *quotient = ok ? n / d : 0;
    }
    if (remainder != nullptr) {
        *remainder = ok ? n % d : 0;
    }
    return ok;
}

// Extension: the results as one value.
struct division {
    int quotient;
    int remainder;
    bool ok;
};

division divide(const int n, const int d) {
    if (d == 0) {
        return division{0, 0, false};
    }
    return division{n / d, n % d, true};
}

void test(const int n, const int d) {
    int q{-1};
    int r{-1};
    const bool ok{divmod(n, d, &q, &r)};
    cout << n << " / " << d << ": ok=" << ok << ", quotient=" << q << ", remainder=" << r;
    if (ok) {
        cout << ", check: " << q * d + r;       // always n again
    }
    cout << '\n';
}

int main() {
    test(17, 5);                            // 3, 2
    test(23, 0);                            // false, 0, 0
    test(-7, 2);                            // -3, -1: rounded toward zero, the remainder has the sign of `n`
    test(7, -2);                            // -3, 1

    int q{0};
    divmod(100, 7, &q, nullptr);            // only the quotient
    cout << "100 / 7 = " << q << " (remainder not needed)\n";

    const auto [quotient, remainder, ok]{divide(17, 5)};
    cout << "divide(17, 5): " << quotient << ", " << remainder << ", ok=" << ok << '\n';

    // Extension, in Compiler Explorer with -O1: `divmod` writes its results to memory, through the addresses in `rdx`
    // and `rcx` (x86-64 Linux and macOS). `divide` returns its 12 bytes in two registers: quotient and remainder in
    // `rax`, `ok` in `edx` - ARM64: `x0` and `x1`. With `long long` members, `division` has 24 bytes: more than two
    // registers can hold, so the caller passes the address of the result in `rdi` (ARM64: `x8`), and `divide` writes
    // there. The border is at 16 bytes.
    // `std::div` from <cstdlib> does the same as `divide`, and returns a `std::div_t`.

    return EXIT_SUCCESS;
}
