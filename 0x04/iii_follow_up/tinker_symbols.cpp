// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
* *
 * Teaching Focus
 * - The tool of this unit: `nm` lists the symbols of an object file - the names of the functions it defines, and of
 *   those it needs from elsewhere.
 * - Every function gets a machine name that encodes its parameter types: two overloads, two names.
 * - `nm -C` and `c++filt` translate the machine names back.
 * - `T`: ordinary functions, and member functions defined outside the class. Exactly one object file may define
 *   them - so they belong in a `.cpp` file.
 * - `W`: template instances, `inline` functions, member functions defined in the class body. Every translation unit
 *   that uses them makes its own copy, the linker keeps one - so they may be in a header, and templates must be.
 * - A named namespace is part of the name - in the source and in the symbol. It costs nothing at runtime.
 * - `-O0` vs. `-O2`: the `W`s are inlined and disappear, the `T`s stay.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The next functions are, on purpose, not in the unnamed namespace: other translation units could call them. That is
 * the normal case for the functions of a real project - and the case that `nm` shows best.
 */

/* --- `area` --- Two overloads: the area of a square and of a rectangle. */
double area(const double side) {
    return side * side;
}

double area(const double width, const double height) {
    return width * height;
}

/* --- `circle` --- A class with a member function, defined outside the class. */
class circle {
public:
    explicit circle(const double radius) : radius_{radius} {}

    double area() const;

private:
    double radius_;
};

double circle::area() const {
    return 3.14159 * radius_ * radius_;
}

namespace {

    /* --- `show_areas` --- Uses all three - and `cout`, which is defined somewhere else. */
    void show_areas() {
        print_function_header();

        const circle c{1.0};
        cout << " 1| square=" << area(2.0) << ", rectangle=" << area(2.0, 3.0) << ", circle=" << c.area() << '\n';
    }

}

/* --- The object file ---
 * What the program prints is not the point - three areas. The point is the object file the compiler makes of it.
 * Build this snippet. CMake compiles it into an object file before it links the executable:
 * `cmake-build-debug/0x04/CMakeFiles/c_symbols.dir/i_preparation/c_symbols.cpp.o` (`.obj` on Windows).
 * Or compile it yourself, in a terminal in this folder - `-c` means: compile only, do not link:
 * `g++ -std=c++23 -I ../../utils -c c_symbols.cpp` - and you get `c_symbols.o`.
 */

/* --- `nm` ---
 * `nm c_symbols.o` prints one line per symbol: an address in the object file, a letter, a name - in a strange form,
 * see below.
 * - `T` - defined here, in the code ("text") section: `area`, `circle::area`, `main`.
 * - `U` - undefined: used here, but defined somewhere else - `cout`, the `operator<<` for `double`. The linker has to
 *   find them, in another object file or in a library.
 * - `t` - defined here, but local: nobody outside this object file can call it - `show_areas`, in the unnamed
 *   namespace.
 * - `W` - you will also see some of these, e.g. for the constructor of `circle`, which is defined inside the class, and
 *   for `print_function_header`: see future snippets. (gcc lists the constructor twice - two names for one
 *   constructor, a detail of the platform's conventions.)
 * The list is long: most lines come from the standard library, pulled in by `cout` and `print_function_header`. Look
 * for the names you know, e.g. `nm c_symbols.o | grep -E 'area|show_areas|main|cout'` (on Windows: in Git Bash, or
 * `findstr area`). A Release build (`-O2`) has fewer lines - which of your functions are gone, and why?
 * `nm` is part of the compiler tools on Linux and macOS (Xcode Command Line Tools), and of CLion's bundled MinGW on
 * Windows. In the browser: Compiler Explorer (godbolt.org), with "Demangle identifiers" switched off in the output
 * options, shows the same names in the assembly.
 */

/* --- Mangled names ---
 * The names look strange: `_Z4aread` and `_Z4areadd`. That is name mangling: `_Z` starts a C++ name, `4` is the
 * length of `area`, and the rest are the parameter types - `d` for `double`. So the two overloads have two different
 * names, and the linker never has to know about overloading. That answers the teaser of the recap: the linker looks for
 * `_Z4areadd`, not for `area`.
 * - `_ZNK6circle4areaEv` - `N...E` a nested name `circle::area`, `K` for `const`, `v` for no parameters.
 * - On macOS every name gets one more `_` in front: `__Z4aread`.
 * - `nm -C c_symbols.o` shows the names demangled, `c++filt _Z4areadd` translates a single one.
 * - Look for the `U` of `cout` - what is it called?
 * - !![#symbols]
 */

/* -- .Q&A -- !![`area` returns a `double` - where is the return type in the name?](#a-401) */

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

/* --- `units::to_fahrenheit` ---
 * A function in a named namespace. Outside it, its full name is `units::to_fahrenheit` - another `to_fahrenheit` in
 * another namespace would be a different function, and nothing would clash. For the compiler, a namespace is just a
 * part of the name, as a class is for a member function: no object, no memory, no instruction.
 * - !![#namespace]
 */
namespace units {

    double to_fahrenheit(const double celsius) {
        return celsius * 9.0 / 5.0 + 32.0;
    }

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
        cout << " 3| largest in Fahrenheit=" << units::to_fahrenheit(largest(temperatures)) << '\n';
    }

}

/* --- The symbols ---
 * Build as Debug and list the symbols of
 * `cmake-build-debug/0x04/CMakeFiles/d_symbols_revisited.dir/iii_follow_up/tinker_symbols_revisited.cpp.o`, filtered for the
 * names of this program: `nm -C <file> | grep -E 'total|largest|midpoint| range::|units::|use_all'`. With gcc (clang
 * lists the same):
 * - `T total(...)` and `T range::contains(double) const` - defined here, once.
 * - `T units::to_fahrenheit(double)` - an ordinary function, its namespace in front. Without `-C`, the mangled name is
 *   `_ZN5units13to_fahrenheitEd`: `N...E` a nested name, as for `circle::area` in the preparation.
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
 * into `main`. `total`, `range::contains` and `units::to_fahrenheit` stay, although they were inlined as well: another
 * object file could call them, and the compiler of one file never knows.
 */

/* --- `main` --- */
int main() {
    show_areas();
    use_all_kinds();

    return EXIT_SUCCESS;
}
