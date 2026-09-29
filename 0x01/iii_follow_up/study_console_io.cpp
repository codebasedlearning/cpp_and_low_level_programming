// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Output with `cout` and `<<`, input with `cin` and `>>`.
 * - From now on, tasks may read their input from the console.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::cin, std::string;


/* ---- Content ---- */

namespace {

    /* --- `print_to_console` ---
     * `<<` shifts values into the stream, one after the other - any expression that has a value can be printed.
     */
    void print_to_console() {
        print_function_header();

        const int i{23};
        cout << " 1| i=" << i << '\n';
        cout << " 2| 2*i+3=" << 2 * i + 3 << ", i*i=" << i * i << '\n';
    }

    /* --- `read_from_console` ---
     * `>>` reads from the console into a variable. For numbers it skips leading spaces and stops at the first character
     * that does not fit. For a `string` it reads one word, up to the next space.
     * There is no error handling here yet - for what happens with wrong input, see future snippets.
     */
    void read_from_console() {
        print_function_header();

        int n{};
        cout << " 1| Enter a number n: ";
        cin >> n;

        string word{};
        cout << " 2| Enter a word: ";
        cin >> word;

        cout << " 3| n=" << n << ", n*n=" << n * n << ", word='" << word << "'\n";
    }

}

/* --- `main` --- */
int main() {
    print_to_console();
    read_from_console();

    return EXIT_SUCCESS;
}
