// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The return type is deduced from the `return` statements - or given with `->`.
 * - `auto` parameters make a generic lambda, `<typename T>` a lambda template when the type needs a name.
 * - Init-captures create members with any name and any value: `[total = 0]`, `[v = std::move(v)]`.
 * - `[=]` and `[&]` capture whatever the body uses - convenient, and it hides what is captured. In a member function,
 *   `[=]` captures `this`, not the members.
 * - `[this]` and `[*this]`: the object by address, or a copy of it.
 * - A lambda called where it is written initializes a `const` with a few lines of logic.
 * - A lambda cannot call itself by name - three ways around it.
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <version>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector, std::function;


/* ---- Content ---- */

namespace {

    /* --- `choose_the_return_type` ---
     * Without `->`, all `return` statements must agree on the type: `return 0;` and `return n / 2.0;` would be `int`
     * and `double` - a compiler error. `-> double` decides, and the `0` is converted.
     * A lambda is `constexpr` by itself if its body allows it: `square` is evaluated at compile time in the
     * `static_assert`.
     */
    void choose_the_return_type() {
        print_function_header();

        const auto half = [](const int n) -> double {
            if (n < 0) {
                return 0;
            }
            return n / 2.0;
        };
        cout << " 1| half(-4)=" << half(-4) << ", half(5)=" << half(5) << '\n';

        constexpr auto square = [](const int x) { return x * x; };
        static_assert(square(4) == 16);
        cout << " 2| square(4)=" << square(4) << " - checked at compile time\n";
    }

    /* --- `write_a_generic_lambda` ---
     * `auto` as a parameter type makes `operator()` a template (see the preparation, in C++ Insights): `less` compares
     * two `int`s, two `string`s, whatever has a `<`. When the body needs the type - for the element type of a vector,
     * say - C++20 allows a template parameter list after the `[]`.
     */
    void write_a_generic_lambda() {
        print_function_header();

        const auto less = [](const auto& a, const auto& b) { return a < b; };
        cout << " 1| less(1, 2)=" << less(1, 2) << ", less for two strings: " << less(string{"b"}, string{"a"}) << '\n';

        const auto first_or = []<typename T>(const vector<T>& values, const T& fallback) {
            return values.empty() ? fallback : values.front();
        };
        cout << " 2| first_or: " << first_or(vector<int>{7, 8}, 0) << ", " << first_or(vector<string>{}, string{"none"})
             << '\n';
    }

    /* --- `use_init_captures` ---
     * An init-capture is `name = expression`: a member with that name, initialized with that value - not necessarily a
     * variable from outside. `[&hits = counter]` is a reference with another name. `[text = std::move(text)]` moves
     * (see the session).
     */
    void use_init_captures() {
        print_function_header();

        int counter{0};
        string text{"moved into the lambda"};
        auto count_and_tell = [total = 0, &hits = counter, text = std::move(text)]() mutable {
            ++total;
            ++hits;
            return text + ", call " + std::to_string(total);
        };
        cout << " 1| " << count_and_tell() << '\n';
        cout << " 2| " << count_and_tell() << ", counter=" << counter << ", text is empty: " << text.empty() << '\n';
    }

    /* --- `capture_by_default` ---
     * `[=]` captures by copy, `[&]` by reference - every variable the body uses, and only those. Short, and it hides
     * what the lambda depends on: a `[&]` lambda that is stored somewhere dangles as soon as one of its variables is
     * gone, and nobody sees which. Mixed forms name the exceptions: `[=, &count]`, `[&, factor]`.
     * Prefer the list - `[factor, &count]` says it all.
     * - !![#capture]
     */
    void capture_by_default() {
        print_function_header();

        const int factor{3};
        int count{0};
        const auto by_copy = [=](const int x) { return x * factor; };
        const auto by_reference = [&](const int x) { count += x; };
        const auto mixed = [=, &count](const int x) { count += x * factor; };
        by_reference(2);
        mixed(2);
        cout << " 1| by_copy(2)=" << by_copy(2) << ", count=" << count << '\n';
    }

