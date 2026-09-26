// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Kind: optional study - for the curious, not exam-relevant.
 *
 * Teaching Focus
 * - `goto` - so that you recognize it, not so that you use it.
 * - Every loop ends up as jumps in the machine code.
 * - Leaving nested loops: the one case where `goto` is defensible - and
 *   the usual alternative.
 */

#include <iostream>
#include <cstdlib>                          // for EXIT_SUCCESS
#include <cbl/printing.hpp>                 // for `print_function_header`

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `loop_with_for` --- The usual way, for comparison. */
    void loop_with_for() {
        print_function_header();

        for (int i{5}; i <= 10; ++i) {
            cout << " 1|   i=" << i << '\n';
        }
    }

    /* --- `loop_with_goto` ---
     * `goto` jumps to a label. The compiler turns every loop into exactly such
     * jumps, so `goto` is not less powerful - it is harder to read and to
     * maintain. That is why we do not use it. You will meet it in old C code
     * (`goto cleanup;`); C++ has better tools for that, which come later.
     */
    void loop_with_goto() {
        print_function_header();

        int i{5};
    again:                                  // a label, the target of `goto`
        cout << " 1|   i=" << i << '\n';
        if (i++ < 10) {
            goto again;
        }

        /* -- .Look at the machine code. --
         * Let the compiler stop after translating to assembly:
         *     g++ -std=c++23 -S -O0 -I ../../utils may_goto.cpp
         * and open `may_goto.s`. Find both functions (their names look strange,
         * that is called name mangling) and the jump instructions in them -
         * `jmp`, `jle`, ... on x86, `b`, `ble`, ... on ARM.
         */
    }

    /* --- `escape_nested_loops` ---
     * `break` leaves only the inner loop, but here we want out of both.
     * A `goto` to a label right after the loops does exactly that.
     */
    void escape_nested_loops() {
        print_function_header();

        for (int i{1}; i <= 9; ++i) {
            for (int j{1}; j <= 9; ++j) {
                if (i * j == 42) {
                    cout << " 1| found " << i << "*" << j << "=42\n";
                    goto done;
                }
            }
        }
        cout << " 2| not found\n";
    done:
        cout << " 3| after the loops\n";
    }

    /* --- `find_factors` ---
     * The usual alternative: put the loops into a function of their own.
     * `return` leaves both loops at once - and the search gets a name.
     */
    bool find_factors(const int product) {
        for (int i{1}; i <= 9; ++i) {
            for (int j{1}; j <= 9; ++j) {
                if (i * j == product) {
                    cout << " 1| found " << i << "*" << j << "=" << product << '\n';
                    return true;
                }
            }
        }
        return false;
    }

    /* --- `escape_with_return` --- Same search as `escape_nested_loops`, without `goto`. */
    void escape_with_return() {
        print_function_header();

        if (!find_factors(42)) {
            cout << " 2| not found\n";
        }
        cout << " 3| after the loops\n";
    }

}

/* --- Open questions ---
 * - How different are `loop_with_for` and `loop_with_goto` in `may_goto.s`?
 * - Rewrite `loop_with_goto` with `while`. Which version is easier to read?
 * - A third way out of nested loops is a `bool` flag. Write it - which of
 *   the three versions do you prefer, and why?
 * - Java has `break outer;` for exactly this case. C++ does not. Why might
 *   that be less of a loss than it seems?
 * - What problem does `goto cleanup;` solve in C?
 */

/* --- `main` --- */
int main() {
    loop_with_for();
    loop_with_goto();
    escape_nested_loops();
    escape_with_return();

    return EXIT_SUCCESS;
}
