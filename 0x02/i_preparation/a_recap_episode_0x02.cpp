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

/* --- Namespaces ---
 * All snippet functions are embedded in an unnamed namespace, thus visible only in this file. In a single-file program
 * like this one it makes no difference yet - it is a habit we will need once programs consist of several files.
 * - !![#namespace]
 */
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

    /* --- `pot_checked` ---
     * b^n for b >= 1 and n >= 0, as in 'Blue Canyon' - but before every multiplication it checks whether the result
     * still fits into an `int`.
     * The check cannot overflow itself: it divides instead of multiplying.
     * Returns -1 if the result does not fit - a valid result is never negative.
     * - !![#constexpr]
     */
    int pot_checked(const int b, const int n) {
        const int max{numeric_limits<int>::max()};  // or (follows): constexpr int
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

/* --- `main` --- */
int main() {
    recall_where_values_live();
    recall_platform_sizes();
    recall_silent_overflow();

    return EXIT_SUCCESS;
}
