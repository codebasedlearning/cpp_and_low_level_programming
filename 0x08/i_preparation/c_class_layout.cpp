// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: the class layout. The compiler knows where every member of a class is - clang prints it for
 *   you, member by member, with its offset, and gcc the size and the table of virtual functions.
 * - A class with a `virtual` function has one member more than you wrote - the dump shows it, and so does `sizeof`.
 * - The same in the debugger: the first bytes of such an object are an address.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The next classes and functions are the ones to paste into Compiler Explorer. They use nothing from `cbl` and no
 * header, so they can be pasted as they are - see below.
 */

/* --- `tag` --- A plain struct: one `int`. */
struct tag {
    int id;
};

/* --- `shape` --- The shape of the previous snippet, with a number instead of a name, and a second virtual function. */
class shape {
public:
    explicit shape(const int id) : id_{id} {}
    virtual ~shape() = default;

    int id() const { return id_; }
    virtual double area() const = 0;
    virtual const char* name() const { return "shape"; }

private:
    int id_;
};

/* --- `circle` --- A shape with a radius. */
class circle : public shape {
public:
    circle(const int id, const double radius) : shape{id}, radius_{radius} {}

    double area() const override { return 3.14159 * radius_ * radius_; }
    const char* name() const override { return "circle"; }

private:
    double radius_;
};

/* --- `make_circle` and `make_tag` --- They make the compiler lay out both classes. */
circle make_circle(const int id, const double radius) {
    return circle{id, radius};
}

tag make_tag(const int id) {
    return tag{id};
}

namespace {

    /* --- `predict_the_sizes` ---
     * Predict first: a `tag` is one `int`. A `shape` has one `int`, too - and member functions, which take no space
     * (see previous snippets). A `circle` adds a `double`. Then run it.
     * If `sizeof(shape)` surprises you: see below.
     */
    void predict_the_sizes() {
        print_function_header();

        cout << " 1| sizeof(tag)=" << sizeof(tag) << ", sizeof(shape)=" << sizeof(shape) << ", sizeof(circle)="
             << sizeof(circle) << '\n';
    }

    /* --- `look_with_the_debugger` ---
     * Set a breakpoint on the line with ` 2|` and run it in the debugger. See below for what to look for.
     */
    void look_with_the_debugger() {
        print_function_header();

        const circle c{make_circle(1, 2.0)};
        const shape& s{c};
        cout << " 1| &c=" << &c << ", &s=" << &s << '\n';
        cout << " 2| s.name()=" << s.name() << ", s.area()=" << s.area() << '\n';
    }

}

/* --- The layout, with clang ---
 * Open Compiler Explorer (godbolt.org), paste everything from `struct tag` to the end of `make_tag`, and choose
 * x86-64 clang. In the field for the compiler options, write
 *     -std=c++23 -Xclang -fdump-record-layouts -fsyntax-only
 * `-Xclang` hands the next option to the inner part of clang, `-fsyntax-only` stops before the machine code - the
 * assembly pane stays empty. Open the output of the compiler (the button below the assembly) and find:
 *        0 | class circle
 *        0 |   class shape (primary base)
 *        0 |     (shape vtable pointer)
 *        8 |     int id_
 *       16 |   double radius_
 *          | [sizeof=24, dsize=24, align=8,
 *          |  nvsize=24, nvalign=8]
 * The numbers on the left are offsets, in bytes. A `circle` starts with a `shape`, and the `shape` starts with a
 * member you did not write: the vtable pointer, 8 bytes. `id_` comes after it, then 4 bytes of padding, then
 * `radius_`. For `tag`, the dump says `int id` at 0, `sizeof=4` - no hidden member: `tag` has no virtual function.
 * On your own machine, clang and Apple clang have the same option - but they dump every class of the headers, too,
 * a few hundred of them. Search for `class circle`.
 * - !![#vtable]
 */

/* --- The table, with clang ---
 * Now replace the options by
 *     -std=c++23 -Xclang -fdump-vtable-layouts
 * (this one needs the machine code, so no `-fsyntax-only`). The output shows the table the pointer points to:
 *     Vtable for 'circle' (6 entries).
 *        0 | offset_to_top (0)
 *        1 | circle RTTI
 *            -- (circle, 0) vtable address --
 *            -- (shape, 0) vtable address --
 *        2 | circle::~circle() [complete]
 *        3 | circle::~circle() [deleting]
 *        4 | double circle::area() const
 *        5 | const char *circle::name() const
 * One entry per virtual function - two for the destructor - and two entries in front of them. The line
 * `vtable address` says where the pointer in the object points: not to the start of the table, but to entry 2. What
 * the entries are for is the story of the session.
 */

/* --- The same with gcc and MSVC ---
 * gcc has `-fdump-lang-class`. It writes a file next to the object file - in CLion's build folder, under
 * `CMakeFiles/c_class_layout.dir`, a file ending in `.class`. Search it for `Class circle` and `Vtable for circle`:
 * `size=24`, `vptr=((& circle::_ZTV6circle) + 16)` - the pointer points 16 bytes into the table, entry 2 - and the
 * six entries. gcc does not list the offsets of the members, clang does.
 * MSVC has an undocumented option for the layout, `/d1reportSingleClassLayoutcircle` - in Compiler Explorer, too.
 */

/* --- In the debugger ---
 * Stop at the breakpoint in `look_with_the_debugger` and open `c` in the variables view.
 * - With gdb (Linux, MinGW), `c` has a member you did not write: `_vptr.shape` (built with gcc) or `_vptr$shape`
 *   (built with clang), and its value is shown as `<vtable for circle+16>`.
 * - lldb (macOS) does not show it - but it uses it: `s` is a `const shape&`, and lldb shows it as a `circle`. How
 *   does it know? Look at the first 8 bytes of `c` in the memory view: an address - the same for every circle.
 * And `&c` and `&s` are the same address: `s` refers to the circle as a whole, which starts with its `shape`.
 */

/* --- `main` --- */
int main() {
    predict_the_sizes();
    look_with_the_debugger();

    return EXIT_SUCCESS;
}
