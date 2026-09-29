// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* --- `temperature.cpp` ---
 * The source file: the definitions of the member functions. It includes its own header first - so the compiler checks
 * that declarations and definitions match, and that the header compiles on its own.
 * `temperature::` in front of each name says: this is the member function of `temperature`, not a new free function.
 */

#include "temperature.hpp"                  // quotes: search next to this file first

#include <stdexcept>

// Fine in a `.cpp` file, it applies only here. Never put `using` into a header - it would apply to every file that
// includes it.
using std::invalid_argument;

temperature::temperature(const double celsius) : celsius_{celsius} {
    if (celsius < -273.15) {
        throw invalid_argument{"below absolute zero"};
    }
}

double temperature::celsius() const {
    return celsius_;
}

double temperature::fahrenheit() const {
    return celsius_ * 9.0 / 5.0 + 32.0;
}

void temperature::warm_up(const double delta) {
    if (celsius_ + delta < -273.15) {
        throw invalid_argument{"below absolute zero"};
    }
    celsius_ += delta;
}
