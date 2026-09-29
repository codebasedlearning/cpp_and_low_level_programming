// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* --- `temperature.hpp` ---
 * The header: what a user of the class needs to know - its name, its member functions, and its data members. Only
 * declarations of the member functions, no bodies; those are in `temperature.cpp`.
 * - !![#header-and-source]
 */

#pragma once                                // include this file only once per translation unit

/* -- .Include guard. --
 * `#pragma once` protects against the header being pasted twice into the same translation unit, which would define the
 * class twice. The classic form, `#ifndef TEMPERATURE_HPP` / `#define TEMPERATURE_HPP` / `#endif`, does the same, see
 * the headers in `utils/cbl`.
 * - !![#include-guard]
 */

/* --- `temperature` ---
 * A temperature in degrees Celsius, never below absolute zero - the constructor checks it.
 */
class temperature {
public:
    explicit temperature(double celsius);   // throws `std::invalid_argument` below -273.15

    double celsius() const;
    double fahrenheit() const;
    void warm_up(double delta);

    /* -- .Why are the private members in the header? --
     * `main.cpp` never sees the bodies of the member functions - but to create a `temperature` on its stack, it must
     * know how many bytes to reserve. So every translation unit that uses the class needs its data members, private or
     * not: `private` is a rule for the compiler, not a secret.
     */
private:
    double celsius_;
};
