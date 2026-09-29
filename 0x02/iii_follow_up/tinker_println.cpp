// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::format` (C++20) and `std::println` (C++23): formatting with `{}`.
 * - The format string is checked at compile time - unlike C's `printf`.
 * - A feature-test macro: the preprocessor asks what the library supports.
 */

#include <iostream>
#include <version>                          // defines the feature-test macros
#include <cstdlib>
#include <cbl/printing.hpp>

// Include only what this library version has - older ones lack <format> or <print>.
#if defined(__cpp_lib_format)
#include <format>
#endif
#if defined(__cpp_lib_print)
#include <print>
#endif

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `use_format` --- `{}` is replaced by the next argument. - !![#format] */
    void use_format() {
        print_function_header();

#if defined(__cpp_lib_format)
        const int n{23};
        cout << std::format(" 1| n={}, n*n={}, pi={:.3f}\n", n, n * n, 3.14159);

        // cout << std::format(" 2| {} {}\n", n);  // compiler error: too few arguments
#else
        cout << " 1| `std::format` is not available in this library version\n";
#endif
    }

    /* --- `use_println` --- The same, without `cout`, if your library has it. */
    void use_println() {
        print_function_header();

#if defined(__cpp_lib_print)
        std::println(" 1| println: {} and {}", 1, 2);
#else
        cout << " 1| `std::println` is not available in this library version\n";
#endif
    }

}

/* --- `main` --- */
int main() {
    use_format();
    use_println();

    return EXIT_SUCCESS;
}
