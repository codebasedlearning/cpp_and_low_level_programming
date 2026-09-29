// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Functions: declaration vs. definition.
 * - Every call gets its own stack frame - recursion makes that visible.
 * - `int` overflow is silent.
 * - In passing: the `?:` operator, scopes and `auto`.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- Declaration vs. definition ---
     * The compiler reads top to bottom and must know a function's signature before it is called. A declaration gives
     * only the signature, the definition adds the body - later in the file, or in another file.
     * - !![#declaration-vs-definition]
     */
    int fib(int n);                         // declaration only, definition at the end

    /* --- `factorial` ---
     * Recursive: `factorial(5)` calls `factorial(4)`, ... down to 1. Every call gets its own stack frame with its own
     * `n` - five calls, five frames.
     * - !![#recursion]
     */
    int factorial(int n) {
        return (n > 1) ? n * factorial(n - 1) : 1;  // `cond ? a : b` means: a if cond is true, else b
    }

    /* --- `call_recursive_functions` --- */
    void call_recursive_functions() {
        print_function_header();

        const int n{5};
        cout << " 1| factorials and Fibonaccis up to " << n << ":\n";
        for (int i{1}; i <= n; ++i) {
            cout << " 2|   " << i << "!=" << factorial(i) << ", fib(" << i << ")=" << fib(i) << '\n';
        }

        /* -- .Watch the stack frames. --
         * Set a breakpoint inside `factorial`, start with 'Debug' and look at the 'Frames' list while you continue: one
         * entry per call, and each frame has its own `n`.
         */

        /* -- .Q&A -- !![How many calls does `fib(30)` need?](#a-109) */
    }

    /* --- `show_silent_overflow` ---
     * `int` has a fixed size, and 13! does not fit into it.
     * - !![#undefined-behavior]
     */
    void show_silent_overflow() {
        print_function_header();

        cout << " 1| 12!=" << factorial(12) << '\n';
        cout << " 2| 13!=" << factorial(13) << " (correct: 6227020800)\n";

        /* -- .No error, no exception. --
         * Signed overflow is undefined behaviour - the program simply goes on with a wrong value.
         */

        /* -- .Q&A -- !![What happens with `factorial(1'000'000)`?](#a-110) */
    }

    /* --- `fib` ---
     * The definition belonging to the declaration at the top.
     * - `result` exists only inside this function, its scope.
     * - `auto` lets the compiler deduce the type from the right-hand side:
     *   `result` is a plain `int`, fixed at compile time - nothing dynamic.
     * - !![#scope]
     * - !![#auto]
     */
    int fib(int n) {
        auto result = (n <= 1) ? n : fib(n - 1) + fib(n - 2);
        return result;
    }

}

/* --- `main` --- */
int main() {
    call_recursive_functions();
    show_silent_overflow();

    return EXIT_SUCCESS;
}
