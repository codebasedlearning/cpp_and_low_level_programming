// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An object is its data members - `sizeof` of a class is its members plus padding, nothing hidden.
 * - `private` costs nothing at runtime: the compiler checks it, the machine never hears of it.
 * - Member functions are not stored in the object - there is one copy of the code for all objects.
 * - A member function is an ordinary function with a hidden first parameter: `this`, the address of the object.
 * - A `const` member function gets a `this` to a `const` object - that is why only those can be called on `const`
 *   objects.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - How do you recognize a constructor and a destructor - and when does each of them run?
 * - `count_` is private. What happens if `use_a_class` writes `c.count_ = 0;`?
 * - `visitors` and `errors` are two `counter`s. How many `count_` are there?
 * - In the debugger: which line does the frames list mark as the caller of a destructor?
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `plain_point` --- Two `double`s, all public, no functions - the data only. */
    struct plain_point {
        double x;
        double y;
    };

    /* --- `point` ---
     * The same data, but private, with a constructor and three member functions.
     * One difference to `plain_point` shows in the braces: `plain_point q{1.0, 2.0}` fills the members directly - it is
     * an aggregate. `point p{1.0, 2.0}` calls the constructor; with private members, there is no other way in.
     * - !![#aggregate]
     */
    class point {
    public:
        // Assigns in the body, as in the preparation. A better way: see next snippets.
        point(const double x, const double y) {
            x_ = x;
            y_ = y;
        }

        void scale(double f);               // only declared here, defined below the class

        double x() const { return x_; }     // `const`: see `call_const_member_functions`

        // `this` is the address of the object the function was called on.
        void show_address() const {
            cout << " a|   this=" << this << '\n';
        }

    private:
        double x_;
        double y_;
    };

    /* --- `point::scale` ---
     * A member function can also be defined outside the class; `point::` says whose function it is. Inside, `x_` means
     * the `x_` of the object the function was called on.
     */
    void point::scale(const double f) {
        x_ *= f;
        y_ *= f;
    }

    /* --- `show_the_size` ---
     * `plain_point` and `point` have the same size: two `double`s. The constructor, the member functions and `private`
     * add nothing - an object is its data, nothing else. Neither does `struct` or `class`: they differ only in the
     * default access, `public` or `private`.
     * - !![#sizeof]
     * - !![#struct-vs-class]
     */
    void show_the_size() {
        print_function_header();

        const point p{1.0, 2.0};
        cout << " 1| sizeof(plain_point)=" << sizeof(plain_point) << ", sizeof(point)=" << sizeof(point) << '\n';
        cout << " 2| &p=" << &p << ", p.x()=" << p.x() << '\n';

        /* -- .Memory view. --
         * Set a breakpoint on ` 2|` and look at `&p` in the memory view: 16 bytes, two `double`s - `1.0` is
         * `00 00 00 00 00 00 f0 3f` in little-endian. No type tag, no header, no pointer to the functions.
         */

        /* -- .Q&A -- !![So does `private` protect the bytes at runtime?](#a-301) */
    }

    /* --- `show_this` ---
     * Two objects, one function `show_address` - the code exists once. It knows which object to work on because each
     * call gets the object's address, and inside it is called `this`.
     * - !![#this]
     */
    void show_this() {
        print_function_header();

        const point p{1.0, 2.0};
        const point q{3.0, 4.0};
        cout << " 1| &p=" << &p << '\n';
        p.show_address();
        cout << " 2| &q=" << &q << '\n';
        q.show_address();
    }

    /* --- `named_like_its_members` ---
     * Parameters with the same names as the members - common in other people's code. Inside the function, the name
     * means the parameter; the member is `this->x`. That is why the members in this course end with `_`.
     */
    class named_like_its_members {
    public:
        named_like_its_members(const double x, const double y) {
            this->x = x;                    // the member `x` gets the parameter `x`
            this->y = y;
            // x = x;                       // the parameter to itself - the member stays uninitialized
        }

        double sum() const { return x + y; }    // no parameter here, so `x` is the member

    private:
        double x;
        double y;
    };

    /* --- `use_this_explicitly` ---
     * `x_` inside a member function is short for `this->x_` - usually you do not write it. Here you must.
     */
    void use_this_explicitly() {
        print_function_header();

        const named_like_its_members p{1.0, 2.0};
        cout << " 1| p.sum()=" << p.sum() << '\n';
    }

    /* --- `call_const_member_functions` ---
     * On a `const` object, only member functions marked `const` can be called. The `const` after the parameter list
     * belongs to the hidden parameter: in `x() const`, `this` is a `const point*` - so `x_ = 0;` in its body would be a
     * compiler error. The compiler goes by the declaration, not by what the body happens to do: a getter without
     * `const` is useless on a `const` object, even if it only reads.
     * - !![#const-member-function]
     */
    void call_const_member_functions() {
        print_function_header();

        const point p{1.0, 2.0};
        cout << " 1| p.x()=" << p.x() << '\n';
        // p.scale(2.0);                    // compiler error: `p` is `const`, `scale` is not
    }

    /* --- `scale` ---
     * The same work as `point::scale`, as a free function on a `plain_point`: the object comes in as a pointer.
     */
    void scale(plain_point* p, const double f) {
        p->x *= f;                          // `p->x` is short for `(*p).x`
        p->y *= f;
    }

    /* --- `compare_member_and_free_function` ---
     * `p.scale(2.0)` is, for the machine, `scale(&p, 2.0)`: the address of `p` becomes the first argument, `this`.
     */
    void compare_member_and_free_function() {
        print_function_header();

        point p{1.0, 2.0};
        p.scale(2.0);                       // `this` is `&p`

        plain_point q{1.0, 2.0};
        scale(&q, 2.0);                     // explicitly
        cout << " 1| p.x()=" << p.x() << ", q.x=" << q.x << '\n';

        /* -- .Look at the machine code. --
         * Copy both structs and both `scale` functions into Compiler Explorer (godbolt.org), without the unnamed
         * namespace, and compile with `-O1`. The two functions are the same instructions:
         * - x86-64: the address of the object arrives in register `rdi`, the factor in `xmm0`.
         * - ARM64 (Apple Silicon): in `x0` and `d0`.
         * The first parameter of the member function is `this` - it is just not written in the parameter list.
         * - Only the names differ: `_ZN5point5scaleEd` and `_Z5scaleP11plain_pointd` (on macOS with one more `_` in
         *   front). Both encode the parameter types, the member function also its class. What these names are for: see
         *   future snippets.
         * - A member function defined inside the class body, like `x()`, only shows up if it is used and not inlined -
         *   that is why `scale` is defined outside here.
         */
    }

}

/* --- `main` --- */
int main() {
    show_the_size();
    show_this();
    use_this_explicitly();
    call_const_member_functions();
    compare_member_and_free_function();

    return EXIT_SUCCESS;
}
