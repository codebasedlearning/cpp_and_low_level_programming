// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Two pointers may point to the same object - they alias. The compiler has to allow for that.
 * - So after a write through one pointer, it must load again through the other: it cannot keep the value in a
 *   register.
 * - A reference is no different - for the machine, it is an address, too.
 * - `__restrict` (not standard, but gcc, clang and MSVC know it) promises that they do not alias - and if the promise
 *   is broken, the behavior is undefined.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `add_twice` --- Adds `*b` to `*a`, twice. */
    void add_twice(int* a, const int* b) {
        *a += *b;
        *a += *b;
    }

    /* --- `add_twice_cached` ---
     * The "optimized" version: load `*b` once. It is only the same as `add_twice` if `a` and `b` point to different
     * `int`s.
     */
    void add_twice_cached(int* a, const int* b) {
        const int value{*b};
        *a += value;
        *a += value;
    }

    /* --- `add_twice_restrict` --- The same as `add_twice`, with a promise. */
    void add_twice_restrict(int* __restrict a, const int* __restrict b) {
        *a += *b;
        *a += *b;
    }

    /* --- `call_with_the_same_address` ---
     * With two different `int`s, all three functions add 2. With the same `int` twice, `add_twice` doubles it twice -
     * the second `*b` sees the first write. `add_twice_cached` does not, and gives another result: the compiler may not
     * make this change on its own, because it would change the meaning of the program.
     * `add_twice_restrict` with the same address breaks its promise: undefined behavior - look at its machine code,
     * but do not call it that way.
     */
    void call_with_the_same_address() {
        print_function_header();

        int x{1};
        int y{1};
        add_twice(&x, &y);
        cout << " 1| different: x=" << x << '\n';

        int n{1};
        add_twice(&n, &n);
        cout << " 2| add_twice(&n, &n):        n=" << n << '\n';

        int m{1};
        add_twice_cached(&m, &m);
        cout << " 3| add_twice_cached(&m, &m): m=" << m << '\n';

        int k{1};
        const int one{1};
        add_twice_restrict(&k, &one);       // fine: different objects
        cout << " 4| add_twice_restrict:       k=" << k << '\n';
    }

}

/* --- The machine code ---
 * Paste the three functions into Compiler Explorer and compile with `-O2`.
 * - `add_twice`: load `*b`, add it to `*a`, store `*a` - and then load `*b` again, because the store may have changed
 *   it. Two loads from `[rsi]`.
 * - `add_twice_restrict`: one load of `*b`, doubled in a register (`add eax, eax`), and one add to `*a`.
 * - `add_twice_cached`: the same as the `__restrict` version.
 * The compiler can only keep a value in a register as long as it can prove that nobody else writes to it. Pointers
 * and references make that hard - one more reason why small values are passed by value (see previous snippets).
 * The compiler has one more rule on its side: pointers to different types, e.g. an `int*` and a `double*`, are assumed
 * not to alias ("strict aliasing"). How that rule can bite: see future snippets.
 */

/* --- `main` --- */
int main() {
    call_with_the_same_address();

    return EXIT_SUCCESS;
}
