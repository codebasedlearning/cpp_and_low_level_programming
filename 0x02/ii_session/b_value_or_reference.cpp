// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Call by value copies the argument into the stack frame of the function.
 * - Call by reference passes the object itself - no copy.
 * - `const&`: no copy and no change - the default for large read-only parameters.
 * - The same question in a range-based `for`.
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::string, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `entry` --- A small struct to pass around. */
    struct entry {
        int key;
        int value;
    };

    /* --- `set_by_value` --- `e` is a copy of the argument, in this function's frame. */
    void set_by_value(entry e) {
        e.key = 0;
        cout << " a|   set_by_value:           &e=" << &e << ", e.key=" << e.key << '\n';
    }

    /* --- `set_by_reference` ---
     * `e` is the argument itself. Under the hood, the address of the argument is passed - how references are
     * implemented: see previous snippets.
     */
    void set_by_reference(entry& e) {
        e.key = 0;
        cout << " c|   set_by_reference:       &e=" << &e << ", e.key=" << e.key << '\n';
    }

    /* --- `print_by_const_reference` --- No copy, and no change. */
    void print_by_const_reference(const entry& e) {
        cout << " b|   print_by_const_reference: &e=" << &e << ", e.key=" << e.key << '\n';
        // e.key = 0;                       // compiler error
    }

    /* --- `compare_value_and_reference` ---
     * Compare the addresses: by value, the parameter lives somewhere else - it is a copy. By reference, it has the
     * address of the argument.
     * - !![#parameter-passing]
     */
    void compare_value_and_reference() {
        print_function_header();

        entry e{1, 2};
        cout << " 1| caller: &e=" << &e << ", e.key=" << e.key << '\n';

        set_by_value(e);
        cout << " 2| after set_by_value:     e.key=" << e.key << '\n';

        print_by_const_reference(e);

        set_by_reference(e);
        cout << " 3| after set_by_reference: e.key=" << e.key << '\n';
    }

    /* --- `sum_by_value` --- The whole vector is copied for every call. */
    long long sum_by_value(const vector<int> v) {
        long long sum{0};
        for (const int x : v) {
            sum += x;
        }
        return sum;
    }

    /* --- `sum_by_const_reference` --- Same work, no copy. */
    long long sum_by_const_reference(const vector<int>& v) {
        long long sum{0};
        for (const int x : v) {
            sum += x;
        }
        return sum;
    }

    /* --- `measure_a_copy` ---
     * For an `int`, a copy costs nothing worth mentioning. For a `vector` with ten million `int`s, every call by value
     * copies 40 MB.
     * Measure it as Release - how: see previous snippets.
     */
    void measure_a_copy() {
        print_function_header();

        const vector<int> v(10'000'000, 1);

        stopwatch watch{};
        long long sum{0};
        for (int i{0}; i < 10; ++i) {
            sum += sum_by_value(v);
        }
        cout << " 1| by value:           sum=" << sum << ", " << watch.elapsed_ms() << " ms\n";

        watch.reset();
        sum = 0;
        for (int i{0}; i < 10; ++i) {
            sum += sum_by_const_reference(v);
        }
        cout << " 2| by const reference: sum=" << sum << ", " << watch.elapsed_ms() << " ms\n";

        /* -- .Q&A -- !![So should every parameter be a `const&`?](#a-203) */
    }

    /* --- `copy_in_loops` ---
     * `for (auto name : names)` copies every element into `name`. With `const auto&`, `name` refers to the element in
     * the vector instead.
     * `auto` lets the compiler deduce the type - here `string`.
     * - !![#range-based-for]
     */
    void copy_in_loops() {
        print_function_header();

        const vector<string> names{"Ada Lovelace, Countess of Lovelace",
                                   "Grace Brewster Murray Hopper",
                                   "Barbara Liskov, Turing Award 2008"};

        for (auto name : names) {
            cout << " 1| copy:      &name=" << &name << ", " << name << '\n';
        }
        for (const auto& name : names) {
            cout << " 2| reference: &name=" << &name << ", " << name << '\n';
        }

        /* -- .Compare the addresses. --
         * - With a copy, `name` is always the same local variable - same address, new content in every round. For long
         *   strings, every round allocates.
         * - With a reference, `name` is each element in turn - the addresses step through the vector, `sizeof(string)`
         *   apart.
         */
    }

}

/* --- `main` --- */
int main() {
    compare_value_and_reference();
    measure_a_copy();
    copy_in_loops();

    return EXIT_SUCCESS;
}
