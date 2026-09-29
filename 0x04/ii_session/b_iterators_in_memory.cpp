// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An iterator marks a position in a container: `begin()` the first element, `end()` one past the last.
 * - `*it` is the element, `++it` moves to the next one - the same for a `vector`, a `list` and a `set`.
 * - `find` searches between two iterators and returns the second one for "not found".
 * - Inside, an iterator into a `vector` is essentially an address: `++it` adds the size of one element.
 * - The range-based `for` is an iterator loop in disguise.
 * - An iterator into a `list` has the same size, but `++it` follows a pointer from node to node.
 * - Two iterators are a view, too: where it starts, where it ends. The algorithms know nothing else.
 * - An iterator is only valid as long as the element it points to stays where it is.
 */

#include <iostream>
#include <vector>
#include <list>
#include <span>
#include <set>                              // for set
#include <algorithm>                        // for find, sort
#include <iterator>                         // for next
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector, std::list, std::set, std::span;
using std::find, std::sort, std::next;


/* ---- Content ---- */

namespace {

    /* --- `walk_with_an_iterator` ---
     * A range-based `for` without the sugar. `it` starts at `begin()` and moves on with `++it` until it reaches
     * `end()`, which is not an element - it marks the end. The same loop works for a `set`, which keeps its elements
     * sorted, each only once.
     * - !![#iterator]
     */
    void walk_with_an_iterator() {
        print_function_header();

        const vector<int> v{7, 3, 5, 3};
        cout << " 1| v:";
        for (auto it{v.begin()}; it != v.end(); ++it) {        // `auto`: the type is `vector<int>::const_iterator`
            cout << ' ' << *it;
        }
        cout << '\n';

        const set<int> s{7, 3, 5, 3};
        cout << " 2| s:";
        for (auto it{s.begin()}; it != s.end(); ++it) {
            cout << ' ' << *it;
        }
        cout << '\n';
    }

    /* --- `find_an_element` ---
     * `find` gets two iterators - where to start, where to stop - and returns an iterator to the element, or the
     * second one if there is none.
     */
    void find_an_element() {
        print_function_header();

        const vector<int> v{2, 3, 5, 7};
        if (const auto it{find(v.begin(), v.end(), 5)}; it != v.end()) {
            cout << " 1| found " << *it << ", the next one is " << *(it + 1) << '\n';
        }
        if (find(v.begin(), v.end(), 4) == v.end()) {
            cout << " 2| 4 not found\n";
        }
    }

    /* --- `show_an_iterator` ---
     * `&*it` is the address of the element the iterator points to. It starts at `v.data()`, and each `++it` adds
     * `sizeof(int)` bytes - the next element, right behind. `sizeof(it)` is one word: an address.
     * In the library, the iterator is a small class around a pointer; with `-O2` nothing of the class is left.
     */
    void show_an_iterator() {
        print_function_header();

        const vector<int> v{10, 20, 30, 40};
        cout << " 1| sizeof(iterator)=" << sizeof(v.begin()) << ", v.data()=" << v.data() << '\n';
        for (auto it{v.begin()}; it != v.end(); ++it) {
            cout << " 2|   &*it=" << &*it << ", *it=" << *it << '\n';
        }
        // cout << *v.end();                // undefined behavior: `end()` is one past the last element - no element

        /* -- .Memory view. --
         * Set a breakpoint in the loop and look at `&it` in the memory view: 8 bytes, the address of the current
         * element. Step, and it grows by 4.
         */
    }

    /* --- `rewrite_range_for` ---
     * The compiler rewrites the range-based `for` into an iterator loop - roughly the second loop. Note that `end()` is
     * called only once, before the loop starts.
     * cppinsights.io shows the rewritten code. In Compiler Explorer with `-O2`, both loops are the same instructions:
     * a register holds the address of the element, `add ..., 4` moves it on - the iterator is gone, the address is
     * left. A loop with an index is almost the same; it computes `data + 4 * i` instead.
     * - !![#range-based-for]
     */
    void rewrite_range_for() {
        print_function_header();

        const vector<int> v{10, 20, 30, 40};
        int sum{0};
        for (const int& x : v) {
            sum += x;
        }
        cout << " 1| sum=" << sum << '\n';

        sum = 0;
        for (auto it{v.begin()}, end{v.end()}; it != end; ++it) {
            const int& x{*it};
            sum += x;
        }
        cout << " 2| sum=" << sum << '\n';
    }

