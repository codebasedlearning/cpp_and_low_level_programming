// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Which cast for what: `static_cast` for numbers, enums and `void*`; `reinterpret_cast` to look at bytes;
 *   `std::bit_cast` for the bits of a value; `const_cast` almost never; `dynamic_cast` in the next unit. No C-style
 *   casts.
 * - Often the best cast is none: the right type from the start, `std::lround`, `std::as_const`, `std::to_underlying`.
 * - Signed and unsigned in one comparison: `-1 < 1u` is `false`. `std::cmp_less` compares the values.
 * - `explicit` conversion operators - and `explicit operator bool`, which still works in an `if`.
 * - Enums: chosen values, `using enum` (C++20), a name for every value, all values in an array, a checked conversion
 *   from a number.
 */

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <optional>
#include <cstdint>
#include <bit>
#include <cmath>                            // for lround
#include <utility>                          // for as_const, to_underlying, cmp_less
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::hex, std::dec, std::string, std::string_view, std::vector, std::array, std::optional;
using std::uint16_t, std::uint64_t;


/* ---- Content ---- */

namespace {

    /* --- `legacy_length` ---
     * An old C-style interface: it takes a `char*` without `const`, although it only reads. The one honest use of
     * `const_cast`: calling such a function with a text you must not change - if you know that it does not write.
     */
    int legacy_length(char* text) {
        int length{0};
        while (text[length] != '\0') {
            ++length;
        }
        return length;
    }

    /* --- `choose_a_cast` ---
     * One line per job:
     * - `static_cast<double>(n)` - a number to another number.
     * - `static_cast<int*>(raw)` - a `void*` back to the type it came from. `void*` is an address without a type, as
     *   `malloc` returns it (see previous snippets).
     * - `reinterpret_cast<const unsigned char*>(&n)` - the bytes of an object.
     * - `std::bit_cast<uint64_t>(1.0)` - the bits of a `double`, as a number.
     * - `const_cast<char*>(...)` - for `legacy_length`, see above.
     * - !![#casts]
     */
    void choose_a_cast() {
        print_function_header();

        int n{7};
        cout << " 1| static_cast<double>(n) / 2=" << static_cast<double>(n) / 2 << '\n';

        void* raw{&n};
        const int* back{static_cast<int*>(raw)};
        cout << " 2| *back=" << *back << '\n';

        const unsigned char* bytes{reinterpret_cast<const unsigned char*>(&n)};
        cout << " 3| first byte of n=" << static_cast<int>(bytes[0]) << '\n';

        cout << " 4| bits of 1.0=0x" << hex << std::bit_cast<uint64_t>(1.0) << dec << '\n';

        const string title{"Kind of Blue"};
        cout << " 5| legacy_length=" << legacy_length(const_cast<char*>(title.c_str())) << '\n';
    }

    /* --- `avoid_a_cast` ---
     * - `static_cast<int>(2.5)` cuts off, `std::lround(2.5)` rounds - half away from zero - and returns a `long`.
     * - `std::as_const(v)` gives a `const` reference: the `const` version of a member function is called, without a
     *   `const_cast` in the other direction.
     * - `std::to_underlying(e)` (C++23) instead of `static_cast<int>(e)`: the number in the right type, whatever the
     *   underlying type is.
     */
    void avoid_a_cast() {
        print_function_header();

        cout << " 1| static_cast<int>(2.5)=" << static_cast<int>(2.5) << ", std::lround(2.5)=" << std::lround(2.5)
             << ", std::lround(-2.5)=" << std::lround(-2.5) << '\n';

        vector<int> v{1, 2, 3};
        const vector<int>& view{std::as_const(v)};
        cout << " 2| view.front()=" << view.front() << '\n';
    }

