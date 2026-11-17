// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Four named casts, four jobs: `static_cast` converts a value, `std::bit_cast` keeps the bits of a value,
 *   `reinterpret_cast` reads the same address as another type, `const_cast` takes the `const` away.
 * - In the machine, a cast is an instruction - or nothing at all. The casts that change only the type of a pointer
 *   cost nothing, and they check nothing either.
 * - `const_cast` does not make an object writable. Writing through it is fine if the object itself is not `const`,
 *   and undefined behavior if it is. The cast is the same in both cases - the object decides.
 * - The C-style cast `(T)x` asks for no cast in particular, and may pick one you did not ask for.
 * - Every line with undefined behavior below is commented out.
 */

#include <iostream>
#include <bit>                              // for bit_cast
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::hex, std::dec, std::uint32_t, std::bit_cast;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The next four functions are for Compiler Explorer, see the end of this snippet: each one does exactly one cast. As
 * in previous snippets, they keep their own names in the assembly.
 */
uint32_t value_of(const float f) { return static_cast<uint32_t>(f); }
uint32_t bits_of(const float f) { return bit_cast<uint32_t>(f); }
const unsigned char* bytes_of(const int* p) { return reinterpret_cast<const unsigned char*>(p); }
int* without_const(const int* p) { return const_cast<int*>(p); }

namespace {

    /* --- `convert_or_keep_the_bits` ---
     * One `float`, two casts, two answers.
     * - `static_cast` converts the value: 1.0 becomes the integer 1, a new bit pattern, computed by an instruction.
     * - `bit_cast` (C++20) keeps the four bytes as they are and reads them as an integer: `0x3f800000` - the sign,
     *   the exponent and the digits of 1.0, as the processor stores them. It works in both directions: the
     *   bits `0x40490fdb` are the `float` closest to pi. `bit_cast` needs two types of the same size that can be
     *   copied byte by byte - a `float` has 4 bytes, a `double` 8, and that does not compile.
     * - !![#casts]
     */
    void convert_or_keep_the_bits() {
        print_function_header();

        const float f{1.0f};
        cout << " 1| value_of(1.0f)=" << value_of(f) << ", bits_of(1.0f)=0x" << hex << bits_of(f) << dec << '\n';
        const uint32_t bits{0x40490fdb};
        cout << " 2| bit_cast<float>(0x40490fdb)=" << bit_cast<float>(bits) << '\n';

        // const double d{bit_cast<double>(f)};  // compiler error: 4 bytes are not 8
    }

    /* --- `look_at_the_bytes` ---
     * `reinterpret_cast` keeps the address and changes only the type the compiler sees: `bytes` points to `n`, but as
     * to four `unsigned char`s. The one thing it may always do: look at the bytes of an object through an
     * `unsigned char*` (or a `char*`, or a `std::byte*`). `0x12345678` is stored with its lowest byte first,
     * `78 56 34 12` - little-endian, the byte order of x86-64 and of ARM64 in every common system.
     * `static_cast` refuses: an `int` is no `unsigned char`, so its pointer does not convert. And the old trick for
     * the bits of a `float`, `*reinterpret_cast<const uint32_t*>(&f)`, is undefined behavior: the compiler may assume
     * that a `uint32_t*` and a `float*` never point to the same object - the strict aliasing rule. gcc warns, as
     * Release. And it usually works - that is the dangerous case. `bit_cast` does it right.
     *
     * Example: Assume
     *      int store_and_read(int* i, float* f) { *i = 1; *f = 2.0f; return *i; }
     * Here the compiler doesn’t have to assume that a write through one changes what’s behind the other.
     * If you call it with
     *      store_and_read(&n, reinterpret_cast<float*>(&n))
     * the function may return the wrong number. The program broke the compiler’s assumption, and the optimized code
     * is "wrong" in a perfectly legal way.
     *
     * And now the house rule "the address of a character is printed via `const void*`": for `cout`, an
     * `unsigned char*` is a C string, it prints characters up to the next `'\0'` (see previous snippets). The
     * `static_cast` to `const void*` says: the address, please.
     * - !![#strict-aliasing]
     */
    void look_at_the_bytes() {
        print_function_header();

        const int n{0x12345678};
        const unsigned char* bytes{bytes_of(&n)};
        cout << " 1| &n=" << &n << ", bytes=" << static_cast<const void*>(bytes) << '\n';
        cout << hex << " 2| " << static_cast<int>(bytes[0]) << ' ' << static_cast<int>(bytes[1]) << ' '
             << static_cast<int>(bytes[2]) << ' ' << static_cast<int>(bytes[3]) << dec << '\n';

        // const unsigned char* b{static_cast<const unsigned char*>(&n)};  // compiler error: invalid static_cast
        // const float f{1.0f};
        // const uint32_t bits{*reinterpret_cast<const uint32_t*>(&f)};    // undefined behavior: strict aliasing
    }

