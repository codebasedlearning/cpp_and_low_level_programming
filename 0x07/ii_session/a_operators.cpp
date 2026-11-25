// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An operator for your own class is a function with a funny name: `operator+=`, `operator+`, `operator==`,
 *   `operator<<`.
 * - `a += b` changes `a` - a member function that returns `*this`. `a + b` builds a new object - a free function.
 * - One implementation, `+=`, and the others built on it: `+` with a copy of the left operand, `-` with the left
 *   operand taken by value - and `+=` again, with the right operand negated.
 * - `price + 100` and `100 + price` need two functions, and the second one only works as a free function: the left
 *   operand of a member operator is always `*this`.
 * - Comparisons return a `bool`; with `==`, you get `!=` for free, and with `<`, the algorithms can sort.
 * - Every operator can also be called by its name.
 */

#include <iostream>
#include <sstream>
#include <compare>
#include <vector>
#include <algorithm>
#include <cmath>                            // for llround
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::vector, std::sort, std::llround;


/* ---- Content ---- */

namespace {

    /* --- `money` ---
     * An amount of money, in cents - a `long long`, so that nothing is lost to rounding. The constructor is `explicit`:
     * a plain number is not an amount of money by itself (see previous snippets).
     */
    class money {
    public:
        explicit money(const long long cents) : cents_{cents} {}

        long long cents() const { return cents_; }

        /* -- .`operator long long`. --
         * A conversion operator: the way out, from an amount back to a number - `static_cast<long long>(price)`. It has
         * no return type in front, the name says it. And it is `explicit`, as the constructor, the way in: only when
         * asked for. It does the same as `cents()` - here, it is a demo.
         * Without `explicit`, a `money` turns into a number wherever one fits: `price == 2000` compiles and compares
         * cents with a bare number, and so does `long long n = price;`. And `price + 50` gets two candidates: our
         * `operator+(money, long long)`, perfect for `price`, and the built-in `+` for `long long` and `int`, perfect
         * for `50`. Neither is better for both operands - clang: "use of overloaded operator '+' is ambiguous". gcc
         * only warns, and takes ours. One line, two compilers, two answers.
         * - !![#conversion-operator]
         */
        explicit operator long long() const { return cents_; }
        // operator long long() const { return cents_; }        // clang: `price + 50` is ambiguous, gcc: a warning

        /* -- .`from_euro`. --
         * A named constructor (see previous snippets): `money::from_euro(19.99)` says the unit in its name, and the
         * constructor stays in cents. It is `static`: it is called on the class, needs no object, and has no `this` -
         * it builds a new one. `19.99 * 100` is not 1999 for a `double`, but 1998.9999999999998: 19.99 has no exact
         * binary form. Cutting off the fraction would lose a cent; `llround` rounds to the nearest whole number of
         * cents, and returns it as a `long long`.
         */
        static money from_euro(const double euros) { return money{llround(euros * 100)}; }

        /* -- .`operator+=`. --
         * `price += tip` changes `price`, so it is a member function: `price` is `*this`, `tip` is the parameter. It
         * returns `*this` by reference, as the `+=` for an `int` does - that is why `(a += b) += c` works for both.
         * It is the only place where amounts are added - the ` a|` line shows every time it runs.
         */
        money& operator+=(const money& other) {
            cout << " a| - operator+=: " << cents_ << "ct + " << other.cents_ << "ct\n";
            cents_ += other.cents_;
            return *this;
        }

        money& operator*=(const long long factor) {
            cout << " b| - operator*=: " << cents_ << "ct * " << factor << "\n";
            cents_ *= factor;
            return *this;
        }

        /* -- .Unary minus. -- One operand, so no parameter: the operand is `*this`. */
        money operator-() const {
            return money{-cents_};
        }

        /* --- .friend ---
         * `friend` gives this one function access to the private members - granted by the class, it cannot be taken.
         * Defined in the class body, it is a free function all the same: a hidden friend. The compiler finds it only
         * when a `money` is involved - it does not show up as a candidate for every other `<<` in the program.
         * - !![#friend]
         */
        friend ostream& operator<<(ostream& os, const money& m) {
            return os << m.cents() << "ct";
        }

