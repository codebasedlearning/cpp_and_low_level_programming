// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `operator<<` makes your own class printable: `cout << t` works like `cout << 42`.
 * - `[[nodiscard]]` asks the compiler to warn if a result is thrown away.
 * - `std::string_view` as a parameter: a function that only reads a text takes a literal, a `string`, or a part of one
 *   - without copying it.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>                      // for string_view
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::ostringstream;
using std::string, std::string_view, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `track` --- A piece of music: a title and a length in seconds. */
    class track {
    public:
        track(const string& title, const int seconds) : title_{title}, seconds_{seconds} {}

        /* -- .`[[nodiscard]]`. --
         * An attribute in double brackets: calling `t.seconds();` and ignoring the result is almost certainly a
         * mistake, so the compiler should say so. It is a warning, not an error, and it changes nothing in the
         * program.
         * - !![#nodiscard]
         */
        [[nodiscard]] const string& title() const { return title_; }
        [[nodiscard]] int seconds() const { return seconds_; }

    private:
        string title_;
        int seconds_;
    };

    /* --- `operator<<` ---
     * The output operator for a `track`: a free function with a funny name. `cout << t` calls `operator<<(cout, t)`.
     * It takes the stream by reference - streams cannot be copied - and the track by `const&`, and it only uses the
     * public getters.
     * It returns the stream, so that the next `<<` can go on writing to it.
     */
    ostream& operator<<(ostream& os, const track& t) {
        const int seconds{t.seconds() % 60};
        os << t.title() << " (" << t.seconds() / 60 << ':' << (seconds < 10 ? "0" : "") << seconds << ')';
        return os;
    }

    /* --- `print_a_track` ---
     * `cout << " 1| " << t << '\n'` is a chain: `((cout << " 1| ") << t) << '\n'`. Each `<<` returns `cout`, the
     * left operand of the next one.
     */
    void print_a_track() {
        print_function_header();

        const track t{"So What", 562};
        cout << " 1| " << t << '\n';

        /* -- .Any stream. --
         * `operator<<` takes an `ostream&` - `cout` is one, and so is an `ostringstream`, a stream that writes into a
         * `string`. The same function works for both.
         */
        ostringstream text;
        text << t;
        cout << " 2| as a string: '" << text.str() << "', " << text.str().size() << " characters\n";
    }

    /* --- `total_seconds` --- The length of an album. The result is the whole point, so: `[[nodiscard]]`. */
    [[nodiscard]] int total_seconds(const vector<track>& album) {
        int sum{0};
        for (const track& t : album) {
            sum += t.seconds();
        }
        return sum;
    }

    /* --- `ignore_a_result` --- Remove the `//` below and build - read the warning. */
    void ignore_a_result() {
        print_function_header();

        const vector<track> album{{"So What", 562}, {"Freddie Freeloader", 589}, {"Blue in Green", 337}};
        cout << " 1| total=" << total_seconds(album) << " seconds\n";
        // total_seconds(album);            // warning: ignoring return value
    }

    /* --- `count_words` ---
     * A function that only reads a text. A `string_view` parameter accepts a `string`, a literal, or a part of a text
     * - and never copies the characters. Inside, it works like a `const string`: `size`, `[]`, `find`, ...
     * - !![#string-view]
     */
    int count_words(const string_view text) {
        int words{0};
        bool in_word{false};
        for (const char c : text) {
            if (c == ' ') {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                ++words;
            }
        }
        return words;
    }

    /* --- `view_a_text` --- Three kinds of text, one parameter type. */
    void view_a_text() {
        print_function_header();

        const string title{"Freddie Freeloader"};
        const track t{"Blue in Green", 337};
        cout << " 1| literal: " << count_words("So What") << " words\n";
        cout << " 2| string: " << count_words(title) << " words\n";
        cout << " 3| getter: " << count_words(t.title()) << " words\n";
    }

}

/* --- Teaser ---
 * `count_words` promises not to copy the text. What is inside a `string_view`, then - and what happens if the text it
 * looks at is gone?
 */

/* --- `main` --- */
int main() {
    print_a_track();
    ignore_a_result();
    view_a_text();

    return EXIT_SUCCESS;
}
