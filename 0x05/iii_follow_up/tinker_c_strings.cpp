// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `<cstring>`: the C functions for C strings and for raw memory. Each of them trusts you with the sizes.
 * - `strcpy` and `strcat` write as many characters as the source has - the destination must be big enough, and nobody
 *   checks it.
 * - `strncpy` stops at a count - and may leave the result without a `'\0'`.
 * - `strchr` and `strstr` return a pointer into the text, or `nullptr`.
 * - `memcpy`, `memset` and `memcmp` work on bytes: a count in bytes, no `'\0'`, no types.
 * - From a `std::string` to a C string with `c_str()`, and back with a constructor. `std::string` does the size work
 *   for you.
 */

#include <iostream>
#include <string>
#include <cstring>                          // for strcpy, strncpy, strcat, strstr, memcpy, memset, memcmp
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;
using std::strlen, std::strcmp, std::strcpy, std::strncpy, std::strcat, std::strchr, std::strstr;
using std::memcpy, std::memset, std::memcmp;


/* ---- Content ---- */

namespace {

    /* --- `copy_into_a_buffer` ---
     * A buffer is an array of `char` with room for the text and its `'\0'`. `strcpy` copies up to and including the
     * `'\0'`, `strcat` appends at the `'\0'` it finds. Neither knows how big `buffer` is - it is only an address.
     * With 40 bytes, `strcpy` would write 24 of them past the end of `buffer` - gcc and clang warn, if they can see it.
     * - !![#c-string]
     */
    void copy_into_a_buffer() {
        print_function_header();

        char buffer[16]{};                  // 15 characters and the '\0'
        strcpy(buffer, "Kind of Blue");
        strcat(buffer, "!");
        cout << " 1| buffer='" << buffer << "', strlen=" << strlen(buffer) << ", sizeof=" << sizeof(buffer) << '\n';
        // strcpy(buffer, "A Love Supreme, Part I: Acknowledgement");      // undefined behavior: 40 bytes into 16
    }

    /* --- `copy_with_a_limit` ---
     * `strncpy(dest, src, n)` copies at most `n` characters. If the source is longer, it stops - and does not write a
     * `'\0'`. The result is not a C string until you add one. So the idiom is: copy one less than the size, and set the
     * last character yourself.
     */
    void copy_with_a_limit() {
        print_function_header();

        const char title[]{"Kind of Blue"};
        char small[5];
        strncpy(small, title, sizeof(small) - 1);
        small[sizeof(small) - 1] = '\0';
        cout << " 1| small='" << small << "'\n";
    }

    /* --- `compare_and_search` ---
     * `strcmp` returns 0 for equal texts, a negative number if the first one sorts first, a positive one otherwise.
     * `strchr` searches a character, `strstr` a text; both return a pointer into the text - and the difference to its
     * start is the index - or `nullptr`.
     */
    void compare_and_search() {
        print_function_header();

        const char* const title{"Kind of Blue"};
        cout << " 1| strcmp(\"Blue\", \"Kind\")=" << (strcmp("Blue", "Kind") < 0 ? "negative" : "not negative") << '\n';
        if (const char* found{strchr(title, 'B')}; found != nullptr) {
            cout << " 2| 'B' at index " << found - title << ", the rest: " << found << '\n';
        }
        if (strstr(title, "Green") == nullptr) {
            cout << " 3| no 'Green' in '" << title << "'\n";
        }
    }

    /* --- `copy_raw_memory` ---
     * A C array cannot be assigned (see previous snippets), but its bytes can be copied: `memcpy(dest, src, bytes)`.
     * The count is in bytes, so `sizeof` - not the number of elements. `memset` sets every byte to a value, `memcmp`
     * compares bytes.
     * The trap: `memset(b, 1, sizeof(b))` does not make four 1s - it makes every byte 1.
     * In C++, `std::copy` and `std::fill` do the same with elements and types - and for types like `int`, they end up
     * as the same byte copy.
     */
    void copy_raw_memory() {
        print_function_header();

        const int a[4]{1, 2, 3, 4};
        int b[4]{};
        memcpy(b, a, sizeof(a));
        cout << " 1| b[3]=" << b[3] << ", equal: " << (memcmp(a, b, sizeof(a)) == 0) << '\n';

        memset(b, 0, sizeof(b));
        cout << " 2| after memset 0: b[0]=" << b[0] << '\n';
        memset(b, 1, sizeof(b));
        cout << " 3| after memset 1: b[0]=" << b[0] << '\n';

        /* -- .Q&A -- !![Why is `b[0]` not 1?](#a-509) */
    }

    /* --- `convert_to_and_from_string` ---
     * `string{p}` walks to the `'\0'` and copies the characters; `string{p, n}` takes exactly `n` of them - the way to
     * make a `string` of a part. `c_str()` gives the way back, with a `'\0'`, valid until the string is changed or
     * destroyed.
     */
    void convert_to_and_from_string() {
        print_function_header();

        const char* const title{"Kind of Blue"};
        const string whole{title};
        const string first_word{title, 4};
        cout << " 1| whole='" << whole << "', first_word='" << first_word << "'\n";
        cout << " 2| strlen(whole.c_str())=" << strlen(whole.c_str()) << '\n';
    }

}

/* --- `main` --- */
int main() {
    copy_into_a_buffer();
    copy_with_a_limit();
    compare_and_search();
    copy_raw_memory();
    convert_to_and_from_string();

    return EXIT_SUCCESS;
}
