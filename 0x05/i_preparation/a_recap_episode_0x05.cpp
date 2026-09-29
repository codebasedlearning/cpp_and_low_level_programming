// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x04 - the things to remember.
 */

#include <iostream>
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::string_view, std::span, std::vector, std::find;


/* ---- Content ---- */

namespace {

    /* --- `recall_views` ---
     * A view is two words: where, and how long. It copies nothing, owns nothing - and keeps nothing alive. A view of a
     * temporary dangles after the `;`.
     */
    void recall_views() {
        print_function_header();

        const string text{"Kind of Blue, 1959"};
        const string_view kind{string_view{text}.substr(0, 4)};
        const void* chars{text.data()};
        const void* viewed{kind.data()};
        cout << " 1| sizeof(kind)=" << sizeof(kind) << ", text at " << chars << ", kind at " << viewed << '\n';

        const vector<int> v{10, 20, 30, 40};
        const span<const int> middle{span<const int>{v}.subspan(1, 2)};
        cout << " 2| sizeof(middle)=" << sizeof(middle) << ", v.data()=" << v.data() << ", middle.data()="
             << middle.data() << '\n';
    }

    /* --- `recall_iterators` ---
     * An iterator into a `vector` is essentially an address: `*it` reads there, `++it` adds `sizeof(int)`. `end()` is
     * one past the last element - a position, not an element - and `find` returns it for "not found".
     */
    void recall_iterators() {
        print_function_header();

        const vector<int> v{2, 3, 5, 7};
        for (auto it{v.begin()}; it != v.end(); ++it) {
            cout << " 1|   &*it=" << &*it << ", *it=" << *it << '\n';
        }
        if (find(v.begin(), v.end(), 4) == v.end()) {
            cout << " 2| 4 not found\n";
        }
    }

    /* --- `largest` --- A function template: a recipe, compiled once per type that is used. */
    template <typename T>
    T largest(const T& a, const T& b) {
        return a < b ? b : a;
    }

    /* --- `recall_templates` ---
     * Two calls, two types, two functions in the object file: `largest<int>` and `largest<double>`.
     */
    void recall_templates() {
        print_function_header();

        cout << " 1| " << largest(3, 7) << ", " << largest(2.5, 1.5) << '\n';
    }

}

/* --- Symbols ---
 * `nm` lists the symbols of an object file: `T` defined here, `U` needed from elsewhere, `t` local, `W` weak - a
 * template instance or an `inline` function, of which the linker keeps one. The names are mangled, `_Z4areadd` is
 * `area(double, double)`; `nm -C` and `c++filt` translate them. Templates and `inline` functions live in headers,
 * ordinary functions in exactly one `.cpp` file.
 */

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`), `g++ -S` or Compiler Explorer (godbolt.org).
 * - The debugger: breakpoints, the memory view, stepping and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 * - `nm`, `nm -C` and `c++filt` - which functions are in which object file, and under which name.
 */

/* --- Teaser ---
 * A `span` is an address and a count, a `vector` iterator is an address wrapped in a class. Drop the class and the
 * count, and keep only the address. What can you still do with it - and who remembers where the elements end?
 */

/* --- `main` --- */
int main() {
    recall_views();
    recall_iterators();
    recall_templates();

    return EXIT_SUCCESS;
}
