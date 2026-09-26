// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Loops and `if` work as in Java - a quick look for completeness.
 * - `if` with an init-statement: a variable that lives only as long as needed.
 * - `while`, `do`-`while`, `break`, `continue` and `switch` are in
 *   `iii_follow_up/must_control_flow.cpp`.
 */

#include <iostream>
#include <cstdlib>                          // for EXIT_SUCCESS
#include <cbl/printing.hpp>                 // for `print_function_header`

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `simple_loop` --- The classic `for`: init; condition; increment. */
    void simple_loop() {
        print_function_header();

        int sum{0};
        for (int i{1}; i <= 10; ++i) {
            sum += i;
        }
        cout << " 1| 1+2+...+10=" << sum << '\n';
        // cout << i;                       // compiler error: `i` exists only inside the loop
    }

    /* --- `simple_if` --- As in Java; the `else` part is optional. */
    void simple_if() {
        print_function_header();

        const int n{23};
        const int n2{n * n};                // lives until the end of the function
        if (n2 > 500) {
            cout << " 1| n^2=" << n2 << " is greater than 500\n";
        } else {
            cout << " 2| n^2=" << n2 << " is at most 500\n";
        }
    }

    /* --- `if_with_init` ---
     * In `if (init; condition)` the variable `n2` exists only inside the
     * `if`/`else`. Scope and lifetime go together: once the `if` is done,
     * the stack slot of `n2` can be reused. Compare with `simple_if`.
     * - !![#scope]
     */
    void if_with_init() {
        print_function_header();

        const int n{23};
        if (int n2{n * n}; n2 > 500) {
            cout << " 1| n^2=" << n2 << " is greater than 500\n";
        } else {
            cout << " 2| n^2=" << n2 << " is at most 500\n";
        }
        // cout << n2;                      // compiler error: `n2` is out of scope - try it
    }

}

/* --- `main` --- */
int main() {
    simple_loop();
    simple_if();
    if_with_init();

    return EXIT_SUCCESS;
}
