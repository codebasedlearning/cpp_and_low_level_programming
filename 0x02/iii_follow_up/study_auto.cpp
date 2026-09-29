// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `auto`: the compiler deduces the type from the initializer.
 * - `auto&` and `const auto&` - especially in range-based `for` loops.
 * - Deduced return types, and the trailing return type `auto f(...) -> type`.
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `deduce_from_literals` ---
     * The type comes from the right-hand side and is fixed at compile time - nothing dynamic. Use `auto` where it makes
     * code clearer, not everywhere.
     * - !![#auto]
     */
    void deduce_from_literals() {
        print_function_header();

        auto f{1.23f};                      // float
        auto d{2.34};                       // double
        auto i{1};                          // int
        auto l{2L};                         // long
        auto c{'x'};                        // char
        cout << " 1| sizeof: f=" << sizeof(f) << ", d=" << sizeof(d) << ", i=" << sizeof(i)
             << ", l=" << sizeof(l) << ", c=" << sizeof(c) << '\n';
    }

    /* --- `deduce_references` ---
     * Plain `auto` always deduces a value type - it copies. If you want a reference, you have to say so: `auto&` or
     * `const auto&`.
     */
    void deduce_references() {
        print_function_header();

        int n{1};
        auto copy{n};                       // int - a copy
        auto& ref{n};                       // int& - an alias
        copy = 2;
        ref = 3;
        cout << " 1| n=" << n << ", copy=" << copy << ", ref=" << ref << '\n';

        vector<int> v{1, 2, 3};
        for (auto& x : v) {                 // modify the elements themselves
            x *= 10;
        }
        for (const auto& x : v) {           // read without copying
            cout << " 2|   x=" << x << '\n';
        }
    }

    /* --- `twice` --- The return type is deduced from the `return` statement. */
    auto twice(const string& s) {
        return s + s;
    }

    /* --- `deduce_return_types` --- */
    void deduce_return_types() {
        print_function_header();

        const auto s{twice("ab")};          // string
        cout << " 1| s='" << s << "', s.size()=" << s.size() << '\n';
    }

    auto add(const int a, const int b) -> int { return a + b; }        // explicit, at the end
    auto sub(const int a, const int b) { return a - b; }               // deduced from `return`
    auto mul(const int a, const double b) { return a * b; }            // deduced: double

    // The parameters are only known after `(`, so `decltype(a / b)` must come at the end. In front of the name it would
    // not compile:
    //     decltype(a / b) divide(int a, double b)
    auto divide(const int a, const double b) -> decltype(a / b) { return a / b; }

    /* --- `declare_trailing_return_types` ---
     * `auto f(...) -> type` names the return type after the parameters - needed where it depends on them, as in
     * `divide`.
     * - !![#decltype]
     */
    void declare_trailing_return_types() {
        print_function_header();

        cout << " 1| add(2,3)=" << add(2, 3) << ", sub(3,4)=" << sub(3, 4) << '\n';
        cout << " 2| mul(2,5.1)=" << mul(2, 5.1) << ", divide(4,8.0)=" << divide(4, 8.0) << '\n';
    }

}

/* --- `main` --- */
int main() {
    deduce_from_literals();
    deduce_references();
    deduce_return_types();
    declare_trailing_return_types();

    return EXIT_SUCCESS;
}
