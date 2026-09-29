// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The containers besides `array` and `vector`: `list`, `deque`, `set`, `map`, `unordered_set`, `unordered_map`.
 * - The same everyday operations for all of them: create, insert, find, erase, iterate.
 * - `map` and `unordered_map`: `[]` inserts a missing key, `find` and `contains` do not.
 * - `pair` and structured bindings.
 * - A member function `find` knows how its container is built - `std::find` only knows two iterators.
 * - What they cost: except for `vector`, `array` and (mostly) `deque`, every element lives in a node of its own.
 */

#include <iostream>
#include <string>
#include <list>
#include <deque>                            // for deque
#include <set>
#include <map>                              // for map
#include <unordered_set>                    // for unordered_set
#include <unordered_map>                    // for unordered_map
#include <utility>                          // for pair
#include <algorithm>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::string;
using std::list, std::deque, std::set, std::map, std::unordered_set, std::unordered_map;
using std::pair, std::find;


/* ---- Content ---- */

namespace {

    /* --- `print_all` --- From the session: works for every container with printable elements. */
    template <typename T>
    void print_all(const T& container) {
        for (const auto& x : container) {
            cout << ' ' << x;
        }
        cout << '\n';
    }

    /* --- `use_a_list` ---
     * A doubly linked list: inserting and erasing anywhere is cheap, once you have an iterator to the place - no
     * element moves. There is no `[]`: to reach the fifth element, you walk.
     * - !![#containers]
     */
    void use_a_list() {
        print_function_header();

        list<int> l{2, 3, 5};
        l.push_front(1);
        l.push_back(8);
        auto it{l.begin()};
        ++it;
        ++it;                               // at the 3
        l.insert(it, 42);                   // before the 3
        cout << " 1| l:"; print_all(l);

        l.erase(it);                        // the 3 - the other iterators stay valid
        l.remove(42);                       // all elements with the value 42
        cout << " 2| l:"; print_all(l);
    }

    /* --- `use_a_deque` ---
     * A double-ended queue: `push_front` and `push_back` are cheap, and `[]` works. The elements are in blocks of
     * fixed size, not in one piece like in a `vector`.
     */
    void use_a_deque() {
        print_function_header();

        deque<int> d{2, 3};
        d.push_front(1);
        d.push_back(4);
        d.pop_front();
        cout << " 1| d:"; print_all(d);
        cout << " 2| d[1]=" << d[1] << ", front=" << d.front() << ", back=" << d.back() << '\n';
    }

    /* --- `use_a_set` ---
     * Each value at most once, always sorted - a balanced tree. `insert` of a value that is there already does
     * nothing. Finding is fast: O(log n), a few steps down the tree.
     * - !![#associative-containers]
     */
    void use_a_set() {
        print_function_header();

        set<string> tags{"jazz", "modal", "1959"};
        tags.insert("jazz");                // already there - nothing happens
        tags.insert("trumpet");
        cout << " 1| tags:"; print_all(tags);
        cout << " 2| contains modal: " << tags.contains("modal") << ", size=" << tags.size() << '\n';

        tags.erase("1959");
        if (const auto it{tags.find("jazz")}; it != tags.end()) {
            cout << " 3| found " << *it << '\n';
        }
    }

    /* --- `use_a_map` ---
     * Keys with values, sorted by key. `m[key]` returns the value - and if the key is missing, it inserts it first,
     * with a default value. For reading, use `find`, `contains` or `at` (which throws).
     * The elements are `pair<const string, int>`; the structured binding `[name, year]` names both parts.
     * - !![#structured-bindings]
     */
    void use_a_map() {
        print_function_header();

        map<string, int> recorded{{"So What", 1959}, {"Blue in Green", 1959}};
        recorded["Naima"] = 1959;           // insert
        recorded["So What"] = 1958;         // overwrite
        for (const auto& [name, year] : recorded) {
            cout << " 1|   " << name << ": " << year << '\n';
        }

        cout << " 2| size=" << recorded.size() << ", Giant Steps? " << recorded.contains("Giant Steps") << '\n';
        cout << " 3| recorded[\"Giant Steps\"]=" << recorded["Giant Steps"] << '\n';   // inserts it, with 0
        cout << " 4| size=" << recorded.size() << " - one more\n";
        recorded.erase("Giant Steps");
    }

