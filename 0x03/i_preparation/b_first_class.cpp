// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A `class` bundles data and the functions that work on it - the member functions.
 * - `private` data can only be used by the member functions.
 * - A constructor sets up a new object, a destructor cleans up when it ends.
 * - Every object has its own data.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `counter` ---
     * A counter that can only go up. The count is private, so nobody outside can set it back - the only way to change
     * it is `increment`.
     * `class` is like `struct`, with one difference: its members are private unless you write `public:`.
     * - !![#struct-vs-class]
     */
    class counter {
    public:
        /* -- .Constructor. --
         * Same name as the class, no return type. It runs when an object is created, and sets up its members.
         * - !![#constructor]
         */
        counter(const int start) {
            count_ = start;                 // later we see a better way to init such members
            cout << " a|   counter constructed, count=" << count_ << '\n';
        }

        /* -- .Destructor. --
         * `~` and the class name, no parameters. It runs when the object ends - you never call it yourself.
         * - !![#destructor]
         */
        ~counter() {
            cout << " b|   counter destroyed, count=" << count_ << '\n';
        }

        // A member function: it works on the data of the object it is called on.
        void increment() {
            ++count_;
        }

        // Reads the private member for the outside world.
        int count() {
            return count_;
        }

    private:
        int count_;                         // a trailing `_` is a common way to mark private members
    };                                      // note the `;`

    /* --- `use_a_class` --- Create an object, call its member functions with `.` - and watch the last lines. */
    void use_a_class() {
        print_function_header();

        counter c{10};                      // calls the constructor with 10
        c.increment();
        c.increment();
        cout << " 1| count=" << c.count() << '\n';

        // c.count_ = 0;                    // compiler error: `count_` is private
        cout << " 2| end of function\n";
    }                                       // here the destructor of `c` runs

    /* --- `use_two_objects` ---
     * One class, two objects - each has its own `count_`. `increment` works on the object before the `.`.
     */
    void use_two_objects() {
        print_function_header();

        counter visitors{0};
        counter errors{100};
        visitors.increment();
        cout << " 1| visitors=" << visitors.count() << ", errors=" << errors.count() << '\n';
    }

}

/* --- Teaser ---
 * How big is a `counter` in memory - and where is the code of `increment` stored? Once per object? And how does
 * `increment` know which `count_` to change?
 */

/* --- `main` --- */
int main() {
    use_a_class();
    use_two_objects();

    return EXIT_SUCCESS;
}
