// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The everyday `string` operations: access, search, modify.
 * - `[]` vs. `at()`: without and with bounds check.
 * - `string::npos` - the answer "not found".
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

namespace {

    /* --- `access_characters` ---
     * `[]` does not check the index - a wrong one is undefined behaviour.
     * `at()` checks it and stops the program with an exception (more on exceptions later). You pay for the check only
     * if you ask for it.
     * - !![#undefined-behavior]
     */
    void access_characters() {
        print_function_header();

        string s{"Example"};
        cout << " 1| s='" << s << "', s.size()=" << s.size() << ", s.empty()=" << s.empty() << '\n';
        cout << " 2| s[1]='" << s[1] << "', s.at(1)='" << s.at(1) << "'\n";
        cout << " 3| s + s='" << s + s << "'\n";

        // cout << s.at(42);                // throws an exception - try it
    }

    /* --- `search_strings` ---
     * `find` searches from the front, `rfind` from the back. Both return the position - or `string::npos` if there is
     * nothing to find.
     */
    void search_strings() {
        print_function_header();

        const string s{"Example"};

        string::size_type pos{s.find("amp")};
        cout << " 1| find(\"amp\"): pos=" << pos << ", found=" << (pos != string::npos) << '\n';

        pos = s.find("xyz");
        cout << " 2| find(\"xyz\"): pos=" << pos << ", found=" << (pos != string::npos) << '\n';

        pos = s.rfind("e");
        cout << " 3| rfind(\"e\"): pos=" << pos << '\n';
    }

    /* --- `modify_strings` --- `replace`, `erase`, `insert` and `append` change the string itself. */
    void modify_strings() {
        print_function_header();

        string s{"Example!"};

        s.replace(0, 4, "Sam");             // from position 0, replace 4 characters
        cout << " 1| replace: s='" << s << "'\n";

        s.erase(s.size() - 1, 1);           // from the last position, erase 1 character
        cout << " 2| erase:   s='" << s << "'\n";

        s.insert(0, "XXL-");
        cout << " 3| insert:  s='" << s << "'\n";

        s.append("s");                      // same as: s += "s";
        cout << " 4| append:  s='" << s << "'\n";
    }

}

/* --- `main` --- */
int main() {
    access_characters();
    search_strings();
    modify_strings();

    return EXIT_SUCCESS;
}
