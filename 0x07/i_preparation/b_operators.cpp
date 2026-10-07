// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An operator for your own class is a function with a funny name: `operator+=`, `operator+`, `operator==`,
 *   `operator<<`.
 * - `a += b` changes `a` - a member function that returns `*this`. `a + b` builds a new object - a free function,
 *   written with `+=`.
 * - Comparisons return a `bool`; with `==`, you get `!=` for free, and with `<`, the algorithms can sort.
 * - Every operator can also be called by its name.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::vector, std::sort;


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

        /* -- .`operator+=` and `operator-=`. --
         * `price += tip` changes `price`, so it is a member function: `price` is `*this`, `tip` is the parameter. It
         * returns `*this` by reference, as the `+=` for an `int` does - that is why `(a += b) += c` works for both.
         */
        money& operator+=(const money& other) {
            cents_ += other.cents_;
            return *this;
        }

        money& operator-=(const money& other) {
            cents_ -= other.cents_;
            return *this;
        }

    private:
        long long cents_;
    };

    /* --- `operator+` and `operator-` ---
     * `price + tip` changes neither of them: it returns a new amount. It is a free function with two parameters, the
     * left and the right operand. The work is done by `+=` - one place to get it right.
     * - !![#operator-overloading]
     */
    money operator+(const money& a, const money& b) {
        money sum{a};
        sum += b;
        return sum;
    }

    money operator-(const money& a, const money& b) {
        money difference{a};
        difference -= b;
        return difference;
    }

    /* --- `operator==` and `operator<` --- Two comparisons, both return a `bool`. */
    bool operator==(const money& a, const money& b) {
        return a.cents() == b.cents();
    }

    bool operator<(const money& a, const money& b) {
        return a.cents() < b.cents();
    }

    /* --- `operator<<` ---
     * The output operator, as in unit 0x04: the stream on the left, the amount on the right, and the stream is returned
     * for the next `<<`.
     */
    ostream& operator<<(ostream& os, const money& m) {
        return os << m.cents() << " ct";
    }

    /* --- `use_arithmetic` --- `+=` and `-=` change `price`; `+` and `-` leave both operands alone. */
    void use_arithmetic() {
        print_function_header();

        money price{1250};
        const money tip{150};
        price += tip;
        cout << " 1| price=" << price << '\n';
        price -= money{200};
        cout << " 2| price=" << price << '\n';

        const money total{price + tip};
        const money change{money{2000} - total};
        cout << " 3| total=" << total << ", change=" << change << ", price=" << price << '\n';

        /* -- .Q&A -- !![Why does `operator+=` return a `money&`, and `operator+` a `money`?](#a-701) */
    }

    /* --- `call_by_name` ---
     * An operator is a function, so it can be called like one - by its name. `a += b` is the member function call
     * `a.operator+=(b)`, `a + b` is the free function call `operator+(a, b)`, and `cout << a` is `operator<<(cout, a)`.
     * Nobody writes it that way - but the compiler reads it that way.
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
    }

    /* --- `compare_amounts` ---
     * You wrote `==` and `<`. The compiler gives you `!=` for free: it reads `a != b` as `!(a == b)` (C++20). `>`,
     * `<=` and `>=` it does not derive from `<` - that takes one more operator, see future snippets.
     * `sort` needs nothing but `<`: it compares two elements with it, and swaps them if necessary.
     */
    void compare_amounts() {
        print_function_header();

        const money a{500};
        const money b{700};
        cout << " 1| a == b: " << (a == b) << ", a != b: " << (a != b) << ", a < b: " << (a < b) << '\n';

        vector<money> prices{money{1999}, money{499}, money{2500}, money{999}};
        sort(prices.begin(), prices.end());
        cout << " 2| sorted:";
        for (const money& m : prices) {
            cout << ' ' << m;
        }
        cout << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_arithmetic();
    call_by_name();
    compare_amounts();

    return EXIT_SUCCESS;
}
