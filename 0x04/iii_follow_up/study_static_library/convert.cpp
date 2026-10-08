// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* --- `convert.cpp` --- Part of the library - but no program in this folder calls it. See `main.cpp`. */

#include "weather.hpp"

namespace weather {

    double to_fahrenheit(const double celsius) {
        return celsius * 9.0 / 5.0 + 32.0;
    }

}
