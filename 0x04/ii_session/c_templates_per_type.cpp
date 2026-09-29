// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A function template: one function for many types - `template <typename T>`, and the compiler fills in the type.
 * - A class template: one class for many types - `maybe<int>`, `maybe<string>`, like `vector<int>`.
 * - A template is a recipe, not code. The compiler generates one function or class per type - an instantiation.
 * - Only what is used is generated: a template nobody calls leaves no trace in the object file.
 * - Each instantiation of a class template has its own layout and its own `sizeof`.
 * - The member functions of a class template are instantiated only when they are called.
 * - A specialization replaces the recipe for particular types - with other code, or with another layout.
 */

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector, std::list, std::set;


/* ---- Content ---- */

namespace {

    /* --- `print_all` ---
     * A function template. `T` is a placeholder for a type; for each call, the compiler looks at the argument and puts
     * in the type - `vector<int>`, `list<string>`, ... All it needs is that the body compiles for it: `begin` and `end`
     * for the loop, and elements that `cout` can print.
     * - !![#template]
     */
    template <typename T>
    void print_all(const T& container) {
        for (const auto& x : container) {
            cout << ' ' << x;
        }
        cout << '\n';
    }

    /* --- `use_a_function_template` --- One function in the source, three containers. */
    void use_a_function_template() {
        print_function_header();

        const vector<int> v{7, 3, 5, 3};
        const list<string> l{"a", "list", "of", "words"};
        const set<int> s{7, 3, 5, 3};
        cout << " 1| vector:"; print_all(v);
        cout << " 2| list:";   print_all(l);
        cout << " 3| set:";    print_all(s);
    }

    /* --- `largest` --- The larger of two values - for every type that has a `<`. */
    template <typename T>
    T largest(const T& a, const T& b) {
        return a < b ? b : a;
    }

    /* --- `smallest` --- The same the other way round - but nobody calls it. */
    template <typename T>
    T smallest(const T& a, const T& b) {
        return b < a ? b : a;
    }

    /* --- `instantiate_per_type` ---
     * Three calls, three types - the compiler deduces `T` from the arguments and generates `largest<int>`,
     * `largest<double>` and `largest<string>`: three functions, each compiled for its type, as if you had written them
     * by hand. No type check at runtime, no common code for all types.
     * - !![#instantiation]
     */
    void instantiate_per_type() {
        print_function_header();

        const string a{"apple"};
        const string b{"banana"};
        cout << " 1| " << largest(3, 7) << ", " << largest(2.5, 1.5) << ", " << largest(a, b) << '\n';
        cout << " 2| " << largest<double>(3, 7.5) << '\n';     // `T` given explicitly: 3 becomes a `double`

        // largest(3, 7.5);                 // compiler error: `T` is `int` or `double`? The compiler does not guess.

        /* -- .Count them with `nm`. --
         * The tool from the preparation. Build this snippet as Debug and list the symbols of its object file,
         * `cmake-build-debug/0x04/CMakeFiles/c_templates_per_type.dir/ii_session/c_templates_per_type.cpp.o`:
         * `nm -C <file> | grep largest` - `largest<int>`, `largest<double>` and one for `string`, with a long name,
         * each with a `t`, because they are in the unnamed namespace. `grep smallest` finds nothing: a template that is
         * never used is never compiled into code. `print_all` is there three times, too.
         * - Without `-C`, the names show the template argument: `...largestIiE...` for `int`, `IdE` for `double`.
         * - As Release, most of them are gone - inlined, like any small function.
         */
    }

    /* --- `maybe` ---
     * A class template: the `optional_int` from 'Coral Creek', with any type `T` instead of `int` - a value and a flag.
     * The type goes into the angle brackets, `maybe<int>`, as for `vector<int>`. `twice` only makes sense for numbers.
     */
    template <typename T>
    class maybe {
    public:
        maybe() = default;
        explicit maybe(const T& value) : value_{value}, has_value_{true} {}

        bool has_value() const { return has_value_; }
        T value_or(const T& fallback) const { return has_value_ ? value_ : fallback; }
        T twice() const { return value_ * 2; }

    private:
        T value_{};
        bool has_value_{false};
    };

    /* --- `show_sizes_per_type` ---
     * `maybe<char>`, `maybe<int>` and `maybe<double>` are three classes with three layouts: the value, one `bool`, and
     * padding up to the alignment of `T` (see previous snippets). One recipe, three sizes.
     */
    void show_sizes_per_type() {
        print_function_header();

        cout << " 1| sizeof(maybe<char>)=" << sizeof(maybe<char>) << ", sizeof(maybe<int>)=" << sizeof(maybe<int>)
             << ", sizeof(maybe<double>)=" << sizeof(maybe<double>) << '\n';
    }

