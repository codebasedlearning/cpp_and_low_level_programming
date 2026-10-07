// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An operator for a class is a function: `a + b` is the call `operator+(a, b)`, `a += b` is `a.operator+=(b)` - with
 *   `a` as the hidden `this`.
 * - As Debug, that is a `call`. As Release, the call is inlined, and `a + b` for a `money` is the same machine code as
 *   for two `long long`s.
 * - In the object file, an operator has a name like every other function.
 * - You choose what an operator does, not how it binds: precedence, associativity and the number of operands are those
 *   of the built-in operator.
 * - Some operators the compiler writes itself: `!=` from `==`, and `<`, `<=`, `>`, `>=` from `<=>` (C++20).
 * - An overloaded `&&` is a call like any other: both operands are evaluated before it - the short-circuit is gone.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `price += tip` and `price + tip`: which one is a member function, which one a free function - and what does each
 *   of them return?
 * - How do you call `+=` and `+` for two `money`s by name?
 * - `enum class suit { clubs, diamonds, hearts, spades };` - what is `static_cast<int>(suit::hearts)`, and why does
 *   `const int n{suit::hearts};` not compile?
 * - What did C++ Insights make of `cout << total << '\n'`?
 */

#include <iostream>
#include <compare>                          // for strong_ordering
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `money`, its operators, `add_money` and `add_cents` are for `nm` and Compiler Explorer, below - as in previous
 * snippets, they keep their own names in the object file.
 */

/* --- `money` --- The amount of money from the preparation, in cents. */
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

/* --- `operator+` --- A free function, built with `+=`. */
money operator+(const money& a, const money& b) {
    money sum{a};
    sum += b;
    return sum;
}

/* --- `operator<<` --- The amount, in cents. */
ostream& operator<<(ostream& os, const money& m) {
    return os << m.cents() << " ct";
}

/* --- `add_money` and `add_cents` --- The same addition, with a class and with a number, both by value. */
money add_money(const money a, const money b) {
    return a + b;
}

long long add_cents(const long long a, const long long b) {
    return a + b;
}

namespace {

    /* --- `show_the_calls` ---
     * The operator and the call by name are the same call - the output is the same, and so is the machine code.
     * `a += b` calls a member function: `a` becomes `this`, the hidden first argument of unit 0x03, and `b` the second
     * argument. `a + b` calls a free function with two arguments.
     * The operators are not in the object: `sizeof(money)` is the size of its `long long`, as for every member function
     * (see previous snippets).
     */
    void show_the_calls() {
        print_function_header();

        money a{1000};
        const money b{234};
        cout << " 1| a + b=" << a + b << ", operator+(a, b)=" << operator+(a, b) << '\n';

        a += b;
        a.operator+=(b);
        cout << " 2| a=" << a << '\n';
        cout << " 3| sizeof(money)=" << sizeof(money) << ", sizeof(long long)=" << sizeof(long long) << '\n';
    }

    /* --- `compare_the_machine_code` --- Two functions, the same result. The difference: see below. */
    void compare_the_machine_code() {
        print_function_header();

        cout << " 1| add_money=" << add_money(money{1250}, money{150}) << ", add_cents=" << add_cents(1250, 150)
             << '\n';

        /* -- .Q&A -- !![`money` has a constructor and member functions. Why does it travel in a register?](#a-704) */
    }

}

/* --- The machine code of `+` ---
 * Paste `money`, `operator+`, `add_money` and `add_cents` into Compiler Explorer, x86-64 gcc.
 * - With `-O0`, `add_money` stores `a` and `b` in its frame, puts their addresses into `rdi` and `rsi`, and calls
 *   `operator+(money const&, money const&)`. That one copies `a` into `sum` and calls
 *   `money::operator+=(money const&)`, with the address of `sum` as `this`. Two calls for one `+` - and `add_cents`
 *   has none: its `+` is an `add` instruction.
 * - With `-O2`, both are the same two instructions: `lea rax, [rdi+rsi]` and `ret`. The calls are inlined, the class is
 *   gone - a `money` arrives in a register, as a `long long` does, and the sum leaves in `rax`. On ARM64:
 *   `add x0, x0, x1`, for both.
 * The functions `operator+` and `money::operator+=` are still there in the listing - `operator+` because another file
 * could call it, `money::operator+=` only as Debug.
 * An operator costs what a function costs - and a small function, as Release, costs nothing.
 * - !![#zero-overhead]
 */

/* --- The names in the object file ---
 * `nm -C` on the object file of this snippet - in CLion's build folder, under
 * `CMakeFiles/a_operators_are_calls.dir` - lists `operator+(money const&, money const&)` and
 * `money::operator+=(money const&)` next to `add_money` and `add_cents`. Without `-C`, the mangled names show the
 * operators as two letters: `_ZplRK5moneyS1_` - `pl` for plus - and `_ZN5moneypLERKS_` - `pL` for `+=`. `ls` is `<<`,
 * `eq` is `==`, `ss` is `<=>`. The linker sees names, not symbols like `+`.
 * - !![#symbols]
 */

