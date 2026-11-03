// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - In almost every expression, an array converts to the address of its first element: array decay. The count is
 *   lost.
 * - A parameter `int a[4]` is an `int*` - the 4 is a comment the type system ignores.
 * - So C passes an address and a count - or marks the end in the data, as a C string does with its `'\0'`.
 * - `a` and `&a` are the same address with two types - and the type decides how far `+ 1` goes.
 * - `std::span` takes the count before it is lost; `std::array` never loses it.
 * - The length of a C string is found by walking: `strlen` counts up to the `'\0'`.
 * - `cout` prints a `const char*` as text, `==` compares the addresses, `strcmp` the characters.
 * - C functions have names without mangling - `extern "C"`.
 */

#include <iostream>
#include <string>
#include <span>
#include <array>
#include <iterator>
#include <cstring>                          // for strlen, strcmp, strchr
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::span, std::array, std::size;
using std::strlen, std::strcmp, std::strchr, std::size_t, std::ptrdiff_t;


/* ---- Content ---- */

namespace {

    /* --- `show_the_decay` ---
     * `const int* p{a};` - no `&`, and still an address: the array has decayed to a pointer to its first element. From
     * then on, only the address is left: `sizeof(p)` is the size of a pointer, and nothing in `p` says "4".
     * That is also how `a[2]` works on the array itself: `a` decays to `&a[0]`, and `a[2]` is `*(a + 2)` (see previous
     * snippets).
     * Where an array does not decay: in `sizeof(a)`, in `&a`, and when it is bound to a reference. That is how
     * `std::size(a)` and the range-based `for` know the count - they get the array itself, not its address.
     * - !![#pointer-arithmetic]
     */
    void show_the_decay() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        const int* p{a};
        cout << " 1| a=" << a << ", &a[0]=" << &a[0] << ", p=" << p << '\n';
        cout << " 2| sizeof(a)=" << sizeof(a) << ", size(a)=" << size(a) << ", sizeof(p)=" << sizeof(p) << '\n';
        cout << " 3| a[2]=" << a[2] << ", *(a+2)=" << *(a + 2) << ", p[2]=" << p[2] << '\n';
    }

    /* --- `sum_four` ---
     * Looks like a function that takes an array of four `int`s. It takes a `const int*`: an array parameter is adjusted
     * to a pointer, and the 4 is thrown away. Inside, `values` is only an address - `sizeof(values)` would be 8, and
     * the compiler warns about it. Remove the `//` and read the warning.
     */
    int sum_four(const int values[4]) {
        // cout << sizeof(values);          // warning: will return the size of 'const int*'
        return values[0] + values[1] + values[2] + values[3];
    }

    /* --- `sum_all` --- The C way: the address, and the count as a second parameter. */
    int sum_all(const int* values, const size_t count) {
        int sum{0};
        for (size_t i{0}; i < count; ++i) {
            sum += values[i];
        }
        return sum;
    }

    /* --- `pass_an_array` ---
     * `sum_four` accepts any address of an `int` - also of an array with two elements. It compiles, and it reads two
     * `int`s that are not there. The 4 was a promise nobody checks: at best, gcc warns as Release, after inlining.
     * C's answer is a second parameter, the count - and the caller must get it right. `std::size(a)` does, as long as
     * the array itself is in sight.
     */
    void pass_an_array() {
        print_function_header();

        const int a[4]{1, 2, 3, 4};
        const int b[2]{1, 2};
        cout << " 1| sum_four(a)=" << sum_four(a) << '\n';
        // cout << sum_four(b);             // compiles - undefined behavior: reads b[2] and b[3]
        cout << " 2| sum_all(a, size(a))=" << sum_all(a, size(a)) << ", sum_all(b, size(b))=" << sum_all(b, size(b))
             << '\n';
    }

    /* --- `keep_the_count` ---
     * The two ways out of the decay, from the previous units.
     * - A `span` is built from the array before it decays: it takes the address and the count from the type
     *   `const int[4]` - and then it is a view, two words (see previous snippets).
     * - A `std::array` is a class around a C array: it is passed like any object, by value or by reference, and it
     *   never turns into a pointer. `size()` is part of its type.
     * - !![#span]
     */
    void keep_the_count() {
        print_function_header();

        const int a[4]{1, 2, 3, 4};
        const span<const int> view{a};
        const array<int, 4> numbers{1, 2, 3, 4};
        cout << " 1| view.size()=" << view.size() << ", view.data()=" << view.data() << ", a=" << a << '\n';
        cout << " 2| numbers.size()=" << numbers.size() << ", sizeof(numbers)=" << sizeof(numbers) << '\n';
    }

    /* --- `compare_a_and_its_address` ---
     * `a` decays to the address of the first element, an `int*`. `&a` is the address of the whole array - the same
     * number, but of type `const int (*)[4]`, "pointer to an array of four `int`s". Now add 1: the type decides the
     * step. `a + 1` goes one `int` further, `&a + 1` one whole array, 16 bytes.
     */
    void compare_a_and_its_address() {
        print_function_header();

        const int a[4]{10, 20, 30, 40};
        cout << " 1| a=" << a << ", &a=" << &a << '\n';
        cout << " 2| a+1=" << a + 1 << ", &a+1=" << &a + 1 << '\n';
        cout << " 3| sizeof(*a)=" << sizeof(*a) << ", sizeof(*&a)=" << sizeof(*&a) << '\n';
    }

    /* --- `show_a_c_string` ---
     * A C string is an array of `char` with a `'\0'` at the end - the character with the value 0. The literal `"Kind"`
     * has five: four letters and the zero, which the compiler adds. The zero is the only thing that says where the text
     * ends: the array decays, the count is lost, the `'\0'` stays in the data.
     * `cout << p` for a `const char*` does not print the address: it prints the characters, up to the `'\0'`. That is
     * why this course prints addresses of characters via `const void*`.
     * - !![#c-string]
     */
    void show_a_c_string() {
        print_function_header();

        const char text[]{"Kind"};
        const char* p{text};
        const void* where{p};
        cout << " 1| sizeof(text)=" << sizeof(text) << ", strlen(text)=" << strlen(text) << '\n';
        cout << " 2| p as text: " << p << ", p as address: " << where << '\n';

        const int numbers[2]{1, 2};
        cout << " 3| numbers: " << numbers << '\n';

        /* -- .Memory view. --
         * Set a breakpoint on ` 1|` and look at `&text`: `4b 69 6e 64 00` - 'K', 'i', 'n', 'd', and the zero.
         */

        /* -- .Q&A -- !![Why is `numbers` printed as an address, but `p` as text?](#a-504) */
    }

    /* --- `length_of` ---
     * `strlen`, rewritten: walk to the `'\0'`, and return the distance to the start - a pointer difference.
     */
    ptrdiff_t length_of(const char* text) {
        const char* p{text};
        while (*p != '\0') {
            ++p;
        }
        return p - text;
    }

    /* --- `walk_to_the_zero` ---
     * Both count the same characters, and both walk through all of them: the length of a C string costs time
     * proportional to its length, every time. A `std::string` stores its size - `size()` walks nothing.
     */
    void walk_to_the_zero() {
        print_function_header();

        const char title[]{"A Love Supreme"};
        cout << " 1| length_of=" << length_of(title) << ", strlen=" << strlen(title) << '\n';
    }

    /* --- `compare_c_strings` ---
     * `pa == pb` compares two addresses - two arrays, two places, so `false`, although the characters are the same.
     * `strcmp` compares the characters, and returns 0 if they are equal (negative or positive otherwise, as for
     * sorting). As soon as one side is a `std::string`, `==` compares the characters again.
     * Two literals with the same text may or may not share their characters - the compiler decides. So never compare
     * C strings with `==`.
     */
    void compare_c_strings() {
        print_function_header();

        const char a[]{"Blue"};
        const char b[]{"Blue"};
        const char* pa{a};
        const char* pb{b};
        cout << " 1| pa==pb: " << (pa == pb) << ", strcmp(pa, pb)=" << strcmp(pa, pb) << '\n';
        cout << " 2| string{pa}==pb: " << (string{pa} == pb) << '\n';
    }

    /* --- `call_a_c_function` ---
     * A C function expects a C string. `c_str()` gives the address of the characters of a `std::string` - with a
     * `'\0'` behind them, guaranteed. The other way round, a `string` is built from a `const char*` by walking to the
     * `'\0'`, once.
     */
    void call_a_c_function() {
        print_function_header();

        const string album{"Kind of Blue"};
        cout << " 1| strlen(album.c_str())=" << strlen(album.c_str()) << ", album.size()=" << album.size() << '\n';
    }

}

