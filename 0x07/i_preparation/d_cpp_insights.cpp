// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: C++ Insights (cppinsights.io). It shows your code the way the compiler sees it - every
 *   hidden call, every silent conversion, every function the compiler writes for you - as C++ again.
 * - `a + b` becomes `operator+(a, b)`, and `cout << total << '\n'` becomes two nested calls.
 * - `n * 1.5` gets a `static_cast<double>` you did not write.
 * - A range-based `for` becomes the loop with iterators from unit 0x04.
 * - The special members of a class (unit 0x03) appear - those that the program uses.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::vector;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The next functions are the ones to paste into C++ Insights. They use nothing from `cbl`, so they can be pasted as
 * they are - see below.
 */

/* --- `money` --- The amount of money from the previous snippets, reduced to `+=`. */
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

/* --- `operator+` --- A new amount, built with `+=`. */
money operator+(const money& a, const money& b) {
    money sum{a};
    sum += b;
    return sum;
}

/* --- `operator<<` --- The amount, printed in cents. */
ostream& operator<<(ostream& os, const money& m) {
    return os << m.cents() << " ct";
}

/* --- `print_total` --- Two operators, three calls. */
void print_total(const money& price, const money& tip) {
    const money total{price + tip};
    cout << total << '\n';
}

/* --- `scale` --- An `int` times a `double`. */
double scale(const int n) {
    return n * 1.5;
}

/* --- `sum_all` --- A range-based `for` over a `vector`. */
long long sum_all(const vector<int>& values) {
    long long sum{0};
    for (const int x : values) {
        sum += x;
    }
    return sum;
}

/* --- `point` and `copy_a_point` --- A struct without a single member function - or is it? */
struct point {
    int x;
    int y;
};

point copy_a_point(const point& p) {
    point q{p};
    q = p;
    return q;
}

namespace {

    /* --- `use_the_functions` --- The results are not the point - what the compiler makes of the code is. */
    void use_the_functions() {
        print_function_header();

        cout << " 1| ";
        print_total(money{1250}, money{150});
        cout << " 2| scale(3)=" << scale(3) << ", sum_all=" << sum_all(vector<int>{1, 2, 3, 4}) << '\n';
        const point p{copy_a_point(point{3, 4})};
        cout << " 3| p=(" << p.x << ", " << p.y << ")\n";
    }

}

/* --- Using C++ Insights ---
 * Open cppinsights.io, delete the example on the left, and paste the two `#include`s for `<iostream>` and `<vector>`,
 * the `using` line, and everything from `class money` to the end of `copy_a_point`. Choose C++23 (or C++20), and press
 * the run button. On the right is your code the way the compiler reads it. Some names depend on the version of C++
 * Insights and on its standard library - the ideas do not.
 * C++ Insights is built on clang. gcc and MSVC do the same - the rules are in the standard, not in the compiler.
 */

/* --- What it shows ---
 * - `print_total`: `const money total = {operator+(price, tip)};` - the `+` is a call of the free function. And the
 *   output line is `std::operator<<(operator<<(std::cout, total), '\n');` - first your `operator<<`, which returns the
 *   stream, then the one of the library for a `char`, with that stream as its left operand.
 * - `operator<<`: `std::operator<<(os.operator<<(m.cents()), " ct")` - two kinds of `<<`. The one for a `long long` is
 *   a member function of the stream, `os.operator<<(...)`; the one for a text is a free function in `std`.
 * - `operator+`: `sum.operator+=(b);` - the member function, called on `sum`. And in `operator+=`, the compiler writes
 *   `this->cents_` - the hidden `this` of unit 0x03, made visible.
 * - `scale`: `return static_cast<double>(n) * 1.5;` - the conversion you did not write. `sum += x` in `sum_all` gets
 *   one too: `static_cast<long long>(x)`.
 * - `sum_all`: the range-based `for` is gone. There is a reference to the vector, `__range1`, two iterators, `__begin1`
 *   and `__end1`, and a `for` loop with `operator==`, `operator++()` and `operator*()` - the loop of unit 0x04, written
 *   by the compiler.
 * - `money` and `point`: at the end of each, a few commented lines like
 *   `// inline constexpr money(const money &) noexcept = default;` - special members the compiler wrote.
 */

/* -- .Q&A -- !![Why does `money` get a copy and a move constructor, but no copy assignment?](#a-703) */

/* --- `main` --- */
int main() {
    use_the_functions();

    return EXIT_SUCCESS;
}
