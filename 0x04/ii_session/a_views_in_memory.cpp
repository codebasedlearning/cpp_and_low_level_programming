// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A `string_view` is two words: where the characters are, and how many - 16 bytes, whatever the length of the text.
 * - It owns nothing: no copy, no allocation - and nobody keeps the characters alive for it.
 * - `substr` of a `string` copies, `substr` of a view only moves the two words.
 * - `data()` of a view is not a C string: the view knows its length, the characters behind it do not.
 * - `std::span` is the same idea for elements: a view of an array, a vector, or a part of them.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `operator<<` returns the stream. What would stop working if it returned `void`?
 * - What does the compiler do if you ignore the result of a `[[nodiscard]]` function - an error or a warning?
 * - `count_words` takes a `string_view`. Which three kinds of text did it get - and which of them was copied?
 * - In the output of `nm`: what does `U` mean - and why does `area` appear under two different names?
 */

#include <iostream>
#include <string>
#include <string_view>
#include <span>                             // for span
#include <array>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::string_view;
using std::span, std::array, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `show_a_view` ---
     * The view points to the characters of `text` - same address - and knows their number. `sizeof` is two words, a
     * pointer and a size, for a text of 18 characters and for one of 18 million.
     * Addresses of characters are printed via `const void*`: `cout` prints a `char*` as text, not as an address.
     * - !![#string-view]
     */
    void show_a_view() {
        print_function_header();

        const string text{"Kind of Blue, 1959"};
        const string_view view{text};
        const void* chars{text.data()};
        const void* viewed{view.data()};
        cout << " 1| sizeof(view)=" << sizeof(view) << ", view.size()=" << view.size() << '\n';
        cout << " 2| text.data()=" << chars << ", view.data()=" << viewed << '\n';

        /* -- .Memory view. --
         * Set a breakpoint on ` 2|` and look at `&view` in the memory view: 16 bytes, two words - 18 = `12` in hex, and
         * the address of the characters (little-endian). Nothing else, no characters. The order depends on the library:
         * gcc's libstdc++ stores the size first, libc++ (Apple clang) and MSVC the address first. Check yours.
         */
    }

    /* --- `take_a_part` ---
     * `text.substr(0, 4)` creates a new `string` with its own characters - a copy, and for longer parts a heap
     * allocation. `view.substr(0, 4)` creates a new view: the same address, a smaller size. Nothing is copied.
     * Look where the copy is: 4 characters fit into the `string` object itself (SSO), so they are on the stack.
     * - !![#sso]
     */
    void take_a_part() {
        print_function_header();

        const string text{"Kind of Blue, 1959"};
        const string_view view{text};

        const string copied{text.substr(0, 4)};
        const string_view viewed{view.substr(0, 4)};
        const void* in_text{text.data()};
        const void* in_copy{copied.data()};
        const void* in_view{viewed.data()};
        cout << " 1| text at " << in_text << '\n';
        cout << " 2| copy at " << in_copy << " '" << copied << "'\n";
        cout << " 3| view at " << in_view << " '" << viewed << "'\n";
    }

    /* --- `try_data_as_text` ---
     * `kind` is 4 characters long - `cout << kind` knows that. But `kind.data()` is only the address, a `const char*`,
     * and `cout` prints a `const char*` up to the next `'\0'`. That is the end of `text`, not of the view.
     * With a view into the middle of a buffer without any `'\0'`, it prints until it happens to find one - undefined
     * behavior. A function that expects a C string (from C libraries, see future snippets) gets
     * `string{view}.c_str()`.
     */
    void try_data_as_text() {
        print_function_header();

        const string text{"Kind of Blue, 1959"};
        const string_view kind{string_view{text}.substr(0, 4)};
        cout << " 1| kind='" << kind << "'\n";
        cout << " 2| kind.data()='" << kind.data() << "'\n";
    }

    /* --- `where_by_ref` and `where_by_view` --- Both only read, both print where the characters are. */
    void where_by_ref(const string& s) {
        const void* chars{s.data()};
        cout << " a|   const string&: characters at " << chars << '\n';
    }

    void where_by_view(const string_view s) {
        const void* chars{s.data()};
        cout << " b|   string_view:   characters at " << chars << '\n';
    }

    /* --- `pass_a_literal` ---
     * A literal is not a `string`: its characters are in the program, in read-only static memory - look at the
     * address, far away from the stack. To call `where_by_ref`, the compiler builds a temporary `string` from it:
     * a copy of all characters, on the heap for a text this long. `where_by_view` gets a view of the literal itself.
     * - !![#temporary]
     */
    void pass_a_literal() {
        print_function_header();

        const char* const title{"A Love Supreme, Part I: Acknowledgement"};
        const void* literal{title};
        cout << " 1| the literal is at " << literal << '\n';
        where_by_ref(title);
        where_by_view(title);

        /* -- .Q&A -- !![Should a `string_view` be passed as `const string_view&`?](#a-402) */
    }

    /* --- `make_title` --- Returns a new `string` - by value. */
    string make_title() {
        return "Kind of Blue - the Miles Davis album from 1959";
    }

    /* --- `try_a_dangling_view` ---
     * `make_title()` returns a temporary `string`. The view points into its characters - and the temporary dies at the
     * `;`. From then on `view` is two words pointing to released memory. Neither gcc 13 nor clang 18 warns here.
     * Binding the temporary to a `const string&` would have kept it alive as long as the reference - a view gets no
     * such favor: it is an object of its own, initialized with an address.
     * - !![#dangling-pointer]
     */
    void try_a_dangling_view() {
        print_function_header();

        const string_view view{make_title()};
        const void* chars{view.data()};
        cout << " 1| view.size()=" << view.size() << ", view.data()=" << chars << " - still two valid words\n";
        // cout << view;                    // undefined behavior: the characters are gone

        /* -- .Q&A -- !![Remove the `//`: what does it print, as Debug and as Release?](#a-403) */
    }

    /* --- `sum_up` ---
     * A `span<const int>` is a view of `int`s next to each other in memory: where the first one is, and how many.
     * Like `string_view`, it takes an `array`, a `vector`, or a part of them - without copying.
     * - !![#span]
     */
    long long sum_up(const span<const int> values) {
        long long sum{0};
        for (const int x : values) {
            sum += x;
        }
        return sum;
    }

    /* --- `show_a_span` ---
     * One function, three sources. `first(2)` and `subspan(1, 2)` are views of a part - the same idea as `substr` of a
     * view.
     */
    void show_a_span() {
        print_function_header();

        const array<int, 4> a{1, 2, 3, 4};
        const vector<int> v{10, 20, 30, 40, 50};
        const span<const int> all{v};
        cout << " 1| sizeof(all)=" << sizeof(all) << ", all.data()=" << all.data() << ", v.data()=" << v.data() << '\n';
        cout << " 2| sum_up(a)=" << sum_up(a) << ", sum_up(v)=" << sum_up(v) << '\n';
        cout << " 3| first two=" << sum_up(all.first(2)) << ", middle three=" << sum_up(all.subspan(1, 3)) << '\n';
    }

    /* --- `double_all` --- A `span<int>` - without `const` - may change the elements it looks at. */
    void double_all(const span<int> values) {
        for (int& x : values) {
            x *= 2;
        }
    }

    /* --- `write_through_a_span` ---
     * The `const` of the parameter belongs to the view - it will not look elsewhere. Whether the elements are `const`
     * is in the angle brackets: `span<int>` or `span<const int>`.
     * A `span` with the length in the type, `span<int, 3>`, needs only one word: the length is known at compile time.
     */
    void write_through_a_span() {
        print_function_header();

        vector<int> v{1, 2, 3, 4, 5};
        double_all(span{v}.last(2));        // only the last two
        cout << " 1| v=" << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << ' ' << v[4] << '\n';

        array<int, 3> a{1, 2, 3};
        const span<int, 3> fixed{a};
        cout << " 2| sizeof(span<int>)=" << sizeof(span<int>) << ", sizeof(span<int, 3>)=" << sizeof(fixed) << '\n';
    }

}

/* --- `main` --- */
int main() {
    show_a_view();
    take_a_part();
    try_data_as_text();
    pass_a_literal();
    try_a_dangling_view();
    show_a_span();
    write_through_a_span();

    return EXIT_SUCCESS;
}