namespace {

    /* --- `chain_the_calls` ---
     * A chain is nested calls. `cout << a << '\n'` is `operator<<(operator<<(cout, a), '\n')`: the inner call returns
     * the stream, and the outer one writes to it - this is why `operator<<` returns the stream. `x = y = z` goes the
     * other way, from right to left: `x.operator=(y.operator=(z))`, with the copy assignment the compiler wrote for
     * `money`.
     * You choose what an operator does, not how it binds. `<<` binds more tightly than `==`: `cout << a == b` is
     * `(cout << a) == b`, a comparison of a stream with a `money` - the compiler refuses. Parentheses help.
     */
    void chain_the_calls() {
        print_function_header();

        const money a{100};
        cout << " 1| a=" << a << '\n';
        operator<<(operator<<(cout, " 2| a="), a) << '\n';

        money x{1};
        money y{2};
        const money z{3};
        x = y = z;
        cout << " 3| x=" << x << ", y=" << y << '\n';

        cout << " 4| a + z=" << a + z << '\n';     // `+` binds more tightly than `<<`
        // cout << a == z;                  // compiler error: no match for `operator==` (`ostream` and `money`)

        /* -- .The order of evaluation. --
         * Since C++17, the operands of `<<` are evaluated from left to right, for the built-in and for an overloaded
         * `<<` alike: in `cout << f() << g()`, `f` is called before `g`. The same holds for `>>`, `=` (right before
         * left), `&&`, `||` and `,`. Before C++17, the order of the calls in such a chain was unspecified.
         */
    }

    /* --- `version` ---
     * Two defaulted operators: `==` and `<=>`, the three-way comparison (C++20). The compiler writes both, member by
     * member, in the order of declaration: first `major`, then `minor`. `<=>` returns a `std::strong_ordering`: less,
     * equal or greater.
     * - !![#spaceship]
     */
    struct version {
        int major;
        int minor;

        bool operator==(const version&) const = default;
        auto operator<=>(const version&) const = default;
    };

    /* --- `rewrite_comparisons` ---
     * `!=`, `<`, `<=`, `>` and `>=` are not declared anywhere - and they work. The compiler rewrites them: `a != b`
     * becomes `!(a == b)`, and `a < b` becomes `(a <=> b) < 0`. C++ Insights shows it:
     * `!a.operator==(b)` and `operator<(a.operator<=>(b), ...)`. And `nm -C`, as Debug, shows `version::operator==`
     * and `version::operator<=>` - no `operator!=`, no `operator<`.
     * Member by member means: version 1.2 is older than 1.10, because 2 < 10. As texts, "1.2" and "1.10" compare the
     * other way round.
     */
    void rewrite_comparisons() {
        print_function_header();

        const version a{1, 2};
        const version b{1, 10};
        cout << " 1| a == b: " << (a == b) << ", a != b: " << (a != b) << '\n';
        cout << " 2| a < b: " << (a < b) << ", a >= b: " << (a >= b) << '\n';
    }

    /* --- `flag` and its `operator&&` --- A `bool` in a struct, with an `&&` of its own. */
    struct flag {
        bool on;
    };

    flag operator&&(const flag& a, const flag& b) {
        return flag{a.on && b.on};
    }

    /* --- `check_bool` and `check_flag` --- Report that they are evaluated. */
    bool check_bool(const char* name, const bool value) {
        cout << " a|   " << name << " evaluated\n";
        return value;
    }

    flag check_flag(const char* name, const bool value) {
        cout << " b|   " << name << " evaluated\n";
        return flag{value};
    }

    /* --- `lose_the_short_circuit` ---
     * The built-in `&&` stops as soon as the result is known: if the left operand is `false`, the right one is never
     * evaluated - that is what makes `p != nullptr && *p > 0` safe. An overloaded `&&` is a function call, and before a
     * function is called, all its arguments are evaluated. The left one first (C++17), but both. So: do not overload
     * `&&`, `||` and `,` - they would look like the built-in ones, and behave differently.
     * - !![#short-circuit]
     */
    void lose_the_short_circuit() {
        print_function_header();

        cout << " 1| built-in &&\n";
        const bool b{check_bool("left", false) && check_bool("right", true)};
        cout << " 2| result " << b << "\n 3| overloaded &&\n";
        const flag f{check_flag("left", false) && check_flag("right", true)};
        cout << " 4| result " << f.on << '\n';

        /* -- .Q&A -- !![Could you write an `operator&&` for `flag` that skips the right operand?](#a-705) */
    }

}

/* --- `main` --- */
int main() {
    show_the_calls();
    compare_the_machine_code();
    chain_the_calls();
    rewrite_comparisons();
    lose_the_short_circuit();

    return EXIT_SUCCESS;
}
