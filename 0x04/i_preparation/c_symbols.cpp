// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: `nm` lists the symbols of an object file - the names of the functions it defines, and of
 *   those it needs from elsewhere.
 * - Every function gets a machine name that encodes its parameter types: two overloads, two names.
 * - `nm -C` and `c++filt` translate the machine names back.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


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

/* --- `main` --- */
int main() {
    show_areas();

    return EXIT_SUCCESS;
}
