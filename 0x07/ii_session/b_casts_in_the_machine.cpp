// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A conversion either computes a new value - that is an instruction - or keeps the bits and changes only how the
 *   compiler reads them - that is nothing at all.
 * - Integers: a wider type gets a sign extension; `int` to `unsigned` keeps the bits; a narrower type keeps the lower
 *   bits.
 * - `double` to `int` is an instruction of its own, and it truncates toward zero. Out of range, it is undefined
 *   behavior - and x86-64 and ARM64 answer differently.
 * - `std::bit_cast` (C++20) keeps the bits of a value and gives them another type. `reinterpret_cast` and `const_cast`
 *   change only the type of a pointer.
 * - `const_cast` does not make an object writable: writing to an object that is `const` is undefined behavior.
 * - The C-style cast tries the named casts one after the other - and may pick one you did not ask for.
 * - A conversion to a class type, or from one, is a call.
 * - Every line with undefined behavior below is commented out. Remove the `//` one at a time, run it as Debug and as
 *   Release - and put the `//` back.
 */

#include <iostream>
#include <cstdint>
#include <bit>                              // for bit_cast
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::hex, std::dec, std::uint8_t, std::uint32_t, std::bit_cast;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The next functions are for Compiler Explorer, below: each one does exactly one conversion. As in previous snippets,
 * they keep their own names in the assembly.
 */

/* --- Integer conversions --- */
long long to_long_long(const int n) { return static_cast<long long>(n); }
unsigned to_unsigned(const int n) { return static_cast<unsigned>(n); }
int to_int(const long long n) { return static_cast<int>(n); }
unsigned to_byte(const int n) { return static_cast<uint8_t>(n); }

/* --- Floating-point conversions --- */
int double_to_int(const double d) { return static_cast<int>(d); }
double int_to_double(const int n) { return static_cast<double>(n); }
uint32_t value_of(const float f) { return static_cast<uint32_t>(f); }
uint32_t bits_of(const float f) { return bit_cast<uint32_t>(f); }

/* --- Pointer conversions --- */
const unsigned char* bytes_of(const int* p) { return reinterpret_cast<const unsigned char*>(p); }
int* without_const(const int* p) { return const_cast<int*>(p); }

namespace {

