// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Deerwoods', see ../tasks.md.

#include <iostream>
#include <optional>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::ostream, std::optional, std::runtime_error;

// A value of type `T`, or null - a small `std::optional<T>` with operators.
template <typename T>
class nullable {
public:
    nullable() = default;                                   // null
    nullable(const T value) : value_{value}, has_value_{true} {}   // not `explicit`: `n = 1` should work

    void reset() { has_value_ = false; }

    // `+=` as a member: it changes `*this`, and returns it for chains. Null stays null.
    nullable& operator+=(const nullable& other) {
        if (has_value_ && other.has_value_) {
            value_ += other.value_;
        } else {
            has_value_ = false;
        }
        return *this;
    }

    // `+` as a hidden friend, built on `+=`: a free function - both operands are treated alike - defined inside the
    // class. It is no template, so `n + 1` and `1 + n` convert the `int`; a free function template would not.
    friend nullable operator+(nullable lhs, const nullable& rhs) {
        lhs += rhs;
        return lhs;
    }

    // `explicit`: `if (n)` and `!n` work, `int i = n;` does not - the same as for `unique_ptr`.
    explicit operator bool() const { return has_value_; }

    // `static_cast<T>(n)`; `explicit` so that no conversion happens silently. Null has no value to give.
    explicit operator T() const {
        if (!has_value_) {
            throw runtime_error{"nullable: no value"};
        }
        return value_;
    }

private:
    T value_{};
    bool has_value_{false};
};

template <typename T>
ostream& operator<<(ostream& os, const nullable<T>& n) {
    if (n) {
        return os << static_cast<T>(n);
    }
    return os << "null";
}

int main() {
    nullable<int> n{};
    cout << "n=" << n << ", !n=" << !n << '\n';

    n = 1;                                  // a temporary `nullable<int>{1}`, then the generated assignment
    n += 3;
    cout << "n=" << n << ", static_cast<int>(n)=" << static_cast<int>(n) << '\n';

    const nullable<int> m{10};
    cout << "n + m=" << n + m << ", n + 1=" << n + 1 << ", 1 + n=" << 1 + n << '\n';

    n.reset();
    cout << "after reset: n=" << n << ", n + m=" << n + m << '\n';
    try {
        cout << static_cast<int>(n) << '\n';
    } catch (const runtime_error& e) {
        cout << "static_cast<int>(n): " << e.what() << '\n';
    }

    // extension: a value and a `bool` - the `bool` padded to the alignment of `T`. The same as `std::optional`.
    cout << "sizeof(nullable<int>)=" << sizeof(nullable<int>) << ", sizeof(nullable<double>)="
         << sizeof(nullable<double>) << ", sizeof(nullable<char>)=" << sizeof(nullable<char>) << '\n';
    cout << "sizeof(optional<int>)=" << sizeof(optional<int>) << ", sizeof(optional<double>)="
         << sizeof(optional<double>) << ", sizeof(optional<char>)=" << sizeof(optional<char>) << '\n';

    return EXIT_SUCCESS;
}