/* --- `count_vowels` ---
 * A function for C programs: `extern "C"` switches off name mangling, so it is `count_vowels` for the linker - a name C
 * can call. C has no overloading, so it needs nothing more than the name. It is outside the unnamed namespace, like the
 * functions in the preparation of unit 0x04.
 */
extern "C" int count_vowels(const char* text) {
    int vowels{0};
    for (const char* p{text}; *p != '\0'; ++p) {
        if (strchr("aeiouAEIOU", *p) != nullptr) {
            ++vowels;
        }
    }
    return vowels;
}

/* --- `extern "C"` ---
 * Build this snippet and list the symbols of its object file,
 * `cmake-build-debug/0x05/CMakeFiles/b_arrays_decay.dir/ii_session/b_arrays_decay.cpp.o`:
 * `nm <file> | grep -E 'count_vowels|strlen|strcmp|strchr|length_of'`.
 * - `T count_vowels` - defined here, and no `_Z`: the plain name.
 * - `U strlen`, `U strcmp`, `U strchr` - the C library, plain names as well: `<cstring>` declares them in an
 *   `extern "C"` block.
 * - `t _ZN12_GLOBAL__N_19length_ofEPKc` - a C++ function, mangled as usual, and local.
 * On macOS, C names get one `_` in front: `_count_vowels`, `_strlen`.
 * The calling convention is the same for C and C++ functions - `extern "C"` changes the name, not the registers.
 * - !![#extern-c]
 */

namespace {

    /* --- `use_a_c_function` --- `count_vowels` is an ordinary function for C++, too. */
    void use_a_c_function() {
        print_function_header();

        cout << " 1| count_vowels(\"Kind of Blue\")=" << count_vowels("Kind of Blue") << '\n';
    }

}

/* --- `main` --- */
int main() {
    show_the_decay();
    pass_an_array();
    keep_the_count();
    compare_a_and_its_address();
    show_a_c_string();
    walk_to_the_zero();
    compare_c_strings();
    call_a_c_function();
    use_a_c_function();

    return EXIT_SUCCESS;
}
