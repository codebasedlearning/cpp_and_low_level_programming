// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `assert` checks a condition at runtime - a simple way to test your code.
 * - A failing `assert` stops the program at once, with a non-zero exit status.
 * - In release builds every `assert` is gone.
 */

#include <iostream>
#include <cassert>                          // for `assert`
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `is_even` --- The function under test. */
    bool is_even(const int n) {
        return n % 2 == 0;
    }

    /* --- `test_with_assert` ---
     * `assert(condition)` does nothing if the condition is true. If it is false, it prints file, line and condition,
     * and aborts the program - no exception, no clean-up, just the end.
     * - !![#assert]
     */
    void test_with_assert() {
        print_function_header();

        cout << " 1| is_even(4)=" << is_even(4) << ", is_even(7)=" << is_even(7) << '\n';

        assert(is_even(4));
        assert(!is_even(7));
        assert(is_even(0));
        cout << " 2| all tests passed\n";

        // assert(is_even(3));              // fails - try it, then check `echo $?` (if on terminal)
    }

    /* --- `disable_asserts_in_release` ---
     * If the macro `NDEBUG` is defined, as in a typical release build, every `assert` disappears - including the code
     * inside it. So never put work into an `assert` that the program needs.
     */
    void disable_asserts_in_release() {
        print_function_header();

        // `#ifdef` asks the preprocessor whether a macro is defined.
#ifdef NDEBUG
        cout << " 1| NDEBUG is defined, asserts are off\n";
#else
        cout << " 1| NDEBUG is not defined, asserts are on\n";
#endif
    }

}

/* --- `main` --- */
int main() {
    test_with_assert();
    disable_asserts_in_release();

    return EXIT_SUCCESS;
}
