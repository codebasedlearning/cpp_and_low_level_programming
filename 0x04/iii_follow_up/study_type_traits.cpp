// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Specializations can compute with types: a type trait answers a question about a type, at compile time.
 * - `static_assert` checks such an answer while compiling - no cost at runtime.
 * - `if constexpr` picks a branch at compile time; the other one is not even generated.
 * - Concepts (C++20) state what a template needs - and give readable error messages.
 * - And a template can even compute numbers while compiling: template metaprogramming, the old-fashioned way.
 */

#include <iostream>
#include <string>
#include <type_traits>                      // for is_integral_v, is_pointer_v
#include <concepts>                         // for integral
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

namespace {

    /* --- `is_a_pointer` ---
     * A type trait of our own: the general recipe says `false`, the partial specialization for `T*` says `true` - the
     * same trick as `maybe<T*>` in the session. `std::is_pointer_v` from `<type_traits>` does exactly this.
     * `static constexpr`: a constant that belongs to the class, not to an object - `static` members: see future
     * snippets.
     * - !![#type-traits]
     */
    template <typename T>
    struct is_a_pointer {
        static constexpr bool value{false};
    };

    template <typename T>
    struct is_a_pointer<T*> {
        static constexpr bool value{true};
    };

    /* --- `ask_about_types` ---
     * The answers exist at compile time - `static_assert` checks them there. Change one to a wrong claim and build.
     * - !![#assert]
     */
    void ask_about_types() {
        print_function_header();

        static_assert(!is_a_pointer<int>::value);
        static_assert(is_a_pointer<int*>::value);
        static_assert(std::is_pointer_v<const char*>);
        static_assert(sizeof(int) >= 4, "this program needs an int with at least 32 bits");
        cout << " 1| int: " << is_a_pointer<int>::value << ", int*: " << is_a_pointer<int*>::value
             << ", integral double? " << std::is_integral_v<double> << '\n';
    }

    /* --- `describe` ---
     * `if constexpr` decides while compiling. For `describe<int>`, only the first branch is generated; for
     * `describe<string>`, only the last - `value * 2` would not even compile for a `string`, and it does not have to.
     */
    template <typename T>
    string describe(const T& value) {
        if constexpr (std::is_integral_v<T>) {
            return "a whole number, twice is " + std::to_string(value * 2);
        } else if constexpr (std::is_pointer_v<T>) {
            return "an address";
        } else {
            return "something else";
        }
    }

    /* --- `branch_at_compile_time` --- */
    void branch_at_compile_time() {
        print_function_header();

        const int n{21};
        cout << " 1| " << describe(n) << '\n';
        cout << " 2| " << describe(&n) << '\n';
        cout << " 3| " << describe(string{"so what"}) << '\n';
    }

    /* --- `gcd_of` ---
     * `std::integral T` instead of `typename T`: the template only accepts integer types. `gcd_of(1.5, 2.5)` fails at
     * the call, with a message that names the unsatisfied concept - not deep inside the body, as `std::gcd` did for
     * `fraction<double>` in 'Sparrow Town'.
     * - !![#concepts]
     */
    template <std::integral T>
    T gcd_of(T a, T b) {
        while (b != 0) {
            const T r{a % b};
            a = b;
            b = r;
        }
        return a;
    }

    /* --- `constrain_a_template` --- Remove the `//` and read the error. */
    void constrain_a_template() {
        print_function_header();

        cout << " 1| gcd_of(12, 18)=" << gcd_of(12, 18) << ", gcd_of(35L, 14L)=" << gcd_of(35L, 14L) << '\n';
        // cout << gcd_of(1.5, 2.5);        // error: constraints not satisfied, `double` is not `integral`
    }

    /* --- `fib` ---
     * Template metaprogramming: `fib<N>` is defined by `fib<N-1>` and `fib<N-2>`, and two full specializations stop
     * the recursion. The compiler instantiates `fib<10>`, `fib<9>`, ... and computes the value; the program only
     * contains the result. Today, a `constexpr` function does the same, readably - this is how it was done before.
     */
    template <int N>
    struct fib {
        static constexpr int value{fib<N - 1>::value + fib<N - 2>::value};
    };

    template <>
    struct fib<1> {
        static constexpr int value{1};
    };

    template <>
    struct fib<0> {
        static constexpr int value{0};
    };

    /* --- `compute_while_compiling` --- */
    void compute_while_compiling() {
        print_function_header();

        static_assert(fib<10>::value == 55);
        cout << " 1| fib<10>=" << fib<10>::value << ", fib<20>=" << fib<20>::value << '\n';
    }

}

/* --- `main` --- */
int main() {
    ask_about_types();
    branch_at_compile_time();
    constrain_a_template();
    compute_while_compiling();

    return EXIT_SUCCESS;
}
