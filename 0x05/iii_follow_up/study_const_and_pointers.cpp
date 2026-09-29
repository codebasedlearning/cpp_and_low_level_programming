// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Two things can be `const` in a pointer declaration: the object it points to, and the pointer itself.
 * - Read the declaration from right to left: `const int* p` - `p` is a pointer to an `int` that is `const`;
 *   `int* const q` - `q` is a `const` pointer to an `int`.
 * - `int*` converts to `const int*`, but not back: adding a promise is fine, dropping one is not.
 * - In a parameter, the `const` before the `*` concerns the caller; the one after it concerns only the function.
 * - The machine knows nothing of it: `const` is checked by the compiler.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `point_to_const` ---
     * `const int* p` - through `p`, the `int` can be read, not written. `p` itself may move on to another `int`. And
     * the `int` may still change through its own name - `p` only promises not to change it, like a `const&` (see
     * previous snippets).
     * `int const* p` is the same type; some people prefer to write the `const` always to the right of what it protects.
     * - !![#const-pointer]
     */
    void point_to_const() {
        print_function_header();

        int n{1};
        const int m{2};
        const int* p{&n};
        // *p = 10;                         // compiler error: `*p` is const
        n = 3;                              // fine - `n` is not const, and `p` sees it
        cout << " 1| *p=" << *p << '\n';
        p = &m;                             // fine - `p` itself is not const
        cout << " 2| *p=" << *p << '\n';
    }

    /* --- `use_a_const_pointer` ---
     * `int* const q` - `q` always points to `n`, but through it, `n` can be changed. That is exactly what a reference
     * is: a `const` pointer that is dereferenced automatically.
     */
    void use_a_const_pointer() {
        print_function_header();

        int n{1};
        int m{2};
        int* const q{&n};
        *q = 10;                            // fine - the `int` is not const
        // q = &m;                          // compiler error: `q` is const
        cout << " 1| n=" << n << ", m=" << m << '\n';

        const int* const r{&m};             // both: `r` does not move, and `*r` is read-only
        cout << " 2| *r=" << *r << '\n';
    }

    /* --- `convert_pointers` ---
     * From `int*` to `const int*`: the new pointer promises more, so that is allowed. The other way round would break
     * the promise - a compiler error. That is why a function with a `const int*` parameter accepts both kinds of
     * pointers. A string literal is an array of `const char`: `char* s{"text"}` is an error in C++ (C allowed it). The
     * characters are in read-only memory - writing to them would end the program.
     */
    void convert_pointers() {
        print_function_header();

        int n{1};
        int* p{&n};
        const int* cp{p};                   // fine: a promise added
        // int* back{cp};                   // compiler error: a promise dropped
        const char* title{"Blue Train"};
        // char* writable{"Blue Train"};    // compiler error in C++
        cout << " 1| *cp=" << *cp << ", title=" << title << '\n';
    }

    /* --- `average` and `fill` ---
     * The signature is a contract. `const int* values`: the caller's elements are safe. `int* values`: they will be
     * written. The `const` after the `*` in `int* const values` would be like `const int count` - it only concerns the
     * function's own copy of the address. The caller does not care, and in a pure declaration it can be left out.
     */
    double average(const int* values, const int count) {
        double sum{0.0};
        for (int i{0}; i < count; ++i) {
            sum += values[i];
        }
        return count == 0 ? 0.0 : sum / count;
    }

    void fill(int* const values, const int count, const int value) {
        for (int i{0}; i < count; ++i) {
            values[i] = value;
        }
        // values = nullptr;                // compiler error: the function's own copy is const
    }

    /* --- `read_the_signature` --- Input and output, told apart by one word. */
    void read_the_signature() {
        print_function_header();

        int a[4]{1, 2, 3, 4};
        cout << " 1| average=" << average(a, 4) << '\n';
        fill(a, 4, 7);
        cout << " 2| after fill: a[0]=" << a[0] << ", a[3]=" << a[3] << '\n';

        /* -- .Q&A -- !![Where is the `const` in the machine code?](#a-510) */
    }

}

/* --- `main` --- */
int main() {
    point_to_const();
    use_a_const_pointer();
    convert_pointers();
    read_the_signature();

    return EXIT_SUCCESS;
}