    /* --- `convert_integers` ---
     * -5 is `0xfffffffb` in 32 bits - two's complement. As a `long long`, the value stays -5: the sign bit is copied
     * into the 32 new bits, a sign extension. As an `unsigned`, the bits stay the same - and the same bits, read
     * without a sign, are 4294967291. From a wider type to a narrower one, the upper bits are dropped: 5'000'000'000 is
     * `0x12a05f200`, and the lower 32 bits are 705032704; 300 is `0x12c`, and its lowest byte is 44. For signed
     * targets this is defined since C++20 - before, it was up to the compiler.
     * `cout` prints a `uint8_t` as a character: it is an `unsigned char`. 321 keeps its lowest byte, 65 - an `A`. To
     * see the number, cast it once more.
     * - !![#integer-conversions]
     */
    void convert_integers() {
        print_function_header();

        const int n{-5};
        cout << " 1| n=" << n << ", as long long=" << to_long_long(n) << ", as unsigned=" << to_unsigned(n) << '\n';
        cout << hex << " 2| bits: n=" << n << ", as long long=" << to_long_long(n) << ", as unsigned=" << to_unsigned(n)
             << dec << '\n';
        cout << " 3| to_int(5'000'000'000)=" << to_int(5'000'000'000) << ", to_byte(300)=" << to_byte(300) << '\n';
        const uint8_t byte{static_cast<uint8_t>(321)};
        cout << " 4| byte=" << byte << ", as a number: " << static_cast<int>(byte) << '\n';
    }

    /* --- `convert_floating_point` ---
     * `double` to `int` cuts off the fraction - toward zero, so -2.99 becomes -2. The other direction is exact for
     * every `int`. A `float` is not: it has 24 bits for the digits, so 16'777'217, which needs 25, becomes 16'777'216.
     * A value that does not fit into an `int` - 1e10 - is undefined behavior. The machine gives an answer anyway, and
     * not the same one everywhere, see the Q&A.
     */
    void convert_floating_point() {
        print_function_header();

        cout << " 1| double_to_int(2.99)=" << double_to_int(2.99) << ", double_to_int(-2.99)=" << double_to_int(-2.99)
             << ", int_to_double(7)=" << int_to_double(7) << '\n';
        const float as_float{static_cast<float>(16'777'217)};
        cout << " 2| 16'777'217 as a float: " << static_cast<long long>(as_float) << '\n';
        // cout << double_to_int(1e10);     // undefined behavior: 1e10 does not fit into an `int`

        /* -- .Q&A -- !![Remove the `//`: what do you get as Debug and as Release - on x86-64 and on ARM64?](#a-706) */
    }

    /* --- `keep_the_bits` ---
     * Two ways to get a `uint32_t` from the `float` 1.0. `static_cast` converts the value: 1. `bit_cast` keeps the four
     * bytes as they are and reads them as an integer: `0x3f800000` - the sign, the exponent and the digits of 1.0, as
     * the processor stores them (see future snippets).
     * `bit_cast` needs two types of the same size that can be copied byte by byte. The old trick,
     * `*reinterpret_cast<const uint32_t*>(&f)`, is undefined behavior: the compiler may assume that a `uint32_t*` and a
     * `float*` never point to the same object - the strict aliasing rule - and reorder reads and writes on that
     * assumption. It usually works. That is the dangerous case.
     * - !![#strict-aliasing]
     */
    void keep_the_bits() {
        print_function_header();

        const float f{1.0f};
        cout << " 1| value_of(1.0f)=" << value_of(f) << ", bits_of(1.0f)=0x" << hex << bits_of(f) << dec << '\n';
        // const uint32_t bits{*reinterpret_cast<const uint32_t*>(&f)};  // undefined behavior: strict aliasing
    }

    /* --- `look_at_the_bytes` ---
     * The one thing `reinterpret_cast` may always do: look at the bytes of an object through an `unsigned char*` (or a
     * `char*`, or a `std::byte*`). The address stays the same - only its type changes. `0x12345678` is stored with its
     * lowest byte first: `78 56 34 12`. That is little-endian, the byte order of x86-64 and of ARM64 in every common
     * system; more in future snippets.
     * And why the house rule "the address of a character is printed via `const void*`"? For `cout`, a `char*` - and
     * an `unsigned char*` - is a C string: it prints characters up to the next `'\0'` (see previous snippets). The
     * `static_cast` to `const void*` says: the address, please.
     */
    void look_at_the_bytes() {
        print_function_header();

        const int n{0x12345678};
        const unsigned char* bytes{bytes_of(&n)};
        cout << " 1| &n=" << &n << ", bytes=" << static_cast<const void*>(bytes) << '\n';
        cout << hex << " 2| " << static_cast<int>(bytes[0]) << ' ' << static_cast<int>(bytes[1]) << ' '
             << static_cast<int>(bytes[2]) << ' ' << static_cast<int>(bytes[3]) << dec << '\n';
    }

    /* --- `global_limit` --- A constant outside of any function: static storage, see previous snippets. */
    const int global_limit{42};

    /* --- `try_to_write_a_constant` ---
     * `const_cast` takes the `const` away from the type of a pointer - no instruction. Through `p`, `counter` looked
     * `const`, but it is not: writing to it is fine. `limit` is `const` itself, and writing to it is undefined
     * behavior, cast or no cast. What happened with gcc and clang, as Debug and as Release:
     * - `limit`: the write happens - but the compiler knows that a `const int` initialized with 42 is 42, and has put
     *   the 42 into the instructions. `limit` prints 42, `*&limit` reads the memory and prints 43.
     * - `global_limit`: its place is in a part of memory that the operating system has marked as read-only. The write
     *   is stopped: exit status 139, the same signal as for a null pointer (see previous snippets). Not so with clang
     *   as Release: it removed the write - it cannot happen in a correct program - and the program printed 42.
     * `const_cast` is for one case only: a function that takes a pointer without `const` and does not write through
     * it - an old C interface.
     * - !![#casts]
     */
    void try_to_write_a_constant() {
        print_function_header();

        int counter{23};
        const int* p{&counter};
        *without_const(p) = 24;
        cout << " 1| counter=" << counter << '\n';

        const int limit{42};
        // *without_const(&limit) = 43;     // undefined behavior: `limit` is const
        cout << " 2| limit=" << limit << ", *&limit=" << *&limit << '\n';
        // *without_const(&global_limit) = 43;  // undefined behavior: `global_limit` is const
        cout << " 3| global_limit=" << global_limit << '\n';

        /* -- .Q&A -- !![One object, two values: how can `limit` and `*&limit` differ?](#a-707) */
    }

    /* --- `compare_with_a_c_style_cast` ---
     * `(T)x` asks for no cast in particular. The compiler tries `const_cast`, `static_cast`, then both, then
     * `reinterpret_cast`, then that with a `const_cast` - and takes the first that compiles. `(const long long*)&d` is
     * a `reinterpret_cast`: a `long long*` to a `double`, and reading through it would break the aliasing rule. The
     * `static_cast` is refused - which is the point of `static_cast`. `(int*)&global_limit` is a `const_cast` that
     * nobody sees. C++ Insights shows which cast the compiler chose. `-Wold-style-cast` finds every C-style cast.
     */
    void compare_with_a_c_style_cast() {
        print_function_header();

        const double d{2.5};
        const long long* p{(const long long*)&d};
        // const long long* q{static_cast<const long long*>(&d)};  // compiler error: invalid static_cast
        const int* q{(int*)&global_limit};
        cout << " 1| &d=" << &d << ", p=" << p << ", q=" << q << '\n';
    }

    /* --- `ratio` ---
     * A fraction that can be converted to a `double`. `operator double()` is a conversion operator: it has no return
     * type in front, the name says it. `explicit` means: only when asked for, with a `static_cast` - as for an
     * `explicit` constructor, the other direction (see previous snippets).
     * - !![#conversion-operator]
     */
    class ratio {
    public:
        ratio(const long long numerator, const long long denominator)
            : numerator_{numerator}, denominator_{denominator} {}

        explicit operator double() const {
            return static_cast<double>(numerator_) / static_cast<double>(denominator_);
        }

    private:
        long long numerator_;
        long long denominator_;
    };

    /* --- `convert_by_a_call` ---
     * `static_cast<double>(third)` calls `third.operator double()` - C++ Insights writes it that way. As Release, the
     * call is inlined, and what is left are the instructions inside: two conversions and a division.
     * Without `explicit`, `const double d = third;` would compile, too - and so would `third * 3.0`, with a call nobody
     * sees.
     */
    void convert_by_a_call() {
        print_function_header();

        const ratio third{1, 3};
        cout << " 1| static_cast<double>(third)=" << static_cast<double>(third) << '\n';
        // const double d = third;          // compiler error: the conversion is explicit
    }

}

/* --- The machine code of a conversion ---
 * Paste the functions from `to_long_long` to `without_const` into Compiler Explorer, with `#include <cstdint>` and
 * `#include <bit>` in front and `using std::uint8_t, std::uint32_t, std::bit_cast;`. `-O2`, x86-64 gcc and ARM64 gcc:
 * - `to_long_long`: `movsx rax, edi` (clang: `movsxd`) - `sxtw x0, w0` on ARM64. The sign extension.
 * - `to_unsigned` and `to_int`: `mov eax, edi` - on ARM64 only `ret`. On x86-64, the `mov` only brings the argument
 *   from its register into the result register; a function that returns its `int` unchanged has the same `mov`. The
 *   bits are not touched.
 * - `to_byte`: `movzx eax, dil` - `and w0, w0, 255`. The lowest byte, the rest set to 0.
 * - `double_to_int`: `cvttsd2si eax, xmm0` - `fcvtzs w0, d0`: convert with truncation, toward zero. A `double` arrives
 *   in a floating-point register, `xmm0` or `d0`, not in `edi` or `w0`.
 * - `int_to_double`: `cvtsi2sd xmm0, edi` (gcc clears `xmm0` first) - `scvtf d0, w0`.
 * - `value_of`: `cvttss2si rax, xmm0` - `fcvtzu w0, s0`. `bits_of`: `movd eax, xmm0` - `fmov w0, s0`, a copy from the
 *   floating-point register to an integer register, nothing converted.
 * - `bytes_of` and `without_const`: `mov rax, rdi` - on ARM64 only `ret`. The pointer goes back as it came.
 * Three answers, then: an instruction that computes (`movsx`, `cvttsd2si`), an instruction that only copies bits
 * (`movd`), or nothing at all. The casts that change only the type of a pointer never cost anything - and never check
 * anything either.
 */

/* --- `main` --- */
int main() {
    convert_integers();
    convert_floating_point();
    keep_the_bits();
    look_at_the_bytes();
    try_to_write_a_constant();
    compare_with_a_c_style_cast();
    convert_by_a_call();

    return EXIT_SUCCESS;
}