    /* --- .`twice` --- An old interface: it takes an `int*` without `const` - but it only reads. */
    int twice(int* p) { return 2 * *p; }

    /* --- .`raise` --- It promises not to change `*p` - and does, with a `const_cast`. */
    void raise(const int* p) { ++*without_const(p); }

    /* --- `global_limit` --- A constant outside of any function: static storage, see previous snippets. */
    const int global_limit{42};

    /* --- `take_the_const_away` ---
     * `const_cast` takes the `const` away from the type of a pointer or a reference - no instruction, the address
     * stays. Two cases where that is fine:
     * - `twice` wants an `int*`, and `limit` is a `const int`. `twice` does not write, so nothing happens to `limit`.
     *   That is the one honest use case: an interface that forgot its `const`, typically old C code.
     * - `counter` is not `const` - only `raise` looks at it through a pointer to `const`. The write through the cast
     *   is fine: the object was writable all along. Fine, but rude: `raise` breaks the promise of its own signature.
     *
     * The same `raise`, the same cast, the same write - but now the object itself is `const`, and that is undefined
     * behavior, cast or no cast. `raise` cannot tell the difference: it sees an address, not the object behind it.
     * So the compiler cannot refuse the cast - it has to trust you, and it does: it relies on a `const` object never
     * changing. What happened with gcc and clang, as Debug and as Release:
     * - `number`: a `const int` initialized with 42 is a constant, so the compiler has put the 42 into the
     *   instructions, even as Debug. The write happens - in memory. `number` prints 42, `*&limit` reads the memory and
     *   prints 43. One object, two values.
     * - `global_limit`: its place is in a part of memory that the operating system has marked as read-only. The
     *   processor stops the write: exit status 139, the same signal as for a null pointer (see previous snippets). Not
     *   so with clang as Release: it removed the write - it cannot happen in a correct program - and printed 42.
     * Neither a crash nor a wrong value is promised - that is what undefined means. The rule behind it is simple: a
     * `const` object is never written, not even through a `const_cast`.
     * - !![#undefined-behavior]
     */
    void take_the_const_away() {
        print_function_header();

        const int limit{21};
        cout << " 1| twice(limit)=" << twice(const_cast<int*>(&limit)) << '\n';
        // twice(&limit);                   // compiler error: no conversion from `const int*` to `int*`

        int counter{23};
        raise(&counter);
        cout << " 2| counter=" << counter << '\n';

        const int number{42};
        raise(&number);
        cout << " 3| number=" << number << ", internally: " << *(&number) << '\n';

        // raise(&global_limit);            // undefined behavior: `global_limit` is const

        /* -- .Q&A -- !![One object, two values: how can `limit` and `*&limit` differ?](#a-707) */
    }

}

/* --- The casts at a glance ---
 *   cast                changes                     in the machine              checks
 *   `static_cast`       the value                   an instruction, or nothing  the compiler: does it make sense?
 *   `bit_cast`          the type, keeps the bits    a copy between registers    the compiler: the same size?
 *   `reinterpret_cast`  the type of a pointer       nothing                     nothing
 *   `const_cast`        the `const` of a pointer    nothing                     nothing
 *   `(T)x`              one of the above - the compiler picks it, not you
 * Two more: `dynamic_cast` checks at run time whether an object is of a derived class - it needs `virtual`, see
 * future units. And a cast to or from a class type is a call: a constructor, or a conversion operator.
 * - !![#conversion-operator]
 */

/* --- `main` --- */
int main() {
    convert_or_keep_the_bits();
    look_at_the_bytes();
    take_the_const_away();

    return EXIT_SUCCESS;
}
