// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An enum is a number: its underlying type - `int` by default for an `enum class`, or the type you choose.
 *   `sizeof` shows it, and a small one makes a struct small.
 * - `enum class` costs nothing: the same bytes and the same instructions as a plain `enum`. The difference is only in
 *   what the compiler allows.
 * - Every value of the underlying type is a valid value of the enum - with a name or without one.
 * - A `switch` over an enum becomes a table - with a range check for exactly those values without a name.
 * - An operator for an enum is a free function, like every other: `|` for two flags is one `or`.
 */

#include <iostream>
#include <cstdint>
#include <utility>                          // for to_underlying
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::uint8_t, std::to_underlying;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `suit`, `points_of`, `permission`, its operators and `combine` are for Compiler Explorer, below - as in previous
 * snippets, they keep their own names in the assembly.
 */

/* --- `suit` --- The four suits, in one byte: the underlying type is chosen after the `:`. */
enum class suit : uint8_t { clubs, diamonds, hearts, spades };

/* --- `points_of` --- A `switch` with one `case` for every value. */
int points_of(const suit s) {
    switch (s) {
        case suit::clubs:    return 12;
        case suit::diamonds: return 9;
        case suit::hearts:   return 10;
        case suit::spades:   return 11;
    }
    return 0;
}

/* --- `permission` ---
 * Flags: every value is one bit - 1, 2, 4 - so that several of them fit into one byte together. `read | write` is 3,
 * a value without a name.
 */
enum class permission : uint8_t { none = 0, read = 1, write = 2, execute = 4 };

/* --- `operator|` and `operator&` ---
 * An `enum class` has no `|` of its own - it is not an `int`. Two free functions give it one: take the numbers,
 * combine their bits, and turn the result back into a `permission`. An enum has no members, so an operator for it is
 * always a free function.
 */
permission operator|(const permission a, const permission b) {
    return static_cast<permission>(to_underlying(a) | to_underlying(b));
}

permission operator&(const permission a, const permission b) {
    return static_cast<permission>(to_underlying(a) & to_underlying(b));
}

/* --- `combine` --- Two flags, one set. */
permission combine(const permission a, const permission b) {
    return a | b;
}

namespace {

    /* --- `color` and `rank` --- A plain `enum`, and an `enum class` without a chosen type. */
    enum color { red, green, blue };
    enum class rank { seven, eight, nine, ten, jack, queen, king, ace };

    /* --- `card` and `small_card` --- The same card, with the default type and with one byte. */
    struct card {
        rank r;
        uint8_t owner;
    };

    struct small_card {
        suit s;
        uint8_t owner;
    };

    /* --- `show_the_underlying_type` ---
     * An `enum class` without a `:` has the underlying type `int`: 4 bytes, for eight values. A plain `enum` has one
     * the compiler chooses - `unsigned int` with gcc and clang, as long as no value is negative. `suit` has one byte.
     * In a struct, that makes a difference: `card` is 8 bytes - 4 for the `rank`, 1 for the `owner`, 3 of padding
     * (see previous snippets) - and `small_card` 2. 52 cards: 416 bytes or 104.
     * `std::to_underlying` (C++23) returns the number in the underlying type, without naming it. For `suit`, that is a
     * `uint8_t` - and `cout` would print a character, so here it is cast to `int` once more.
     * C++ Insights shows the chosen type: `enum class rank : int`. With the option for padding, it shows the three
     * empty bytes in `card`, too.
     * - !![#enum-class]
     */
    void show_the_underlying_type() {
        print_function_header();

        cout << " 1| sizeof(color)=" << sizeof(color) << ", sizeof(rank)=" << sizeof(rank) << ", sizeof(suit)="
             << sizeof(suit) << '\n';
        cout << " 2| sizeof(card)=" << sizeof(card) << ", sizeof(small_card)=" << sizeof(small_card) << '\n';
        cout << " 3| to_underlying(rank::ace)=" << to_underlying(rank::ace) << ", suit::spades is "
             << static_cast<int>(to_underlying(suit::spades)) << '\n';

        /* -- .The checks are free. --
         * `c == red` for a `color` and `s == suit::clubs` for a `suit` compile to the same compare instruction as for
         * a number of the same size. `enum class` refuses `int n{suit::hearts}` and `suit::hearts + 1` - in the
         * compiler. In the machine, nothing is left of the difference.
         */
    }

