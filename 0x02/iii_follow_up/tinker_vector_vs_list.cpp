// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::list` stores every element in its own heap block, linked to the next.
 * - The same work on a `vector` and a `list` - and why the `vector` wins.
 */

#include <iostream>
#include <vector>
#include <list>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::vector, std::list;


/* ---- Content ---- */

namespace {

    /* --- `show_where_the_elements_are` ---
     * In a `vector` the elements lie next to each other; in a `list` each one is somewhere on the heap, with the links
     * to its neighbors.
     */
    void show_where_the_elements_are() {
        print_function_header();

        const vector<int> v{1, 2, 3};
        const list<int> l{1, 2, 3};
        for (const auto& x : v) {
            cout << " 1| vector element at " << &x << '\n';
        }
        for (const auto& x : l) {
            cout << " 2| list element at   " << &x << '\n';
        }
    }

    /* --- `sum_them_up` ---
     * Summing is the same work for both. Measure it as Release.
     * - !![#containers]
     */
    void sum_them_up() {
        print_function_header();

        constexpr int n{5'000'000};
        const vector<int> v(n, 1);
        const list<int> l(n, 1);

        stopwatch watch{};
        long long sum{0};
        for (const auto& x : v) {
            sum += x;
        }
        cout << " 1| vector: sum=" << sum << ", " << watch.elapsed_ms() << " ms\n";

        watch.reset();
        sum = 0;
        for (const auto& x : l) {
            sum += x;
        }
        cout << " 2| list:   sum=" << sum << ", " << watch.elapsed_ms() << " ms\n";
    }

}

/* --- `main` --- */
int main() {
    show_where_the_elements_are();
    sum_them_up();

    return EXIT_SUCCESS;
}
