// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A first look at pointers: a variable that holds an address - `int*` and `void*`.
 * - A reference is a second name for an existing variable - no copy, same address.
 * - A reference must be initialized and cannot be rebound.
 * - `const&`: reading yes, changing no.
 * - A reference is not a pointer - but the compiler usually implements it as one.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `fraction f{1, 2};` - which member gets the 1? And what does `const fraction g{};` contain?
 * - `std::array<int, 3>` or `std::vector<int>` - what is the difference? Why must the 3 be known at compile time?
 * - `vector<int> v(10, 1)` and `vector<int> v{10, 1}` - how many elements does each one have?
 * - Why does the stopwatch example print its result - and why measure as Release?
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `take_addresses` ---
     * `&n` is the address of `n`. A pointer is a variable that holds such an address - `int*` says: at this address
     * there is an `int`. With `*p` you get to that `int`, to read or to write it.
     * - !![#pointer]
     */
    void take_addresses() {
        print_function_header();

        int n{23};
        int* p{&n};                         // `p` holds the address of `n`
        cout << " 1| n=" << n << ", &n=" << &n << '\n';
        cout << " 2| p=" << p << ", *p=" << *p << ", &p=" << &p << ", sizeof(p)=" << sizeof(p) << '\n';

        *p = 42;                            // writes to `n`
        cout << " 3| n=" << n << '\n';

        /* -- .Debugger. --
         * Set a breakpoint on `*p = 42;` and start with 'Debug'.
         * - In the variables view, `p` shows an address - the same as `&n`.
         * - In the memory view, enter `&p`: there are the 8 bytes of that address.
         * - Enter `p`: there is `n`, first 23, then 42 after the next step.
         */

        /* -- .Q&A -- !![Where does `p` itself live?](#a-201) */
    }

    /* --- `show_void_pointers` ---
     * `void*` is an address without a type: we know where, but not what. So it cannot be dereferenced - that is also
     * why `cout` prints it as a number, while it prints a `const char*` as text (see 0x01).
     */
    void show_void_pointers() {
        print_function_header();

        int n{23};
        const void* v{&n};                  // every object address converts to `const void*`
        cout << " 1| v=" << v << '\n';

        // cout << *v;                      // compiler error: an address without a type cannot be read
    }

    /* --- `define_references` ---
     * `int& m{n};` makes `m` another name for `n`. Whatever you do with `m`, you do with `n` - they are the same object
     * at the same address.
     * - !![#reference]
     */
    void define_references() {
        print_function_header();

        int n{1};
        int& m{n};
        cout << " 1| n=" << n << ", m=" << m << ", &n=" << &n << ", &m=" << &m << '\n';

        n = 2;
        cout << " 2| n=" << n << ", m=" << m << '\n';

        m = 3;                              // changes `n`
        cout << " 3| n=" << n << ", m=" << m << '\n';

        /* -- .Q&A -- !![Does `m` need memory of its own?](#a-202) */
    }

    /* --- `try_to_rebind` ---
     * A reference is bound once, when it is initialized - there is no syntax to bind it to something else later. That
     * is why it must be initialized.
     */
    void try_to_rebind() {
        print_function_header();

        int n{1};
        int& m{n};
        const int k{4};
        m = k;                              // assigns 4 to `n` - `m` still refers to `n`
        cout << " 1| n=" << n << ", m=" << m << ", k=" << k << '\n';

        // int& r;                          // compiler error: a reference must be initialized
    }

    /* --- `read_through_const_references` ---
     * Through `const int& c{n};` you can read `n`, but not change it. `n` itself may still change - and `c` sees it.
     * - !![#const-correctness]
     */
    void read_through_const_references() {
        print_function_header();

        int n{1};
        const int& c{n};
        n = 2;
        cout << " 1| n=" << n << ", c=" << c << '\n';

        // c = 3;                           // compiler error - try it
    }

    /* --- `holds_reference` and `holds_pointer` --- Two structs, to compare their size. */
    struct holds_reference {
        int& r;
    };

    struct holds_pointer {
        int* p;
    };

    /* --- `increment_by_reference` and `increment_by_pointer` --- The same work, two notations. */
    void increment_by_reference(int& x) {
        ++x;
    }

    void increment_by_pointer(int* x) {
        ++*x;
    }

    /* --- `compare_references_and_pointers` ---
     * A reference is not a pointer: it cannot be null, cannot be rebound, has no arithmetic, and the syntax hides the
     * address. But when a reference must exist at runtime - as a member, or as a parameter - the compiler usually
     * stores an address, exactly like a pointer.
     */
    void compare_references_and_pointers() {
        print_function_header();

        cout << " 1| sizeof(holds_reference)=" << sizeof(holds_reference)
             << ", sizeof(holds_pointer)=" << sizeof(holds_pointer) << '\n';

        int n{1};
        increment_by_reference(n);          // passes the address of `n` - you just do not see it
        increment_by_pointer(&n);           // passes the address of `n` - explicitly
        cout << " 2| n=" << n << '\n';

        /* -- .Look at the machine code. --
         * Paste the two `increment_` functions into Compiler Explorer (godbolt.org), or run
         * `g++ -std=c++23 -S -O0 -I ../../utils a_pointers_and_references.cpp` and find them in the `.s` file. The
         * instructions are the same.
         */
    }

    /* --- `declare_several_references` ---
     * The `&` belongs to the variable, not to the type - careful with several variables in one line.
     */
    void declare_several_references() {
        print_function_header();

        int n{1};
        int& k1{n}, k2{n};                  // `k1` is a reference, `k2` a plain `int` - a copy
        n = 42;
        cout << " 1| n=" << n << ", k1=" << k1 << ", k2=" << k2 << '\n';

        /* -- .`int& k` or `int &k`? --
         * That is why many people write the `&` (and the `*`) next to the variable: `int &k1, k2;` shows that only
         * `k1` is a reference - the C tradition, e.g. `int *p`. Others write it next to the type, `int& k`, because
         * it is part of the type - the C++ tradition, and the style of this course. Whatever the style: one variable
         * per declaration, and the trap is gone.
         * - The same holds for pointers: `int* p1, p2;` makes only `p1` a pointer, `p2` is a plain `int`.
         * - The C++ Core Guidelines recommend exactly this: `int& k` and `int* p` next to the type (NL.18), and one
         *   name per declaration (ES.10).
         */
    }

}

/* --- `main` --- */
int main() {
    take_addresses();
    show_void_pointers();
    define_references();
    try_to_rebind();
    read_through_const_references();
    compare_references_and_pointers();
    declare_several_references();

    return EXIT_SUCCESS;
}
