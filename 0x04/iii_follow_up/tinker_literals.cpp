// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The suffixes `s` and `sv`: `"text"s` is a `std::string`, `"text"sv` a `std::string_view` - not a `const char*`.
 * - A `sv` literal knows its length at compile time. A view built from a `const char*` has to count - `strlen` - at
 *   runtime.
 * - A `'\0'` inside the text: the `const char*` stops there, the literals do not.
 */

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::string_view;

/* -- .The literal operators. --
 * `s` and `sv` are functions with funny names, `operator""s` and `operator""sv`, in the namespace `std::literals`. Many
 * programs write `using namespace std::literals;` - the one `using namespace` that is widely accepted, because it pulls
 * in nothing but suffixes. Here we name the two operators, as we name everything else.
 */
using std::operator""s, std::operator""sv;


/* ---- Content ---- */

namespace {

    /* --- `use_suffixes` ---
     * Three initializers that look almost the same, three types - `auto` shows the difference.
     */
    void use_suffixes() {
        print_function_header();

        const auto plain{"Kind of Blue"};   // const char*
        const auto text{"Kind of Blue"s};   // string
        const auto view{"Kind of Blue"sv};  // string_view
        cout << " 1| sizeof: " << sizeof(plain) << ", " << sizeof(text) << ", " << sizeof(view) << '\n';
        cout << " 2| sizes: " << text.size() << ", " << view.size() << '\n';
    }

    /* --- `count_the_characters` ---
     * `"Kind of Blue"sv` is `constexpr`: its size is known while compiling - `static_assert` checks it then, at no cost
     * at runtime. A view built from a `const char*` - a parameter, say - must find the `'\0'` first: in Compiler
     * Explorer with `-O2`, `count` ends in a jump to `strlen`.
     */
    size_t count(const char* const text) {
        const string_view view{text};
        return view.size();
    }

    void count_the_characters() {
        print_function_header();

        constexpr auto title{"Kind of Blue"sv};
        static_assert(title.size() == 12);  // checked by the compiler
        cout << " 1| title.size()=" << title.size() << ", count(...)=" << count("Kind of Blue") << '\n';
    }

    /* --- `hide_a_zero` ---
     * `"ab\0cd"` is six bytes: five characters and the terminating `'\0'`. A view from the `const char*` counts up to
     * the first `'\0'` - 2. The literals take the length from the compiler - 5, including the zero in the middle.
     */
    void hide_a_zero() {
        print_function_header();

        const string_view from_pointer{"ab\0cd"};
        const auto from_sv{"ab\0cd"sv};
        const auto from_s{"ab\0cd"s};
        cout << " 1| sizeof(\"ab\\0cd\")=" << sizeof("ab\0cd") << ", from_pointer=" << from_pointer.size()
             << ", from_sv=" << from_sv.size() << ", from_s=" << from_s.size() << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_suffixes();
    count_the_characters();
    hide_a_zero();

    return EXIT_SUCCESS;
}