        /* -- .`operator<=>`. --
         * The three-way comparison (C++20), written by the compiler: `= default` compares member by member, in the
         * order of declaration - and `money` has one member, `cents_`, so member by member is just: compare the
         * amounts. This one line gives all six comparisons - `==`, `!=`, `<`, `<=`, `>`, `>=` - and `sort` works with
         * them. `a <=> b` returns a `std::strong_ordering`: less, equal or greater. It is a member: the compiler needs
         * `cents_`, which is private.
         *
         * There is not even an `==` line: a class that defaults `<=>` and declares no `==` gets a defaulted `==`,
         * too. Only then - a `<=>` written by hand gives no `==`. The reason is speed: for a `string` or a `vector`,
         * `==` can stop at different sizes, while `<=>` has to look at the elements.
         * - !![#spaceship]
         *
         * a <=> b returns an object with one of three values (less, equal, greater). You never use it directly.
         * You compare it with 0, and that’s exactly what the compiler’s rewrites do:
         *  a <  b    becomes   (a <=> b) <  0
         *  a >= b    becomes   (a <=> b) >= 0
         *  a <  b    or, reversed:  0 < (b <=> a)
         *
         *  The type: a promise about what "equal" means.
         *  type                "equal" means                                                   example
         *  strong_ordering	    the two are interchangeable: equal values behave identically    int, money
         *  weak_ordering       equivalent for sorting, but distinguishable	case-insensitive    strings: "Abc" and "abc"
         *  partial_ordering    some pairs aren’t comparable at all                             double: with a NaN, <, >, <=, >= and == are all false
         *
         * Everyone called this op "Spaceship" (Herb Sutter).
         */
        //auto operator<=>(const money&) const = default;
        friend auto operator<=>(const money&, const money&) = default;   // two parameters, no `this` - and still defaulted

    private:
        long long cents_;
    };

    // a+b => operator+(a,b)

    /* --- .`operator+` ---
     * `price + tip` changes neither of them: it returns a new amount. It is a free function with two parameters, the
     * left and the right operand. It adds nothing itself: it copies the left operand into `sum`, and lets `+=` do the
     * work - one place to get it right.
     * - !![#operator-overloading]
     *
     * Important here: use the member operation `+=` such that the 'heavy' work is implemented only at one place.
     */
    money operator+(const money& a, const money& b) {
        money sum{a};
        sum += b;                           // leads to the basic operation
        return sum;
    }

    /* --- .`operator-` ---
     * The same idea, written shorter: the left operand is taken by value. The parameter `a` is a new object, built from
     * the argument (see previous units) - a copy already, so it is the one to change and to return: no local, no copy
     * by hand. For a temporary like `money{2000}`, there is not even a copy: the argument is built right in the
     * parameter. `return a` moves it out - a parameter cannot be built in the caller's place, the caller built it.
     * And the work is `+=` again, with the right operand negated: subtracting is adding the opposite. So `-` needs no
     * `-=` of its own - one implementation for all three.
     */
    money operator-(money a, const money& b) {
        a += -b;                            // or: a += money{-b.cents()};
        return a;
    }

    /* --- .`operator+` with a number ---
     * `price + 100` and `100 + price`: an amount and a number of cents, in both orders. The constructor is `explicit`,
     * so a number does not turn into a `money` by itself - each order needs a function of its own, and the order of
     * the parameters is the order of the operands. The first one is written like `-`, the left operand by value; the
     * second one just calls the first - adding does not care about the order.
     */
    money operator+(money m, const long long cents) {
        m += money{cents};
        return m;

        /* -- .Why not `return m += money{cents};`. --
         * It compiles, and the result is the same - but it copies, where the two lines move. `return m;` returns a
         * name, the name of a parameter: the compiler knows that `m` is about to die, and moves from it.
         * `m += money{cents}` is no name, it is an expression - a `money&`, a reference to some object that might
         * live on. The compiler cannot know that it may steal from it, so it copies. For a `money`, copying 8 bytes is
         * all a move does anyway, and the machine code is the same. For a class that owns a block, it is a deep copy -
         * without a word from the compiler.
         */
    }

    money operator+(const long long cents, const money& m) {
        return m + cents;
    }

    /* --- .`operator-` with a number ---
     * `price - 50` is written like `price + 50`, with the number negated. But `50 - price` cannot just call the other
     * order, as the second `+` does: `-` does not commute, and `return m - cents;` would compute `price - 50` - the
     * wrong sign, and no compiler notices. Instead: the opposite of the amount, plus the number.
     */
    money operator-(money m, const long long cents) {
        m += money{-cents};
        return m;
    }

    money operator-(const long long cents, const money& m) {
        return -m + cents;
    }

    // same ideas for *

    money operator*(money m, const long long factor) {
        m *= factor;
        return m;
    }
    money operator*(const long long factor, const money& m) {
        return m * factor;
    }

    // We do not need them as we have `<=>`, so just as examples
    //
    // bool operator==(const money& a, const money& b) { return a.cents() == b.cents(); }
    // bool operator<(const money& a, const money& b) { return a.cents() < b.cents(); }
    // ...

    // We already have it as fiend, so just as example
    //
    // ostream& operator<<(ostream& os, const money& m) {
    //     return os << m.cents() << "ct";
    // }

    /* --- .`operator>>` ---
     * The input operator: reads a number of cents from any `istream` - `cin`, a file, a string - into an existing
     * `money`. If the stream cannot read a number, it goes into its failed state, and `m` stays as it was.
     */
    std::istream& operator>>(std::istream& is, money& m) {
        long long cents{0};
        if (is >> cents) {
            m = money{cents};
        }
        return is;
    }

