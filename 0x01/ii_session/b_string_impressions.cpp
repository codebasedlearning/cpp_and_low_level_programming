// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::string` - looks familiar, but it is a value, not a reference.
 * - The object has a fixed size; where do the characters live?
 * - Growing a string may move its characters - a first "invisible cost".
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

namespace {

    /* --- `use_strings` ---
     * Looks like in any other language: an object with member functions.
     * - !![#string]
     */
    void use_strings() {
        print_function_header();

        string hello{"Hello!"};
        cout << " 1| hello='" << hello << "'\n";

        cout << " 2| hello.size()=" << hello.size()
             << ", hello.empty()=" << hello.empty()
             << ", hello.substr(1,3)='" << hello.substr(1, 3) << "'\n";

        // op+ exists for strings.
        cout << " 3| hello + \" C++\"='" << hello + " C++" << "'\n";

        /* -- .No `new`, no `()`. --
         * `string s;` already is an empty string, initialized by its default constructor. It is neither
         * `string s = new string();` (Java) nor `string s();` - that is valid syntax, but declares a function!
         * One more reason for braces: `string s{};`.
         */
        string s;
        cout << " 4| s='" << s << "', s.size()=" << s.size() << '\n';

        /* -- .Q&A -- !![Where are differences here to, say, Java?](#a-106) */
    }

    /* --- `show_where_strings_live` ---
     * A `string` object has a fixed size, no matter how long its text is.
     * So where are the characters? `c_str()` gives their address.
     * - !![#stack-and-heap]
     */
    void show_where_strings_live() {
        print_function_header();

        int n{23};
        string s1{"C++"};
        string s2{"A rather long text that certainly does not fit into the object itself."};
        int m{42};

        // `const void*` is an address without a type (a pointer, more on that later) - `cout` prints it as an address,
        // whereas a `const char*` is printed as text.
        const void* chars1{s1.c_str()};
        const void* chars2{s2.c_str()};

        cout << " 1| sizeof(string)=" << sizeof(string) << " bytes\n";
        cout << " 2| &n =" << &n << '\n';
        cout << " 3| &s1=" << &s1 << ", characters at " << chars1 << '\n';
        cout << " 4| &s2=" << &s2 << ", characters at " << chars2 << '\n';
        cout << " 5| &m =" << &m << '\n';

        /* -- .Compare the addresses. --
         * - The objects `s1` and `s2` sit on the stack, next to `n` and `m`.
         * - The characters of the short `s1` lie inside the object, those of the long `s2` somewhere else entirely - on
         *   the heap.
         * - The threshold depends on the library, gcc and clang differ.
         * - !![#sso]
         */

        /* -- .Q&A -- !![Is `sizeof(s2)` bigger than `sizeof(s1)`?](#a-107) */
    }

    /* --- `copy_strings` ---
     * Assignment copies the characters - afterwards there are two independent strings, not two references to one string
     * as in Java.
     */
    void copy_strings() {
        print_function_header();

        string s1{"Intro"};
        string s2{};
        s2 = s1;                            // a copy, not a reference
        s2 += "!";
        cout << " 1| s1='" << s1 << "', s2='" << s2 << "'\n";

        const void* before{s2.c_str()};
        cout << " 2| &s2=" << &s2 << ", characters at " << before << '\n';

        s2 += " And now the text gets much too long for the object itself.";
        const void* after{s2.c_str()};
        cout << " 3| &s2=" << &s2 << ", characters at " << after << '\n';

        /* -- .Q&A -- !![Why did the characters move, but not `s2` itself? What did that cost?](#a-108) */
    }

}

/* --- `main` --- */
int main() {
    use_strings();
    show_where_strings_live();
    copy_strings();

    return EXIT_SUCCESS;
}
