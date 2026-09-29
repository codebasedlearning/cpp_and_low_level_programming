// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A second, closer look at the symbols of an object file: what the compiler puts there, and how the linker uses
 *   them.
 * - `T`: ordinary functions, and member functions defined outside the class. Exactly one object file may define
 *   them - so they belong in a `.cpp` file.
 * - `W`: template instances, `inline` functions, member functions defined in the class body. Every translation unit
 *   that uses them makes its own copy, the linker keeps one - so they may be in a header, and templates must be.
 * - `-O0` vs. `-O2`: the `W`s are inlined and disappear, the `T`s stay.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * As in the preparation, the next functions and the class are not in the unnamed namespace - as if they came from a
 * header and a source file, see previous snippets. So `nm` shows how they would be seen by other translation units.
 */

/* --- `total` --- An ordinary function: in a real project, declared in the header, defined in a `.cpp` file. */
double total(const vector<double>& values) {
    double sum{0.0};
    for (const double v : values) {
        sum += v;
    }
    return sum;
}

/* --- `largest` ---
 * A template - in a real project, completely in the header. `values` must not be empty.
 * - !![#template]
 */
template <typename T>
T largest(const vector<T>& values) {
    T result{values.front()};
    for (const T& v : values) {
        if (result < v) {
            result = v;
        }
    }
    return result;
}

/* --- `midpoint` ---
 * An `inline` function: "this definition may appear in several translation units, and it is the same one everywhere".
 * In a header without `inline`, two `.cpp` files that include it would both define `midpoint`:
 * `multiple definition` (see previous snippets).
 * - !![#odr]
 */
inline double midpoint(const double a, const double b) {
    return (a + b) / 2.0;
}

/* --- `range` ---
 * A class with both kinds of member functions: the constructor and `width` are defined in the class body - `inline`
 * without the word - and `contains` is defined below the class, as it would be in the `.cpp` file.
 */
class range {
public:
    range(const double low, const double high) : low_{low}, high_{high} {}

    double width() const { return high_ - low_; }
    bool contains(double x) const;

private:
    double low_;
    double high_;
};

bool range::contains(const double x) const {
    return low_ <= x && x <= high_;
}

namespace {

    /* --- `use_all_kinds` --- Calls everything above - `largest` for `double` and for `int`. */
    void use_all_kinds() {
        print_function_header();

        const vector<double> temperatures{18.5, 21.0, 19.5, 23.0};
        const vector<int> floors{3, 12, 7};
        const range comfortable{19.0, 22.0};
        cout << " 1| total=" << total(temperatures) << ", largest=" << largest(temperatures)
             << ", highest building=" << largest(floors) << " floors\n";
        cout << " 2| midpoint=" << midpoint(temperatures.front(), temperatures.back())
             << ", width=" << comfortable.width() << ", 21.0 comfortable? " << comfortable.contains(21.0) << '\n';
    }

}

/* --- The symbols ---
 * Build as Debug and list the symbols of
 * `cmake-build-debug/0x04/CMakeFiles/d_symbols_revisited.dir/ii_session/d_symbols_revisited.cpp.o`, filtered for the
 * names of this program: `nm -C <file> | grep -E 'total|largest|midpoint| range::|use_all'`. With gcc (clang lists the
 * same):
 * - `T total(...)` and `T range::contains(double) const` - defined here, once.
 * - `W double largest<double>(...)`, `W int largest<int>(...)` - one instance per type.
 * - `W midpoint(double, double)`, `W range::width() const`, `W range::range(double, double)` - the `inline` ones.
 * - `t (anonymous namespace)::use_all_kinds()` - local.
 * On macOS, `nm` shows the `W`s as `T`; `nm -m` says `weak external`.
 * - !![#symbols]
 */

/* --- Two translation units ---
 * Now split this file in your head, as in unit 0x03: a header with `largest`, `midpoint`, the class and the declaration
 * of `total`; `statistics.cpp` with the definitions of `total` and `range::contains`; `main.cpp` and `report.cpp`, both
 * including the header, both calling `largest<double>`.
 * - `statistics.o`: `T total`, `T range::contains`.
 * - `main.o` and `report.o`: `U total`, `U range::contains` - only declared there - and each its own
 *   `W largest<double>`, `W midpoint`, ... The compiler of `main.cpp` cannot know that the compiler of `report.cpp`
 *   generates the same instance.
 * The linker then connects every `U` with exactly one `T` - none: `undefined reference`, two: `multiple definition`.
 * Of several `W`s with the same name it keeps one and drops the others. They are all the same, the ODR says so - and
 * nobody checks it.
 * - !![#translation-unit]
 */

/* -- .Q&A -- !![Two object files contain `largest<double>`. Which one ends up in the program?](#a-407) */

/* --- Why templates live in the header ---
 * Suppose the body of `largest` were only in `statistics.cpp`, and the header had only the declaration,
 * `template <typename T> T largest(const std::vector<T>& values);`.
 * - `main.o` and `report.o` get a `U largest<double>(...)`: they know the recipe exists, but not what it says, so they
 *   cannot generate the code.
 * - `statistics.o` has the recipe, but nothing in `statistics.cpp` calls `largest<double>` - so it generates nothing.
 * - The linker: `undefined reference to 'double largest<double>(...)'`.
 * An ordinary function is compiled once, where it is defined. A template can only be compiled where it is used - so its
 * body must be visible there: in the header. Try it for real in task 'Fox Hollow'.
 */

/* --- Debug and Release ---
 * Build as Release and list the symbols again. The `W`s are gone: `largest`, `midpoint`, `width` and the constructor
 * are small, so the compiler copied their code into `use_all_kinds` - inlined. `use_all_kinds` is gone, too, inlined
 * into `main`. `total` and `range::contains` stay, although they were inlined as well: another object file could call
 * them, and the compiler of one file never knows.
 */

/* --- `main` --- */
int main() {
    use_all_kinds();

    return EXIT_SUCCESS;
}
