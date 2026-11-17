// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Member or free: an operator that changes its left operand - `=`, `+=`, `[]`, `()`, `++` - is a member. A symmetric
 *   one - `+`, `*`, `==`, `<` - is a free function, so that both operands are treated alike.
 * - `+` is written with `+=`: one place for the work. The short form takes the left operand by value.
 * - `==` and `<=>` with `= default` give you all six comparisons.
 * - `[]` comes in two versions, `const` and not; `()` makes an object callable.
 * - Prefix `++` returns the object, postfix `++` a copy of the old value.
 * - `<<` and `>>` for streams. `friend` for a function that needs the private members - or for a factory.
 * - What not to overload - the built-in comma is surprising enough - and when to write `noexcept`.
 */

#include <iostream>
#include <sstream>
#include <array>
#include <compare>
#include <limits>
#include <string>
#include <optional>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::istream, std::istringstream, std::array, std::size_t, std::string, std::optional;


/* ---- Content ---- */

namespace {

    /* --- `polynom` ---
     * `[]` in two versions: `const` for reading, returning a value; non-`const` for writing, returning a reference to
     * the element. The compiler picks the `const` one for a `const polynom`. `()` - the call operator - makes the
     * object usable like a function: `p(2.0)` evaluates the polynomial.
     */
    class polynom {
    public:
        polynom(const double c0, const double c1, const double c2, const double c3) : coefficients_{c0, c1, c2, c3} {}

        double operator[](const size_t i) const { return coefficients_.at(i); }
        double& operator[](const size_t i) { return coefficients_.at(i); }

        double operator()(const double x) const {
            double result{0.0};
            for (size_t i{coefficients_.size()}; i-- > 0; ) {
                result = result * x + coefficients_[i];
            }
            return result;
        }

    private:
        array<double, 4> coefficients_;
    };

    /* --- `use_index_and_call` --- Read, write, evaluate. `at` throws for an index out of range. */
    void use_index_and_call() {
        print_function_header();

        polynom p{1.0, 2.0, 3.0, 4.0};
        cout << " 1| p[3]=" << p[3] << ", p(2.0)=" << p(2.0) << '\n';
        p[3] = 0.0;
        const polynom& q{p};
        cout << " 2| q[3]=" << q[3] << ", q(2.0)=" << q(2.0) << '\n';
        // q[3] = 1.0;                      // compiler error: the `const` version returns a value
    }

    /* --- `lap_counter` ---
     * Prefix `++c`: count, and return the counter itself, by reference. Postfix `c++`: the parameter `int` is only a
     * marker that tells the two apart - it keeps a copy of the old value, counts, and returns the copy.
     * (lap = a round on a track)
     */
    class lap_counter {
    public:
        int laps() const { return laps_; }

        lap_counter& operator++() {
            ++laps_;
            return *this;
        }

        lap_counter operator++(int) {       // With param `int` the function simply has a different signature.
            const lap_counter old{*this};   // This shows why post-inc is often more complex.
            ++*this;
            return old;
        }

    private:
        int laps_{0};
    };

    /* --- `increment_before_and_after` --- The difference is what the expression gives back. */
    void increment_before_and_after() {
        print_function_header();

        lap_counter c;
        const lap_counter before{++c};
        const lap_counter after{c++};
        cout << " 1| ++c gave " << before.laps() << ", c++ gave " << after.laps() << ", c is " << c.laps() << '\n';

        /* -- .Q&A -- !![Why is `++it` the habit in loops, and not `it++`?](#a-711) */
    }

    /* --- `safe` and `locksmith` ---
     * A whole class as a friend: every member function of `locksmith` may use the private members of `safe`. Typical
     * for two classes that belong together - a container and its nodes, or a container and its iterator.
     */
    class safe {
    public:
        explicit safe(const int code) : code_{code} {}

    private:
        int code_;

        friend class locksmith;
    };

    class locksmith {
    public:
        static int open(const safe& s) { return s.code_; }
    };

    /* --- `badge` and `issue_badge` ---
     * A friend factory: the constructor is private, so nobody can create a `badge` - except `issue_badge`, a free
     * function the class names as its friend. The factory checks what the constructor alone could not decide, here
     * whether a name was given, and it could keep a register of all badges. The member version of the same idea is a
     * `static` function, a named constructor (see the follow-up on `static`).
     */
    class badge {
    public:
        const string& name() const { return name_; }

    private:
        explicit badge(const string& name) : name_{name} {}

        string name_;

        friend optional<badge> issue_badge(const string& name);
    };

    optional<badge> issue_badge(const string& name) {
        if (name.empty()) {
            return std::nullopt;
        }
        return badge{name};
    }

    /* --- `use_friends` ---
     * `friend` is a matter for the compiler: it decides who may name a private member. In the machine, `private`,
     * `public` and `friend` leave no trace.
     */
    void use_friends() {
        print_function_header();

        const safe s{1234};
        cout << " 1| code=" << locksmith::open(s) << '\n';

        // const badge b{"Ella"};           // does not compile: the constructor is private
        const optional<badge> b{issue_badge("Ella")};
        const optional<badge> nobody{issue_badge("")};
        cout << " 2| badge for " << b->name() << ", badge for nobody? " << nobody.has_value() << '\n';
    }

    /* --- `try_the_comma` ---
     * The built-in comma evaluates its left operand, drops the result, and gives the right one. And `=` binds tighter
     * than `,`: `n = 1, 2, 3;` is `(n = 1), 2, 3;`. With parentheses, `n = (1, 2, 3);` is `n = 3;`. Both compilers warn
     * (`-Wunused-value`, switched off here, on purpose) - a reason more not to give `,` a meaning of your own.
     */
    void try_the_comma() {
        print_function_header();

        int n{0};
        n = 1, 2, 3;
        cout << " 1| n=" << n << '\n';
        n = (1, 2, 3);
        cout << " 2| n=" << n << '\n';
    }

}

/* --- Best Practises ---
 * What not to overload:
 * - `&&`, `||` and `,`: an overloaded one evaluates both operands, the built-in one does not.
 * - Unary `&`: everybody expects the address (see intro).
 * - Anything whose meaning would surprise a reader: `+` that prints, `==` that changes an object, `<<` for a
 *   `vector` that sorts it first.
 *
 * And what to keep:
 * - `==` and `!=` agree
 * - `<` is a strict order
 * - `a += b` and `a = a + b` give the same result.
 *
 * Where a named function is clearer:
 * - `v.dot(w)` rather than `v * w` for vectors - use the name.
 *
 * `noexcept` on operators
 * - Write `noexcept` on an operator that cannot fail: comparisons, `swap`, and above all the move constructor and move
 *   assignment - a `vector` that grows moves its elements only if their move is `noexcept`, otherwise it copies (see
 *   previous snippets). For the others it is a promise to the reader and the compiler. Break it, and an exception that
 *   leaves a `noexcept` function ends the program with `std::terminate`. An operator that allocates - `+` for strings,
 *   `<<` for streams - can throw, and gets no `noexcept`.
 * - !![#noexcept]
 */

/* --- `main` --- */
int main() {
    use_index_and_call();
    increment_before_and_after();
    use_friends();
    try_the_comma();

    return EXIT_SUCCESS;
}
