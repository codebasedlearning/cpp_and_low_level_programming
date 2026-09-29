// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A stream has a state: good, fail, eof, bad - like a small status register.
 * - After a failed read, every further read fails until the state is cleared.
 */

#include <iostream>
#include <limits>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::cin;


/* ---- Content ---- */

namespace {

    /* --- `read_with_check` ---
     * `cin >> n` can be used as a condition: it is true if the read succeeded.
     * Enter a word instead of a number and watch what happens.
     * - !![#streams]
     */
    void read_with_check() {
        print_function_header();

        int n{};
        cout << " 1| Enter a number n: ";
        if (cin >> n) {
            cout << " 2| n=" << n << ", good=" << cin.good() << '\n';
            return;
        }
        cout << " 3| failed, good=" << cin.good() << ", fail=" << cin.fail() << '\n';

        // The state is sticky: this read fails, too - nothing is read at all.
        int m{};
        cin >> m;
        cout << " 4| second read: fail=" << cin.fail() << '\n';

        // Reset the state and throw away the rest of the line - the usual idiom.
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << " 5| after clear and ignore: good=" << cin.good() << '\n';
    }

}

/* --- `main` --- */
int main() {
    read_with_check();

    return EXIT_SUCCESS;
}
