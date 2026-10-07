// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::format` (C++20): a format string with `{}` for each argument - checked at compile time, unlike C's `printf`.
 * - Inside the braces, after a `:`: width, fill and alignment, sign, precision and base - `{:>8}`, `{:08.3f}`,
 *   `{:#x}`.
 * - A specialization of `std::formatter` makes your own type formattable (see previous snippets: specializations).
 * - The result is a `string` - on the heap if it is long. `format_to_n` writes into a buffer of your own instead.
 * - The snippet asks the library for `<format>` first, as in unit 0x02 - so that an old library does not break the
 *   build. It is required study nevertheless: if it says "not available", update the compiler (gcc 13 or later).
 */

#include <iostream>
#include <version>
#include <array>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

#if defined(__cpp_lib_format)
#include <format>
#endif

using std::cout, std::string, std::array;


/* ---- Content ---- */

namespace {

    /* --- `money` --- The amount of the unit, in cents. */
    class money {
    public:
        explicit money(const long long cents) : cents_{cents} {}
        long long cents() const { return cents_; }

    private:
        long long cents_;
    };

}

#if defined(__cpp_lib_format)

/* --- `std::formatter<money>` ---
 * A full specialization of the class template `std::formatter` for `money`. It inherits from the one for
 * `long long`: that one reads the part after the `:` - width, fill, alignment - and the `format` below applies it to
 * the cents, then appends " ct". A specialization of a `std` template is one of the few things a program may add to
 * the namespace `std`.
 * - !![#specialization]
 */
template <>
struct std::formatter<money> : std::formatter<long long> {
    auto format(const money& m, std::format_context& context) const {
        const auto out{std::formatter<long long>::format(m.cents(), context)};
        return std::format_to(out, " ct");
    }
};

#endif

namespace {

    /* --- `format_numbers` ---
     * Width, alignment, zeros, sign, base. The format string is checked while compiling: `std::format` takes it as a
     * `std::format_string`, whose constructor is `consteval` (see previous snippets) - it runs in the compiler, counts
     * the braces and checks each specification against the type of its argument.
     * - !![#format]
     */
    void format_numbers() {
        print_function_header();

#if defined(__cpp_lib_format)
        cout << std::format(" 1| [{}] [{:5}] [{:<5}] [{:^5}] [{:05}] [{:+}]\n", 42, 42, 42, 42, 42, 42);
        cout << std::format(" 2| [{:#x}] [{:#b}] [{:#o}] [{:X}]\n", 254, 10, 10, 254);
        // cout << std::format(" 3| {} {}\n", 1);   // compiler error: two `{}`, one argument
#else
        cout << " 1| `std::format` is not available in this library version\n";
#endif
    }

    /* --- `format_floating_point` --- Precision, width, and the three styles: fixed, scientific, general. */
    void format_floating_point() {
        print_function_header();

#if defined(__cpp_lib_format)
        const double pi{3.14159};
        cout << std::format(" 1| [{:.3f}] [{:8.2f}] [{:08.2f}] [{:.2e}] [{:.4g}]\n", pi, pi, pi, 1234.0, 1234.0);
#else
        cout << " 1| `std::format` is not available in this library version\n";
#endif
    }

    /* --- `format_texts` --- Alignment with a fill character, and a text cut to its first characters. */
    void format_texts() {
        print_function_header();

#if defined(__cpp_lib_format)
        cout << std::format(" 1| [{:>10}] [{:<10}] [{:^10}] [{:.3}] [{:*^10}]\n", "So What", "So What", "So What",
                            "So What", "Ho");
#else
        cout << " 1| `std::format` is not available in this library version\n";
#endif
    }

    /* --- `format_own_type` ---
     * `{}` and `{:>10}` for a `money`, through the specialization above. The width is applied to the cents - the " ct"
     * comes after them. To align the whole text, format it into a `string` first, and that one with a width.
     */
    void format_own_type() {
        print_function_header();

#if defined(__cpp_lib_format)
        const money price{1999};
        cout << std::format(" 1| [{}] [{:>10}] [{:<10}]\n", price, price, price);
#else
        cout << " 1| `std::format` is not available in this library version\n";
#endif
    }

    /* --- `format_without_the_heap` ---
     * `std::format` returns a `string`: a short result fits into the `string` object itself, a long one needs a heap
     * block - `heap_watch` counts it (as Debug; see previous snippets). `format_to_n` writes into a buffer you provide,
     * at most `n` characters, and returns where it stopped and how long the whole result would have been - no
     * allocation at all. Mind the `'\0'`: `format_to_n` does not write one, a C string needs it.
     */
    void format_without_the_heap() {
        print_function_header();

#if defined(__cpp_lib_format)
        {
            const heap_watch heap{};
            const string short_text{std::format("{:>8}", 42)};
            cout << " 1| [" << short_text << "] - " << heap.allocations() << " allocations\n";
        }
        {
            const heap_watch heap{};
            const string long_text{std::format("{:>40}", 42)};
            cout << " 2| [" << long_text << "] - " << heap.allocations() << " allocations\n";
        }
        {
            const heap_watch heap{};
            array<char, 64> buffer{};
            const auto result{std::format_to_n(buffer.data(), buffer.size() - 1, "{:>40}|{:.3f}", 42, 3.14159)};
            *result.out = '\0';
            cout << " 3| [" << buffer.data() << "] - " << heap.allocations() << " allocations, " << result.size
                 << " characters\n";
        }
#else
        cout << " 1| `std::format` is not available in this library version\n";
#endif
    }

}

/* --- `main` --- */
int main() {
    format_numbers();
    format_floating_point();
    format_texts();
    format_own_type();
    format_without_the_heap();

    return EXIT_SUCCESS;
}
