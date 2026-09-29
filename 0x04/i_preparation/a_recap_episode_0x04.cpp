// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x03 - the things to remember.
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::runtime_error;


/* ---- Content ---- */

namespace {

    /* --- `point` ---
     * An object is its data members, plus padding - nothing else. The constructor initializes them in the member
     * initializer list, in declaration order; `x()` is `const`, so it can be called on a `const point`.
     */
    class point {
    public:
        point(const double x, const double y) : x_{x}, y_{y} {}

        double x() const { return x_; }

        void scale(const double f) {        // for the machine: `scale(&p, f)` - `this` is the hidden first argument
            x_ *= f;
            y_ *= f;
        }

    private:
        double x_;
        double y_;
    };

    /* --- `recall_objects_are_bytes` ---
     * Two `double`s, 16 bytes. The member functions exist once in the program, not in every object.
     */
    void recall_objects_are_bytes() {
        print_function_header();

        point p{1.0, 2.0};
        p.scale(2.0);
        cout << " 1| sizeof(point)=" << sizeof(point) << ", &p=" << &p << ", p.x()=" << p.x() << '\n';
    }

    /* --- `tracer` --- Reports its birth, its copies and its death. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed\n";
        }

        tracer(const tracer& other) : name_{other.name_ + "'"} {
            cout << " b|   " << name_ << ": copy-constructed\n";
        }

        tracer& operator=(const tracer& other) {
            cout << " c|   " << name_ << ": assigned from " << other.name_ << '\n';
            name_ = other.name_ + "'";
            return *this;
        }

        ~tracer() {
            cout << " d|   " << name_ << ": destroyed\n";
        }

    private:
        string name_;
    };

    /* --- `recall_copy_or_assign` ---
     * A copy construction creates a new object, a copy assignment overwrites an existing one. The destructors run at
     * the `}`, in reverse order of construction.
     */
    void recall_copy_or_assign() {
        print_function_header();

        const tracer a{"a"};
        tracer b{a};                        // copy constructor
        const tracer c{"c"};
        b = c;                              // copy assignment
        cout << " 1| end of function\n";
    }

    /* --- `fail` --- Creates a local object, then throws. */
    void fail() {
        const tracer local{"local"};
        throw runtime_error{"failed"};
    }

    /* --- `recall_stack_unwinding` ---
     * On the way to the `catch`, every frame is removed, and every local object in it is destroyed - RAII: the cleanup
     * is in the destructor, and it cannot be forgotten.
     */
    void recall_stack_unwinding() {
        print_function_header();

        try {
            fail();
        } catch (const runtime_error& e) {
            cout << " 1| caught: " << e.what() << '\n';
        }
    }

}

/* --- Header and source ---
 * A class in real projects: the header declares it, including its data members - every translation unit that creates
 * an object needs its `sizeof`. The source file defines the member functions, the linker joins the object files.
 * - A missing definition: `undefined reference` (gcc), `Undefined symbols` (Apple clang).
 * - A function body in a header that is included twice: `multiple definition`, `duplicate symbol`. Fix: move it into
 *   the `.cpp` file, or mark it `inline`.
 */

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`), `g++ -S` or Compiler Explorer (godbolt.org).
 * - The debugger: breakpoints, the memory view, stepping (into, over, out) and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 */

/* --- Teaser ---
 * The linker looks for `fraction::fraction(int, int)` - by name. But `print(int)` and `print(double)` have the same
 * name, and so do a hundred member functions called `size`. Under which name does the linker look for a function?
 */

/* --- `main` --- */
int main() {
    recall_objects_are_bytes();
    recall_copy_or_assign();
    recall_stack_unwinding();

    return EXIT_SUCCESS;
}
