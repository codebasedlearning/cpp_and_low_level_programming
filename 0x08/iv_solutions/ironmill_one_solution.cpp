// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Iron Mill', see ../tasks.md. Run it as Release.

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <numbers>
#include <random>
#include <cstddef>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::vector, std::unique_ptr, std::make_unique, std::size_t;

class shape {
public:
    virtual ~shape() = default;
    virtual double area() const = 0;
};

class circle final : public shape {
public:
    explicit circle(const double radius) : radius_{radius} {}
    double area() const override { return std::numbers::pi * radius_ * radius_; }

private:
    double radius_;
};

class rectangle final : public shape {
public:
    rectangle(const double width, const double height) : width_{width}, height_{height} {}
    double area() const override { return width_ * height_; }

private:
    double width_;
    double height_;
};

class triangle final : public shape {
public:
    triangle(const double a, const double b, const double c) : a_{a}, b_{b}, c_{c} {}
    double area() const override {
        const double s{(a_ + b_ + c_) / 2.0};
        return std::sqrt(s * (s - a_) * (s - b_) * (s - c_));
    }

private:
    double a_;
    double b_;
    double c_;
};

// Without `virtual`: a tag and three numbers - for a circle, only `a` is used, for a rectangle `a` and `b`.
enum class kind { circle, rectangle, triangle };

struct tagged_shape {
    kind k;
    double a;
    double b;
    double c;
};

double area(const tagged_shape& s) {
    switch (s.k) {
        case kind::circle:    return std::numbers::pi * s.a * s.a;
        case kind::rectangle: return s.a * s.b;
        case kind::triangle: {
            const double h{(s.a + s.b + s.c) / 2.0};
            return std::sqrt(h * (h - s.a) * (h - s.b) * (h - s.c));
        }
    }
    return 0.0;
}

// The size of shape `i` - the same in all three versions. Triangles are equilateral, so that they exist.
double size_of(const size_t i) {
    return 1.0 + static_cast<double>(i % 10);
}

unique_ptr<shape> make_shape(const kind k, const double size) {
    switch (k) {
        case kind::circle:    return make_unique<circle>(size);
        case kind::rectangle: return make_unique<rectangle>(size, 2.0);
        case kind::triangle:  return make_unique<triangle>(size, size, size);
    }
    return nullptr;
}

double sum_virtual(const vector<unique_ptr<shape>>& shapes) {
    double sum{0.0};
    for (const unique_ptr<shape>& s : shapes) {
        sum += s->area();
    }
    return sum;
}

double sum_tagged(const vector<tagged_shape>& shapes) {
    double sum{0.0};
    for (const tagged_shape& s : shapes) {
        sum += area(s);
    }
    return sum;
}

int main() {
    constexpr size_t n{1'000'000};

    vector<kind> kinds;
    std::mt19937 generator{23};
    std::uniform_int_distribution<int> pick{0, 2};
    for (size_t i{0}; i < n; ++i) {
        kinds.push_back(static_cast<kind>(pick(generator)));
    }

    heap_watch heap{};
    vector<unique_ptr<shape>> random_order;
    for (size_t i{0}; i < n; ++i) {
        random_order.push_back(make_shape(kinds[i], size_of(i)));
    }
    cout << "random:  " << heap.allocations() << " allocations, " << heap.bytes() << " bytes\n";

    heap.reset();
    vector<unique_ptr<shape>> sorted;
    for (const kind wanted : {kind::circle, kind::rectangle, kind::triangle}) {
        for (size_t i{0}; i < n; ++i) {
            if (kinds[i] == wanted) {
                sorted.push_back(make_shape(kinds[i], size_of(i)));
            }
        }
    }
    cout << "sorted:  " << heap.allocations() << " allocations, " << heap.bytes() << " bytes\n";

    heap.reset();
    vector<tagged_shape> tagged;
    for (size_t i{0}; i < n; ++i) {
        const double size{size_of(i)};
        tagged.push_back(kinds[i] == kind::rectangle ? tagged_shape{kinds[i], size, 2.0, 0.0}
                                                     : tagged_shape{kinds[i], size, size, size});
    }
    cout << "tagged:  " << heap.allocations() << " allocations, " << heap.bytes() << " bytes\n";

    stopwatch watch{};
    const double sum_random{sum_virtual(random_order)};
    const double random_ms{watch.elapsed_ms()};
    watch.reset();
    const double sum_sorted{sum_virtual(sorted)};
    const double sorted_ms{watch.elapsed_ms()};
    watch.reset();
    const double sum_switch{sum_tagged(tagged)};
    const double tagged_ms{watch.elapsed_ms()};

    cout << "virtual, random order: " << random_ms << " ms, sum " << sum_random << '\n';
    cout << "virtual, sorted:       " << sorted_ms << " ms, sum " << sum_sorted << '\n';
    cout << "tag and switch:        " << tagged_ms << " ms, sum " << sum_switch << '\n';

    // Our runs, Release: gcc 13 and clang 18 on x86-64 Linux - random about 11 ms, sorted about 5 ms, tag and switch
    // about 8 ms; gcc 11 on an Apple silicon Mac, in a Linux VM - 5 ms, 1 ms, 4 ms.
    // - random: the jump goes to one of three functions, at random - the processor guesses wrong about two times in
    //   three. And every shape is a block of its own on the heap: a million allocations, and a pointer to follow.
    // - sorted: the same calls, but a million times the same target in a row, then the next - predicted almost always.
    // - tag and switch: no heap, no pointer - the shapes lie one after the other, 32 bytes each. But the `switch` is a
    //   branch, too, and with random kinds it is as hard to predict as the virtual call; sorted, it would be the
    //   fastest of all.
    // So the price of `virtual` is mostly the price of the unpredictable jump and of the objects on the heap - the two
    // loads themselves are cheap.
    // Memory: 16, 24 and 32 bytes for a circle, a rectangle and a triangle - the vptr and the `double`s - each in a
    // block of its own, which costs more than its bytes (unit 0x06), plus 8 bytes for the `unique_ptr` in the vector.
    // A `tagged_shape` is 32 bytes, all in one block. The bytes `heap_watch` counts include every buffer the vectors
    // had while they grew - with `reserve(n)`, 8 and 32 million bytes for the vectors themselves.
    // `final` does not help the first two sums: `s->area()` is called through a `unique_ptr<shape>`, and a `shape` can
    // be any of the three. `final` helps where the static type is the final class.
    // The `switch` in Compiler Explorer, `-O2`: with three cases that compute something, gcc makes comparisons and
    // jumps, no table - a table pays off with more cases (unit 0x07).

    return EXIT_SUCCESS;
}