    /* --- `compare_signed_and_unsigned` ---
     * In `-1 < 1u`, both operands must get one type first, and the rules pick `unsigned`: -1 becomes 4294967295, and
     * that is not less than 1. The same trap: `v.size() - 1` for an empty `vector` is not -1, but the largest
     * `size_t`. gcc warns about the comparison with `-Wall`, clang with `-Wextra` (`-Wsign-compare`). `std::cmp_less`
     * (C++20) compares the values, as a mathematician would.
     */
    void compare_signed_and_unsigned() {
        print_function_header();

        const int minus_one{-1};
        const unsigned one{1};
        // cout << (minus_one < one);       // warning: comparison of integers of different signs - and false
        cout << " 1| std::cmp_less(-1, 1u)=" << std::cmp_less(minus_one, one) << '\n';

        const vector<int> empty;
        cout << " 2| empty.size() - 1=" << empty.size() - 1 << '\n';

        /* -- .Q&A -- !![Why is `-1 < 1u` false?](#a-710) */
    }

    /* --- `measurement` ---
     * A value that may be missing. `explicit operator bool` answers "is there one?" - and because it is `explicit`, it
     * works only where a condition is expected: `if (m)`, `!m`, `m && ...`. `const bool b = m;` and `m + 1` do not
     * compile - without `explicit`, `m + 1` would silently be `1` or `2`.
     * - !![#conversion-operator]
     */
    class measurement {
    public:
        measurement() = default;
        explicit measurement(const double value) : value_{value}, known_{true} {}

        explicit operator bool() const { return known_; }
        double value() const { return value_; }

    private:
        double value_{0.0};
        bool known_{false};
    };

    /* --- `convert_explicitly` --- A condition is allowed, a silent conversion is not. */
    void convert_explicitly() {
        print_function_header();

        const measurement none;
        const measurement some{21.5};
        for (const measurement& m : {none, some}) {
            if (m) {
                cout << " 1| value " << m.value() << '\n';
            } else {
                cout << " 2| no value\n";
            }
        }
        // const bool b = some;             // compiler error: the conversion is explicit
    }

    /* --- `http_status` --- Values chosen by hand, and a type that fits them. */
    enum class http_status : uint16_t { ok = 200, not_found = 404, teapot = 418 };

    /* --- `suit` --- The four suits once more. */
    enum class suit { clubs, diamonds, hearts, spades };

    /* --- `all_suits` --- Every value, for a loop: an enum cannot be iterated by itself. */
    constexpr array<suit, 4> all_suits{suit::clubs, suit::diamonds, suit::hearts, suit::spades};

    /* --- `name_of` ---
     * `using enum suit;` (C++20) brings the names into this function: `clubs` instead of `suit::clubs`, but only here.
     * No `default:` - so `-Wswitch` warns if a value is added to `suit` and forgotten here.
     */
    string_view name_of(const suit s) {
        using enum suit;
        switch (s) {
            case clubs:    return "clubs";
            case diamonds: return "diamonds";
            case hearts:   return "hearts";
            case spades:   return "spades";
        }
        return "?";
    }

    /* --- `suit_from` ---
     * A checked conversion from a number - from a file, say. `static_cast<suit>(n)` would accept every `int`; here only
     * 0 to 3 give a `suit`, everything else "none".
     */
    optional<suit> suit_from(const int n) {
        if (n < 0 || n > 3) {
            return std::nullopt;
        }
        return static_cast<suit>(n);
    }

    /* --- `use_enums` --- Chosen values, all names, and a number checked before it becomes a `suit`. */
    void use_enums() {
        print_function_header();

        const http_status status{http_status::teapot};
        cout << " 1| status=" << std::to_underlying(status) << ", sizeof=" << sizeof(http_status) << '\n';

        cout << " 2|";
        for (const suit s : all_suits) {
            cout << ' ' << name_of(s);
        }
        cout << '\n';

        for (const int n : {2, 7}) {
            if (const optional<suit> s{suit_from(n)}) {
                cout << " 3| " << n << " is " << name_of(*s) << '\n';
            } else {
                cout << " 4| " << n << " is no suit\n";
            }
        }
    }

}

/* --- `main` --- */
int main() {
    choose_a_cast();
    avoid_a_cast();
    compare_signed_and_unsigned();
    convert_explicitly();
    use_enums();

    return EXIT_SUCCESS;
}
