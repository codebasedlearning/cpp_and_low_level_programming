// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Coral Creek', see ../tasks.md.

#include <iostream>
#include <optional>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::optional, std::runtime_error;

// An `int` that may be missing - a small `std::optional<int>`.
class optional_int {
public:
    optional_int() = default;               // empty, from the default member initializers
    explicit optional_int(const int value) : value_{value}, has_value_{true} {}

    bool has_value() const { return has_value_; }

    int value() const {
        if (!has_value_) {
            throw runtime_error{"optional_int is empty"};
        }
        return value_;
    }

    int value_or(const int fallback) const {
        return has_value_ ? value_ : fallback;
    }

    void set(const int value) {
        value_ = value;
        has_value_ = true;
    }

    void clear() {
        has_value_ = false;
    }

private:
    int value_{0};
    bool has_value_{false};
};

int main() {
    optional_int temperature{};
    cout << "has_value=" << temperature.has_value() << ", value_or(-1)=" << temperature.value_or(-1)
         << " (expected 0, -1)\n";

    try {
        const int v{temperature.value()};
        cout << "value=" << v << " - never printed\n";
    } catch (const runtime_error& e) {
        cout << "value(): " << e.what() << " (expected)\n";
    }

    temperature.set(21);
    cout << "has_value=" << temperature.has_value() << ", value=" << temperature.value() << " (expected 1, 21)\n";

    temperature.clear();
    cout << "has_value=" << temperature.has_value() << " (expected 0)\n";

    const optional_int set{7};
    cout << "value_or(-1)=" << set.value_or(-1) << " (expected 7)\n";

    // Extension: an `int` (4 bytes) and a `bool` (1 byte) - plus 3 bytes padding, so that in an array the next `int` is
    // aligned. `std::optional<int>` does exactly the same.
    cout << "sizeof(int)=" << sizeof(int) << ", sizeof(optional_int)=" << sizeof(optional_int)
         << ", sizeof(optional<int>)=" << sizeof(optional<int>) << '\n';

    return EXIT_SUCCESS;
}