    /* --- `tracer` ---
     * From the old course: a class that reports its copies, and three member functions with a lambda each.
     * - `set_by_reference`: `[&]` - in a member function, that includes `this`; `n_` is `this->n_`.
     * - `set_in_a_copy`: `[*this]` - a copy of the whole object inside the lambda (C++17). The copy is `const`, as
     *   every captured copy, so the lambda must be `mutable` to change it - and the object itself stays as it was.
     * - `set_through_this`: `[this]` - the address of the object, 8 bytes; changes reach the object.
     * `[=]` in a member function captures `this`, too - the pointer, not a copy of the members. That surprises people,
     * so C++20 deprecated it: write `[=, this]` or `[*this]`, whichever you mean.
     */
    class tracer {
    public:
        tracer() = default;
        tracer(const tracer& other) : n_{other.n_} { cout << " a|   copied\n"; }
        tracer& operator=(const tracer&) = default;

        void set_by_reference() {
            [&] { n_ = 11; }();
        }

        void set_in_a_copy() {
            [*this]() mutable { n_ = 22; }();
        }

        void set_through_this() {
            [this] { n_ = 33; }();
        }

        int n() const { return n_; }

    private:
        int n_{-1};
    };

    /* --- `capture_this_or_a_copy` --- The three of them, called on one object. */
    void capture_this_or_a_copy() {
        print_function_header();

        tracer t;
        t.set_by_reference();
        cout << " 1| after [&]:     n=" << t.n() << '\n';
        t.set_in_a_copy();
        cout << " 2| after [*this]: n=" << t.n() << " - the copy was changed\n";
        t.set_through_this();
        cout << " 3| after [this]:  n=" << t.n() << '\n';
    }

    /* --- `initialize_a_const` ---
     * A value that takes a few lines to work out - with `if`s, early returns - and should be `const` afterwards: write
     * the lines into a lambda and call it right away, `[&] { ... }()`. It runs once, and the result initializes the
     * `const`. `[&]` is fine here, for once: the lambda is gone at the `;` of the same statement. (From the old course;
     * elsewhere this is known as an IIFE, an immediately invoked function expression.)
     */
    void initialize_a_const() {
        print_function_header();

        const int signal{65};
        const bool logged_in{true};
        const string status{[&] {
            if (!logged_in) {
                return "not logged in";
            }
            if (signal < 80) {
                return "poor connection";
            }
            return "connected";
        }()};
        cout << " 1| status: " << status << '\n';
    }

    /* --- `call_recursively` ---
     * A lambda has no name of its own, and `fib` is not declared yet inside its own initializer - so it cannot call
     * itself by that name. Three ways:
     * - A `std::function` captured by reference: works, and costs what `std::function` costs (see the session).
     * - Pass the lambda to itself as an argument: `fib(fib, 10)`. A little odd, and free.
     * - C++23: an explicit object parameter, `this auto self` - the lambda object itself, as the first parameter. gcc
     *   from version 14, clang from 18, MSVC from 17.2; the snippet asks the compiler first.
     */
    void call_recursively() {
        print_function_header();

        const function<int(int)> slow_fib = [&slow_fib](const int n) {
            return n < 2 ? n : slow_fib(n - 1) + slow_fib(n - 2);
        };
        cout << " 1| std::function: " << slow_fib(10) << '\n';

        const auto fib = [](const auto& self, const int n) -> int {
            return n < 2 ? n : self(self, n - 1) + self(self, n - 2);
        };
        cout << " 2| passed to itself: " << fib(fib, 10) << '\n';

#if defined(__cpp_explicit_this_parameter)
        const auto modern_fib = [](this const auto& self, const int n) -> int {
            return n < 2 ? n : self(n - 1) + self(n - 2);
        };
        cout << " 3| this auto: " << modern_fib(10) << '\n';
#else
        cout << " 3| no explicit object parameter in this compiler\n";
#endif
    }

}

/* --- `main` --- */
int main() {
    choose_the_return_type();
    write_a_generic_lambda();
    use_init_captures();
    capture_by_default();
    capture_this_or_a_copy();
    initialize_a_const();
    call_recursively();

    return EXIT_SUCCESS;
}
