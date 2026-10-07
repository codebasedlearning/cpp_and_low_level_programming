// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `class circle : public shape` - a circle is a shape: it has all the members of `shape`, and its own.
 * - The constructor of `circle` calls the one of `shape` in its member initializer list. The base is constructed
 *   first, and destroyed last.
 * - A circle can be passed where a `const shape&` is expected.
 * - A `virtual` function is chosen by the object, a non-virtual one by the type the compiler sees. In Java, every
 *   method is virtual - in C++, only those you mark.
 * - `= 0` makes a function pure virtual, and the class abstract: there is no plain `shape`.
 * - Shapes of different kinds in one `vector`: as `unique_ptr<shape>` - and then the destructor of `shape` must be
 *   `virtual`.
 */

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <numbers>                          // for numbers::pi
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::string_view, std::vector, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

namespace {

    /* --- `shape` ---
     * The base class. Every shape has a name. `describe` is an ordinary member function, `area` is `virtual` - and
     * `= 0`: `shape` does not know how to compute an area, every derived class must say it. The destructor is
     * `virtual`, too - see `own_shapes_through_the_base`.
     * - !![#inheritance]
     */
    class shape {
    public:
        explicit shape(const string& name) : name_{name} {
            cout << " a|   shape " << name_ << ": constructed\n";
        }

        virtual ~shape() {
            cout << " b|   shape " << name_ << ": destroyed\n";
        }

        const string& name() const { return name_; }
        string_view describe() const { return "some shape"; }
        virtual double area() const = 0;

    private:
        string name_;
    };

    /* --- `circle` ---
     * `: public shape` - a circle is a shape. It inherits `name` and `describe`, adds a radius, and overrides `area`.
     * `override` says that `area` replaces the one of the base; it is not required - but the compiler checks it.
     * `describe` has the same name as the one in `shape`, but it is not `virtual`: two functions, see below.
     */
    class circle : public shape {
    public:
        circle(const string& name, const double radius) : shape{name}, radius_{radius} {
            cout << " c|   circle " << name << ": constructed\n";
        }

        ~circle() override {
            cout << " d|   circle " << name() << ": destroyed\n";
        }

        string_view describe() const { return "a circle"; }
        double area() const override { return std::numbers::pi * radius_ * radius_; }

    private:
        double radius_;
    };

    /* --- `square` --- Another shape, without messages of its own. */
    class square : public shape {
    public:
        square(const string& name, const double side) : shape{name}, side_{side} {}

        double area() const override { return side_ * side_; }

    private:
        double side_;
    };

    /* --- `derive_a_class` ---
     * A circle has everything a shape has: `c.name()` is the function of `shape`. The constructors run from the base to
     * the derived class - `shape` first, then `circle`. The destructors run the other way round: `circle` first, then
     * `shape`.
     */
    void derive_a_class() {
        print_function_header();

        const circle c{"c", 2.0};
        cout << " 1| name=" << c.name() << ", area=" << c.area() << '\n';

        /* -- .Q&A -- !![Why does `circle` call `shape{name}` in its initializer list - and if it did not?](#a-801) */
    }

    /* --- `print_shape` --- Takes any shape - a circle, a square, whatever comes later. */
    void print_shape(const shape& s) {
        cout << " e|   " << s.name() << ": describe=" << s.describe() << ", area=" << s.area() << '\n';
    }

    /* --- `pass_a_circle_as_a_shape` ---
     * A circle is a shape, so `print_shape` takes it - by reference, no copy. Inside, the compiler sees a `shape`.
     * - `s.describe()` is not `virtual`: the compiler calls the `describe` of the type it sees - "some shape".
     * - `s.area()` is `virtual`: the `area` of the object is called - the one of the circle.
     * Java would print "a circle" twice: there, every method is virtual. In C++, you decide - see the session for the
     * price.
     * - !![#virtual]
     */
    void pass_a_circle_as_a_shape() {
        print_function_header();

        const circle c{"c", 1.0};
        cout << " 1| c.describe()=" << c.describe() << '\n';
        print_shape(c);
        const square q{"q", 2.0};
        print_shape(q);
    }

    /* --- `try_to_create_a_shape` ---
     * `shape` has a pure virtual function: it is an abstract class, and there are no objects of it - only of classes
     * derived from it that override every pure virtual function.
     * - !![#abstract-class]
     */
    void try_to_create_a_shape() {
        print_function_header();

        // const shape s{"s"};              // compiler error: `shape` is abstract, `area` is pure virtual
        cout << " 1| no shape, only circles and squares\n";
    }

    /* --- `own_shapes_through_the_base` ---
     * Circles and squares in one `vector`: the elements must have one type, so they are `unique_ptr<shape>` - owners of
     * objects on the heap (see previous snippets). `make_unique<circle>` gives a `unique_ptr<circle>`, and it becomes a
     * `unique_ptr<shape>`, as a `circle*` becomes a `shape*`.
     * At the end of the function, every `unique_ptr<shape>` deletes its object - through a `shape*`. That this runs the
     * destructor of the circle, too, is the job of `virtual` in front of `~shape()`.
     * - !![#virtual-destructor]
     */
    void own_shapes_through_the_base() {
        print_function_header();

        vector<unique_ptr<shape>> shapes;
        shapes.push_back(make_unique<circle>("c1", 1.0));
        shapes.push_back(make_unique<square>("q1", 3.0));
        shapes.push_back(make_unique<circle>("c2", 0.5));

        double total{0.0};
        for (const unique_ptr<shape>& s : shapes) {
            total += s->area();
        }
        cout << " 1| " << shapes.size() << " shapes, total area=" << total << '\n';
    }

}

/* --- `main` --- */
int main() {
    derive_a_class();
    pass_a_circle_as_a_shape();
    try_to_create_a_shape();
    own_shapes_through_the_base();

    return EXIT_SUCCESS;
}
