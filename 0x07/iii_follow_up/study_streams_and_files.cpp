// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A stream is a source or a sink of characters: the console, a string, a file. The same `<<`, `>>` and `getline`
 *   work for all of them.
 * - `istringstream` takes a text apart, `ostringstream` builds one; manipulators format the numbers.
 * - `ofstream` and `ifstream` open a file in their constructor and close it in their destructor - RAII.
 * - Check whether a file could be opened - every time.
 * - A text file holds characters, a binary file the bytes of the values: the same numbers, two sizes.
 */

#include <iostream>
#include <sstream>
#include <fstream>                          // for ifstream, ofstream
#include <iomanip>                          // for setw, setfill, setprecision
#include <filesystem>                       // for path, temp_directory_path, file_size
#include <string>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::istringstream, std::ostringstream, std::ifstream, std::ofstream;
using std::string, std::vector, std::getline;
namespace fs = std::filesystem;


/* ---- Content ---- */

namespace {

    /* --- `read_from_a_string` ---
     * `>>` skips spaces and line breaks, then reads one word, or one number. `getline` reads everything up to the end
     * of the line - spaces included - and drops the `'\n'`. With a third argument, it stops at another character.
     * - !![#streams]
     */
    void read_from_a_string() {
        print_function_header();

        istringstream words{"Davis 1926 Coltrane 1926"};
        string name;
        int year{0};
        while (words >> name >> year) {
            cout << " 1| " << name << ", born " << year << '\n';
        }

        istringstream lines{"So What;562\nBlue in Green;337\n"};
        string line;
        while (getline(lines, line)) {
            istringstream fields{line};
            string title;
            int seconds{0};
            getline(fields, title, ';');
            fields >> seconds;
            cout << " 2| '" << title << "', " << seconds << " s\n";
        }
    }

    /* --- `write_to_a_string` ---
     * An `ostringstream` collects the output, `str()` returns it. `setw` sets the width of the next item only,
     * `setfill` the character that fills it; `fixed` and `setprecision` stay until they are changed; `hex` too, until
     * `dec`.
     */
    void write_to_a_string() {
        print_function_header();

        ostringstream text;
        text << '[' << std::setw(6) << 42 << "] [" << std::setfill('0') << std::setw(6) << 42 << "] ["
             << std::fixed << std::setprecision(2) << 3.14159 << "] [" << std::hex << 255 << std::dec << ']';
        cout << " 1| " << text.str() << '\n';
    }

    /* --- `path_in_temp` ---
     * A path in the directory for temporary files - `/tmp` on Linux and macOS, somewhere under your user folder on
     * Windows. A relative path would depend on the directory the program runs in, and in CLion that is the build
     * folder.
     */
    fs::path path_in_temp(const string& name) {
        return fs::temp_directory_path() / name;
    }

    /* --- `write_and_read_a_text_file` ---
     * `ofstream out{path}` creates the file - or empties an existing one - and `out` closes it at its `}`. Then
     * `ifstream` opens it for reading. A stream tests as `false` if it could not be opened, or if a read failed.
     */
    void write_and_read_a_text_file() {
        print_function_header();

        const fs::path path{path_in_temp("cbl_tracks.txt")};
        {
            ofstream out{path};
            if (!out) {
                cout << " 1| cannot write " << path << '\n';
                return;
            }
            out << "So What;562\n" << "Freddie Freeloader;589\n" << "Blue in Green;337\n";
        }

        ifstream in{path};
        if (!in) {
            cout << " 2| cannot read " << path << '\n';
            return;
        }
        string line;
        int count{0};
        while (getline(in, line)) {
            ++count;
            cout << " 3| line " << count << ": " << line << '\n';
        }
        fs::remove(path);
    }

    /* --- `compare_text_and_binary` ---
     * A thousand `int`s from 1'000'000 on - once as text, seven digits and a `'\n'` each; once as the bytes of the
     * `int`s, 4 each. `write` takes the address of the first byte as a `const char*` and a count of bytes: the
     * `reinterpret_cast` to look at bytes (see previous snippets). `std::ios::binary` switches off any translation of
     * line ends - on Windows, a text file turns every `'\n'` into two bytes.
     * Reading back: the text needs `>>`, which parses the digits; the binary file needs only `read`, which copies the
     * bytes into place. But only a program on a machine with the same `int` and the same byte order can read them.
     */
    void compare_text_and_binary() {
        print_function_header();

        vector<int> values(1000);
        int next{1'000'000};
        for (int& v : values) {
            v = next;
            ++next;
        }

        const fs::path text_path{path_in_temp("cbl_values.txt")};
        const fs::path binary_path{path_in_temp("cbl_values.bin")};
        {
            ofstream text{text_path};
            for (const int v : values) {
                text << v << '\n';
            }
            ofstream binary{binary_path, std::ios::binary};
            binary.write(reinterpret_cast<const char*>(values.data()),
                         static_cast<std::streamsize>(values.size() * sizeof(int)));
        }
        cout << " 1| text: " << fs::file_size(text_path) << " bytes, binary: " << fs::file_size(binary_path)
             << " bytes\n";

        ifstream binary{binary_path, std::ios::binary};
        int first{0};
        binary.read(reinterpret_cast<char*>(&first), sizeof(first));
        cout << " 2| first value from the binary file: " << first << '\n';

        binary.close();
        fs::remove(text_path);
        fs::remove(binary_path);

        /* -- .Q&A -- !![When is the text file the smaller one - and why is text often better anyway?](#a-713) */
    }

}

/* --- `main` --- */
int main() {
    read_from_a_string();
    write_to_a_string();
    write_and_read_a_text_file();
    compare_text_and_binary();

    return EXIT_SUCCESS;
}