    /* --- `accept_any_value` ---
     * `static_cast<suit>(7)` is not an error, and not undefined behavior: `suit` is a `uint8_t` with four names, and
     * every value of a `uint8_t` is a valid `suit`. Since C++17, braces accept a number, too - for an enum with a fixed
     * underlying type, and every `enum class` has one. `points_of` must be ready for a `suit` without a name - that is
     * what the `return 0` after the `switch` is for.
     * A plain `enum` without a `:` is stricter: its valid values are only those that fit into the bits of its largest
     * name - 0 to 3 for `color`. `static_cast<color>(7)` is undefined behavior.
     */
    void accept_any_value() {
        print_function_header();

        const suit odd{static_cast<suit>(7)};
        const suit braced{7};
        cout << " 1| odd=" << static_cast<int>(to_underlying(odd)) << ", braced="
             << static_cast<int>(to_underlying(braced)) << ", points_of(odd)=" << points_of(odd) << '\n';
    }

    /* --- `switch_to_a_table` ---
     * Four suits, four numbers - see below what the compiler makes of the `switch`. Remove the `case` for `spades` and
     * build: `-Wall` includes `-Wswitch`, which warns that a value is not handled. With a `default:`, that warning is
     * gone.
     */
    void switch_to_a_table() {
        print_function_header();

        cout << " 1| clubs " << points_of(suit::clubs) << ", diamonds " << points_of(suit::diamonds) << ", hearts "
             << points_of(suit::hearts) << ", spades " << points_of(suit::spades) << '\n';

        /* -- .Q&A -- !![Why is a `default:` in a `switch` over an enum often a bad idea?](#a-709) */
    }

    /* --- `has` --- Is every bit of `p` set in `set`? */
    bool has(const permission set, const permission p) {
        return (set & p) == p;
    }

    /* --- `combine_flags` ---
     * `read | write` calls `operator|` - a free function with a funny name, as in previous snippets, now for an enum.
     * The set is one byte, and it holds a value without a name, 3. Bits and masks in detail: see future snippets.
     */
    void combine_flags() {
        print_function_header();

        const permission rw{combine(permission::read, permission::write)};
        cout << " 1| read | write=" << static_cast<int>(to_underlying(rw)) << ", sizeof(permission)="
             << sizeof(permission) << '\n';
        cout << " 2| has write: " << has(rw, permission::write) << ", has execute: " << has(rw, permission::execute)
             << '\n';
    }

}

/* --- The machine code of an enum ---
 * Paste `suit`, `points_of`, `permission`, its two operators and `combine` into Compiler Explorer, with
 * `#include <cstdint>` and `#include <utility>` and `using std::uint8_t, std::to_underlying;` in front. `-O2`:
 * - `points_of`, x86-64 gcc: `cmp dil, 3` and `ja` - is the value greater than 3? Then return 0. Otherwise
 *   `mov eax, DWORD PTR [rax+rdi*4]` - the result is read from a table, `CSWTCH`, with the numbers 12, 9, 10, 11: the
 *   `switch` has become an array lookup, indexed by the enum. The check before it is there because a `suit` may be 7.
 *   clang does the same, its table is called `switch.table`; ARM64: `cmp w0, 3`, `bhi` and an `ldr` from the table.
 * - `combine`: `mov eax, esi` and `or eax, edi` - on ARM64 `orr w0, w1, w0`. The same as for two `uint8_t`s: the
 *   `static_cast`s and `to_underlying` are nothing, the `|` is one instruction.
 * An enum is a number with names, and the names are gone when the compiler is done.
 */

/* --- `main` --- */
int main() {
    show_the_underlying_type();
    accept_any_value();
    switch_to_a_table();
    combine_flags();

    return EXIT_SUCCESS;
}
