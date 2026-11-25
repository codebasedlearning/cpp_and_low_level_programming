// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An operator for your own class can live in three places: as a member function, as a free function outside the
 *   class, or as a `friend` - a free function that the class lets in.
 * - `cout << p` cannot be a member of your class: the left operand is `cout`, and the left operand of a member operator
 *   is always `*this`. So `operator<<` is a free function.
 * - A free function sees only the public part of a class. `friend` gives one function access to the private members -
 *   it is still not a member: no `this`, two parameters.
 * - A `friend` defined inside the class is found only when an object of the class is involved: a hidden friend.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream;


/* ---- Content ---- */

namespace {

    /* --- .`number`--- A class with a surprise. */
    struct number {
        int n;

        // instead of &m you can use number* a{std::addressof(m)};
        number* operator&() const { return nullptr; }
    };

    /* --- `op_intro` --- You can do a lot - but not all make sense. */
    void op_intro() {
        print_function_header();

        // number has one (public) member `n` (no fake, this is the truth)
        number m{23};
        cout << " 1| m.n: " << m.n << '\n';

        number* p{&m};
        cout << " 2| p->n: " << p->n << '\n';
    }

    /* --- `celsius` and its `operator<<` ---
     * A temperature with a getter. `operator<<` is a free function outside the class: the stream on the left, the
     * temperature on the right, and the stream is returned for the next `<<` (see previous units). It needs nothing but
     * the public `degrees()`.
     */
    class celsius {
    public:
        explicit celsius(const double degrees) : degrees_{degrees} {}

        double degrees() const { return degrees_; }

    private:
        double degrees_;
    };

    ostream& operator<<(ostream& os, const celsius& c) {
        return os << c.degrees() << " C";
    }

    /* --- `point` ---
     * No getters this time - `x_` and `y_` are private, and nobody outside may read them. But `operator<<` must. So the
     * class names it as a `friend`: this one function may use the private members. Only the class can grant that, from
     * inside; nobody can make itself a friend from outside.
     * A `friend` is not a member: it has no `this`, and it takes both operands as parameters. Defined right here in
     * the class body, it is a hidden friend: the compiler finds it only when a `point` takes part, through the types
     * of the arguments. It could also be declared here and defined outside - then the class only says
     * `friend ostream& operator<<(ostream& os, const point& p);`.
     * - !![#friend]
     *
     * And as a member? It compiles - but the class is on the left now: `operator<<(ostream&) const` is called as
     * `b.operator<<(cout)`, so you have to write `b << cout`. Nobody wants that. The left operand of a member operator
     * is always `*this`, and in `cout << b` the left operand is the stream - a class you cannot add members to.
     *
     */
    class point {
    public:
        point(const int x, const int y) : x_{x}, y_{y} {}

        friend ostream& operator<<(ostream& os, const point& p) {
            return os << '(' << p.x_ << ", " << p.y_ << ')';
        }

    private:
        int x_;
        int y_;
    };

    /* --- `print_with_a_getter` --- The free function, with the public part only. */
    void print_them() {
        print_function_header();

        const celsius today{21.5};
        cout << " 1| today: " << today << '\n';

        const point p{3, 4};
        cout << " 2| p=" << p << ", operator<<(cout, p): ";
        operator<<(cout, p) << '\n';
    }

}

/* --- Where an operator lives ---
 * A member, if the left operand is always your class and the operator changes it: `+=`, `=`, `[]`. A free function, if
 * the left operand can be something else: `<<` with a stream on the left, `+` with a number on the left (see future
 * snippets) - outside the class if the public part is enough, as a `friend` if it is not.
 * - !![#adl]
 */

/* --- `main` --- */
int main() {
    op_intro();
    print_them();

    return EXIT_SUCCESS;
}
