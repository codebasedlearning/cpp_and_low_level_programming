// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `const`: cannot change after initialization - the value may come at runtime.
 * - `constexpr`: known at compile time; functions that may run at compile time.
 * - `consteval`: functions that must run at compile time.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    int times3(const int n) { return n * 3; }
    constexpr int times4(const int n) { return n * 4; }
    consteval int times5(const int n) { return n * 5; }

    /* --- `compare_const_and_constexpr` ---
     * A `const` may be initialized with a value computed at runtime. A `constexpr` must be computable by the compiler -
     * that is why it can be used where the compiler needs a number, e.g. as `std::array` size.
     * - !![#const]
     * - !![#constexpr]
     */
    void compare_const_and_constexpr() {
        print_function_header();

        const int a{times3(3)};             // fine: const, value from a normal function
        constexpr int b{times4(3)};         // fine: computed by the compiler
        // constexpr int c{times3(3)};      // compiler error: times3 is not constexpr
        cout << " 1| a=" << a << ", b=" << b << '\n';

        int m{7};                           // not a constant
        cout << " 2| times4(m)=" << times4(m) << " - a constexpr function at runtime\n";
    }

    /* --- `call_consteval_functions` ---
     * A `consteval` function can only be called with constants - every call is evaluated by the compiler.
     * - !![#consteval]
     */
    void call_consteval_functions() {
        print_function_header();

        constexpr int k{times5(3)};
        cout << " 1| k=" << k << '\n';

        // int m{7};
        // cout << times5(m);               // compiler error: m is not a constant
    }

}

/* --- `main` --- */
int main() {
    compare_const_and_constexpr();
    call_consteval_functions();

    return EXIT_SUCCESS;
}
