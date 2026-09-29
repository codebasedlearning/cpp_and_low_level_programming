// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A pointer is a variable, so it has an address - and another pointer can point to it: `int**`.
 * - To move the caller's pointer, a function needs its address (`const char**`) - or a reference to it
 *   (`const char*&`).
 * - A 2D array is one block, row after row. An array of pointers is not: each row is somewhere else.
 * - `main` can take the command line: `argc` and `argv`, an array of C strings.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `point_to_a_pointer` ---
     * `pp` holds the address of `p`, `p` holds the address of `n`. Each `*` goes one step: `*pp` is `p`, `**pp` is `n`.
     * Each step is a load from memory - the machine follows the addresses one by one.
     * - !![#pointer]
     */
    void point_to_a_pointer() {
        print_function_header();

        int n{15};
        int* p{&n};
        int** pp{&p};
        cout << " 1| &n=" << &n << ", p=" << p << ", &p=" << &p << ", pp=" << pp << '\n';
        cout << " 2| *pp=" << *pp << ", **pp=" << **pp << '\n';
        **pp = 16;                          // writes to `n`
        cout << " 3| n=" << n << '\n';
    }

    /* --- `skip_spaces` and `skip_spaces_ref` ---
     * Both move the caller's pointer past the leading spaces of a text. The first one gets the address of the pointer -
     * the C way; the second one a reference to it - the C++ way. The machine code is the same: an address of an
     * address.
     */
    void skip_spaces(const char** text) {
        while (**text == ' ') {
            ++*text;
        }
    }

    void skip_spaces_ref(const char*& text) {
        while (*text == ' ') {
            ++text;
        }
    }

    /* --- `move_the_callers_pointer` --- Two texts, two ways, one result. */
    void move_the_callers_pointer() {
        print_function_header();

        const char* first{"   Kind of Blue"};
        skip_spaces(&first);
        const char* second{"   Blue Train"};
        skip_spaces_ref(second);
        cout << " 1| first='" << first << "', second='" << second << "'\n";
    }

    /* --- `compare_grid_and_rows` ---
     * `grid` is one block of six `int`s, row after row - row-major. `grid[1][0]` is three `int`s after `grid[0][0]`,
     * and `grid[i][j]` is computed: `i * 3 + j` elements from the start.
     * `rows` is an array of two pointers, each to a row somewhere else. `rows[i][j]` looks the same, but it is two
     * steps: load the address of the row, then index it.
     */
    void compare_grid_and_rows() {
        print_function_header();

        const int grid[2][3]{{1, 2, 3}, {4, 5, 6}};
        cout << " 1| sizeof(grid)=" << sizeof(grid) << ", &grid[0][0]=" << &grid[0][0]
             << ", &grid[1][0]=" << &grid[1][0] << ", distance=" << &grid[1][0] - &grid[0][0] << '\n';

        const int row0[3]{1, 2, 3};
        const int row1[3]{4, 5, 6};
        const int* const rows[2]{row0, row1};
        cout << " 2| sizeof(rows)=" << sizeof(rows) << ", rows[0]=" << rows[0] << ", rows[1]=" << rows[1] << '\n';
        cout << " 3| grid[1][2]=" << grid[1][2] << ", rows[1][2]=" << rows[1][2] << '\n';

        /* -- .Q&A -- !![Why does `grid` not convert to an `int**`?](#a-511) */
    }

    /* --- `show_the_arguments` ---
     * `argv` is an array of `argc` C strings: `argv[0]` is the name of the program (usually), then the arguments, and
     * `argv[argc]` is `nullptr`. `char* argv[]` is a parameter, so it is a `char**` (see previous snippets).
     * In CLion, add arguments under Run | Edit Configurations... | Program arguments - or run the executable in a
     * terminal, `./study_pointers_to_pointers one two`.
     */
    void show_the_arguments(const int argc, char* argv[]) {
        print_function_header();

        for (int i{0}; i < argc; ++i) {
            cout << " 1|   argv[" << i << "]='" << argv[i] << "'\n";
        }
        cout << " 2| argv[argc] is null: " << (argv[argc] == nullptr) << '\n';
    }

}

/* --- `main` --- The other allowed form of `main`: with the command line. */
int main(int argc, char* argv[]) {
    point_to_a_pointer();
    move_the_callers_pointer();
    compare_grid_and_rows();
    show_the_arguments(argc, argv);

    return EXIT_SUCCESS;
}
