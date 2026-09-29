// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x02 - the things to remember.
 */

#include <iostream>
#include <array>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::array, std::vector, std::runtime_error;


/* ---- Content ---- */

namespace {

    /* --- `recall_references` ---
     * A reference is a second name for an existing object - same object, same address, no copy. Where it must exist at
     * runtime, as a parameter or a member, the compiler usually stores an address.
     */
    void recall_references() {
        print_function_header();

        int n{1};
        int& m{n};
        m = 2;                              // changes `n`
        cout << " 1| n=" << n << ", &n=" << &n << ", &m=" << &m << '\n';
    }

    /* --- `sum_all` --- `const&`: no copy and no change - the default for anything bigger that is only read. */
    long long sum_all(const vector<int>& v) {
        long long sum{0};
        for (const int x : v) {
            sum += x;
        }
        return sum;
    }

    /* --- `recall_value_or_reference` ---
     * By value copies the argument into the stack frame of the function - cheap for an `int`, expensive for a `vector`
     * with a million elements. Measure it, as Release.
     */
    void recall_value_or_reference() {
        print_function_header();

        const vector<int> v(1'000'000, 1);
        cout << " 1| sum=" << sum_all(v) << " - and not a single element copied\n";
    }

    /* --- `recall_where_elements_live` ---
     * A `std::array` is its elements. A `std::vector` is a small object - where, how many, how many fit - and its
     * elements are on the heap. When the capacity is full, `push_back` moves them all, and every reference into the old
     * block dangles.
     */
    void recall_where_elements_live() {
        print_function_header();

        const array<int, 3> a{1, 2, 3};
        vector<int> v{1, 2, 3};
        cout << " 1| sizeof(a)=" << sizeof(a) << ", sizeof(v)=" << sizeof(v) << '\n';
        cout << " 2| &v=" << &v << ", elements at " << v.data() << ", capacity=" << v.capacity() << '\n';
        v.push_back(4);
        cout << " 3| after push_back: elements at " << v.data() << ", capacity=" << v.capacity() << '\n';
    }

    /* --- `wasteful` --- 10 bytes of data. */
    struct wasteful {
        char a;
        double b;
        char c;
    };

    /* --- `recall_padding` ---
     * A `struct` is its members in declaration order, each aligned - plus the gaps in between. Order them from large to
     * small.
     */
    void recall_padding() {
        print_function_header();

        cout << " 1| sizeof(wasteful)=" << sizeof(wasteful) << " for 10 bytes of data\n";
    }

    /* --- `stack` ---
     * The stack from 'Harshire': the data in a `struct`, the functions outside. Every function needs the stack as a
     * parameter, by reference, because it changes it.
     */
    struct stack {
        array<int, 3> data{};
        size_t next{0};
    };

    /* --- `push` --- Throws if the stack is full - an error the caller cannot ignore. */
    void push(stack& s, const int value) {
        if (s.next >= s.data.size()) {
            throw runtime_error{"stack is full"};
        }
        s.data[s.next] = value;
        ++s.next;
    }

    /* --- `recall_functions_on_a_struct` ---
     * Nothing stops anyone from writing `s.next = 42;` - the struct cannot protect itself.
     */
    void recall_functions_on_a_struct() {
        print_function_header();

        stack s{};
        push(s, 1);
        push(s, 2);
        cout << " 1| s.next=" << s.next << ", top=" << s.data[s.next - 1] << '\n';

        /* -- .Teaser. --
         * In this unit, `push` moves into the type, and the parameter `s` disappears from the parameter list - but not
         * from the machine code.
         */
    }

}

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`) - the same code can behave differently.
 * - The debugger: breakpoints, the frames list and the memory view.
 * - `g++ -E` (after the preprocessor) and `g++ -S` (assembly), or Compiler Explorer (godbolt.org).
 * - `echo $?` - the exit status of the last program.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 */

/* --- `main` --- */
int main() {
    recall_references();
    recall_value_or_reference();
    recall_where_elements_live();
    recall_padding();
    recall_functions_on_a_struct();

    return EXIT_SUCCESS;
}
