// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'South Birds Gale', see ../tasks.md.

#include <iostream>
#include <memory>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::unique_ptr, std::make_unique, std::strlen, std::size_t;

// The caller owns the result and must `delete[]` it.
char* reverse(const char* text) {
    const size_t length{strlen(text)};
    char* result{new char[length + 1]};
    for (size_t i{0}; i < length; ++i) {
        result[i] = text[length - 1 - i];
    }
    result[length] = '\0';
    return result;
}

// The same, and the caller gets the owner with it.
unique_ptr<char[]> make_reverse(const char* text) {
    const size_t length{strlen(text)};
    unique_ptr<char[]> result{make_unique<char[]>(length + 1)};     // all '\0' already
    for (size_t i{0}; i < length; ++i) {
        result[i] = text[length - 1 - i];
    }
    return result;                          // no std::move: built in the caller's place, or moved
}

// Extension: yes - `unique_ptr<char[]>` takes over the block and calls `delete[]`. It is safe as long as nothing can
// throw between the `new[]` in `reverse` and the constructor of the `unique_ptr`. With `unique_ptr<char>`, it would
// call `delete` instead of `delete[]` - undefined behavior; the compiler does not stop you.
unique_ptr<char[]> make_reverse_by_wrapping(const char* text) {
    return unique_ptr<char[]>{reverse(text)};
}

int main() {
    const heap_watch heap{};
    for (const char* text : {"stressed", "Sei fein, nie fies", "a", ""}) {
        char* reversed{reverse(text)};
        cout << "reverse('" << text << "') = '" << reversed << "'\n";
        delete[] reversed;                  // forget this, and live() below is 4
    }
    cout << "after reverse: live=" << heap.live() << '\n';

    {
        const unique_ptr<char[]> r{make_reverse("desserts")};
        const unique_ptr<char[]> s{make_reverse_by_wrapping("drawer")};
        cout << "make_reverse: '" << r.get() << "', '" << s.get() << "'\n";
    }                                       // both deleted here - nothing for the caller to forget
    cout << "after make_reverse: live=" << heap.live() << '\n';

    return EXIT_SUCCESS;
}