    /* --- `use_unordered_containers` ---
     * The same interface, but hashed instead of sorted: the elements are in buckets, chosen by a hash of the key.
     * Finding is O(1) on average - and the order is whatever the hashes make of it.
     */
    void use_unordered_containers() {
        print_function_header();

        const unordered_set<int> primes{2, 3, 5, 7, 11, 13};
        cout << " 1| primes:"; print_all(primes);

        unordered_map<string, string> instrument{{"Miles", "trumpet"}, {"Bill", "piano"}};
        instrument["John"] = "saxophone";
        if (const auto it{instrument.find("Bill")}; it != instrument.end()) {
            cout << " 2| " << it->first << " plays " << it->second << '\n';    // `it->first` is `(*it).first`
        }
    }

    /* --- `use_a_pair` ---
     * Two values of any types, `first` and `second` - e.g. the elements of a `map`, or two results of one function.
     * A `struct` with good names is often the better choice.
     * - !![#pair-tuple]
     */
    pair<int, int> min_and_max(const set<int>& values) {
        return {*values.begin(), *values.rbegin()};         // a `set` is sorted
    }

    void use_a_pair() {
        print_function_header();

        const set<int> values{7, 3, 9, 1};
        const auto [smallest, largest]{min_and_max(values)};
        cout << " 1| smallest=" << smallest << ", largest=" << largest << '\n';
    }

    /* --- `compare_find` ---
     * `std::find` gets two iterators and walks from one to the other - for a `set` node by node, O(n). It cannot know
     * that the elements are in a sorted tree. `s.find(x)` can: it goes down the tree, about 20 steps for a million
     * elements, O(log n). Rule: if a container has a member function for a job, use it. Measure it, as Release.
     */
    void compare_find() {
        print_function_header();

        set<int> s{};
        for (int i{0}; i < 1'000'000; ++i) {
            s.insert(i);
        }

        stopwatch watch{};
        const bool walked{find(s.begin(), s.end(), 999'999) != s.end()};
        const double walk_ms{watch.elapsed_ms()};
        watch.reset();
        const bool descended{s.find(999'999) != s.end()};
        const double descend_ms{watch.elapsed_ms()};
        cout << " 1| std::find: " << walked << " after " << walk_ms << " ms\n";
        cout << " 2| s.find:    " << descended << " after " << descend_ms << " ms\n";
    }

    /* --- `show_what_it_costs` ---
     * The container objects are small - some addresses and a size. The elements are elsewhere: in a `list`, `set`,
     * `map` and the unordered ones, each element in a node of its own, allocated on its own, with one to three
     * addresses next to it. Print the addresses of the elements of a `set<int>`, as for the `list` in the session.
     * The numbers depend on the library - compare them with a friend's.
     */
    void show_what_it_costs() {
        print_function_header();

        cout << " 1| sizeof: list=" << sizeof(list<int>) << ", deque=" << sizeof(deque<int>)
             << ", set=" << sizeof(set<int>) << ", map=" << sizeof(map<int, int>)
             << ", unordered_map=" << sizeof(unordered_map<int, int>) << '\n';

        const set<int> s{1, 2, 3};
        for (const int& x : s) {
            cout << " 2|   &x=" << &x << '\n';
        }
    }

}

/* --- Which one? ---
 * - `vector` - the default. Contiguous, cache-friendly, fast to iterate - even where the others promise better O().
 * - `array` - the size is known at compile time.
 * - `deque` - a queue: cheap at both ends.
 * - `list` - many inserts and erases in the middle, and iterators that must stay valid.
 * - `unordered_map` / `unordered_set` - look up by key, order does not matter.
 * - `map` / `set` - look up by key, and the order matters, e.g. to print sorted.
 */

/* --- `main` --- */
int main() {
    use_a_list();
    use_a_deque();
    use_a_set();
    use_a_map();
    use_unordered_containers();
    use_a_pair();
    compare_find();
    show_what_it_costs();

    return EXIT_SUCCESS;
}
