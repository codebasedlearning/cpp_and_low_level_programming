// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A conversion turns a value into a value of another type. Many happen silently: an `int` becomes a `double` in
 *   `n * 1.5` - and a `double` becomes an `int` in `int n = 2.9;`, where the fraction is lost.
 * - `static_cast<T>(x)` asks for a conversion explicitly: it is visible, easy to search for, and the compiler checks
 *   that it makes sense. `double` to `int` cuts off the fraction.
 * - `enum class`: a type with a few named values. Inside, the values are numbers - but they do not turn into an `int`
 *   by themselves, a `static_cast` does it.
 * - A plain `enum` does: its names spill into the surrounding scope, and its values become `int`s silently.
 */

#include <iostream>
#include <string_view>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string_view;


/* ---- Content ---- */

namespace {

    /* --- `convert_silently` ---
     * `sum / count` divides two `int`s - an integer division, the remainder is gone. The `double` comes too late:
     * `average` gets 2, converted to 2.0. In `count * 1.5`, the compiler converts `count` to a `double` first, then it
     * multiplies - silently, and here that is what you want. The other direction loses the fraction. With `=` it
     * compiles - `-Wconversion` warns; with braces it is an error, a narrowing conversion (see previous snippets).
     */
    void convert_silently() {
        print_function_header();

        const int sum{10};
        const int count{4};
        const double average{sum / count};
        const double scaled{count * 1.5};
        cout << " 1| sum / count=" << sum / count << ", average=" << average << ", count * 1.5=" << scaled << '\n';

        // const int n = 2.9;               // compiles: n is 2 - with `-Wconversion`, a warning
        // const int m{2.9};                // compiler error: narrowing conversion
    }

    /* --- `convert_explicitly` ---
     * `static_cast<double>(sum)` converts `sum` before the division - and now it divides two `double`s. `double` to
     * `int` cuts off the fraction: 2.99 becomes 2, -2.99 becomes -2 - toward zero, not rounded. A `char` is a small
     * integer: 65 and `'A'` are the same byte, `static_cast` says which one you want to see.
     * - !![#casts]
     */
    void convert_explicitly() {
        print_function_header();

        const int sum{10};
        const int count{4};
        const double average{static_cast<double>(sum) / count};
        cout << " 1| average=" << average << '\n';

        const double price{2.99};
        cout << " 2| static_cast<int>(2.99)=" << static_cast<int>(price) << ", static_cast<int>(-2.99)="
             << static_cast<int>(-price) << '\n';
        cout << " 3| static_cast<char>(65)=" << static_cast<char>(65) << ", static_cast<int>('A')="
             << static_cast<int>('A') << '\n';

        /* -- .The C-style cast. --
         * In C code and in old C++, you will see `(double)sum` or `(int)price`. It works here - but it may do much more
         * than you asked for, see next snippets. In this course: `static_cast`, and the other named casts, only.
         */

        /* -- .Q&A -- !![`static_cast<double>(sum / count)` - is that 2.5, too?](#a-702) */
    }

    /* --- `suit` ---
     * An `enum class` - a scoped enumeration: a type of its own, with four named values. The compiler numbers them
     * from 0: `clubs` is 0, `spades` is 3. The names belong to the type: `suit::hearts`, not `hearts`.
     * - !![#enum-class]
     */
    enum class suit { clubs, diamonds, hearts, spades };

    /* --- `name_of` --- A `switch` over an enum: one `case` per value. */
    string_view name_of(const suit s) {
        switch (s) {
            case suit::clubs:    return "clubs";
            case suit::diamonds: return "diamonds";
            case suit::hearts:   return "hearts";
            case suit::spades:   return "spades";
        }
        return "?";                         // not reached - as long as nobody makes a `suit` from 7, see next snippets
    }

    /* --- `use_an_enum` ---
     * A `suit` is not an `int`: `cout << trump` does not compile, and neither does `int n{suit::hearts}`. To get the
     * number, ask for it with `static_cast<int>`; to get a `suit` from a number, `static_cast<suit>`.
     */
    void use_an_enum() {
        print_function_header();

        const suit trump{suit::hearts};
        cout << " 1| trump=" << name_of(trump) << ", number " << static_cast<int>(trump) << '\n';

        const suit last{static_cast<suit>(3)};
        cout << " 2| static_cast<suit>(3)=" << name_of(last) << ", trump == last: " << (trump == last) << '\n';

        // cout << trump;                   // compiler error: no `operator<<` for a `suit`
        // const int n{suit::hearts};       // compiler error: no conversion to `int`
    }

    /* --- `color` --- A plain `enum`, the old kind, from C. */
    enum color { red, green, blue };

    /* --- `compare_with_a_plain_enum` ---
     * The names `red`, `green` and `blue` are in the surrounding scope - no `color::` needed, and no other `red` in
     * this namespace any more. And a `color` becomes an `int` wherever an `int` fits: `cout << c` prints a number,
     * `green + 1` is 2. Convenient - until two enums with a `red` meet, or a color is compared with a number by
     * mistake. Prefer `enum class`.
     */
    void compare_with_a_plain_enum() {
        print_function_header();

        const color c{blue};
        const int next{green + 1};
        cout << " 1| c=" << c << ", green + 1=" << next << '\n';
    }

}

/* --- `main` --- */
int main() {
    convert_silently();
    convert_explicitly();
    use_an_enum();
    compare_with_a_plain_enum();

    return EXIT_SUCCESS;
}
