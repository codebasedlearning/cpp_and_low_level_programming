// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Several template parameters, and a return type the compiler deduces with `auto`.
 * - `std::tuple`: a class template with any number of type parameters - and its layout, padding included.
 * - `std::initializer_list`: what braces with a list of values turn into - a view, again.
 */

#include <iostream>
#include <string>
#include <tuple>                            // for tuple
#include <vector>
#include <initializer_list>                 // for initializer_list
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector;
using std::tuple, std::get, std::initializer_list;


/* ---- Content ---- */

namespace {

    /* --- `larger` ---
     * `largest(3, 7.5)` from the session does not compile: `T` cannot be `int` and `double` at once. With two
     * parameters, `T` and `S`, it can. The return type is whatever `a < b ? b : a` gives - `auto` lets the compiler
     * work it out: `double` for `int` and `double`.
     * - !![#auto]
     */
    template <typename T, typename S>
    auto larger(const T& a, const S& b) {
        return a < b ? b : a;
    }

    /* --- `use_two_parameters` --- */
    void use_two_parameters() {
        print_function_header();

        const auto x{larger(3, 7.5)};
        const auto y{larger(8, 7.5)};
        cout << " 1| larger(3, 7.5)=" << x << ", larger(8, 7.5)=" << y << " - both doubles, sizeof " << sizeof(y)
             << '\n';
    }

    /* --- `use_a_tuple` ---
     * A `pair` with any number of members. The members have no names, only positions: `get<0>(t)`, where the 0 must be
     * known at compile time - each member has its own type. A structured binding gives them names.
     * A tuple is its members plus padding - like a `struct`, and with the same trap: `char, double, char` wastes 14
     * bytes. (libstdc++ stores the members in reverse order; the size is the same.)
     * - !![#pair-tuple]
     */
    void use_a_tuple() {
        print_function_header();

        const tuple<int, double, string> t{1, 2.5, "three"};
        cout << " 1| " << get<0>(t) << ", " << get<1>(t) << ", " << get<2>(t) << '\n';

        const auto [n, d, s]{t};
        cout << " 2| n=" << n << ", d=" << d << ", s=" << s << '\n';

        cout << " 3| sizeof(tuple<char, double, char>)=" << sizeof(tuple<char, double, char>)
             << ", sizeof(tuple<double, char, char>)=" << sizeof(tuple<double, char, char>) << '\n';
    }

    /* --- `bag` ---
     * A constructor that takes an `initializer_list<T>` is chosen for braces with a list of values - that is why
     * `vector<int>{5, 23}` has two elements. The compiler puts the values into a hidden array, and the list is a view
     * of it: an address and a count.
     */
    template <typename T>
    class bag {
    public:
        bag(const initializer_list<T> values) : items_{values} {
            cout << " a|   " << values.size() << " values, sizeof(values)=" << sizeof(values) << '\n';
        }

        size_t size() const { return items_.size(); }

    private:
        vector<T> items_;
    };

    /* --- `use_an_initializer_list` --- */
    void use_an_initializer_list() {
        print_function_header();

        const bag<int> numbers{2, 3, 5, 7, 11};
        const bag<string> words{"so", "what"};
        cout << " 1| numbers.size()=" << numbers.size() << ", words.size()=" << words.size() << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_two_parameters();
    use_a_tuple();
    use_an_initializer_list();

    return EXIT_SUCCESS;
}
