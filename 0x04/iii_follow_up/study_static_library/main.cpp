// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A static library is an archive of object files: `libweather.a` holds `stats.o`, `report.o` and `convert.o`.
 *   CMake builds it with `add_library(... STATIC ...)`, a program uses it with `target_link_libraries`.
 * - The linker takes from the archive only the object files that define something the program still needs - an
 *   unused one is left out.
 * - A variable shared by several files: declared `extern` in the header, defined in exactly one `.cpp` file. Or
 *   `inline` in the header, as for functions.
 * - With the GNU linker, the order matters: first the files that need a symbol, then the library that defines it.
 */

#include "weather.hpp"                      // the library's header - all this file knows about the library

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `use_the_library` ---
     * Calls into the library as into any other file - the header declares, the linker connects. `values_seen` is the
     * library's variable, read here through its `extern` declaration. Predict it before you run: `report` calls `mean`
     * and `spread`, and `spread` calls `mean` again.
     */
    void use_the_library() {
        print_function_header();

        const vector<double> temperatures{18.5, 21.0, 19.5, 23.0};
        cout << " 1| " << weather::report(temperatures) << '\n';
        cout << " 2| values_seen=" << weather::values_seen << ", decimals=" << weather::decimals << '\n';
    }

}

/* --- Build ---
 * The `CMakeLists.txt` of this unit builds the library from the three `.cpp` files of this folder, then this program
 * from `main.cpp` alone, and links the two. The library file is in the build folder of the unit:
 * `cmake-build-debug/0x04/libweather.a` (MSVC: `weather.lib`). In the terminal, from this folder:
 * - `g++ -std=c++23 -c stats.cpp report.cpp convert.cpp` - three object files.
 * - `ar rcs libweather.a stats.o report.o convert.o` - `ar` packs them into one archive, `r` adds or replaces, `c`
 *   creates, `s` writes an index of the symbols.
 * - `g++ -std=c++23 -I ../../../utils main.cpp libweather.a -o weather` - compile `main.cpp` and link it with the
 *   library.
 * - !![#static-library]
 */

/* --- Look into the archive ---
 * `ar t libweather.a` lists the members: `stats.o`, `report.o`, `convert.o`. `nm -C libweather.a` lists the symbols per
 * member, with the letters you know (gcc on Linux; see previous snippets for macOS):
 * - `stats.o`: `T weather::mean`, `T weather::spread`, and `B weather::values_seen` - a variable defined here, set to
 *   0 when the program starts (`B`: the zero-initialized data).
 * - `report.o`: `T weather::report`, and `U weather::mean`, `U weather::spread` - the library needs itself.
 * - `convert.o`: `T weather::to_fahrenheit`.
 * - `weather::decimals`, the `inline` variable, in every object file that uses it: `u` with gcc, `V` with clang - both
 *   "one of these, the linker keeps one".
 * `main.o` has `U weather::report` and `U weather::values_seen`. The `[abi:cxx11]` behind `report` is a tag of
 * libstdc++: a function returning a `std::string`.
 */

/* --- What the linker takes ---
 * `nm -C` on the program: `mean`, `spread`, `report` and `values_seen` are there - but `to_fahrenheit` is not. The
 * linker goes through the archive and takes a member only if it defines a symbol that is still undefined. Nobody needs
 * `to_fahrenheit`, so `convert.o` stays in the archive. That is the difference to listing all `.cpp` files in
 * `add_executable`, as in unit 0x03: an object file given directly is always linked, whether it is used or not.
 */

/* --- The order ---
 * `g++ libweather.a main.o -o weather` fails with `undefined reference to 'weather::report...'`. The GNU linker (Linux,
 * MinGW) reads its inputs from left to right: when it sees the archive, nothing is undefined yet, so it takes nothing,
 * and it does not come back. Libraries go last - CMake does that for you.
 */

/* --- A dynamic library, in short ---
 * A static library is copied into the program at link time. A dynamic library - `.so` on Linux, `.dylib` on macOS,
 * `.dll` on Windows - stays a file of its own: the program only notes its name, and the system loads it when the
 * program starts. One copy on the disk for many programs, and an update without relinking - but the program does not
 * run without it. CMake: `add_library(... SHARED ...)`.
 */

/* --- `main` --- */
int main() {
    use_the_library();

    return EXIT_SUCCESS;
}
