// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A `struct` is its members, one after the other - plus gaps.
 * - Alignment and padding: why the order of the members matters.
 * - What that means for a million of them in a `vector`.
 */

#include <iostream>
#include <vector>
#include <cstddef>                          // for `offsetof`
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `wasteful` and `compact` --- The same members, in a different order. */
    struct wasteful {
        char a;
        double b;
        char c;
    };

    struct compact {
        double b;
        char a;
        char c;
    };

    /* --- `show_alignment` ---
     * Every type has an alignment: its address must be a multiple of it. For the primitive types, that is usually their
     * size.
     */
    void show_alignment() {
        print_function_header();

        cout << " 1| char:   sizeof=" << sizeof(char) << ", alignof=" << alignof(char) << '\n';
        cout << " 2| int:    sizeof=" << sizeof(int) << ", alignof=" << alignof(int) << '\n';
        cout << " 3| double: sizeof=" << sizeof(double) << ", alignof=" << alignof(double) << '\n';
    }

    /* --- `show_padding` ---
     * To keep `b` aligned, the compiler inserts unused bytes - padding. And the whole struct is padded so that in an
     * array the next `b` is aligned, too.
     * `offsetof` shows where each member starts.
     * - !![#sizeof]
     */
    void show_padding() {
        print_function_header();

        cout << " 1| wasteful: sizeof=" << sizeof(wasteful) << ", offsets a=" << offsetof(wasteful, a)
             << ", b=" << offsetof(wasteful, b) << ", c=" << offsetof(wasteful, c) << '\n';
        cout << " 2| compact:  sizeof=" << sizeof(compact) << ", offsets b=" << offsetof(compact, b)
             << ", a=" << offsetof(compact, a) << ", c=" << offsetof(compact, c) << '\n';

        /* -- .Q&A -- !![Why does the compiler not reorder the members for you?](#a-205) */
    }

    /* --- `multiply_the_padding` --- In a `vector` the difference is multiplied. */
    void multiply_the_padding() {
        print_function_header();

        const vector<wasteful> w(1'000'000);
        const vector<compact> c(1'000'000);
        cout << " 1| 1'000'000 x wasteful: " << w.size() * sizeof(wasteful) / 1'000'000 << " MB\n";
        cout << " 2| 1'000'000 x compact:  " << c.size() * sizeof(compact) / 1'000'000 << " MB\n";
        cout << " 3| &w[0]=" << &w[0] << ", &w[1]=" << &w[1] << '\n';

        /* -- .Memory view. --
         * Set a breakpoint here and look at `&w[0]` in the memory view. Which bytes are padding?
         */
    }

}

/* --- `main` --- */
int main() {
    show_alignment();
    show_padding();
    multiply_the_padding();

    return EXIT_SUCCESS;
}
