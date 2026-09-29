// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Thornbury', see ../tasks.md.

#include <iostream>
#include <string>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // copied into your own project

using std::cout, std::string, std::size_t, std::ptrdiff_t, std::strlen;

// Walk to the '\0' - the distance to the start is the length.
ptrdiff_t length(const char* text) {
    const char* p{text};
    while (*p != '\0') {
        ++p;
    }
    return p - text;
}

int count(const char* text, const char c) {
    int result{0};
    for (const char* p{text}; *p != '\0'; ++p) {
        if (*p == c) {
            ++result;
        }
    }
    return result;
}

// At most `size - 1` characters, then the '\0' - always, also if the text does not fit.
bool copy_into(char* dest, const size_t size, const char* src) {
    if (size == 0) {
        return *src == '\0';
    }
    char* const last{dest + size - 1};      // the place for the '\0'
    while (dest != last && *src != '\0') {
        *dest = *src;
        ++dest;
        ++src;
    }
    *dest = '\0';
    return *src == '\0';                    // true if we copied up to the end of `src`
}

void test_copy(const char* text) {
    char buffer[8];
    const bool fitted{copy_into(buffer, sizeof(buffer), text)};
    cout << "copy_into('" << text << "'): '" << buffer << "', fitted=" << fitted << ", length=" << length(buffer)
         << '\n';
}

int main() {
    const char* const title{"Kind of Blue"};
    cout << "length=" << length(title) << " (strlen: " << strlen(title) << "), count of 'o'=" << count(title, 'o')
         << '\n';

    test_copy("Blue");                      // fits
    test_copy("Giant S");                   // exactly 7 characters - fits, with the '\0' in the last byte
    test_copy("Giant Steps");               // too long: 'Giant S', fitted=0

    // Extension: `strncpy(dest, src, n)` stops after `n` characters - without a '\0' if `src` was longer. `copy_into`
    // always terminates, like the non-standard `strlcpy` of the BSDs.

    // Extension: `strlen` walks ten million characters, `size()` reads a member. As Release, e.g. 1 ms vs. far below
    // a microsecond. (The compiler might move a `strlen` out of a loop, so measure it once, and use the result.)
    const string s(10'000'000, 'x');
    stopwatch watch{};
    const size_t by_strlen{strlen(s.c_str())};
    const double strlen_ms{watch.elapsed_ms()};
    watch.reset();
    const size_t by_size{s.size()};
    const double size_ms{watch.elapsed_ms()};
    cout << "strlen: " << by_strlen << " in " << strlen_ms << " ms, size(): " << by_size << " in " << size_ms
         << " ms\n";

    return EXIT_SUCCESS;
}
