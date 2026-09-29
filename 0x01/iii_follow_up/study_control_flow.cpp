// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `while` and `do`-`while` - as in Java.
 * - `continue` and `break` in loops.
 * - `switch` on integral values, and fall through.
 * - `for` and `if` with init: see previous snippets.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `loop_with_while` ---
     * `while` checks before the body, `do`-`while` after it - so the body of a `do`-`while` runs at least once.
     */
    void loop_with_while() {
        print_function_header();

        int p{1};
        while (p < 100) {
            p *= 2;
        }
        cout << " 1| first power of 2 not below 100: " << p << '\n';

        // Counting digits: even 0 has one, so the body must run at least once.
        int n{12345};
        int digits{0};
        do {
            ++digits;
            n /= 10;
        } while (n != 0);
        cout << " 2| 12345 has " << digits << " digits\n";
    }

    /* --- `skip_and_leave_loops` --- Both work in every kind of loop. */
    void skip_and_leave_loops() {
        print_function_header();

        cout << " 1| i from 5 to 10:\n";
        for (int i{-10}; i <= 20; ++i) {
            if (i < 5) {
                continue;                   // skip the rest, go to the next iteration
            }
            cout << " 2|   i=" << i << '\n';
            if (i >= 10) {
                break;                      // leave the loop
            }
        }
    }

    /* --- `branch_with_switch` ---
     * `switch` works on integral types. The `case` labels must be constants known at compile time.
     * - !![#switch]
     */
    void branch_with_switch() {
        print_function_header();

        const int n{4};
        switch (n) {
            case 3:
                cout << " 1| case 3\n";
                break;                      // jump to the end of the `switch`
            case 4:
                cout << " 2| case 4\n";
                break;
            default:                        // optional: when no `case` matches
                cout << " 3| default\n";
        }
    }

    /* --- `fall_through` ---
     * Without `break`, execution simply continues with the next `case` - sometimes a feature, often a bug.
     * `[[fallthrough]]` says "this is intended", to the reader and to the compiler, which otherwise may warn.
     */
    void fall_through() {
        print_function_header();

        const char c{'A'};                  // `char` is integral, too
        switch (c) {
            case 'A':
                cout << " 1| case 'A'\n";
                [[fallthrough]];
            case 'B':
                cout << " 2| case 'B'\n";
                break;
            default:
                cout << " 3| default\n";
        }
    }

}

/* --- `main` --- */
int main() {
    loop_with_while();
    skip_and_leave_loops();
    branch_with_switch();
    fall_through();

    return EXIT_SUCCESS;
}