    /* --- `use_members_on_demand` ---
     * `maybe<string>` compiles - although `twice` would multiply a `string` by 2. The member functions of a class
     * template are generated only when they are called; `twice` is never called for `string`, so it never has to
     * compile for it. Remove the `//` in front of `words.twice()` and read the error: it points into the template,
     * and names the instantiation that failed.
     * `nm` shows it, too: `maybe<int>::twice() const` is there, `maybe<double>::twice` is not.
     */
    void use_members_on_demand() {
        print_function_header();

        const maybe<int> number{21};
        const maybe<double> nothing{};
        const maybe<string> words{"hello"};
        cout << " 1| " << number.twice() << ", " << nothing.value_or(-1.0) << ", " << words.value_or("-") << '\n';
        // cout << words.twice();           // compiler error, but only now

        /* -- .Q&A -- !![Why is a template not checked completely when it is defined?](#a-405) */
    }

    /* --- `maybe<bool>` ---
     * A full specialization: for `bool`, the recipe is replaced completely - here by one `char` with three states,
     * instead of two `bool`s. The class may look entirely different; only the name is the same.
     * - !![#specialization]
     */
    template <>
    class maybe<bool> {
    public:
        maybe() = default;
        explicit maybe(const bool value) : state_{value ? '2' : '1'} {}

        bool has_value() const { return state_ != '0'; }
        bool value_or(const bool fallback) const { return state_ == '0' ? fallback : state_ == '2'; }

    private:
        char state_{'0'};                   // '0': empty, '1': false, '2': true
    };

    /* --- `maybe<T*>` ---
     * A partial specialization: for every pointer type. A pointer has a free value that means "nothing" - `nullptr` -
     * so no flag is needed, and no padding: one word instead of two.
     */
    template <typename T>
    class maybe<T*> {
    public:
        maybe() = default;
        explicit maybe(T* const value) : value_{value} {}

        bool has_value() const { return value_ != nullptr; }
        T* value_or(T* const fallback) const { return value_ != nullptr ? value_ : fallback; }

    private:
        T* value_{nullptr};
    };

    /* --- `specialize_the_layout` ---
     * The compiler picks the most specialized version that fits: `maybe<bool>` the full one, `maybe<int*>` the
     * partial one, `maybe<char>` and `maybe<long long>` the general recipe - for types of the same size as `bool` and,
     * on 64-bit platforms, as a pointer. The standard library does the same, e.g. `vector<bool>`, which
     * stores single bits.
     */
    void specialize_the_layout() {
        print_function_header();

        int n{42};
        const maybe<bool> flag{false};
        const maybe<int*> address{&n};
        cout << " 1| sizeof(maybe<bool>)=" << sizeof(maybe<bool>) << ", sizeof(maybe<char>)=" << sizeof(maybe<char>)
             << '\n';
        cout << " 2| sizeof(maybe<int*>)=" << sizeof(maybe<int*>) << ", sizeof(maybe<long long>)="
             << sizeof(maybe<long long>) << '\n';
        cout << " 3| flag: " << flag.has_value() << ", " << flag.value_or(true) << '\n';
        cout << " 4| address: " << address.has_value() << ", *value=" << *address.value_or(nullptr) << '\n';

        /* -- .Q&A -- !![`maybe<int*>` has no flag. What does it lose?](#a-406) */
    }

    /* --- `bits_used` ---
     * A function template can be fully specialized, too. For most types, the answer is the bytes times 8 - for `bool`,
     * one bit carries the information.
     * Often an ordinary overload does the same job, and more simply; specialize a function template only if there is
     * nothing to deduce from, as here.
     */
    template <typename T>
    size_t bits_used() {
        return 8 * sizeof(T);
    }

    template <>
    size_t bits_used<bool>() {
        return 1;
    }

    /* --- `specialize_a_function` --- */
    void specialize_a_function() {
        print_function_header();

        cout << " 1| int: " << bits_used<int>() << ", double: " << bits_used<double>()
             << ", bool: " << bits_used<bool>() << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_a_function_template();
    instantiate_per_type();
    show_sizes_per_type();
    use_members_on_demand();
    specialize_the_layout();
    specialize_a_function();

    return EXIT_SUCCESS;
}
