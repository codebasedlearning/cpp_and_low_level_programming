// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'West Rose Cliff', see ../tasks.md.

#include <iostream>
#include <cstdlib>

using std::cout;

int main() {
    // input values - console input comes in the next unit
    const int n{23};
    const char c{'Q'};

    const bool is_n_positive{n > 0};
    const bool is_c_uppercase{'A' <= c && c <= 'Z'};    // 'A'..'Z' are contiguous in ASCII
    cout << "is_n_positive=" << is_n_positive << ", is_c_uppercase=" << is_c_uppercase << '\n';

    if (is_n_positive) {
        cout << "n=" << n << " is greater than 0\n";
    } else {
        cout << "n=" << n << " is not greater than 0\n";
    }

    // extension: `?:` instead of `if`
    cout << "'" << c << "' is " << (is_c_uppercase ? "" : "not ") << "an uppercase letter\n";

    // extension: `std::isupper` takes an `int`, because it must also accept EOF (-1).
    // Passing a negative `char` (possible for non-ASCII characters) is undefined behavior, so the usual idiom converts
    // to `unsigned char` first.

    return EXIT_SUCCESS;
}