    /* --- `show_a_list_iterator` ---
     * A `list` stores each element in a node of its own, allocated on its own, with the address of the next and the
     * previous node. The iterator is one word again - the address of a node - but `++it` cannot add 4: it loads the
     * address of the next node. The addresses of the elements are wherever the allocator put the nodes.
     * `it + 1` does not compile for a `list`: jumping costs a walk, so there is only `++` - or `next(it, n)`, which
     * walks for you.
     */
    void show_a_list_iterator() {
        print_function_header();

        const list<int> l{10, 20, 30, 40};
        cout << " 1| sizeof(iterator)=" << sizeof(l.begin()) << '\n';
        for (auto it{l.begin()}; it != l.end(); ++it) {
            cout << " 2|   &*it=" << &*it << ", *it=" << *it << '\n';
        }
        cout << " 3| the third element: " << *next(l.begin(), 2) << '\n';

        /* -- .Memory view. --
         * With gcc's libstdc++, a node is two addresses followed by the value. Look at the 16 bytes before `&*it`:
         * the address of the next node and of the previous one. Where does the last node point to?
         */

        /* -- .Q&A -- !![Both are 8 bytes. How does `++it` know whether to add 4 or to follow a pointer?](#a-404) */
    }

    /* --- `use_two_iterators` ---
     * An algorithm gets two iterators - where to start, where to stop - and never sees the container. So it works on a
     * part as well as on the whole: `sort` of the first three, `find` in the middle.
     * A pair of iterators is a view, like a `span`, and a `span` can be built from one.
     * - !![#algorithms]
     */
    void use_two_iterators() {
        print_function_header();

        vector<int> v{40, 10, 30, 20, 50};
        sort(v.begin(), v.begin() + 3);     // only the first three
        cout << " 1| v=" << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << ' ' << v[4] << '\n';

        const auto middle_begin{v.begin() + 1};
        const auto middle_end{v.end() - 1};
        const bool found{find(middle_begin, middle_end, 50) != middle_end};
        cout << " 2| 50 in the middle: " << found << '\n';

        const span<const int> middle{middle_begin, middle_end};
        cout << " 3| middle.size()=" << middle.size() << ", middle.data()=" << middle.data()
             << ", &*middle_begin=" << &*middle_begin << '\n';
    }

    /* --- `invalidate_an_iterator` ---
     * When a `vector` grows beyond its capacity, it moves its elements (see previous snippets), and every iterator into
     * the old block dangles - it is the old address. A `list` never moves a node: an iterator into it stays valid when
     * other elements are added or removed.
     * - !![#dangling-pointer]
     */
    void invalidate_an_iterator() {
        print_function_header();

        vector<int> v{1, 2, 3};
        const auto first{v.begin()};
        cout << " 1| vector: &*first=" << &*first << ", capacity=" << v.capacity() << '\n';
        v.push_back(4);
        cout << " 2| after push_back: v.data()=" << v.data() << '\n';
        // cout << *first;                  // undefined behavior: `first` points into the released block

        list<int> l{1, 2, 3};
        const auto head{l.begin()};
        l.push_back(4);
        l.push_front(0);
        cout << " 3| list: *head=" << *head << " - still valid\n";
    }

}

/* --- `main` --- */
int main() {
    walk_with_an_iterator();
    find_an_element();
    show_an_iterator();
    rewrite_range_for();
    show_a_list_iterator();
    use_two_iterators();
    invalidate_an_iterator();

    return EXIT_SUCCESS;
}
