// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x05 - the things to remember.
 */

#include <iostream>
#include <iterator>
#include <span>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::begin, std::end, std::span, std::strlen, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `recall_pointer_arithmetic` ---
     * An address is just a number - until you dereference it. `p + 1` is the next element, `sizeof(T)` bytes further;
     * `p[i]` is `*(p + i)`; the difference of two pointers counts elements. One past the last element may be computed
     * and compared, not dereferenced - it is the `end()` of a C array.
     */
    void recall_pointer_arithmetic() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        const int* p{begin(a)};
        cout << " 1| p=" << p << ", p+1=" << p + 1 << ", p[2]=" << p[2] << ", end-begin=" << end(a) - begin(a) << '\n';
    }

    /* --- `sum_all` --- The C way to pass an array: the address, and the count. */
    int sum_all(const int* values, const size_t count) {
        int sum{0};
        for (size_t i{0}; i < count; ++i) {
            sum += values[i];
        }
        return sum;
    }

    /* --- `recall_the_decay` ---
     * An array decays to the address of its first element, and the count is lost - C passes it along, a `span` keeps
     * it. A C string keeps it in the data: a `'\0'` at the end, and `strlen` walks to it.
     */
    void recall_the_decay() {
        print_function_header();

        const int a[4]{1, 2, 3, 4};
        const span<const int> view{a};
        cout << " 1| sum_all=" << sum_all(a, 4) << ", view.size()=" << view.size() << '\n';

        const char title[]{"Blue Train"};
        cout << " 2| sizeof(title)=" << sizeof(title) << ", strlen(title)=" << strlen(title) << '\n';
    }

    /* --- `recall_a_dangling_pointer` ---
     * A local dies at its `}`. A pointer that outlives it still holds the address - of nothing. The machine does not
     * check; the compiler assumes that it never happens.
     */
    void recall_a_dangling_pointer() {
        print_function_header();

        const int* p{nullptr};
        {
            const int inner{7};
            p = &inner;
            cout << " 1| p=" << p << ", *p=" << *p << " - `inner` is alive\n";
        }
        // cout << *p;                      // undefined behavior: `inner` is gone, `p` still holds its address
    }

}

/* --- Calling convention ---
 * Arguments and results travel in registers while they are small: on x86-64 Linux and macOS the first integers and
 * addresses in `rdi`, `rsi`, ..., the result in `rax`; on ARM64 in `x0`, `x1`, ... A small struct by value costs no
 * more than its members, a large one is copied by the caller - pass it by `const&`. A type with a copy constructor of
 * its own is always passed via an address.
 */

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`).
 * - The debugger: breakpoints, the memory view, stepping and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 * - `nm`, `nm -C` and `c++filt` - which functions are in which object file, and under which name.
 * - `g++ -S` and Compiler Explorer (godbolt.org) - the machine code, x86-64 and ARM64.
 * - AddressSanitizer (`-fsanitize=address`), where the toolchain has it.
 */

/* --- Teaser ---
 * In 'McAllen Spring', `make_node` could not return a node: it lived in the stack frame of `make_node`, and died at
 * its `}`. Where else can an object live, so that it outlives the function that created it - and who ends its life
 * then?
 */

/* --- `main` --- */
int main() {
    recall_pointer_arithmetic();
    recall_the_decay();
    recall_a_dangling_pointer();

    return EXIT_SUCCESS;
}
