// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A C array is its elements, one after the other: `sizeof` is the count times the size of one element.
 * - `p + 1` is the next element, not the next byte: the compiler scales by `sizeof(T)`.
 * - `p[i]` is `*(p + i)` - indexing is pointer arithmetic.
 * - The difference of two pointers counts elements, not bytes.
 * - One past the last element is a valid address, but not an element - the `end()` of a C array.
 * - The iterators of a C array are plain pointers, and the algorithms take them.
 * - An index loop, a pointer loop and a range-based `for` become the same machine code.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `swap_by_value` did not change the caller's variables. What did the other two versions get instead of the values?
 * - `int a[4]{10, 20, 30, 40};` - what are `std::size(a)` and `sizeof(a)`? And what does the compiler say to `b = a;`?
 * - In the assembly of `square`: in which register does the argument arrive on x86-64, where does the result leave?
 * - As Release, `sum_of_squares` has no `call` any more. Why not?
 */

#include <iostream>
#include <iterator>                         // for begin, end
#include <algorithm>
#include <span>
#include <cstddef>                          // for size_t, ptrdiff_t
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::begin, std::end, std::find, std::span, std::size_t, std::ptrdiff_t;


/* ---- Content ---- */

namespace {

    /* --- `show_an_array` ---
     * Four `int`s, 16 bytes, each 4 bytes after the one before. There is nothing else in a C array - no size, no
     * header.
     */
    void show_an_array() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        cout << " 1| sizeof(a)=" << sizeof(a) << '\n';
        for (size_t i{0}; i < 4; ++i) {
            cout << " 2|   &a[" << i << "]=" << &a[i] << ", a[" << i << "]=" << a[i] << '\n';
        }

