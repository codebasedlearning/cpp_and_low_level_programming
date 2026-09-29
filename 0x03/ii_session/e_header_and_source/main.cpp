// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A class in two files: the header `temperature.hpp` declares it, the source file `temperature.cpp` defines its
 *   member functions. That is how classes are written in real projects - the snippets keep them in one file, so that
 *   everything is in one place.
 * - Every `.cpp` file is compiled on its own - a translation unit. The linker joins them into one program.
 * - One executable, several sources: all `.cpp` files go into the `add_executable` of the program.
 * - The typical linker errors: `undefined reference` and `multiple definition` - and how to read them.
 */

#include "temperature.hpp"                  // the declarations are all `main.cpp` gets to see

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::invalid_argument;


/* ---- Content ---- */

namespace {

    /* --- `use_the_class` ---
     * Nothing new in the use of the class. The compiler of this file knows the member functions only by their
     * declarations - it generates calls to them and leaves the addresses open. The linker fills them in.
     * - !![#translation-unit]
     */
    void use_the_class() {
        print_function_header();

        temperature t{20.0};
        t.warm_up(1.5);
        cout << " 1| " << t.celsius() << " C = " << t.fahrenheit() << " F, sizeof(t)=" << sizeof(t) << '\n';
    }

    /* --- `try_absolute_zero` --- The constructor protects the invariant - the object is never born. */
    void try_absolute_zero() {
        print_function_header();

        try {
            const temperature t{-300.0};
            cout << " 1| never printed\n";
        } catch (const invalid_argument& e) {
            cout << " 2| caught: " << e.what() << '\n';
        }

        /* -- .The unnamed namespace, again. --
         * The functions of this file are in an unnamed namespace, since unit 0x01: they are visible only in this
         * translation unit. So another `.cpp` file may have its own `use_the_class` - no conflict. `temperature` is
         * outside of it, because other files need it.
         * - !![#unnamed-namespace]
         */
    }

}

/* --- Build ---
 * The `CMakeLists.txt` of this unit lists both `.cpp` files for this executable - the header is not compiled on its
 * own, it is pasted into both. In the terminal, from this folder:
 * `g++ -std=c++23 -I ../../../utils main.cpp temperature.cpp -o temperature`
 * - Or in two steps: `g++ -std=c++23 -I ../../../utils -c main.cpp` and `g++ -std=c++23 -c temperature.cpp` compile
 *   each file into an object file, `g++ main.o temperature.o -o temperature` links them.
 */

/* --- Linker errors ---
 * Provoke them - once you have seen them on purpose, you recognize them. Undo each change afterwards.
 * - !![#linker-errors]
 */

/* -- .1. A missing source file. --
 * Remove `temperature.cpp` from the `add_executable` in `CMakeLists.txt` (reload CMake), or from the `g++` line. Every
 * file compiles - but the linker finds no definition for the member functions `main.cpp` calls:
 * `undefined reference to 'temperature::temperature(double)'` (gcc), `Undefined symbols for architecture arm64:
 * "temperature::temperature(double)", referenced from: ...` (Apple clang). The message names the function, and the
 * object file that needs it.
 */

/* -- .2. A declaration without a definition. --
 * Declare a member function `double kelvin() const;` in the header, and call it in `use_the_class`, but define it
 * nowhere. The same `undefined reference` - the compiler trusted the declaration, the linker cannot find the body.
 * As long as nobody calls it, nobody notices.
 */

/* -- .3. A definition in the header. --
 * Add a free function with a body to the header, below the class:
 * `double to_kelvin(const double celsius) { return celsius + 273.15; }`
 * Both `.cpp` files include the header, so both object files contain a `to_kelvin`: `multiple definition of
 * 'to_kelvin(double)'` (gcc), `duplicate symbol` (Apple clang). There are two fixes: declare it in the header and
 * define it in the `.cpp` - or mark it `inline`, which says "the same definition may appear in several translation
 * units", like the helpers in `utils/cbl`. Member functions defined inside the class body are `inline` without you
 * writing it.
 */

/* -- .Q&A -- !![The class itself is defined in both translation units, too - why is that no error?](#a-305) */

/* --- `main` --- */
int main() {
    use_the_class();
    try_absolute_zero();

    return EXIT_SUCCESS;
}
