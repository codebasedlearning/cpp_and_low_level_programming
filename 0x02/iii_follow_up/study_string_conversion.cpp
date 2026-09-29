// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `stoi`, `stod` and `to_string`: between numbers and text.
 * - What happens with text that is not a number, or a number that does not fit.
 */

#include <iostream>
#include <string>
#include <array>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::array;
using std::stoi, std::stod, std::to_string;


/* ---- Content ---- */

namespace {

    /* --- `convert_text_to_numbers` ---
     * `stoi` throws `invalid_argument` if the text does not start with a number, and `out_of_range` if the number does
     * not fit into an `int`. It stops at the first character that does not fit - "12abc" gives 12.
     * - !![#string-conversion]
     */
    void convert_text_to_numbers() {
        print_function_header();

        const array<string, 5> inputs{"42", " 7", "12abc", "abc", "99999999999"};
        for (const auto& s : inputs) {
            try {
                const int n{stoi(s)};       // may throw
                cout << " 1| stoi(\"" << s << "\")=" << n << '\n';
            } catch (const std::invalid_argument&) {
                cout << " 2| stoi(\"" << s << "\"): not a number\n";
            } catch (const std::out_of_range&) {
                cout << " 3| stoi(\"" << s << "\"): does not fit into an int\n";
            }
        }
        cout << " 4| stod(\"3.14\")=" << stod("3.14") << '\n';
    }

    /* --- `convert_numbers_to_text` --- `to_string` goes the other way. */
    void convert_numbers_to_text() {
        print_function_header();

        const string s{"n=" + to_string(23) + ", x=" + to_string(1.5)};
        cout << " 1| " << s << '\n';
    }

}

/* --- `main` --- */
int main() {
    convert_text_to_numbers();
    convert_numbers_to_text();

    return EXIT_SUCCESS;
}
