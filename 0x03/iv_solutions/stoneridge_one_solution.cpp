// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Stone Ridge', see ../tasks.md.

#include <iostream>
#include <vector>
#include <cstdlib>

using std::cout, std::vector;

class point {
public:
    point(const double x, const double y) : x_{x}, y_{y} {}

    void where() const {
        cout << "  this=" << this << '\n';
    }

    void scale(double f);                   // extension, defined below

private:
    double x_;
    double y_;
};

void point::scale(const double f) {
    x_ *= f;
    y_ *= f;
}

// Extension: the same work as a free function. In Compiler Explorer, both compile to the same instructions - the
// address of the object is the first argument (`rdi` on x86-64, `x0` on ARM64).
struct point_data {
    double x;
    double y;
};

void scale(point_data* p, const double f) {
    p->x *= f;
    p->y *= f;
}

int main() {
    // Two `double`s - 16 bytes. The member functions are not in the object.
    cout << "sizeof(point)=" << sizeof(point) << '\n';

    point p{1.0, 2.0};
    cout << "&p=" << &p << '\n';
    p.where();                              // the same address

    // The elements of a vector lie next to each other: the addresses are sizeof(point) = 16 bytes apart (0x10).
    const vector<point> points{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    for (const point& q : points) {
        q.where();
    }

    p.scale(2.0);
    point_data d{1.0, 2.0};
    scale(&d, 2.0);

    return EXIT_SUCCESS;
}
