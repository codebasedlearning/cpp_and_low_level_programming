// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A template accepts every type - until its body does something the type cannot. The error then comes from deep
 *   inside the template, not from the call.
 * - A concept (C++20) names a requirement on a type: `std::integral<T>` is `true` or `false`, at compile time.
 * - Four ways to write the constraint: `template <std::integral T>`, `requires` after the template head, `requires`
 *   after the parameters, and `std::integral auto` as the parameter type.
 * - A concept of your own, with a `requires` expression: "`os << x` must compile".
 * - Overloading by concept: the compiler picks the version whose requirement is met.
 * - Nothing of it is left at runtime: a concept is checked while compiling, the generated function is the same.
 */

#include <iostream>
#include <string>
#include <vector>
#include <concepts>                         // for integral, floating_point
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::string, std::vector, std::to_string;


/* ---- Content ---- */

namespace {

    /* --- `twice` --- Unconstrained: every type that has a `+`. */
    template <typename T>
    T twice(const T& x) {
        return x + x;
    }

    /* --- `use_an_unconstrained_template` ---
     * `twice` works for `int` and `double` - and for `string`, where `+` appends: maybe intended, maybe not. For a
     * `vector`, which has no `+`, the error points into the body of `twice`, and the call is only mentioned further
     * down in the message. With a template that calls other templates, that message gets long.
     */
    void use_an_unconstrained_template() {
        print_function_header();

        cout << " 1| twice(21)=" << twice(21) << ", twice(1.5)=" << twice(1.5) << ", twice(string{\"ab\"})="
             << twice(string{"ab"}) << '\n';
        // twice(vector<int>{1, 2});        // compiler error, inside `twice`: no `operator+` for `vector<int>`
    }

    /* --- `half` in four spellings ---
     * The same function four times - pick one style and stay with it. The first one is the most common, the last one
     * the shortest; the `requires` forms can combine several conditions with `&&` and `||`.
     * - !![#concepts]
     */
    template <std::integral T>
    T half_a(const T n) { return n / 2; }

    template <typename T>
        requires std::integral<T>
    T half_b(const T n) { return n / 2; }

    template <typename T>
    T half_c(const T n) requires std::integral<T> { return n / 2; }

    auto half_d(const std::integral auto n) { return n / 2; }

    /* --- `use_standard_concepts` ---
     * With a `double`, the compiler refuses at the call: `half_a(7.0)` - "no matching function", "constraints not
     * satisfied", and the reason: `is_integral_v<double>` is `false` (the definition of `std::integral`). The error
     * names the rule, not a line in the body.
     */
    void use_standard_concepts() {
        print_function_header();

        cout << " 1| half_a(7)=" << half_a(7) << ", half_b(7L)=" << half_b(7L) << ", half_c(8)=" << half_c(8)
             << ", half_d(9)=" << half_d(9) << '\n';
        // cout << half_a(7.0);             // compiler error: `double` does not satisfy `std::integral`
    }

    /* --- `printable` ---
     * A concept of your own. The `requires` expression lists what must compile for a `T` - here `os << x` - without
     * running it. `printable<int>` is `true`, `printable<vector<int>>` is `false`: there is no `operator<<` for a
     * `vector`. Both are known while compiling, so `static_assert` can check them.
     */
    template <typename T>
    concept printable = requires(ostream& os, const T& x) {
        os << x;
    };

    static_assert(printable<int>);
    static_assert(printable<string>);
    static_assert(!printable<vector<int>>);

    /* --- `print_all` --- For a `vector` of anything printable. */
    template <printable T>
    void print_all(const vector<T>& values) {
        for (const T& v : values) {
            cout << v << ' ';
        }
    }

    /* --- `write_a_concept` ---
     * A `vector<vector<int>>` is refused at the call of `print_all`: `vector<int>` does not satisfy `printable`,
     * because `os << x` would be invalid - one clear reason instead of a long message about `operator<<` inside the
     * loop.
     */
    void write_a_concept() {
        print_function_header();

        cout << " 1| ";
        print_all(vector<int>{1, 2, 3});
        print_all(vector<string>{"Kind", "of", "Blue"});
        cout << '\n';
        // print_all(vector<vector<int>>{{1}, {2}});    // compiler error: `vector<int>` is not `printable`
    }

    /* --- `describe` ---
     * Two overloads, two requirements: for an `int` only the first is viable, for a `double` only the second. For a
     * `string`, none - and that is an error at the call.
     */
    string describe(const std::integral auto n) {
        return "integral " + to_string(n);
    }

    string describe(const std::floating_point auto x) {
        return "floating point " + to_string(x);
    }

    /* --- `overload_by_concept` ---
     * What a specialization did for types before C++20, as two ordinary-looking functions. The choice is made while
     * compiling; each call is a direct call of its own instance, as without concepts. Predict the third one before you
     * run it: a `char` is an integral type, too.
     */
    void overload_by_concept() {
        print_function_header();

        cout << " 1| " << describe(42) << ", " << describe(2.5) << ", " << describe('A') << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_an_unconstrained_template();
    use_standard_concepts();
    write_a_concept();
    overload_by_concept();

    return EXIT_SUCCESS;
}
