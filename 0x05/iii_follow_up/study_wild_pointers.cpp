// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A wild pointer holds an address where no object of its type lives - not yet, not any more, or never:
 *   uninitialized, null, dangling, out of bounds.
 * - The machine checks nothing: it reads and writes whatever is at that address - unless the operating system stops
 *   the program.
 * - The compiler assumes that it never happens, and optimizes on that assumption: Debug and Release differ, gcc and
 *   clang differ.
 * - Often it seems to work. That is the dangerous case.
 * - Every line with undefined behavior below is commented out. Remove the `//` one at a time, run it as Debug and as
 *   Release - and put the `//` back.
 */

#include <iostream>
#include <array>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl, std::array;


/* ---- Content ---- */

namespace {

    /* --- `try_an_uninitialized_pointer` ---
     * A pointer without an initializer holds whatever bytes were in its place in the frame - an address, but nobody
     * knows of what. Writing through it writes somewhere. If there is nothing to point to yet, say so: `nullptr`.
     * - !![#nullptr]
     */
    void try_an_uninitialized_pointer() {
        print_function_header();

        // int* p;                          // no initializer: leftover bytes
        // *p = 42;                         // undefined behavior: writes 42 to an unknown address
        const int* q{nullptr};              // nothing yet - and everybody can see that
        cout << " 1| q is null: " << (q == nullptr) << '\n';
    }

    /* --- `try_a_null_pointer` ---
     * `nullptr` is safe to store, to copy and to compare - but not to dereference. On every common platform, the first
     * pages of the address space are not mapped, and the processor reports an access there to the operating system,
     * which ends the program. That is the kind case: you see it at once.
     * `endl` on purpose: flush the output now - when the program is killed, what is still in the buffer is lost.
     */
    void try_a_null_pointer() {
        print_function_header();

        int* p{nullptr};
        cout << " 1| p=" << p << endl;
        // *p = 42;                         // undefined behavior: writes to address 0
        cout << " 2| still alive" << endl;

        /* -- .Q&A -- !![Remove the `//`: what happens, as Debug and as Release?](#a-506) */
    }

    /* --- `find_negative` ---
     * Returns a pointer into the caller's array - to the first negative element, or `nullptr` if there is none. That is
     * fine: the array lives on after the function returns, and the address stays valid as long as it does.
     */
    const int* find_negative(const int* first, const int* last) {
        for (const int* p{first}; p != last; ++p) {
            if (*p < 0) {
                return p;
            }
        }
        return nullptr;
    }

    /* --- `return_a_pointer` --- A returned pointer, and a check before it is used. */
    void return_a_pointer() {
        print_function_header();

        const int a[5]{3, 1, -4, 1, -5};
        if (const int* found{find_negative(a, a + 5)}; found != nullptr) {
            cout << " 1| found " << *found << " at index " << found - a << '\n';
        }
    }

    /* --- `make_a_number` ---
     * The classic: the address of a local variable, returned. `n` dies at the `}` - its place in the frame is free for
     * the next call, and the caller gets a dangling pointer. Both compilers warn, even without `-Wall`. Remove the
     * `//`s, and the call in `try_a_dangling_pointer`, and read the warning.
     * - !![#dangling-pointer]
     */
    // int* make_a_number() {
    //     int n{42};
    //     return &n;                       // warning: address of local variable 'n' returned
    // }

    /* --- `try_a_dangling_pointer` ---
     * The same without a function: a pointer that outlives the block of its object. After the inner `}`, `p` still
     * holds the address - a number - but there is no `inner` any more.
     */
    void try_a_dangling_pointer() {
        print_function_header();

        const int* p{nullptr};
        {
            const int inner{7};
            p = &inner;
            cout << " 1| *p=" << *p << " - fine, `inner` is alive\n";
        }
        // cout << *p;                      // undefined behavior: `inner` is gone
        // const int* number{make_a_number()};
        // cout << number;                  // print the address only - what do gcc and clang give you?

        /* -- .Q&A -- !![`make_a_number` returns `&n`. What do gcc and clang actually return?](#a-507) */
    }

    /* --- `try_out_of_bounds` ---
     * `a[3]` is one past the end: `*(a + 3)`, an address the machine is happy to use. What is there? Some other
     * variable of the frame, padding, the saved registers of the caller - or a guard value that the compiler put there
     * for exactly this case. The addresses tell you who the neighbors are, not what the compiler will do.
     * `std::array` has `at()`, which checks and throws `std::out_of_range`; `[]` does not check - neither for
     * `std::array` nor for `std::vector`.
     * - !![#undefined-behavior]
     */
    void try_out_of_bounds() {
        print_function_header();

        int guard1{1};                      // not `const` - see the Q&A
        int a[3]{10, 20, 30};
        int guard2{2};
        cout << " 1| &guard1=" << &guard1 << ", &a[0]=" << &a[0] << ", &a[3]=" << &a[3] << ", &guard2=" << &guard2
             << endl;
        // a[3] = 99;                       // undefined behavior: writes behind the array
        cout << " 2| guard1=" << guard1 << ", guard2=" << guard2 << ", a[2]=" << a[2] << endl;

        const array<int, 3> checked{10, 20, 30};
        cout << " 3| checked.at(2)=" << checked.at(2) << '\n';
        // checked.at(3);                   // throws `std::out_of_range` - a bug with a name, not a mystery

        /* -- .Q&A -- !![Remove the `//` in front of `a[3] = 99;`: which guard changes?](#a-508) */
    }

    /* --- `read_then_check` ---
     * First the dereference, then the check. It looks careless, but harmless - if `p` is null, the function returns -1,
     * doesn't it?
     */
    int read_then_check(const int* p) {
        const int value{*p};
        if (p == nullptr) {
            return -1;
        }
        return value;
    }

    /* --- `check_too_late` ---
     * Paste `read_then_check` into Compiler Explorer and compile with `-O1`: `mov eax, DWORD PTR [rdi]` and `ret` - the
     * `if` is gone, with gcc and with clang. The reasoning: `*p` was executed, and dereferencing a null pointer is
     * undefined behavior - so `p` cannot be null, so the test is always false, so it can go. The compiler did not break
     * your check; it believed your dereference.
     * Check first, then dereference.
     */
    void check_too_late() {
        print_function_header();

        const int n{23};
        cout << " 1| read_then_check(&n)=" << read_then_check(&n) << '\n';
        // read_then_check(nullptr);        // never -1: the dereference comes first
    }

}

/* --- Taming pointers ---
 * - Initialize every pointer: with an address, or with `nullptr`.
 * - Check for `nullptr` before the dereference - or take a reference, if "nothing" is not a valid argument.
 * - Never return, store or keep the address of a local beyond its `}`.
 * - Stay inside the array: pass a `span` or a `std::array` instead of an address and a count; use `at()` where a
 *   check is worth its price.
 * - Remember that a `vector` moves its elements when it grows: pointers into it dangle (see previous snippets).
 * - Let the tools look: the warnings (`-Wall -Wextra`), and the sanitizers, see future snippets.
 * The mantra of this unit: an address is just a number - until you dereference it.
 */

/* --- `main` --- */
int main() {
    try_an_uninitialized_pointer();
    try_a_null_pointer();
    return_a_pointer();
    try_a_dangling_pointer();
    try_out_of_bounds();
    check_too_late();

    return EXIT_SUCCESS;
}
