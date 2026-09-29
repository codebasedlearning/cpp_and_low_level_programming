// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `throw`, `try` and `catch` - similar to Java.
 * - The standard exceptions, e.g. from `at()`.
 * - Rethrowing. What happens to the stack frames on the way comes later.
 */

#include <iostream>
#include <vector>
#include <stdexcept>                        // for the standard exceptions
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;
using std::runtime_error, std::out_of_range;


/* ---- Content ---- */

namespace {

    /* --- `catch_standard_exceptions` ---
     * `at()` checks the index and throws `std::out_of_range`. Catch exceptions by `const&` - no copy, as always.
     * - !![#exception]
     * - !![#std-exception]
     */
    void catch_standard_exceptions() {
        print_function_header();

        const vector<int> v{1, 2, 3};
        try {
            cout << " 1| v.at(1)=" << v.at(1) << '\n';
            const int x{v.at(5)};           // throws - the next line is never reached
            cout << " 2| v.at(5)=" << x << '\n';
        } catch (const out_of_range& e) {
            cout << " 3| out_of_range: " << e.what() << '\n';
        }
    }

    /* --- `load` --- A function that fails, with a message. */
    int load() {
        throw runtime_error{"file not found"};
    }

    /* --- `catch_in_order` ---
     * The `catch` blocks are tried from top to bottom; `...` catches everything.
     */
    void catch_in_order() {
        print_function_header();

        try {
            load();
        } catch (const out_of_range& e) {
            cout << " 1| out_of_range: " << e.what() << '\n';
        } catch (const runtime_error& e) {
            cout << " 2| runtime_error: " << e.what() << '\n';
        } catch (...) {
            cout << " 3| something else\n";
        }
    }

    /* --- `rethrow` ---
     * `throw;` inside a `catch` passes the same exception on - e.g. after logging it.
     */
    void rethrow() {
        print_function_header();

        try {
            try {
                load();
            } catch (const runtime_error& e) {
                cout << " 1| inner: " << e.what() << ", passing it on\n";
                throw;
            }
        } catch (const runtime_error& e) {
            cout << " 2| outer: " << e.what() << '\n';
        }
    }

}

/* --- `main` --- */
int main() {
    catch_standard_exceptions();
    catch_in_order();
    rethrow();

    return EXIT_SUCCESS;
}
