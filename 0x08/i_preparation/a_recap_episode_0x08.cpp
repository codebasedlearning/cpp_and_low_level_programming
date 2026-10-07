// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x07 - the things to remember.
 */

#include <iostream>
#include <string_view>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string_view;


/* ---- Content ---- */

namespace {

    /* --- `money` --- The amount of money of unit 0x07, in cents, with `+=` and `+`. */
    class money {
    public:
        explicit money(const long long cents) : cents_{cents} {}

        long long cents() const { return cents_; }

        money& operator+=(const money& other) {
            cents_ += other.cents_;
            return *this;
        }

    private:
        long long cents_;
    };

    money operator+(const money& a, const money& b) {
        money sum{a};
        sum += b;
        return sum;
    }

    /* --- `recall_operators_are_calls` ---
     * An operator is just a function with a funny name: `a + b` is the call `operator+(a, b)`, `a += b` is
     * `a.operator+=(b)`, with `a` as `this`. As Debug, a `call`; as Release, the same `lea` as for two `long long`s -
     * the class is gone.
     */
    void recall_operators_are_calls() {
        print_function_header();

        money a{1000};
        const money b{234};
        a += b;
        cout << " 1| a + b=" << (a + b).cents() << ", operator+(a, b)=" << operator+(a, b).cents() << '\n';
    }

    /* --- `recall_casts` ---
     * A cast is an instruction or nothing. `double` to `int` computes a new value and truncates toward zero; `int` to
     * `unsigned` keeps the bits - no instruction at all.
     */
    void recall_casts() {
        print_function_header();

        cout << " 1| static_cast<int>(-2.99)=" << static_cast<int>(-2.99) << ", static_cast<unsigned>(-1)="
             << static_cast<unsigned>(-1) << '\n';
    }

    /* --- `ticket` --- A static counter: one for the class, not one per ticket. */
    class ticket {
    public:
        ticket() : number_{next++} {}
        int number() const { return number_; }

        inline static int next{1};

    private:
        int number_;
    };

    /* --- `recall_statics` ---
     * A static data member is not in the object: `sizeof(ticket)` is its `int` number. The counter lives in static
     * storage, next to the global variables, from the start of the program to its end.
     */
    void recall_statics() {
        print_function_header();

        const ticket a;
        const ticket b;
        cout << " 1| b.number()=" << b.number() << ", sizeof(ticket)=" << sizeof(ticket) << ", next=" << ticket::next
             << ", a.number()=" << a.number() << '\n';
    }

    /* --- `suit` and `name_of` --- An enum is a number with names; the `switch` picks the code by that number. */
    enum class suit { clubs, diamonds, hearts, spades };

    string_view name_of(const suit s) {
        switch (s) {
            case suit::clubs:    return "clubs";
            case suit::diamonds: return "diamonds";
            case suit::hearts:   return "hearts";
            case suit::spades:   return "spades";
        }
        return "?";
    }

    /* --- `recall_enums` ---
     * `sizeof(suit)` is the size of its underlying type, `int`. As Release, the `switch` becomes a table: a range
     * check, then the number is an index into a list of addresses.
     */
    void recall_enums() {
        print_function_header();

        const suit trump{suit::hearts};
        cout << " 1| trump=" << name_of(trump) << ", number " << static_cast<int>(trump) << ", sizeof(suit)="
             << sizeof(suit) << '\n';
    }

}

/* --- Zero overhead, again ---
 * What you do not use, you do not pay for - and what you use, you could not write better by hand. An operator costs
 * what a function costs, a `static_cast<unsigned>` nothing, an `enum class` nothing more than a plain `enum`.
 */

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`).
 * - The debugger: breakpoints, the memory view, stepping and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 * - `heap_watch` and `heap_log` - count allocations, as Debug.
 * - `nm`, `nm -C` and `c++filt` - which functions are in which object file, and under which name.
 * - `g++ -S` and Compiler Explorer (godbolt.org) - the machine code, x86-64 and ARM64.
 * - C++ Insights (cppinsights.io) - the code the way the compiler reads it.
 * - AddressSanitizer (`-fsanitize=address`), where the toolchain has it.
 */

/* --- Teaser ---
 * A `switch` picks the code by a number, from a table. In Java, `s.area()` runs the `area` of a circle or of a square,
 * whatever `s` refers to - no `switch` anywhere. But an object is its data members, and nothing else, since unit 0x03:
 * `sizeof` shows no type in it. So where does the program find the right `area` - and what does it cost?
 */

/* --- `main` --- */
int main() {
    recall_operators_are_calls();
    recall_casts();
    recall_statics();
    recall_enums();

    return EXIT_SUCCESS;
}
