// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `const_iterator`: read, but not write - what `cbegin` and a `const` container give you.
 * - Reverse iterators: `rbegin` and `rend` walk backwards.
 * - `next`, `prev`, `distance`: moving around with any iterator.
 * - Iterator categories: what an iterator can do depends on how the container stores its elements.
 * - Erasing in a loop: `erase` returns the iterator to the next element.
 */

#include <iostream>
#include <vector>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector, std::list, std::unordered_set;
using std::sort, std::next, std::prev, std::distance;


/* ---- Content ---- */

namespace {

    /* --- `use_const_iterators` ---
     * `begin()` of a non-`const` vector returns an `iterator`: `*it` is an `int&`. `cbegin()` returns a
     * `const_iterator`: `*it` is a `const int&`. A `const` container returns `const_iterator`s from `begin()`, too.
     * Inside, both are the same address - the `const` exists only for the compiler.
     */
    void use_const_iterators() {
        print_function_header();

        vector<int> v{1, 2, 3};
        for (auto it{v.begin()}; it != v.end(); ++it) {
            *it *= 10;                      // allowed
        }
        for (auto it{v.cbegin()}; it != v.cend(); ++it) {
            // *it = 0;                     // compiler error: read-only
            cout << " 1|   " << *it << '\n';
        }
    }

    /* --- `walk_backwards` ---
     * `rbegin()` is the last element, `rend()` one before the first. `++` on a reverse iterator moves backwards.
     */
    void walk_backwards() {
        print_function_header();

        const vector<int> v{1, 2, 3, 4};
        cout << " 1| backwards:";
        for (auto it{v.rbegin()}; it != v.rend(); ++it) {
            cout << ' ' << *it;
        }
        cout << '\n';
    }

    /* --- `move_around` ---
     * `next(it, n)` and `prev(it, n)` return a moved copy, `distance(a, b)` counts the steps from `a` to `b`. They
     * work with every iterator - for a `vector` in one step, for a `list` by walking.
     */
    void move_around() {
        print_function_header();

        const list<int> l{10, 20, 30, 40, 50};
        const auto third{next(l.begin(), 2)};
        const auto last{prev(l.end())};
        cout << " 1| third=" << *third << ", last=" << *last << ", distance=" << distance(third, last) << '\n';
    }

    /* --- `know_the_categories` ---
     * What an iterator offers depends on the container:
     * - random access (`vector`, `array`, `deque`): `it + n`, `it[n]`, `it < other` - it is an address, or close to
     *   one.
     * - bidirectional (`list`, `set`, `map`): only `++` and `--` - one node at a time, in both directions.
     * - forward (`unordered_set`, `unordered_map`): only `++`.
     * `sort` needs random access. A `list` sorts itself, with a member function that relinks the nodes.
     * - !![#iterator]
     */
    void know_the_categories() {
        print_function_header();

        vector<int> v{3, 1, 2};
        sort(v.begin(), v.end());
        cout << " 1| v[0]=" << v[0] << ", *(v.begin() + 2)=" << *(v.begin() + 2) << '\n';

        list<int> l{3, 1, 2};
        // sort(l.begin(), l.end());        // compiler error, deep inside `sort`: no `it + n` for a list
        l.sort();
        cout << " 2| l.front()=" << l.front() << ", l.back()=" << l.back() << '\n';

        const unordered_set<int> u{1, 2, 3};
        auto it{u.begin()};
        ++it;                               // fine
        // --it;                            // compiler error: forward only
        cout << " 3| second element of u: " << *it << " - whichever that is\n";
    }

    /* --- `erase_in_a_loop` ---
     * After `erase(it)`, `it` is invalid - its element is gone. `erase` returns the iterator to the element after it,
     * so the loop continues there, and moves on with `++it` only if nothing was erased.
     * For the simple case, C++20 has a free function: `std::erase(v, value)`.
     */
    void erase_in_a_loop() {
        print_function_header();

        vector<int> v{1, 2, 3, 4, 5, 6};
        for (auto it{v.begin()}; it != v.end(); ) {
            if (*it % 2 == 0) {
                it = v.erase(it);
            } else {
                ++it;
            }
        }
        cout << " 1| odd: " << v[0] << ' ' << v[1] << ' ' << v[2] << ", size=" << v.size() << '\n';

        vector<int> w{1, 2, 1, 3};
        std::erase(w, 1);
        cout << " 2| without 1: " << w[0] << ' ' << w[1] << ", size=" << w.size() << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_const_iterators();
    walk_backwards();
    move_around();
    know_the_categories();
    erase_in_a_loop();

    return EXIT_SUCCESS;
}
