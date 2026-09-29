// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `nullptr`: a pointer that points to nothing - and that you can test for.
 * - A pointer can point somewhere else later - a reference cannot.
 * - A C array: a fixed number of elements of one type, one after the other - the ancestor of `std::array`.
 * - `swap` three times: by value, by reference, by pointer. Which ones work, and what does each of them get?
 */

#include <iostream>
#include <iterator>                         // for size
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::size;


/* ---- Content ---- */

namespace {

    /* --- `use_a_null_pointer` ---
     * A pointer does not have to point to an object. `nullptr` is the value for "nothing": it is not the address of any
     * object, so it can be tested. Before you use `*p` on a pointer that might be null, check it.
     * Dereferencing a null pointer is undefined behavior - usually the program crashes. See future snippets.
     * - !![#nullptr]
     */
    void use_a_null_pointer() {
        print_function_header();

        int n{23};
        int* p{nullptr};
        cout << " 1| p=" << p << ", is null: " << (p == nullptr) << '\n';

        p = &n;
        if (p != nullptr) {
            cout << " 2| p=" << p << ", *p=" << *p << '\n';
        }
    }

    /* --- `point_elsewhere` ---
     * Assigning to `p` changes where it points; assigning to `*p` changes the object it points to. A reference is bound
     * once and for all (see previous snippets) - a pointer can be pointed elsewhere as often as you like.
     * - !![#pointer]
     */
    void point_elsewhere() {
        print_function_header();

        int n{1};
        int m{2};
        int* p{&n};
        *p = 10;                            // writes to `n`
        p = &m;                             // now `p` points to `m`
        *p = 20;                            // writes to `m`
        cout << " 1| n=" << n << ", m=" << m << ", p=" << p << ", &m=" << &m << '\n';
    }

    /* --- `define_a_c_array` ---
     * `int a[4]` - four `int`s, one after the other, with no object around them. The count is part of the type and must
     * be known at compile time, as for `std::array<int, 4>`. With an initializer, the compiler can count for you.
     * - Access with `a[i]`, as for a `std::array`. There is no `at()` - and no check.
     * - `std::size(a)` gives the number of elements, `sizeof(a)` the number of bytes.
     * - A range-based `for` works as well.
     */
    void define_a_c_array() {
        print_function_header();

        int a[4]{10, 20, 30, 40};
        a[1] = 21;
        cout << " 1| a[0]=" << a[0] << ", a[1]=" << a[1] << ", size=" << size(a) << ", sizeof=" << sizeof(a) << '\n';

        const double d[]{1.5, 2.5, 3.5};    // the compiler counts: three elements
        cout << " 2| d:";
        for (const double x : d) {
            cout << ' ' << x;
        }
        cout << '\n';

        int b[4]{};                         // all zero
        // b = a;                           // compiler error: a C array cannot be assigned - `std::array` can
        cout << " 3| b[3]=" << b[3] << '\n';

        /* -- .C arrays in C++. --
         * C arrays are what `std::array`, `std::vector` and `std::string` are built on, and every C interface uses
         * them. In your own C++ code, `std::array` is the better choice: it can be copied, compared and passed like any
         * other value, and it knows its size everywhere. Why that last point matters: see future snippets.
         */
    }

    /* --- `swap_by_value` --- Gets copies of the two values. */
    void swap_by_value(int a, int b) {
        const int t{a};
        a = b;
        b = t;
    }

    /* --- `swap_by_reference` --- Gets two references - second names for the caller's variables. */
    void swap_by_reference(int& a, int& b) {
        const int t{a};
        a = b;
        b = t;
    }

    /* --- `swap_by_pointer` --- Gets two addresses. The caller passes them with `&`, the function writes with `*`. */
    void swap_by_pointer(int* a, int* b) {
        const int t{*a};
        *a = *b;
        *b = t;
    }

    /* --- `swap_three_ways` ---
     * The same three lines, three kinds of parameters. By value, the function swaps its own copies, and the caller's
     * variables stay as they were. By reference and by pointer, it swaps the caller's variables. The difference is
     * visible at the call: `&n` says "this may be changed" - a reference does not show it.
     * For real code there is `std::swap` from `<utility>`.
     * - !![#parameter-passing]
     */
    void swap_three_ways() {
        print_function_header();

        int n{1};
        int m{2};
        swap_by_value(n, m);
        cout << " 1| by value:     n=" << n << ", m=" << m << '\n';
        swap_by_reference(n, m);
        cout << " 2| by reference: n=" << n << ", m=" << m << '\n';
        swap_by_pointer(&n, &m);
        cout << " 3| by pointer:   n=" << n << ", m=" << m << '\n';

        /* -- .Q&A -- !![`swap_by_value` swapped something, too. What - and where is it now?](#a-501) */
    }

}

/* --- `main` --- */
int main() {
    use_a_null_pointer();
    point_elsewhere();
    define_a_c_array();
    swap_three_ways();

    return EXIT_SUCCESS;
}
