// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Blue Canyon', see ../tasks.md.

#include <iostream>
#include <cstdlib>

using std::cout;

int pot(const int b, int n);                // declaration only, definition after `main`

int main() {
    // input values - console input comes in the next unit
    const int b{2};
    const int n{10};

    // b^n with `for`
    int res{1};
    for (int i{0}; i < n; ++i) {
        res *= b;
    }
    cout << b << "^" << n << "=" << res << " (for)\n";
    cout << b << "^" << n << "=" << pot(b, n) << " (pot, with while)\n";

    // extension: for b=2, n=31 is the first wrong result - 2^31 does not fit into an `int`
    // (max 2^31-1). Signed overflow is undefined behavior, so there is no error message;
    // usually the result is simply wrong (often negative). With `unsigned int` overflow is
    // well-defined: the result wraps around modulo 2^32, so 2^32 gives 0.

    return EXIT_SUCCESS;
}

// b^n with `while`, for n >= 0
int pot(const int b, int n) {
    int res{1};
    while (n > 0) {
        res *= b;
        --n;
    }
    return res;
}
