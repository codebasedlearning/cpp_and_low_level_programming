// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A template parameter can be a value, too: `fixed_array<int, 3>` - like `std::array`.
 * - Member functions of a class template, defined inside and outside the class.
 * - A member type alias, `value_type`, and alias templates with `using`.
 * - Class template argument deduction (CTAD): `pair_of p{1, 2}` without `<int>`.
 * - `operator<<` for a class template - a function template itself.
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::string, std::vector, std::out_of_range;


/* ---- Content ---- */

namespace {

    /* --- `fixed_array` ---
     * `N` is not a type but a value of type `size_t`, known at compile time. Each `N` gives another class:
     * `fixed_array<int, 3>` and `fixed_array<int, 4>` are as different as `int` and `double`.
     * - !![#instantiation]
     */
    template <typename T, size_t N>
    class fixed_array {
    public:
        using value_type = T;               // a member type: `fixed_array<int, 3>::value_type` is `int`

        size_t size() const { return N; }

        T& at(const size_t i) {
            if (i >= N) {
                throw out_of_range{"fixed_array::at"};
            }
            return data_[i];
        }

        T sum() const;                      // defined below, outside the class

    private:
        // A raw array: `N` elements of type `T`, one after the other, inside the object - see future snippets.
        T data_[N]{};
    };

    /* --- `fixed_array::sum` ---
     * Outside the class, the member function needs the whole template head again, and the class with its arguments.
     * Mind the header: this definition belongs there, too, not into a `.cpp` file.
     */
    template <typename T, size_t N>
    T fixed_array<T, N>::sum() const {
        T result{};
        for (const T& x : data_) {
            result += x;
        }
        return result;
    }

    /* --- `use_a_value_parameter` ---
     * `sizeof` is exactly `N` elements - no size is stored, `size()` returns a constant of the class.
     */
    void use_a_value_parameter() {
        print_function_header();

        fixed_array<int, 3> small{};
        small.at(0) = 1;
        small.at(2) = 5;
        const fixed_array<int, 3>::value_type first{small.at(0)};
        cout << " 1| sum=" << small.sum() << ", first=" << first << ", size=" << small.size() << '\n';
        cout << " 2| sizeof(fixed_array<int, 3>)=" << sizeof(fixed_array<int, 3>)
             << ", sizeof(fixed_array<double, 10>)=" << sizeof(fixed_array<double, 10>) << '\n';
    }

    /* --- `temperatures` and `row` ---
     * `using` gives a type a second name. With `template` in front, the alias itself has parameters: `row<double>` is
     * `fixed_array<double, 4>`.
     * - !![#using]
     */
    using temperatures = vector<double>;

    template <typename T>
    using row = fixed_array<T, 4>;

    /* --- `use_aliases` --- */
    void use_aliases() {
        print_function_header();

        const temperatures week{18.5, 21.0, 19.5};
        row<double> r{};
        r.at(3) = 2.5;
        cout << " 1| week.size()=" << week.size() << ", r.sum()=" << r.sum() << ", r.size()=" << r.size() << '\n';
    }

    /* --- `pair_of` --- Two values of the same type. */
    template <typename T>
    class pair_of {
    public:
        pair_of(const T& first, const T& second) : first_{first}, second_{second} {}

        const T& first() const { return first_; }
        const T& second() const { return second_; }

    private:
        T first_;
        T second_;
    };

    /* --- `operator<<` ---
     * A function template: one `operator<<` for every `pair_of<T>` - the compiler deduces `T` from the argument.
     */
    template <typename T>
    ostream& operator<<(ostream& os, const pair_of<T>& p) {
        return os << '(' << p.first() << ", " << p.second() << ')';
    }

    /* --- `deduce_the_arguments` ---
     * Since C++17, the compiler deduces the template arguments of a class from the constructor arguments, as for a
     * function template: `pair_of p{1, 2}` is a `pair_of<int>`, `vector v{1.5, 2.5}` a `vector<double>`.
     * Handy - but check what it deduced: `pair_of s{"a", "b"}` is a `pair_of<const char*>`, not of `string`.
     */
    void deduce_the_arguments() {
        print_function_header();

        const pair_of p{1, 2};
        const pair_of q{string{"a"}, string{"b"}};
        const vector v{1.5, 2.5};
        cout << " 1| p=" << p << ", q=" << q << ", v.size()=" << v.size() << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_a_value_parameter();
    use_aliases();
    deduce_the_arguments();

    return EXIT_SUCCESS;
}
