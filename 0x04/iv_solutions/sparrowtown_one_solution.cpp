// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Sparrow Town', see ../tasks.md.

#include <iostream>
#include <numeric>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::ostream, std::gcd, std::invalid_argument;

// 'Ravencastle' with `T` instead of `int`. Everything else stays - and the whole class stays in the header, if you
// split it: it is a template.
template <typename T>
class fraction {
public:
    fraction(const T num, const T denom) : num_{num}, denom_{denom} {
        if (denom_ == 0) {
            throw invalid_argument{"denominator 0"};
        }
        const T divisor{gcd(num_, denom_)};
        num_ /= divisor;
        denom_ /= divisor;
        if (denom_ < 0) {
            num_ = -num_;
            denom_ = -denom_;
        }
    }

    T num() const { return num_; }
    T denom() const { return denom_; }

    double value() const {
        return 1.0 * num_ / denom_;
    }

private:
    T num_;
    T denom_;
};

// One `operator<<` for every `fraction<T>` - a function template itself.
template <typename T>
ostream& operator<<(ostream& os, const fraction<T>& f) {
    return os << f.num() << '/' << f.denom();
}

int main() {
    const fraction<int> a{2, 4};
    const fraction<short> b{6, -8};
    const fraction<long long> c{3'000'000'000LL, 9'000'000'000LL};

    cout << "a=" << a << " = " << a.value() << " (expected 1/2 = 0.5)\n";
    cout << "b=" << b << " (expected -3/4)\n";
    cout << "c=" << c << " (expected 1/3)\n";

    // Extension: two `T`s, no padding - 4, 8 and 16 bytes.
    cout << "sizeof: short " << sizeof(fraction<short>) << ", int " << sizeof(fraction<int>)
         << ", long long " << sizeof(fraction<long long>) << '\n';

    /*
     * Extension, `nm -C`: the constructor, `num` and `denom` appear for all three types - they are called, via
     * `operator<<`. `value()` appears only for `int`: it is called only for `fraction<int>`. A member function of a
     * class template is generated only when it is used.
     * `fraction<double>` does not compile: `gcd` accepts only integer types. The error is reported deep in `<numeric>`,
     * and the "required from" lines lead back - to the constructor with `T = double`, and to the line in `main` that
     * asked for it. Without the reduction, it would compile.
     */

    return EXIT_SUCCESS;
}
