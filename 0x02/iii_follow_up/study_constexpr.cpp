// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `const`: cannot change after initialization - the value may come at runtime.
 * - `constexpr`: known at compile time; functions that may run at compile time.
 * - `consteval`: functions that must run at compile time.
 * - `static_assert`: a check that runs while compiling, and costs nothing when the program runs.
 * - Where a value computed by the compiler ends up: in the program file itself, next to the string literals - not on
 *   the stack.
 */

#include <iostream>
#include <array>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::array;


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

        const array<int, times4(2)> slots{};    // the compiler needs the size - and can compute it
        cout << " 3| slots.size()=" << slots.size() << '\n';
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

    /* --- `check_at_compile_time` ---
     * `static_assert` checks a claim while compiling. If it is wrong, the build fails with the message; if it is
     * right, nothing is left of it in the program. Change the 12 to 13 and build.
     * - !![#assert]
     */
    void check_at_compile_time() {
        print_function_header();

        static_assert(times4(3) == 12, "times4 is broken");
        static_assert(sizeof(int) >= 2, "int is too small for this program");
        cout << " 1| both claims were checked by the compiler - nothing to do at runtime\n";
    }

    /* --- `make_squares` and `squares` ---
     * A `constexpr` function may have a loop, local variables and an `array`. Called in a constant expression, the
     * compiler runs it - and writes only the ten results into the program.
     */
    constexpr array<int, 10> make_squares() {
        array<int, 10> result{};
        int n{0};
        for (int& x : result) {
            x = n * n;
            ++n;
        }
        return result;
    }

    constexpr array<int, 10> squares{make_squares()};

    /* --- `show_where_constants_live` ---
     * The table `squares` was computed while compiling. Its address is close to that of a string literal - both are
     * part of the program file, loaded with it, read-only - and far from a local on the stack (see previous snippets:
     * where the characters of a literal live). No loop runs when the program starts, and nothing can change the table.
     */
    void show_where_constants_live() {
        print_function_header();

        const int local{7};
        const char* literal{"So What"};
        const void* table{squares.data()};
        const void* text{literal};
        cout << " 1| squares[7]=" << squares[7] << '\n';
        cout << " 2| table at " << table << ", literal at " << text << ", local at " << &local << '\n';
    }

}

/* --- `main` --- */
int main() {
    compare_const_and_constexpr();
    call_consteval_functions();
    check_at_compile_time();
    show_where_constants_live();

    return EXIT_SUCCESS;
}
