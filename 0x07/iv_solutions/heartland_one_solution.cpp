// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Heart Land', see ../tasks.md.

#include <iostream>
#include <array>
#include <cstddef>
#include <cstdlib>

using std::cout, std::ostream, std::array, std::size_t;

class vec3 {
public:
    vec3(const double x, const double y, const double z) : xyz_{x, y, z} {}

    vec3& operator+=(const vec3& other) {
        for (size_t i{0}; i < 3; ++i) {
            xyz_[i] += other.xyz_[i];
        }
        return *this;
    }

    vec3& operator-=(const vec3& other) {
        for (size_t i{0}; i < 3; ++i) {
            xyz_[i] -= other.xyz_[i];
        }
        return *this;
    }

    vec3& operator*=(const double factor) {
        for (double& c : xyz_) {
            c *= factor;
        }
        return *this;
    }

    double operator[](const size_t i) const { return xyz_.at(i); }
    double& operator[](const size_t i) { return xyz_.at(i); }

    bool operator==(const vec3&) const = default;

private:
    array<double, 3> xyz_;
};

vec3 operator+(vec3 a, const vec3& b) {
    a += b;
    return a;
}

vec3 operator-(vec3 a, const vec3& b) {
    a -= b;
    return a;
}

vec3 operator-(const vec3& v) {
    return vec3{-v[0], -v[1], -v[2]};
}

vec3 operator*(vec3 v, const double factor) {
    v *= factor;
    return v;
}

vec3 operator*(const double factor, const vec3& v) {
    return v * factor;
}

ostream& operator<<(ostream& os, const vec3& v) {
    return os << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
}

// Extension: a generic version. `N` is a non-type template parameter (unit 0x04).
template <typename T, size_t N>
class vec {
public:
    vec() = default;

    vec& operator+=(const vec& other) {
        for (size_t i{0}; i < N; ++i) {
            values_[i] += other.values_[i];
        }
        return *this;
    }

    T operator[](const size_t i) const { return values_.at(i); }
    T& operator[](const size_t i) { return values_.at(i); }

    bool operator==(const vec&) const = default;

private:
    array<T, N> values_{};
};

template <typename T, size_t N>
vec<T, N> operator+(vec<T, N> a, const vec<T, N>& b) {
    a += b;
    return a;
}

template <typename T, size_t N>
ostream& operator<<(ostream& os, const vec<T, N>& v) {
    os << '(';
    for (size_t i{0}; i < N; ++i) {
        os << (i == 0 ? "" : ", ") << v[i];
    }
    return os << ')';
}

int main() {
    vec3 a{1.0, 2.0, 3.0};
    const vec3 b{0.5, 0.5, 0.5};
    cout << "a + b = " << a + b << ", a - b = " << a - b << '\n';
    cout << "2.0 * a = " << 2.0 * a << ", a * 2.0 = " << a * 2.0 << ", -a = " << -a << '\n';
    a[1] = 5.0;
    cout << "a = " << a << ", a == b: " << (a == b) << ", a != b: " << (a != b) << '\n';

    // 24 bytes: three `double`s, the operators are not in the object.
    cout << "sizeof(vec3) = " << sizeof(vec3) << '\n';

    // Extension: `add(a, b)` as Release returns its 24 bytes in memory - at the address the caller passes in `rdi`,
    // with `a` and `b` arriving by address in `rsi` and `rdx` (unit 0x05: larger than 16 bytes). The loops are unrolled
    // and the `at` checks are gone: the indexes are known. The hand-written version adds the first two `double`s with
    // one `addpd` and the third with an `addsd`, 8 instructions. clang 18 makes exactly that of `add`, too. gcc 13
    // adds the same way, but takes the third `double` on a detour through the stack - the copy of `a` that
    // `operator+` takes by value - 4 instructions more. Zero overhead is a promise the optimizer usually keeps: look.

    vec<int, 2> p{};
    p[0] = 3;
    p[1] = 4;
    vec<double, 4> q{};
    q[3] = 1.5;
    cout << "p + p = " << p + p << ", q + q = " << q + q << '\n';

    return EXIT_SUCCESS;
}
