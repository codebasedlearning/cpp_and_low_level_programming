// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A type is a promise about bits: the same four bytes are 1065353216 as an `int` and 1.0 as a `float`.
 * - Integers: two's complement - `-x` is `~x + 1`, and the highest bit says "negative".
 * - Byte order: the lowest byte first (little-endian) on x86-64 and ARM64; the network and some file formats store the
 *   highest first. `std::endian` asks, `std::byteswap` turns.
 * - Floating point (IEEE 754): a sign, an exponent and a mantissa - which is why 0.1 cannot be stored exactly, and
 *   `0.1 + 0.2 != 0.3`.
 * - One place, several types: a `union` stores one of them and forgets which; a `std::variant` remembers - in a tag
 *   next to the value.
 */

#include <iostream>
#include <iomanip>
#include <array>
#include <initializer_list>
#include <bit>
#include <cstdint>
#include <limits>
#include <string>
#include <variant>
#include <version>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::array, std::uint32_t, std::uint64_t, std::numeric_limits;


/* ---- Content ---- */

namespace {

    /* --- `bytes_of` ---
     * The bytes of any simple object, in memory order, as hex digits. `bit_cast` copies the bits into an array of
     * `unsigned char` - no pointer cast, no aliasing question (see previous snippets).
     */
    template <typename T>
    string bytes_of(const T& value) {
        const auto bytes{std::bit_cast<array<unsigned char, sizeof(T)>>(value)};
        constexpr char digits[]{"0123456789abcdef"};
        string text{};
        for (const unsigned char b : bytes) {
            text += digits[b >> 4];
            text += digits[b & 0x0f];
            text += ' ';
        }
        return text;
    }

    /* --- `show_twos_complement` ---
     * -1 is all bits set; the smallest `int` is only the highest bit. To negate, flip all bits and add 1 - one adder
     * for signed and unsigned numbers, which is why every processor does it this way (and C++20 finally says so).
     * - !![#integer-conversions]
     */
    void show_twos_complement() {
        print_function_header();

        const int one{1};
        const int minus_one{-1};
        const int smallest{numeric_limits<int>::min()};
        cout << " 1| 1: " << bytes_of(one) << "  -1: " << bytes_of(minus_one) << " min: " << bytes_of(smallest) << '\n';
        cout << " 2| ~5 + 1 = " << (~5 + 1) << '\n';
    }

    /* --- `ask_for_the_byte_order` ---
     * The bytes of `0x12345678` in memory: `78 56 34 12` - the lowest first, little-endian, as seen in previous
     * snippets. `std::endian::native` (C++20) tells the program what it runs on. A number that goes into a file or over
     * the network in a fixed order - the network's is big-endian - is turned with `std::byteswap` (C++23) where the
     * orders differ.
     * - !![#byte-order]
     */
    void ask_for_the_byte_order() {
        print_function_header();

        const uint32_t n{0x12345678};
        cout << " 1| 0x12345678 in memory: " << bytes_of(n) << '\n';
        cout << " 2| little-endian? " << (std::endian::native == std::endian::little) << '\n';
#if defined(__cpp_lib_byteswap)
        cout << " 3| byteswap: " << std::hex << std::byteswap(n) << std::dec << ", bytes " << bytes_of(std::byteswap(n))
             << '\n';
#else
        cout << " 3| `std::byteswap` is not available in this library version\n";
#endif
    }

    /* --- `take_a_float_apart` ---
     * A `float` is 32 bits: 1 sign bit, 8 bits exponent (stored plus 127), 23 bits mantissa - the digits after a
     * leading 1 that is not stored. 1.0 is `+1.0 * 2^0`: sign 0, exponent 127, mantissa 0 - `0x3f800000`. -2.5 is
     * `-1.01 (binary) * 2^1`: `0xc0200000`. 0.1 has no finite binary fraction, like 1/3 in decimal: the mantissa is
     * rounded, and the last hex digit shows it.
     * - !![#floating-point]
     */
    void take_a_float_apart() {
        print_function_header();

        for (const float f : {1.0f, -2.5f, 0.1f}) {
            const uint32_t bits{std::bit_cast<uint32_t>(f)};
            cout << " 1| " << std::setw(4) << f << " = 0x" << std::hex << bits << std::dec << ": sign " << (bits >> 31)
                 << ", exponent " << ((bits >> 23) & 0xff) << " - 127, mantissa 0x" << std::hex << (bits & 0x7fffff)
                 << std::dec << '\n';
        }
    }

    /* --- `add_one_tenth` ---
     * 0.1 and 0.2 are both rounded, and so is their sum: it ends one bit above the nearest `double` to 0.3.
     * Printed with 17 digits, the difference shows. Compare floating-point numbers with a tolerance, never with `==`.
     * Infinity and NaN are bit patterns, too: all exponent bits set - and NaN is not even equal to itself.
     */
    void add_one_tenth() {
        print_function_header();

        const double sum{0.1 + 0.2};
        cout << std::setprecision(17) << " 1| 0.1 + 0.2 = " << sum << ", == 0.3? " << (sum == 0.3) << '\n';
        cout << " 2| sum: 0x" << std::hex << std::bit_cast<uint64_t>(sum) << ", 0.3: 0x"
             << std::bit_cast<uint64_t>(0.3) << std::dec << std::setprecision(6) << '\n';

        const float inf{numeric_limits<float>::infinity()};
        const float nan{numeric_limits<float>::quiet_NaN()};
        cout << " 3| inf: " << bytes_of(inf) << " nan: " << bytes_of(nan) << " nan == nan? " << (nan == nan) << '\n';
    }

    /* --- `compare_union_and_variant` ---
     * A `union` puts its members at the same address: it is as big as the largest one, and it does not know which one
     * holds a value. Reading another member than the last one written is undefined in C++ (C allows it) - to see the
     * bits of a `float`, use `bit_cast`, as above. A `std::variant` adds what the `union` lacks: an index that says
     * which alternative is in it - one more byte, padded to the alignment of the largest alternative (16 bytes for an
     * `int` or a `double`, with libstdc++). `std::get` checks the index and throws on a mismatch.
     * - !![#optional]
     */
    void compare_union_and_variant() {
        print_function_header();

        union number {
            int i;
            float f;
        };
        number n{};
        n.f = 1.0f;                         // now `f` holds the value; reading `n.i` would be undefined
        cout << " 1| sizeof(number)=" << sizeof(number) << ", n.f=" << n.f << '\n';

        std::variant<int, double> v{42};
        cout << " 2| sizeof(variant<int, double>)=" << sizeof(v) << ", index=" << v.index() << ", int="
             << std::get<int>(v) << '\n';
        v = 2.5;
        cout << " 3| index=" << v.index() << ", holds a double? " << std::holds_alternative<double>(v) << '\n';
    }

}

/* --- `main` --- */
int main() {
    show_twos_complement();
    ask_for_the_byte_order();
    take_a_float_apart();
    add_one_tenth();
    compare_union_and_variant();

    return EXIT_SUCCESS;
}
