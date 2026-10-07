// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Eastbourne', see ../tasks.md.

#include <iostream>
#include <string>
#include <cstdlib>

using std::cout, std::ostream, std::string;

// The general recipe: a point for any coordinate type.
template <typename T>
class point {
public:
    point(const T x, const T y) : x_{x}, y_{y} {}

    T x() const { return x_; }
    T y() const { return y_; }

private:
    T x_;
    T y_;
};

template <typename T>
ostream& operator<<(ostream& os, const point<T>& p) {
    return os << '(' << p.x() << ", " << p.y() << ')';
}

// A point of `bool`s makes no sense - the full specialization makes sure nobody can create one. As in the old unit,
// with a private constructor; `point(bool, bool) = delete;` in the public part says the same more directly.
template <>
class point<bool> {
private:
    point(bool, bool) {}
};

// `describe` for every type, and a full specialization for `char`. The specialization is an ordinary function, not a
// recipe any more: `nm` shows `T describe<char>()` even if nobody calls it - and in a header it would need `inline`.
template <typename T>
string describe() {
    return "a point of numbers";
}

template <>
string describe<char>() {
    return "a point of characters - on purpose?";
}

int main() {
    const point<int> a{1, 2};
    const point<double> b{0.5, -1.5};
    const point<char> c{'x', 'y'};
    cout << "a=" << a << ", b=" << b << ", c=" << c << '\n';

    // point<bool> d{true, false};          // gcc: "is private within this context", clang: "calling a private
                                            // constructor of class 'point<bool>'"

    cout << "point<int>: " << describe<int>() << '\n';
    cout << "point<char>: " << describe<char>() << '\n';

    // extension: sizes - two members of the type, no padding needed between two equal types.
    cout << "sizeof(point<char>)=" << sizeof(point<char>) << ", sizeof(point<int>)=" << sizeof(point<int>)
         << ", sizeof(point<double>)=" << sizeof(point<double>) << ", sizeof(point<bool>)=" << sizeof(point<bool>)
         << '\n';
    // `point<bool>` has no members, but `sizeof` is 1: every object needs an address of its own. It can still not be
    // created - `sizeof` does not create anything.

    return EXIT_SUCCESS;
}