        /* -- .Memory view. --
         * Set a breakpoint on ` 1|` and look at `&a` in the memory view:
         * `0a 00 00 00 14 00 00 00 1e 00 00 00 28 00 00 00` - 10, 20, 30 and 40, each little-endian (see previous
         * snippets). What follows is the next variable of the frame, or padding.
         */
    }

    /* --- `add_to_a_pointer` ---
     * `p + 1` does not add 1 to the address - it adds the size of one element: 4 for an `int`, 8 for a `double`, 1 for
     * a `char`. The type of the pointer tells the compiler how big a step is. That is why `p + 1` and `&a[1]` are the
     * same address.
     * Addresses of characters are printed via `const void*` - why, see next snippets.
     * - !![#pointer-arithmetic]
     */
    void add_to_a_pointer() {
        print_function_header();

        const int a[3]{10, 20, 30};
        const int* p{&a[0]};
        cout << " 1| int:    p=" << p << ", p+1=" << p + 1 << ", p+2=" << p + 2 << ", &a[1]=" << &a[1] << '\n';

        const double d[3]{1.5, 2.5, 3.5};
        const double* q{&d[0]};
        cout << " 2| double: q=" << q << ", q+1=" << q + 1 << ", q+2=" << q + 2 << '\n';

        const char c[3]{'a', 'b', 'c'};
        const char* r{&c[0]};
        const void* r0{r};
        const void* r1{r + 1};
        const void* r2{r + 2};
        cout << " 3| char:   r=" << r0 << ", r+1=" << r1 << ", r+2=" << r2 << '\n';
        cout << " 4| *(p+1)=" << *(p + 1) << ", *(q+1)=" << *(q + 1) << ", *(r+1)=" << *(r + 1) << '\n';
    }

    /* --- `index_with_arithmetic` ---
     * `p[2]` is defined as `*(p + 2)`: go two elements further, then read. So a pointer can be indexed like an array -
     * and `a[2]` on the array itself works the same way, see next snippets.
     * The definition has a curious consequence: `2[p]` is `*(2 + p)`, the same element. It compiles. It belongs in a
     * quiz, not in your code.
     */
    void index_with_arithmetic() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        const int* p{&a[0]};
        cout << " 1| p[2]=" << p[2] << ", *(p+2)=" << *(p + 2) << ", a[2]=" << a[2] << '\n';

        const int* middle{&a[1]};           // an index is relative to where the pointer points
        cout << " 2| middle[0]=" << middle[0] << ", middle[2]=" << middle[2] << '\n';
    }

    /* --- `subtract_pointers` ---
     * The difference of two pointers into the same array is the number of elements between them - the address
     * difference divided by `sizeof(T)`. Its type is `ptrdiff_t`, a signed integer: `first - last` is negative.
     * Pointers into the same array can be compared, too: the one with the higher index is greater.
     * - !![#sizeof]
     */
    void subtract_pointers() {
        print_function_header();

        const double d[5]{1.0, 2.0, 3.0, 4.0, 5.0};
        const double* first{&d[0]};
        const double* last{&d[4]};
        const ptrdiff_t distance{last - first};
        cout << " 1| last-first=" << distance << " elements, first-last=" << first - last << '\n';
        cout << " 2| first=" << first << ", last=" << last << " - " << distance << " times " << sizeof(double)
             << " bytes apart, first<last: " << (first < last) << '\n';
    }

    /* --- `walk_with_a_pointer` ---
     * A pointer loop: start at the first element, stop at one past the last, `++p` in between. This is the iterator
     * loop of unit 0x04, without the class around the address.
     * `std::begin(a)` and `std::end(a)` of a C array return exactly these two pointers - the iterators of a C array are
     * plain pointers. That is where iterators come from: they were designed to look like pointers, so that the
     * algorithms work for both. `find` returns a pointer here, and `found - begin(a)` is the index.
     * - !![#iterator]
     */
    void walk_with_a_pointer() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        const int* const past_the_end{&a[0] + 4};       // the second `const`: this pointer itself never moves
        cout << " 1| a:";
        for (const int* p{&a[0]}; p != past_the_end; ++p) {
            cout << ' ' << *p;
        }
        cout << '\n';

        const int* first{begin(a)};
        const int* last{end(a)};
        cout << " 2| begin(a)=" << first << ", end(a)=" << last << ", last-first=" << last - first << '\n';

        if (const int* found{find(begin(a), end(a), 30)}; found != end(a)) {
            cout << " 3| found " << *found << " at index " << found - begin(a) << '\n';
        }
    }

    /* --- `sum_by_index`, `sum_by_pointer` and `sum_by_range` ---
     * Three ways to sum up `int`s next to each other in memory: an address and a count, two addresses, a `span`.
     */
    int sum_by_index(const int* values, const size_t count) {
        int sum{0};
        for (size_t i{0}; i < count; ++i) {
            sum += values[i];
        }
        return sum;
    }

    int sum_by_pointer(const int* first, const int* last) {
        int sum{0};
        for (const int* p{first}; p != last; ++p) {
            sum += *p;
        }
        return sum;
    }

    int sum_by_range(const span<const int> values) {
        int sum{0};
        for (const int x : values) {
            sum += x;
        }
        return sum;
    }

    /* --- `element_at` --- One element, by index - for the look at the machine code below. */
    int element_at(const int* values, const size_t i) {
        return values[i];
    }

    /* --- `compare_three_loops` ---
     * The same sum, three notations. Which one is the fastest? Look before you guess.
     */
    void compare_three_loops() {
        print_function_header();

        const int a[5]{1, 2, 3, 4, 5};
        cout << " 1| by index=" << sum_by_index(a, 5) << ", by pointer=" << sum_by_pointer(begin(a), end(a))
             << ", by range=" << sum_by_range(a) << ", a[3]=" << element_at(a, 3) << '\n';

        /* -- .Look at the machine code. --
         * Paste the three `sum_by_` functions and `element_at` into Compiler Explorer, with `#include <span>` and
         * `#include <cstddef>` and `using std::size_t, std::span;` in front, and compile with `-O1`.
         * - `element_at` is one instruction, `mov eax, DWORD PTR [rdi+rsi*4]`: the address in `rdi`, plus the index in
         *   `rsi` times 4. The scaling is built into the instruction - x86-64 can multiply an index by 1, 2, 4 or 8 on
         *   the way. For a `double*` it is `*8`; on ARM64 it is `ldr w0, [x0, x1, lsl 2]`, a shift by 2.
         * - The `span` arrives in two registers: the address in `rdi`, the count in `rsi` - exactly the parameters of
         *   `sum_by_index`.
         * - The loops: gcc makes all three the same - add the element at the address, add 4 to the address, compare
         *   with the end. clang keeps a counter in some of them, `[rdi + 4*rcx]`. The notation you choose is for the
         *   reader, not for the machine.
         * - With `-O2`, clang adds four `int`s at a time (`paddd`) - a story for another day.
         */
    }

    /* --- `try_beyond_the_end` ---
     * `&a[0] + 4` - one past the last element - is a valid address: you may compute it and compare with it, as with
     * `end()`. You may not dereference it: there is no element.
     * Any other address outside the array is undefined behavior - even without `*`. For the machine, `p - 1` is just a
     * subtraction; for the language, a pointer may only point into an array or one past its end, and the compiler may
     * rely on that.
     * - !![#undefined-behavior]
     */
    void try_beyond_the_end() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        const int* const past_the_end{&a[0] + 4};
        cout << " 1| past_the_end=" << past_the_end << ", &a[3]=" << &a[3] << '\n';
        // cout << *past_the_end;           // undefined behavior: no element there
        // const int* before{&a[0] - 1};    // undefined behavior - even if it is never dereferenced

        /* -- .Q&A -- !![Why would the language forbid computing an address it never reads?](#a-503) */
    }

}

/* --- `main` --- */
int main() {
    show_an_array();
    add_to_a_pointer();
    index_with_arithmetic();
    subtract_pointers();
    walk_with_a_pointer();
    compare_three_loops();
    try_beyond_the_end();

    return EXIT_SUCCESS;
}
