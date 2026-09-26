// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Kind: optional study - for the curious, not exam-relevant.
 *
 * Teaching Focus
 * - More integer types: `short`, `long long` and the fixed-width types.
 * - `int8_t` is a byte - and `cout` prints it as a character.
 * - `long double`, the type whose size nobody agrees on.
 */

#include <iostream>
#include <cstdint>                          // for the fixed-width types
#include <cstdlib>                          // for EXIT_SUCCESS
#include <cbl/printing.hpp>                 // for `print_function_header`

using std::cout;
using std::int8_t, std::uint16_t, std::int32_t, std::uint64_t;


/* ---- Content ---- */

namespace {

    /* --- `more_integers` --- `short` and `long long`, both signed by default. */
    void more_integers() {
        print_function_header();

        short sh{1};
        long long ll{2};
        cout << " 1| sh=" << sh << ", sizeof(short)=" << sizeof(short) << '\n';
        cout << " 2| ll=" << ll << ", sizeof(long long)=" << sizeof(long long) << '\n';
    }

    /* --- `fixed_width` ---
     * `int` is "some reasonable size". When the number of bits is part of the
     * contract - file formats, network protocols, hardware registers - say so:
     * `int8_t` ... `int64_t`, and `uint8_t` ... `uint64_t` for unsigned.
     */
    void fixed_width() {
        print_function_header();

        uint16_t u2{4};
        int32_t i4{5};
        uint64_t u8{6};
        cout << " 1| u2=" << u2 << ", sizeof(uint16_t)=" << sizeof(uint16_t) << '\n';
        cout << " 2| i4=" << i4 << ", sizeof(int32_t)=" << sizeof(int32_t) << '\n';
        cout << " 3| u8=" << u8 << ", sizeof(uint64_t)=" << sizeof(uint64_t) << '\n';

        // `int8_t` is usually `signed char`, so `cout` prints it as a character.
        // The unary `+` turns it into an `int` first.
        int8_t i1{65};
        cout << " 4| i1=" << i1 << " or " << +i1 << ", sizeof(int8_t)=" << sizeof(int8_t) << '\n';
    }

    /* --- `long_double` --- At least as precise as `double`, but how big? */
    void long_double() {
        print_function_header();

        long double ld{1.0L};
        cout << " 1| ld=" << ld << ", sizeof(long double)=" << sizeof(long double) << '\n';
    }

}

/* --- Open questions ---
 * - `long double` has 16 bytes on x86-64 Linux, but only 8 on Windows and
 *   on Apple Silicon. What does that mean for a file that stores one?
 * - Why is "a byte" and "a small number" the same type in C++?
 * - What happens with `uint8_t u{255}; ++u;`? And with `int8_t i{127}; ++i;`?
 */

/* --- `main` --- */
int main() {
    more_integers();
    fixed_width();
    long_double();

    return EXIT_SUCCESS;
}
