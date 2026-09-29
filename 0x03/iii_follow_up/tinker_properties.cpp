// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - C# and Python have properties: `obj.no = 23` that runs a setter behind the scenes. C++ has none.
 * - A small wrapper class gets close - with a template, a lambda and two operators. All of them come later in the
 *   course; this is a taster, read it as a preview.
 * - And the price: how big is such a property compared with the `int` inside?
 */

#include <iostream>
#include <string>
#include <functional>
#include <utility>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::ostream;
using std::function, std::runtime_error;


/* ---- Content ---- */

namespace {

    /* --- `property` ---
     * A value of any type `T` (a template: the compiler writes one class per type), plus an optional function that is
     * called before every change - e.g. to check it.
     */
    template <typename T>
    class property {
    public:
        property() = default;

        explicit property(const T& value) : value_{value} {}

        property(const T& value, function<void(const T& new_value, const T& old_value)> on_set)
            : value_{value}, on_set_{std::move(on_set)} {}

        // The getter: converts a `property<T>` into a `const T&` wherever a `T` is needed.
        operator const T&() const {
            return value_;
        }

        // The setter: `p = v` calls this function.
        property& operator=(const T& value) {
            if (on_set_) {
                on_set_(value, value_);
            }
            value_ = value;
            return *this;
        }

    private:
        T value_{};
        function<void(const T& new_value, const T& old_value)> on_set_{};
    };

    /* --- `operator<<` --- Makes a property printable, by printing the value inside. */
    template <typename T>
    ostream& operator<<(ostream& os, const property<T>& p) {
        const T& value{p};
        return os << value;
    }

    /* --- `address` --- Two properties; the house number is checked by a lambda, a function without a name. */
    class address {
    public:
        property<string> street;
        property<int> no{
            1,
            [](const int& new_value, const int& old_value) {
                if (new_value <= 0) {
                    throw runtime_error{"number must be > 0"};
                }
                cout << " a|   no: " << old_value << " -> " << new_value << '\n';
            }
        };
    };

    /* --- `use_properties` --- It reads like C#. */
    void use_properties() {
        print_function_header();

        address itc;
        itc.street = "Seffenter Weg";
        itc.no = 23;
        cout << " 1| " << itc.street << " " << itc.no << '\n';

        try {
            itc.no = -1;
        } catch (const runtime_error& e) {
            cout << " 2| rejected: " << e.what() << '\n';
        }
    }

    /* --- `show_the_price` ---
     * The syntax is free, the bytes are not: every `property<int>` carries its `int` - and a `std::function`, which is
     * much larger. Check it on your platform.
     */
    void show_the_price() {
        print_function_header();

        cout << " 1| sizeof(int)=" << sizeof(int) << ", sizeof(property<int>)=" << sizeof(property<int>) << '\n';
        cout << " 2| sizeof(address)=" << sizeof(address) << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_properties();
    show_the_price();

    return EXIT_SUCCESS;
}
