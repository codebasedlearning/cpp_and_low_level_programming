// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Define and initialize variables, preferably with braces `{}`.
 * - A first look at the primitive types.
 * - `const`: a value that cannot change after initialization.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `define_variables` ---
     * A variable has a type, a name and a value. C++ has several ways to initialize it - in this course we prefer
     * braces `{}`, because they work for every type.
     * - !![#declaration-vs-definition]
     * - !![#uniform-initialization]
     */
    void define_variables() {
        print_function_header();

        // Classic style with `=`.
        int v1 = 23;
        cout << " 1| v1=" << v1 << '\n';

        // Uniform (brace) initialization, since C++11.
        int v2{42};
        cout << " 2| v2=" << v2 << '\n';

        // Empty braces give the default value of the type, `0` for `int`.
        int v3{};
        cout << " 3| v3=" << v3 << '\n';
    }

    /* --- `use_primitive_types` ---
     * A first overview. Sizes, ranges and what happens at the limits follow in the session.
     */
    void use_primitive_types() {
        print_function_header();

        // Integers are signed by default.
        unsigned int i{23};
        signed int j{-42};                  // same as: int j{-42}
        cout << " 1| i=" << i << ", type unsigned int\n";
        cout << " 2| j=" << j << ", type signed int\n";

        // A 'longer' integer, at least 64 bit. Long literals are easier to read with `'` as digit separator (C++14) -
        // the compiler ignores it, `123'456'789'012'345` is the same number.
        long long ll{123'456'789'012'345};
        cout << " 3| ll=" << ll << ", type long long\n";

        // A single character, in single quotes.
        char c{'A'};
        cout << " 4| c='" << c << "', type char\n";

        // `true` or `false`, printed as `1` or `0`.
        bool b{true};
        cout << " 5| b=" << b << ", type bool\n";

        // Floating point, single precision - note the `f` suffix, without it `1.2` is a `double` literal.
        float f{1.2f};
        cout << " 6| f=" << f << ", type float\n";

        // Floating point, double precision - the default.
        double d{2.4};
        cout << " 7| d=" << d << ", type double\n";
    }

    /* --- `define_const_variables` ---
     * A `const` variable cannot be changed after initialization, so it must be initialized.
     * Rule of thumb: make it `const` unless it has to change.
     * - !![#const-correctness]
     */
    void define_const_variables() {
        print_function_header();

        int i{1};                           // modifiable
        const int j{2};                     // constant
        cout << " 1| i=" << i << ", j=" << j << '\n';

        i = 3;                              // fine
        // j = 4;                           // compiler error, try it
        cout << " 2| i=" << i << ", j=" << j << '\n';
    }

}

/* --- `main` --- Calls the functions above, one topic after the other. */
int main() {
    define_variables();
    use_primitive_types();
    define_const_variables();

    return EXIT_SUCCESS;
}
