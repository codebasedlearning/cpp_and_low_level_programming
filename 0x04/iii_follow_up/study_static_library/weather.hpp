// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* --- `weather.hpp` ---
 * The header of the library: what a program that uses it needs to know. Declarations only - the definitions are in
 * `stats.cpp`, `report.cpp` and `convert.cpp`, compiled once into the library. Everything is in the namespace
 * `weather`, so that the names of the library do not clash with the names of a program (see previous snippets).
 * - !![#static-library]
 */

#pragma once

#include <string>
#include <vector>

namespace weather {

    double mean(const std::vector<double>& values);         // in `stats.cpp`
    double spread(const std::vector<double>& values);       // in `stats.cpp`
    std::string report(const std::vector<double>& values);  // in `report.cpp`
    double to_fahrenheit(double celsius);                   // in `convert.cpp`

    /* -- .A variable declared with `extern`. --
     * A declaration, not a definition: "there is an `int` with this name, somewhere". `stats.cpp` defines it, exactly
     * once. Without `extern`, every file that includes the header would define its own - `multiple definition`.
     */
    extern int values_seen;

    /* -- .An `inline` variable. --
     * A definition, in the header - allowed, because `inline` says "the same definition may appear in several
     * translation units", as for `inline` functions (C++17). The linker keeps one.
     */
    inline int decimals{1};

}
