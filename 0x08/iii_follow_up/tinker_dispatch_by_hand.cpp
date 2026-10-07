// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `virtual` is not magic - it can be written by hand, and C programs do: a struct of function pointers, one per
 *   "class", and a pointer to it in every object. That is the vtable, without the compiler's help.
 * - A tag and a `switch` (see previous snippets): no pointer, no heap, every object the same size - but every new kind
 *   of shape means a new `case` everywhere.
 * - A template base (CRTP): the base calls the derived class through a template parameter. No vptr, no table, the
 *   calls are direct and inlined - but there is no common type for a mixed container.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `shape_functions` ---
     * The table, by hand: one pointer per "virtual function". `self` is the object - the hidden `this`, made visible.
     * It is a `const void*`, because the table must fit every kind of shape; each function knows what it gets.
     */
    struct shape_functions {
        double (*area)(const void* self);
        const char* (*name)();
    };

    /* --- `table_shape` and `table_circle` ---
     * `table_shape` is nothing but the pointer to the table - the vptr, by hand. A `table_circle` starts with a
     * `table_shape`, so the address of a circle is the address of its shape part, as with inheritance (see the
     * session).
     */
    struct table_shape {
        const shape_functions* functions;
    };

    struct table_circle {
        table_shape base;
        double radius;
    };

    struct table_square {
        table_shape base;
        double side;
    };

    /* --- The functions and the two tables ---
     * One table per "class", with the addresses of its functions - static, made once, shared by all objects, like the
     * vtables of the compiler.
     */
    double circle_area(const void* self) {
        const table_circle* c{static_cast<const table_circle*>(self)};
        return 3.14159 * c->radius * c->radius;
    }

    const char* circle_name() { return "circle"; }

    double square_area(const void* self) {
        const table_square* q{static_cast<const table_square*>(self)};
        return q->side * q->side;
    }

    const char* square_name() { return "square"; }

    const shape_functions circle_functions{circle_area, circle_name};
    const shape_functions square_functions{square_area, square_name};

    /* --- `call_through_a_table` ---
     * `s->functions->area(s)` - the virtual call, spelled out: load the table pointer from the object, load the address
     * of the function from the table, call it with the object. The machine code is the one of the session. The Linux
     * kernel describes its files and devices this way: a struct of function pointers per kind of file.
     * What the compiler does for you: it builds the tables, sets the pointer in every constructor, checks the types -
     * and here, a wrong table in an object is a bug nobody reports.
     */
    void call_through_a_table() {
        print_function_header();

        const table_circle c{{&circle_functions}, 2.0};
        const table_square q{{&square_functions}, 3.0};
        const vector<const table_shape*> shapes{&c.base, &q.base};
        for (const table_shape* s : shapes) {
            cout << " 1| " << s->functions->name() << ": area=" << s->functions->area(s) << '\n';
        }
        cout << " 2| sizeof(table_circle)=" << sizeof(table_circle) << '\n';
    }

    /* --- `kind` and `tagged_shape` ---
     * No table at all: a tag says what the shape is, and one `double` is the radius or the side. Every shape has the
     * same size - so they fit into a `vector` by value, one block, no heap per shape.
     */
    enum class kind { circle, square };

    struct tagged_shape {
        kind k;
        double size;
    };

    /* --- `area_of` --- The `switch` of unit 0x07: a table in the function, selected by the tag. */
    double area_of(const tagged_shape& s) {
        switch (s.k) {
            case kind::circle: return 3.14159 * s.size * s.size;
            case kind::square: return s.size * s.size;
        }
        return 0.0;
    }

    /* --- `switch_on_a_tag` ---
     * The compiler sees all the cases, and can inline them. The price: every function that depends on the kind has such
     * a `switch`, and a new kind of shape changes all of them - `-Wswitch` finds them. With `virtual`, a new class
     * brings its functions along, and no old code changes. Both have their place.
     */
    void switch_on_a_tag() {
        print_function_header();

        const vector<tagged_shape> shapes{{kind::circle, 2.0}, {kind::square, 3.0}};
        for (const tagged_shape& s : shapes) {
            cout << " 1| area=" << area_of(s) << '\n';
        }
        cout << " 2| sizeof(tagged_shape)=" << sizeof(tagged_shape) << '\n';
    }

    /* --- `shape_base` ---
     * The curiously recurring template pattern (CRTP): a class derives from a template instantiated with itself -
     * `crtp_circle : shape_base<crtp_circle>`. So `shape_base` knows the derived type at compile time, and
     * `static_cast<const derived&>(*this)` is the circle. `larger_than` calls `area` of the derived class - no vptr, no
     * table, a direct call, and as Release inlined.
     * - !![#template]
     */
    template <typename derived>
    class shape_base {
    public:
        bool larger_than(const double limit) const { return static_cast<const derived&>(*this).area() > limit; }
    };

    class crtp_circle : public shape_base<crtp_circle> {
    public:
        explicit crtp_circle(const double radius) : radius_{radius} {}
        double area() const { return 3.14159 * radius_ * radius_; }

    private:
        double radius_;
    };

    class crtp_square : public shape_base<crtp_square> {
    public:
        explicit crtp_square(const double side) : side_{side} {}
        double area() const { return side_ * side_; }

    private:
        double side_;
    };

    /* --- `count_larger` --- A template over the type of the shapes - one function per type. */
    template <typename T>
    int count_larger(const vector<T>& shapes, const double limit) {
        int count{0};
        for (const T& s : shapes) {
            if (s.larger_than(limit)) {
                ++count;
            }
        }
        return count;
    }

    /* --- `dispatch_at_compile_time` ---
     * `sizeof(crtp_circle)` is its `double`: the empty base takes no space (see the session), and there is no vptr.
     * But `shape_base<crtp_circle>` and `shape_base<crtp_square>` are two unrelated types: there is no
     * `vector<shape_base*>` for both. Static polymorphism works where the type is known at compile time - in templates.
     * For a mixed collection, decided at run time, it takes `virtual`, a table by hand, or a tag.
     */
    void dispatch_at_compile_time() {
        print_function_header();

        const vector<crtp_circle> circles{crtp_circle{1.0}, crtp_circle{2.0}};
        const vector<crtp_square> squares{crtp_square{3.0}};
        cout << " 1| larger than 5: " << count_larger(circles, 5.0) << " circles, " << count_larger(squares, 5.0)
             << " squares\n";
        cout << " 2| sizeof(crtp_circle)=" << sizeof(crtp_circle) << '\n';
    }

}

/* --- `main` --- */
int main() {
    call_through_a_table();
    switch_on_a_tag();
    dispatch_at_compile_time();

    return EXIT_SUCCESS;
}
