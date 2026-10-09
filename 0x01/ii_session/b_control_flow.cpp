// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Loops and `if` work as in Java - a quick look for completeness.
 * - `if` with an init-statement: a variable that lives only as long as needed.
 * - `while`, `do`-`while`, `break`, `continue` and `switch`: see future snippets.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `loop_with_for` --- The classic `for`: init; condition; increment. */
    void loop_with_for() {
        print_function_header();

        int sum{0};
        for (int i{1}; i <= 10; ++i) {
            sum += i;
        }
        cout << " 1| 1+2+...+10=" << sum << '\n';
        // cout << i;                       // compiler error: `i` exists only inside the loop
    }

    /* --- `branch_with_if` --- As in Java; the `else` part is optional. */
    void branch_with_if() {
        print_function_header();

        const int n{23};
        const int n2{n * n};                // lives until the end of the function
        if (n2 > 500) {
            cout << " 1| n^2=" << n2 << " is greater than 500\n";
        } else {
            cout << " 2| n^2=" << n2 << " is at most 500\n";
        }
    }

    /* --- `branch_with_if_init` ---
     * In `if (init; condition)` the variable `n2` exists only inside the `if`/`else`. Scope and lifetime go together:
     * once the `if` is done, the stack slot of `n2` can be reused. Compare with `branch_with_if`.
     * - !![#scope]
     */
    void branch_with_if_init() {
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
    loop_with_for();
    branch_with_if();
    branch_with_if_init();

    return EXIT_SUCCESS;
}
