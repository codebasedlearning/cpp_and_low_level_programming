// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x01 - the things to remember.
 */

#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::numeric_limits;


/* ---- Content ---- */

namespace {

    /* --- `recall_where_values_live` ---
     * A variable is a few bytes at an address, usually on the stack. Without an initializer, those bytes contain
     * whatever was there before - reading them is undefined behavior, and Debug and Release show different results.
     */
    void recall_where_values_live() {
        print_function_header();

        const int n{0x00112233};
        cout << " 1| n at " << &n << ", " << sizeof(n) << " bytes, little-endian: 33 22 11 00\n";
    }

    /* --- `recall_platform_sizes` ---
     * The standard only fixes minimum sizes - `long` has 8 bytes on Linux and macOS, 4 on Windows. Braces reject
     * conversions that lose information.
     */
    void recall_platform_sizes() {
        print_function_header();

        cout << " 1| sizeof(long)=" << sizeof(long) << ", max int=" << numeric_limits<int>::max() << '\n';
        // int v{2.5};                      // compiler error: narrowing
    }

    /* --- `recall_where_strings_live` ---
     * A `string` object has a fixed size. Short text lives inside it, long text on the heap - and growing may move it.
     * That is where this unit continues.
     */
    void recall_where_strings_live() {
        print_function_header();

        const string s{"A text that is too long for the string object itself."};
        const void* chars{s.c_str()};
        cout << " 1| sizeof(string)=" << sizeof(string) << ", &s=" << &s << ", characters at " << chars << '\n';
    }

    /* --- `pot_checked` ---
     * b^n for b >= 1 and n >= 0, as in 'Blue Canyon' - but before every multiplication it checks whether the result
     * still fits into an `int`.
     * The check cannot overflow itself: it divides instead of multiplying.
     * Returns -1 if the result does not fit - a valid result is never negative.
     */
    int pot_checked(const int b, const int n) {
        const int max{numeric_limits<int>::max()};
        int result{1};
        for (int i{0}; i < n; ++i) {
            if (result > max / b) {
                return -1;                  // `result * b` would be larger than `max`
            }
            result *= b;
        }
        return result;
    }

    /* --- `recall_silent_overflow` ---
     * Every call gets its own stack frame, and `int` overflow is undefined behavior - no error, no exception. But you
     * can check before it happens.
     */
    void recall_silent_overflow() {
        print_function_header();

        cout << " 1| 2^30=" << pot_checked(2, 30) << '\n';
        cout << " 2| 2^31=" << pot_checked(2, 31) << " - does not fit, detected in time\n";
    }

}

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`) - the same code can behave differently.
 * - The debugger: breakpoints, the frames list and the memory view.
 * - `g++ -E` (after the preprocessor) and `g++ -S` (assembly).
 * - `echo $?` - the exit status of the last program.
 */

/* --- `main` --- */
int main() {
    recall_where_values_live();
    recall_platform_sizes();
    recall_where_strings_live();
    recall_silent_overflow();

    return EXIT_SUCCESS;
}
