// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x08 - the things to remember.
 */

#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

namespace {

    /* --- `shape`, `circle` and `square` --- The shapes of unit 0x08: a virtual `area`, a virtual destructor. */
    class shape {
    public:
        virtual ~shape() = default;
        virtual double area() const = 0;
    };

    class circle : public shape {
    public:
        explicit circle(const double radius) : radius_{radius} {}
        double area() const override { return 3.14159 * radius_ * radius_; }

    private:
        double radius_;
    };

    class square final : public shape {
    public:
        explicit square(const double side) : side_{side} {}
        double area() const override { return side_ * side_; }

    private:
        double side_;
    };

    /* --- `recall_the_layout` ---
     * A derived object is the bytes of its base, followed by its own. Without `virtual`, nothing else - a pointer to
     * the base is the same address, and the conversion costs nothing. With `virtual`, every object gets one more
     * member at offset 0: the vptr, 8 bytes, however many virtual functions there are. So a circle with one `double` is
     * 16 bytes.
     */
    void recall_the_layout() {
        print_function_header();

        const circle c{1.0};
        const shape& s{c};
        cout << " 1| sizeof(shape)=" << sizeof(shape) << ", sizeof(circle)=" << sizeof(circle) << ", &c=" << &c
             << ", &s=" << &s << '\n';
    }

    /* --- `recall_the_virtual_call` ---
     * The vptr points to the table of the class - one table per class, in static storage. A virtual call loads the
     * vptr, loads the address of the function from the table, and jumps there: two loads and an indirect jump. Cheap,
     * as long as the processor predicts where the jump goes; expensive with mixed types in random order. When the
     * compiler knows the type - a local object, a `final` class - it calls directly, and inlines.
     */
    void recall_the_virtual_call() {
        print_function_header();

        vector<unique_ptr<shape>> shapes;
        shapes.push_back(make_unique<circle>(1.0));
        shapes.push_back(make_unique<square>(2.0));
        double total{0.0};
        for (const unique_ptr<shape>& s : shapes) {
            total += s->area();
        }
        cout << " 1| total area=" << total << '\n';
    }

    /* --- `recall_the_constructors` ---
     * Each constructor writes the vptr of its own class, the base first. While the constructor of `shape` runs, the
     * object is a shape - a virtual call there reaches `shape`, not `circle`. The destructors write it back, and
     * `delete` through a base pointer needs the virtual destructor. A copy into a base object gets the base's vptr:
     * slicing makes a base, never half a circle.
     */
    void recall_the_constructors() {
        print_function_header();

        const unique_ptr<shape> s{make_unique<circle>(2.0)};
        cout << " 1| area through the base: " << s->area() << " - and `~circle` runs, through the table\n";
    }

}

/* --- A table of addresses ---
 * Three times now, a program picked code from a table of addresses. The `switch` over an enum (unit 0x07): a table
 * that belongs to the function, indexed by a number. The vtable (unit 0x08): a table that belongs to the class, found
 * through the object. In both, the compiler made the table, and the addresses in it are addresses of code - functions
 * and branches live in memory, like everything else.
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
 * - The class layout: clang's `-Xclang -fdump-record-layouts` and `-fdump-vtable-layouts`, gcc's `-fdump-lang-class`.
 * - AddressSanitizer (`-fsanitize=address`), where the toolchain has it.
 */

/* --- Teaser ---
 * `sort` must be told how to compare - by size, by name, backwards. So the comparison is an argument, and it is code.
 * Java passes an object whose class implements `Comparator` - a virtual `compare`, found through the table. C passes
 * the address of a function (you will see it: `qsort`). In C++, you usually write a lambda, right in the call. What is
 * a lambda, in the machine - a function, an object, an address? And what does it cost to call it a million times?
 */

/* --- `main` --- */
int main() {
    recall_the_layout();
    recall_the_virtual_call();
    recall_the_constructors();

    return EXIT_SUCCESS;
}
