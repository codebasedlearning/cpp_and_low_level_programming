// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The bitwise operators `&`, `|`, `^`, `~` work on every bit at once; `<<` and `>>` move all bits. Each of them is a
 *   single instruction.
 * - Masks: set, clear, toggle and test a bit - and take a field out of a packed number.
 * - The traps: a `uint8_t` becomes an `int` before any arithmetic (integer promotion), a shift by the width or more is
 *   undefined, and the right shift of a negative number keeps the sign.
 * - `std::byte`: a byte that only knows bit operations. `<bit>`: `popcount`, `countr_zero`, `has_single_bit`, `rotl`.
 * - The compiler knows the tricks, too: `x * 8` becomes a shift, `x % 8` for an `unsigned` an `and`.
 */

#include <iostream>
#include <bitset>
#include <cstdint>
#include <cstddef>                          // for std::byte
#include <bit>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::bitset, std::uint8_t, std::uint16_t, std::uint32_t;


/* ---- Content ---- */

namespace {

    /* --- `combine_bits` ---
     * `&` keeps a bit where both have it, `|` where one of them has it, `^` where exactly one has it, `~` flips all.
     * `bitset<8>` prints the bits of a number - the highest one first, as we write numbers. `0b` writes a number in
     * binary, `'` separates digits for the reader.
     * - !![#bit-operations]
     */
    void combine_bits() {
        print_function_header();

        const uint8_t a{0b1100'1010};
        const uint8_t b{0b1010'0110};
        cout << " 1| a     =" << bitset<8>{a} << ", b=" << bitset<8>{b} << '\n';
        cout << " 2| a & b =" << bitset<8>(a & b) << '\n';
        cout << " 3| a | b =" << bitset<8>(a | b) << '\n';
        cout << " 4| a ^ b =" << bitset<8>(a ^ b) << '\n';
        cout << " 5| ~a    =" << bitset<8>(static_cast<uint8_t>(~a)) << '\n';
    }

    /* --- `promote_small_types` ---
     * Before `&`, `+`, `~` or `<<`, a `uint8_t` (and every type smaller than `int`) becomes an `int` - integer
     * promotion. `a & b` above was an `int`, `~a` too: as an `int`, `~0b1100'1010` is -203, with 24 more bits set in
     * front. That is why `~a` needs the `static_cast` back to `uint8_t`, and why `-Wconversion` complains when the
     * result goes into a `uint8_t` without one.
     */
    void promote_small_types() {
        print_function_header();

        const uint8_t a{0b1100'1010};
        cout << " 1| sizeof(a)=" << sizeof(a) << ", sizeof(a & a)=" << sizeof(a & a) << ", sizeof(~a)=" << sizeof(~a)
             << '\n';
        cout << " 2| ~a as int=" << ~a << ", as uint8_t=" << static_cast<int>(static_cast<uint8_t>(~a)) << '\n';
    }

    /* --- `shift_bits` ---
     * `x << n` moves the bits `n` places to the left and fills in zeros - `x * 2^n`, as long as nothing falls off.
     * `x >> n` moves them to the right - `x / 2^n` for an `unsigned`. For a negative `int`, `>>` copies the sign bit
     * (defined since C++20, what every compiler did before): -8 >> 1 is -4.
     * - `1u << n` is the number with only bit `n` set - the start of every mask.
     * - A shift by the width of the type or more - `1u << 32` - is undefined, and the sanitizer of unit 0x01 reports
     *   it. x86-64 and ARM64 use only the lowest 5 bits of the count for a 32-bit shift - so the machine would compute
     *   `1u << 0`, if the compiler leaves the shift to it.
     */
    void shift_bits() {
        print_function_header();

        const uint32_t x{5};
        cout << " 1| 5 << 3=" << (x << 3) << ", 40 >> 2=" << (40u >> 2) << ", -8 >> 1=" << (-8 >> 1) << '\n';
        for (unsigned n{0}; n < 4; ++n) {
            cout << " 2| 1u << " << n << " = " << bitset<8>(1u << n) << '\n';
        }
        // cout << (1u << 32);              // undefined: the shift is as wide as the type
    }

    /* --- `use_masks` ---
     * A set of flags in one number - here the permissions of a file, as Unix stores them: read 4, write 2, execute 1,
     * three bits each for the owner, the group and all others. `0755` is octal - one digit per three bits.
     * - set a bit: `mode |= mask`; clear it: `mode &= ~mask`; toggle it: `mode ^= mask`;
     * - test it: `(mode & mask) != 0`.
     */
    void use_masks() {
        print_function_header();

        constexpr unsigned owner_write{0200};
        constexpr unsigned others_execute{0001};
        unsigned mode{0755};
        cout << " 1| mode=" << bitset<9>{mode} << " (0755)\n";
        mode &= ~owner_write;
        cout << " 2| owner may write? " << ((mode & owner_write) != 0) << ", mode=" << bitset<9>{mode} << '\n';
        mode |= owner_write;
        mode ^= others_execute;
        cout << " 3| others may execute? " << ((mode & others_execute) != 0) << ", mode=" << bitset<9>{mode} << '\n';
    }

    /* --- `extract_fields` ---
     * A colour in 32 bits, `0xRRGGBBAA`: shift the field to the bottom, then mask the rest away. Packing is the way
     * back: shift each part into its place and `|` them together. File formats and network protocols are full of this.
     */
    void extract_fields() {
        print_function_header();

        const uint32_t colour{0x3366ccff};
        const uint32_t red{(colour >> 24) & 0xff};
        const uint32_t green{(colour >> 16) & 0xff};
        const uint32_t blue{(colour >> 8) & 0xff};
        cout << " 1| red=" << red << ", green=" << green << ", blue=" << blue << '\n';

        const uint32_t packed{(red << 24) | (green << 16) | (blue << 8) | 0x80};
        cout << " 2| packed=" << std::hex << packed << std::dec << '\n';
    }

    /* --- `use_std_byte` ---
     * `std::byte` is a byte, not a number and not a character: it has the bit operators and nothing else - no `+`, no
     * printing as a letter. For raw memory it says what it is; `to_integer` makes a number of it when needed.
     */
    void use_std_byte() {
        print_function_header();

        const std::byte b{0b0000'1111};
        const std::byte shifted{b << 2};
        // const std::byte sum{b + b};      // does not compile: no arithmetic for bytes
        cout << " 1| b << 2 = " << std::to_integer<int>(shifted) << ", b | 0x30 = "
             << std::to_integer<int>(b | std::byte{0x30}) << '\n';
    }

    /* --- `use_the_bit_header` ---
     * `<bit>` (C++20) names what programs used to write with tricks: how many bits are set, how many zeros at the end,
     * is it a power of two, rotate. The compiler picks the instruction if there is one: `popcount` is `cnt` on ARM64;
     * on x86-64, `popcnt` needs `-mpopcnt` or `-march=native` - otherwise gcc calls a library function and clang writes
     * the classic sequence of shifts and masks.
     */
    void use_the_bit_header() {
        print_function_header();

        const uint32_t x{0b0010'1000};
        cout << " 1| popcount=" << std::popcount(x) << ", countr_zero=" << std::countr_zero(x)
             << ", bit_width=" << std::bit_width(x) << '\n';
        cout << " 2| has_single_bit(64)=" << std::has_single_bit(64u) << ", bit_ceil(100)=" << std::bit_ceil(100u)
             << '\n';
        const uint16_t r{std::rotl(uint16_t{0x8001}, 1)};
        cout << " 3| rotl(0x8001, 1)=" << std::hex << r << std::dec << " - the top bit comes in at the bottom\n";
    }

}

/* --- What the compiler does with it ---
 * In Compiler Explorer, as Release (unit 0x05): `unsigned times8(unsigned x) { return x * 8; }` is a shift - `lsl` on
 * ARM64, a `lea` with a scale on x86-64. `x % 8` for an `unsigned` is `x & 7`. For an `int`, `% 8` takes a few more
 * instructions: -13 % 8 is -5, and a plain `and` would give 3. Write what you mean - `* 8`, `% 8` - and let the
 * compiler choose the bits; `unsigned` where negative numbers cannot occur makes its job simpler.
 */

/* --- `main` --- */
int main() {
    combine_bits();
    promote_small_types();
    shift_bits();
    use_masks();
    extract_fields();
    use_std_byte();
    use_the_bit_header();

    return EXIT_SUCCESS;
}
