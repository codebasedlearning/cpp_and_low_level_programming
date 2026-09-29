// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: measuring how long something takes.
 * - Debug and Release builds can differ a lot.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>                // for `stopwatch`

using std::cout, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `measure_a_loop` ---
     * `stopwatch` starts when it is created; `elapsed_ms()` gives the time since then. Treat it as a black box for now,
     * like `print_function_header`.
     */
    void measure_a_loop() {
        print_function_header();

        // 10 million times the value 1. Note the `()`: with `{}` this would be a vector with the two elements
        // 10'000'000 and 1 - the one exception to our braces rule.
        const vector<int> v(10'000'000, 1);

        stopwatch watch{};
        long long sum{0};
        for (int round{0}; round < 10; ++round) {
            for (size_t i{0}; i < v.size(); ++i) {
                sum += v[i];
            }
        }
        const double ms{watch.elapsed_ms()};

        // Print the result - otherwise the optimizer may drop the whole loop.
        cout << " 1| sum=" << sum << ", took " << ms << " ms\n";

        /* -- .Debug vs. Release. --
         * Run it as Debug (`-O0`) and as Release (`-O2`) and compare the times.
         * From now on: measure with Release, otherwise you measure the missing optimization and not your code.
         */
    }

}

/* --- `main` --- */
int main() {
    measure_a_loop();

    return EXIT_SUCCESS;
}
