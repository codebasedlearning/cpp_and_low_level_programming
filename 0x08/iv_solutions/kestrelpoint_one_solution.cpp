// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Kestrel Point', see ../tasks.md.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <numbers>
#include <cstring>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::string, std::to_string, std::vector, std::unique_ptr, std::make_unique;

// Abstract: three pure virtual functions. `describe` is not virtual - it is the same for all shapes, and it calls the
// virtual functions, whatever the shape is.
class shape {
public:
    virtual ~shape() = default;

    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual string name() const = 0;

    string describe() const {
        return name() + ": area " + to_string(area()) + ", perimeter " + to_string(perimeter());
    }
};

class circle : public shape {
public:
    explicit circle(const double radius) : radius_{radius} {}

    double area() const override { return std::numbers::pi * radius_ * radius_; }
    double perimeter() const override { return 2.0 * std::numbers::pi * radius_; }
    string name() const override { return "circle"; }

private:
    double radius_;
};

class rectangle : public shape {
public:
    rectangle(const double width, const double height) : width_{width}, height_{height} {}

    double area() const override { return width_ * height_; }
    double perimeter() const override { return 2.0 * (width_ + height_); }
    string name() const override { return "rectangle"; }

private:
    double width_;
    double height_;
};

// `final`: see the extension.
class triangle final : public shape {
public:
    triangle(const double a, const double b, const double c) : a_{a}, b_{b}, c_{c} {}

    double area() const override {
        const double s{perimeter() / 2.0};
        return std::sqrt(s * (s - a_) * (s - b_) * (s - c_));
    }
    double perimeter() const override { return a_ + b_ + c_; }
    string name() const override { return "triangle"; }

private:
    double a_;
    double b_;
    double c_;
};

// The first 8 bytes of an object - what the debugger shows as `_vptr.shape` (gdb) or in the memory view.
const void* first_bytes_of(const shape& s) {
    const void* p{nullptr};
    std::memcpy(&p, static_cast<const void*>(&s), sizeof p);
    return p;
}

int main() {
    const heap_watch heap{};
    {
        vector<unique_ptr<shape>> shapes;
        shapes.push_back(make_unique<circle>(1.0));
        shapes.push_back(make_unique<rectangle>(2.0, 3.0));
        shapes.push_back(make_unique<triangle>(3.0, 4.0, 5.0));
        shapes.push_back(make_unique<circle>(2.0));
        shapes.push_back(make_unique<rectangle>(1.0, 10.0));

        double total{0.0};
        const shape* longest{nullptr};
        for (const unique_ptr<shape>& s : shapes) {
            cout << s->describe() << '\n';
            total += s->area();
            if (longest == nullptr || s->perimeter() > longest->perimeter()) {
                longest = s.get();
            }
        }
        cout << "total area " << total << ", largest perimeter: " << longest->describe() << '\n';

        // With gcc and clang on 64-bit platforms: shape 8 - only the vptr -, circle 16, rectangle 24, triangle 32. The
        // number of virtual functions does not count, the vptr is one pointer.
        cout << "sizeof: shape " << sizeof(shape) << ", circle " << sizeof(circle) << ", rectangle "
             << sizeof(rectangle) << ", triangle " << sizeof(triangle) << '\n';

        // The two circles show the same address - the vtable of `circle` - the rectangle another one.
        cout << "first bytes: " << first_bytes_of(*shapes[0]) << ", " << first_bytes_of(*shapes[3]) << ", "
             << first_bytes_of(*shapes[1]) << '\n';
    }
    // As many releases as allocations: the five shapes, the buffers of the `vector` while it grew, and the longer texts
    // of `describe` - everything given back.
    cout << "allocations " << heap.allocations() << ", releases " << heap.releases() << '\n';

    // Extension, without `virtual` in front of `~shape` and with a label of more than 30 characters in `rectangle`:
    // - gcc 13 with `-Wall`: not a word. The `delete` happens inside `std::default_delete`, in a system header, and
    //   warnings from there are not shown. Only `-Wnon-virtual-dtor` (not in `-Wall`) reports it, at the class.
    // - clang 18 with `-Wall`: `delete called on 'shape' that is abstract but has non-virtual destructor`.
    // - heap_watch: two releases fewer than allocations - the labels of the two rectangles. `~rectangle` never ran.
    // Extension, `final`: `triangle` cannot be a base any more - `rectangle` still can. A `square` derived from
    // `rectangle` works here, because the sides cannot be changed after construction; with a `set_width`, it would
    // break the promise of a rectangle - see the follow-up on Liskov's rule.

    return EXIT_SUCCESS;
}
