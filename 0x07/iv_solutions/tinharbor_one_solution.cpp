// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Tin Harbor', see ../tasks.md.

#include <iostream>
#include <cstdint>
#include <bit>
#include <cstdlib>

using std::cout;

// The conversions of lines 1, 3, 4, 6 and 11 as functions with a parameter - for Compiler Explorer, `-O2`:
// - line 1: `cvttsd2si eax, xmm0` on x86-64, `fcvtzs w0, d0` on ARM64 - an instruction, the value is converted.
// - line 3: `mov eax, edi` - on ARM64 only `ret`. Nothing: the same bits.
// - line 4: `movsx rax, edi`, `sxtw x0, w0` - an instruction, the sign extension.
// - line 6: `mov eax, edi` - on ARM64 only `ret`. Nothing: the lower 32 bits.
// - line 11: `movd eax, xmm0`, `fmov w0, s0` - a copy of the bits into an integer register.
int line_1(const double d) { return static_cast<int>(d); }
unsigned line_3(const int n) { return static_cast<unsigned>(n); }
long long line_4(const int n) { return static_cast<long long>(n); }
int line_6(const long long n) { return static_cast<int>(n); }
std::uint32_t line_11(const float f) { return std::bit_cast<std::uint32_t>(f); }

int main() {
    cout << static_cast<int>(7.99) << '\n';                                     //  1: 7 - toward zero
    cout << static_cast<int>(-7.99) << '\n';                                    //  2: -7 - toward zero, not -8
    cout << static_cast<unsigned>(-1) << '\n';                                  //  3: 4294967295 - all bits set
    cout << static_cast<long long>(-1) << '\n';                                 //  4: -1 - sign extension
    cout << static_cast<int>(static_cast<std::uint8_t>(321)) << '\n';           //  5: 65 - 321 is 0x141, one byte: 0x41
    cout << static_cast<int>(3'000'000'000LL) << '\n';                          //  6: -1294967296 - the lower 32 bits
    cout << static_cast<long long>(static_cast<float>(16'777'217)) << '\n';     //  7: 16777216 - 24 bits of digits
    cout << 7 / 2 * 2.0 << '\n';                                                //  8: 6 - 7 / 2 is an int division
    cout << 7 / 2.0 * 2 << '\n';                                                //  9: 7
    cout << (-1 < 1u) << '\n';                                                  // 10: 0 - -1 becomes an unsigned
    cout << std::hex << std::bit_cast<std::uint32_t>(-0.0f) << std::dec << '\n';  // 11: 80000000 - only the sign bit

    // Warnings with `-Wall -Wextra -Wconversion`: only line 10 - gcc about the comparison of different signedness,
    // clang about the conversion of -1 to `unsigned`. The other lines are explicit casts - you asked for them.
    // Extension: for `(unsigned)-1`, C++ Insights shows `static_cast<unsigned int>(-1)`, and `(unsigned*)&x` for an
    // `int x` becomes `reinterpret_cast<unsigned int *>(&x)`.

    cout << line_1(7.99) << ' ' << line_3(-1) << ' ' << line_4(-1) << ' ' << line_6(3'000'000'000LL) << ' ' << std::hex
         << line_11(-0.0f) << std::dec << '\n';

    return EXIT_SUCCESS;
}