    /* --- `spend_money` ---
     * use arithmetics:
     * - Three operators, one implementation: every line ends in `operator+=`, and the ` a|` lines show what it adds.
     *   `+=` changes `price`; `+` and `-` leave their operands alone - `price` and `tip` are the same afterwards.
     *
     * a named constructor
     * - `money::from_euro` is called on the class, not on an object. The second amount shows why it rounds: the same
     * - `19.99 * 100`, with the fraction cut off by `static_cast` (see previous snippets), is a cent short.
     *
     * Three operators, one implementation: every line ends in `operator+=`, and the ` a|` lines show what it adds.
     * `+=` changes `price`; `+` and `-` leave their operands alone - `price` and `tip` are the same afterwards.
     *
     * add a number
     * - Both orders work - because both functions are free. As a member, `money operator+(long long) const` would give
     *   `price + 100`: the compiler reads it as `price.operator+(100)`, and `price` becomes `this`. But `100 + price`
     *   would need `100.operator+(price)` - and a number has no member functions. gcc: "no match for 'operator+'
     *   (operand types are 'int' and 'const money')". The left operand of a member operator is always `*this`, an object
     *   of the class. A free function has no `this`, only two parameters - any type on either side. That is the reason
     *   to write `+` as a free function at all.
     *
     * compare
     * - You wrote no comparison at all, only `<=>` with `= default`. The compiler rewrites the rest, as for `version`
     *   (see previous snippets): `a != b` becomes `!(a == b)`, `a < b` becomes `(a <=> b) < 0`, and so on.
     *   `sort` needs nothing but `<`: it compares two elements with it, and swaps them if necessary.
     * - `==` compares member by member, `<=>` compares member by member in the order of declaration, and the
     *   compiler rewrites `!=`, `<`, `<=`, `>` and `>=` with them (see previous snippets).
     * - !![#spaceship]
     */
    void spend_money() {
        print_function_header();

        money price{2000};
        const money tip{100};

        // op<< in action
        cout << " 1| price=" << price << ", tip=" << tip << '\n';
        price += tip;                       // op+= or price.operator+=(tip)
        cout << " 2| price with tip: " << price << '\n';
        price = price + tip;                // op+
        cout << " 3| price with double tip: " << price << '\n';
        price = price + 50;                 // op+ with long long
        cout << " 4| and +50: " << price << '\n';
        price = price - 50;                 // op- with long long
        const money rest{50 - price};       // op- with long long on the left
        cout << " 5| and -50: " << price << ", 50 - price: " << rest << '\n';

        const money offer{money::from_euro(19.99)};
        cout << " 6| from_euro(19.99)=" << offer << '\n';
        cout << " 7| -offer=" << -offer << '\n';

        money deposit1{2000}, deposit2{100};
        cout << " 8| deposit1 (" << deposit1 << ") < deposit2 (" << deposit2 << ")? " << (deposit1<deposit2) << '\n';
        cout << " 9| deposit1 (" << deposit1 << ") = deposit2 (" << deposit2 << ")? " << (deposit1==deposit2) << '\n';
        cout << "10| deposit1 (" << deposit1 << ") > deposit2 (" << deposit2 << ")? " << (deposit1>deposit2) << '\n';
        cout << "11| deposit1 (" << deposit1 << ") <=> deposit2 (" << deposit2 << ")? "
             // << ((deposit1<=>deposit2)== std::strong_ordering::greater)
             << ((deposit1 <=> deposit2) > 0)
        << '\n';

        /* -- .Q&A -- !![Why does `operator+=` return a `money&`, and `operator+` a `money`?](#a-701) */
        vector prices{money{1999}, money{499}, money{2500}, money{999}};    // or vector<money>
        std::ranges::sort(prices);                                          // or sort(prices.begin(), prices.end());
        cout << "12| sorted:";
        for (const money& m : prices) {
            cout << ' ' << m;
        }
        cout << '\n';
    }

    /* --- `call_by_name` ---
     * An operator is a function, so it can be called like one - by its name. `a += b` is the member function call
     * `a.operator+=(b)`, `a + b` is the free function call `operator+(a, b)`, and `cout << a` is `operator<<(cout, a)`.
     * A cast is a call, too: `static_cast<long long>(c)` is `c.operator long long()`. Nobody writes it that way - but
     * the compiler reads it that way.
     */
    void call_by_name() {
        print_function_header();

        money a{100};
        const money b{23};
        a.operator+=(b);
        cout << " 1| a=" << a << '\n';

        const money c{operator+(a, b)};
        cout << " 2| c=";
        operator<<(cout, c);
        cout << '\n';

        cout << " 3| static_cast<long long>(c)=" << static_cast<long long>(c) << ", c.operator long long()="
             << c.operator long long() << '\n';
    }

    /* --- `read_and_write` --- `>>` from a string stream; the third read fails. */
    void read_and_write() {
        print_function_header();

        std::istringstream input{"250 1999 lots"};
        money m{0};
        input >> m;
        cout << " 1| read " << m << '\n';
        input >> m;
        cout << " 2| read " << m << '\n';
        if (!(input >> m)) {
            cout << " 3| no number - m is still " << m << '\n';
        }
    }

}

/* --- `main` --- */
int main() {
    spend_money();
    call_by_name();
    read_and_write();

    return EXIT_SUCCESS;
}
