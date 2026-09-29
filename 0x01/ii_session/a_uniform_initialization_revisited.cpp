// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Introduce uniform initialization.
 * - Where variables live: the stack, their addresses, the memory view.
 * - What an uninitialized variable really contains - and why reading it is UB.
 * - Sizes and ranges of the integer types, and why braces reject narrowing.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - What does `main` return?
 * - `'\n'` or `endl` - what is the difference, and which one do we use?
 * - What does `int v3{};` contain? And what does `bool b{true};` print?
 * - Why does `float f{1.2f};` need the `f`?
 */

#include <iostream>
#include <cstdlib>
#include <cstddef>                          // for `size_t`
#include <limits>                           // for `numeric_limits`
#include <cbl/printing.hpp>

using std::cout, std::hex, std::dec, std::showbase, std::noshowbase;
using std::numeric_limits, std::size_t;

/* --- Pragmas ---
 * Instructions for the compiler, here: do not warn about the uninitialized variable below - we read it on purpose.
 * - !![#pragma]
 */
#pragma GCC diagnostic ignored "-Wuninitialized"


/* ---- Content ---- */

namespace {

    /* --- `define_variable_without_init` ---
     * Memory is reserved for `v`, but nothing is written into it. Reading it is undefined behaviour (UB). Why does C++
     * allow this at all? Because you only pay for what you use - an initialization costs time.
     * - !![#undefined-behavior]
     * - !![#zero-overhead]
     */
    void define_variable_without_init() {
        print_function_header();

        int v;                              // no value, on purpose
        cout << " 1| v=" << v << '\n';

        /* -- .Q&A -- !![What is the value of `v`?](#a-102) */
    }

    /* --- `init_variable_on_stack` ---
     * `w` gets a value and lives in the stack frame of this function. When the function returns, the frame is
     * released - but not cleared.
     * - !![#stack-and-heap]
     */
    void init_variable_on_stack() {
        print_function_header();

        int w{23};
        cout << " 1| w=" << w << '\n';

        /* -- .Q&A -- !![Can you explain why `v` is 23 for the second run of `define_variable_without_init`?](#a-103) */

        /* -- .Debug vs. Release. --
         * The 23 shows up in a Debug build (`-O0`). Now build with `-O2`, e.g.
         *     g++ -std=c++23 -O2 -I ../../utils a_uniform_initialization_revisited.cpp && ./a.out
         * and look at `v` again. UB does not mean "some random value", it means "no rules at all" - the optimizer may
         * assume it never happens.
         */
    }

    /* --- `show_three_variables_on_stack` --- */
    void show_three_variables_on_stack() {
        print_function_header();

        int w1{0x00112233};
        int w2{0x00445566};
        int w3{0x00778899};

        cout << " 1| sizeof(int)=" << sizeof(int) << " bytes\n";    // also sizeof(w1)
        cout << showbase << hex;                                    // output in hex
        cout << " 2| w1=" << w1 << ", &w1=" << &w1 << '\n';
        cout << " 3| w2=" << w2 << ", &w2=" << &w2 << '\n';
        cout << " 4| w3=" << w3 << ", &w3=" << &w3 << '\n';

        /* -- .Memory view. --
         * Set a breakpoint on the next output, start with 'Debug' instead of 'Run', open the 'Memory View' tab in the
         * 'Debug' window and enter `&w1` (or one of the addresses from the output above).
         * - All three variables are there, with the values from above.
         * - Which one has the lowest address?
         * - Change the value of `w1` in the memory view, then continue.
         */
        cout << " 5| w1=" << w1 << '\n';

        /* -- .Q&A -- !![In which order do the bytes of `0x00112233` appear in the memory view?](#a-104) */

        // Preview, to warm up: a pointer (address) to an int, here w1 – also living on the stack.
        int* pw1{&w1};
        cout << " 6| &w1=" << &w1 << ", pw1=" << pw1 << '\n';

        cout << noshowbase << dec;
    }

    /* --- `show_variables_of_different_types` --- Different types, different sizes. */
    void show_variables_of_different_types() {
        print_function_header();

        int n{0x1234};
        bool b{true};
        double d{1234.5678};                // hex 40934A456D5CFAAD
        bool f{true};
        int m{0x5678};

        cout << " 1| sizeof(int)=" << sizeof(int) << " bytes\n";
        cout << " 2| sizeof(bool)=" << sizeof(bool) << " bytes\n";
        cout << " 3| sizeof(double)=" << sizeof(double) << " bytes\n";

        // `sizeof` yields a `size_t` - the unsigned type for sizes and indexes.
        const size_t size_of_d{sizeof(d)};
        cout << " 4| size_of_d=" << size_of_d << ", sizeof(size_t)=" << sizeof(size_t) << " bytes\n";

        cout << " 5| n=" << showbase << hex << n << dec << " (" << sizeof(n) << "), " << &n << "\n"
             << "    b=" << b << " (" << sizeof(b) << "), " << &b << "\n"
             << "    d=" << d << " (" << sizeof(d) << "), " << &d << "\n"
             << "    f=" << f << " (" << sizeof(f) << "), " << &f << "\n"
             << "    m=" << hex << m << dec << " (" << sizeof(m) << "), " << &m
             << noshowbase << '\n';

        /* -- .Find them in memory. --
         * Set a breakpoint at the closing `}` of this function and find the variables in the memory view.
         * - !![#sizeof]
         */

        /* -- .Q&A -- !![Why are the variables not in declaration order, and why are there gaps?](#a-105) */
    }

    /* --- `show_sizes_and_ranges` ---
     * The standard only fixes minimum sizes, the actual ones depend on the platform. `numeric_limits` gives the range
     * of a type.
     */
    void show_sizes_and_ranges() {
        print_function_header();

        cout << " 1| int:       " << sizeof(int) << " bytes, max=" << numeric_limits<int>::max() << '\n';
        cout << " 2| long:      " << sizeof(long) << " bytes, max=" << numeric_limits<long>::max() << '\n';
        cout << " 3| long long: " << sizeof(long long) << " bytes, max=" << numeric_limits<long long>::max() << '\n';

        /* -- .`long` is a trap. --
         * On Linux and macOS `long` has 8 bytes, on Windows only 4. So `long l{123456789012345};` compiles here and
         * fails on Windows.
         * If you need 64 bits, say so: `long long` or `std::int64_t`.
         * - More on fixed-width types: see future snippets.
         */
    }

    /* --- `compare_equals_and_braces` ---
     * Braces refuse conversions that lose information, `=` does not - the main reason why we prefer braces.
     * - !![#narrowing-conversion]
     */
    void compare_equals_and_braces() {
        print_function_header();

        const double d{2.5};
        int v1 = d;                         // compiles (at most a warning), v1 becomes 2
        cout << " 1| v1=" << v1 << '\n';

        // int v2{d};                       // compiler error: narrowing - try it
    }

}

/* --- `main` --- Note that `define_variable_without_init` runs twice. */
int main() {
    define_variable_without_init();
    init_variable_on_stack();
    define_variable_without_init();
    show_three_variables_on_stack();
    show_variables_of_different_types();
    show_sizes_and_ranges();
    compare_equals_and_braces();

    return EXIT_SUCCESS;
}
