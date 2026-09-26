// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Pine Point', see ../tasks.md.

#include <iostream>
#include <cassert>
#include <cstdlib>

using std::cout;

bool is_prime(const int n);

int main() {
    assert(!is_prime(0));
    assert(!is_prime(1));
    assert(is_prime(2));
    assert(is_prime(7));
    assert(!is_prime(9));
    assert(is_prime(97));
    assert(!is_prime(100));

    // input value - console input comes in the next unit
    const int limit{100};

    cout << "primes below " << limit << ":";
    for (int n{2}; n < limit; ++n) {
        if (is_prime(n)) {
            cout << ' ' << n;
        }
    }
    cout << '\n';

    // extension: there are 78498 primes below 1'000'000. Measure `time ./pinepoint` yourself,
    // once built with -O0 and once with -O2 - the factor depends on your machine.

    return EXIT_SUCCESS;
}

bool is_prime(const int n) {
    if (n < 2) {
        return false;
    }
    for (int d{2}; d * d <= n; ++d) {       // a divisor above sqrt(n) has a partner below it
        if (n % d == 0) {
            return false;
        }
    }
    return true;
}
