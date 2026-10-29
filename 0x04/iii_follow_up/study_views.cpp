// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A taster of C++20 ranges: views that filter and transform a container - chained with `|`.
 * - A view computes nothing in advance: each element is produced when the loop asks for it.
 * - A view is small - it holds where the data is, and what to do with it. And it owns nothing, as every view.
 * - The ranges algorithms take the container itself instead of two iterators: `ranges::find(v, 4)`.
 */

#include <iostream>
#include <vector>
#include <ranges>                           // for views
#include <algorithm>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;

namespace views = std::views;               // a shorter name for a namespace
namespace ranges = std::ranges;


/* ---- Content ---- */

namespace {

    /* --- `is_even` and `square` --- Two ordinary functions; `square` reports each call. */
    bool is_even(const int n) {
        return n % 2 == 0;
    }

    int square(const int n) {
        cout << " a|   square(" << n << ")\n";
        return n * n;
    }

    /* --- `chain_views` ---
     * `v | views::filter(is_even)` is a view of the even elements of `v`, `| views::transform(square)` a view of their
     * squares. Nothing is filtered or squared in this line - it only describes what to do.
     * - !![#ranges]
     */
    void chain_views() {
        print_function_header();

        const vector<int> v{1, 2, 3, 4, 5, 6};
        auto squares{v | views::filter(is_even) | views::transform(square)};  // not `const`: `filter` caches its begin
        cout << " 1| the view is ready, nothing computed yet\n";
        for (const int x : squares) {
            cout << " 2| " << x << '\n';
        }
    }

    /* --- `compute_on_demand` ---
     * `take(2)` stops after two elements - so `square` is called twice, not for all even numbers. A loop that stops
     * early costs nothing for the rest.
     */
    void compute_on_demand() {
        print_function_header();

        const vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
        for (const int x : v | views::filter(is_even) | views::transform(square) | views::take(2)) {
            cout << " 1| " << x << '\n';
        }
    }

    /* --- `show_the_size` ---
     * A view of the first two elements is a reference to the vector and a count - two words. The data stay where they
     * are: change the vector, and the view shows the change; destroy it, and the view dangles.
     */
    void show_the_size() {
        print_function_header();

        vector<int> v{1, 2, 3};
        const auto first_two{v | views::take(2)};
        v[0] = 100;
        cout << " 1| sizeof(first_two)=" << sizeof(first_two) << ", first element=" << *first_two.begin() << '\n';
    }

    /* --- `pass_the_container` ---
     * `ranges::sort(v)` and `ranges::find(v, 4)` are the algorithms of the session, taking the container instead of
     * `v.begin(), v.end()`. Inside, they do the same - and they still return iterators.
     */
    void pass_the_container() {
        print_function_header();

        vector<int> v{5, 3, 4, 1};
        ranges::sort(v);
        const auto it{ranges::find(v, 4)};
        cout << " 1| v[0]=" << v[0] << ", 4 found: " << (it != v.end()) << ", at index " << (it - v.begin()) << '\n';
    }

}

/* --- `main` --- */
int main() {
    chain_views();
    compute_on_demand();
    show_the_size();
    pass_the_container();

    return EXIT_SUCCESS;
}
