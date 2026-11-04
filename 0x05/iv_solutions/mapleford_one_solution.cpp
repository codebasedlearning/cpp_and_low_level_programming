// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Maple Ford', see ../tasks.md.

#include <iostream>
#include <string>
#include <optional>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::string, std::optional, std::logic_error;

// An `int` on the heap, or nothing: `nullptr` means "empty".
class optional_int {
public:
    optional_int() = default;
    explicit optional_int(const int value) : value_{new int{value}} {}

    // An empty object stays empty, a full one gets an `int` of its own.
    optional_int(const optional_int& other) : value_{other.value_ ? new int{*other.value_} : nullptr} {}

    // Full from full: copy the value into the `int` we have - no `new`, no `delete`, and `a = a` is harmless.
    // Full from empty: one `new`. Empty from anything: `reset()`.
    optional_int& operator=(const optional_int& other) {
        if (other.value_ == nullptr) {
            reset();
        } else if (value_ == nullptr) {
            value_ = new int{*other.value_};
        } else {
            *value_ = *other.value_;
        }
        return *this;
    }

    ~optional_int() { delete value_; }             // `delete nullptr` does nothing

    bool has_value() const { return value_ != nullptr; }

    int value() const {
        if (value_ == nullptr) {
            throw logic_error{"optional_int is empty"};
        }
        return *value_;
    }

    void reset() {
        delete value_;
        value_ = nullptr;
    }

    const int* address() const { return value_; }  // only for the test: whose `int` is it?

private:
    int* value_{nullptr};
};

// The same as a template. `T` must be copy constructible (copy constructor) and copy assignable (`*value_ = ...`).
template <typename T>
class heap_optional {
public:
    heap_optional() = default;
    explicit heap_optional(const T& value) : value_{new T{value}} {}

    heap_optional(const heap_optional& other) : value_{other.value_ ? new T{*other.value_} : nullptr} {}

    heap_optional& operator=(const heap_optional& other) {
        if (other.value_ == nullptr) {
            reset();
        } else if (value_ == nullptr) {
            value_ = new T{*other.value_};
        } else {
            *value_ = *other.value_;
        }
        return *this;
    }

    ~heap_optional() { delete value_; }

    bool has_value() const { return value_ != nullptr; }

    const T& value() const {                         // a `const&`: a `string` is not copied
        if (value_ == nullptr) {
            throw logic_error{"heap_optional is empty"};
        }
        return *value_;
    }

    void reset() {
        delete value_;
        value_ = nullptr;
    }

private:
    T* value_{nullptr};
};

void print(const char* name, const optional_int& o) {
    cout << "  " << name << ": ";
    if (o.has_value()) {
        cout << o.value() << " at " << o.address() << '\n';
    } else {
        cout << "empty\n";
    }
}

int main() {
    cout << "optional_int:\n";
    optional_int a{23};
    const optional_int b{a};                         // full from full: its own `int`
    const optional_int empty{};
    optional_int c{empty};                           // empty from empty
    print("a", a);
    print("b", b);
    print("c", c);

    c = a;                                           // full from empty: one `new`
    const optional_int& alias{a};
    a = alias;                                       // `a = a`, harmless - written directly, clang warns
    print("c = a", c);
    a = empty;                                       // empty from full: `reset()`
    print("a = empty", a);
    print("b", b);                                   // untouched

    try {
        cout << a.value() << '\n';
    } catch (const logic_error& e) {
        cout << "  caught: " << e.what() << '\n';
    }

    cout << "heap_optional:\n";
    heap_optional<string> s{"Maple Ford"};
    const heap_optional<string> t{s};
    s.reset();
    cout << "  s has a value: " << s.has_value() << ", t=" << t.value() << '\n';
    heap_optional<int> i{42};
    heap_optional<int> j{};
    j = i;
    cout << "  j=" << j.value() << '\n';

    // Extension: on a 64-bit platform both are 8 bytes - ours is an address, `std::optional<int>` is the `int` itself
    // plus a `bool` (and padding). A copy of ours costs a `new`, a copy of `std::optional<int>` none: it keeps the
    // value inside the object.
    cout << "sizeof(optional_int)=" << sizeof(optional_int) << ", sizeof(optional<int>)=" << sizeof(optional<int>)
         << '\n';

    return EXIT_SUCCESS;
}
