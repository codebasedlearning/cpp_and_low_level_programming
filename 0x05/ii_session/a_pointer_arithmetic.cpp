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
#include <array>
#include <vector>
#include <iterator>                         // for begin, end
#include <algorithm>
#include <span>
#include <cstddef>                          // for size_t, ptrdiff_t
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>                // for `stopwatch`

using std::cout, std::array, std::vector, std::begin, std::end, std::find, std::span, std::size_t, std::ptrdiff_t;


/* ---- Content ---- */

namespace {

    /* --- `show_an_array_and_walk_it` ---
     * Four `int`s, 16 bytes, each 4 bytes after the one before. There is nothing else in a C array - no size, no
     * header.
     */
    void show_an_array_and_walk_it() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        cout << " 1| sizeof(a)=" << sizeof(a) << '\n';
        for (size_t i{0}; i < 4; ++i) {
            cout << " 2|   &a[" << i << "]=" << &a[i] << ", a[" << i << "]=" << a[i] << '\n';
        }

        const double d[3]{1.5, 2.5, 3.5};
        cout << " 3| sizeof(d)=" << sizeof(d) << '\n';
        for (size_t i{0}; i < sizeof(d)/sizeof(double); ++i) {
            cout << " 4|   &d[" << i << "]=" << &d[i] << ", d[" << i << "]=" << d[i] << '\n';
        }

        // Motivation

        const int* pa{};
        cout << " 5| pa=" << pa << '\n';

        pa = a;                             // or =&a[0];
        // Why not an error, I thought `pa` is const and `a` is an array? ...
        cout << " 6| pa=" << pa << ", *pa=" << *pa << '\n';

        pa = pa+1;
        // What is this?
        cout << " 7| pa=" << pa << ", *pa=" << *pa << '\n';

        const double* pd{d};                // or: pd{&d[0]};
        cout << " 8| pd=" << pd << ", *pd=" << *pd << '\n';
        pd = ++pd;
        cout << " 9| pd=" << pd << ", *pd=" << *pd << '\n';

        /* -- .Memory view. --
         * Set a breakpoint on ` 1|` and look at `&a` in the memory view:
         * `0a 00 00 00 14 00 00 00 1e 00 00 00 28 00 00 00` - 10, 20, 30 and 40, each little-endian (see previous
         * snippets). What follows is the next variable of the frame, or padding.
         */
    }

    /* --- `walk_with_a_pointer` ---
     * `p + 1` does not add 1 to the address - it adds the size of one element: 4 for an `int`, 8 for a `double`, 1 for
     * a `char`. The type of the pointer tells the compiler how big a step is. That is why `p + 1` and `&a[1]` are the
     * same address.
     * The difference of two pointers into the same array is the number of elements between them - the address
     * difference divided by `sizeof(T)`. Its type is `ptrdiff_t`, a signed integer: `first - last` is negative.
     * Pointers into the same array can be compared, too: the one with the higher index is greater.
     * - !![#sizeof]
     * - !![#iterator]
     * - !![#pointer-arithmetic]
     *
     *  One past the last element - is a valid address: you may compute it and compare with it, as with
     * `end()`. You may not dereference it: there is no element.
     * Any other address outside the array is undefined behavior - even without `*`. For the machine, `p - 1` is just a
     * subtraction; for the language, a pointer may only point into an array or one past its end, and the compiler may
     * rely on that.
     * - !![#undefined-behavior]
     */
    void walk_with_a_pointer() {
        print_function_header();

        // We wanted to use std::array instead of
        //   const int a[4]{10, 20, 30, 40};
        // but you can exchange it more or less one-to-one, the principles stay the same.
        const array<const int,4> a{10, 20, 30, 40};
        cout << " 1| &a=" << &a << ", a.data=" << a.data() << ", &a[0]=" << &a[0] << '\n';

        const int* p{&a[0]};
        cout << " 2|        p=" << p << ", *p=" << *p << ", &a[0]=" << &a[0] << '\n';
        ++p;
        cout << " 3| ++p -> p=" << p << ", *p=" << *p << ", &a[1]=" << &a[1] << '\n';
        ++p;
        cout << " 4| ++p -> p=" << p << ", *p=" << *p << ", &a[2]=" << &a[2] << '\n';
        ++p;
        cout << " 5| ++p -> p=" << p << ", *p=" << *p << ", &a[3]=" << &a[3] << '\n';

        --p;
        cout << " 6| --p -> p=" << p << ", *p=" << *p << ", &a[2]=" << &a[2] << '\n';
        p -= 2;                             // p = p-2;
        cout << " 7| -=2 -> p=" << p << ", *p=" << *p << ", &a[0]=" << &a[0] << '\n';

        // allowed?
        // --p;
        /* -- .Q&A -- !![Why would the language forbid computing an address it never reads?](#a-503) */

        const int* start{&a[0]};
        const int* past_the_end{start + 4};
        cout << " 8| start=" << start << ", past end=" << past_the_end << ", a:";
        for (auto it{start}; it!=past_the_end; ++it) {
            cout << " " << *it << "(at " << (it-start) << ")";
        }
        cout << '\n';

        const int* abegin{a.begin()};       // use begin(a) for a raw array,
        const int* aend{a.end()};           // and end(a)
        cout << " 9| abegin=" << abegin << ", aend=" << aend << ", a:";

        for (auto it{abegin}; it!=aend; ++it) {
            cout << " " << *it << "(at " << (it-abegin) << ")";
        }
        cout << '\n';

        const ptrdiff_t distance{aend - abegin};
        cout << "10| distance=" << distance << '\n';
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

    /* --- `compare_three_loops` ---
     * The same sum, three notations. Which one is the fastest? Look before you guess.
     */
    void compare_three_loops() {
        print_function_header();

        constexpr int rounds{10};
        const vector<int> v(10'000'000, 1);

        stopwatch watch{};
        long long sum{0};
        for (int round{0}; round < rounds; ++round) {
            sum += sum_by_index(v.data(), v.size());
        }
        cout << " 1| sum=" << sum << ", took " << watch.elapsed_ms() << " ms\n";

        watch.reset();
        sum = 0;
        for (int round{0}; round < rounds; ++round) {
            sum += sum_by_pointer(v.data(), v.data()+v.size());
        }
        cout << " 2| sum=" << sum << ", took " << watch.elapsed_ms() << " ms\n";

        watch.reset();
        sum = 0;
        for (int round{0}; round < rounds; ++round) {
            sum += sum_by_range(span(v.begin(), v.end()));
        }
        cout << " 3| sum=" << sum << ", took " << watch.elapsed_ms() << " ms\n";

        /* -- .Look at the machine code. --
         * Paste the three `sum_by_` functions into Compiler Explorer.
         * - The loops: gcc makes all three the same - add the element at the address, add 4 to the address, compare
         *   with the end. clang keeps a counter in some of them, `[rdi + 4*rcx]`. The notation you choose is for the
         *   reader, not for the machine.
         * - With `-O2`, clang adds four `int`s at a time (`paddd`) - a story for another day.
         */
    }

}

/* --- `main` --- */
int main() {
    show_an_array_and_walk_it();
    walk_with_a_pointer();
    compare_three_loops();

    return EXIT_SUCCESS;
}
