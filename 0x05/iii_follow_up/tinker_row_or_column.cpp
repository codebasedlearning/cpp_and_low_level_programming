// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A 2D array is stored row after row (row-major): `grid[i][j]` is element `i * columns + j` of one block.
 * - Summing it row by row walks through memory in order; column by column, it jumps a whole row with every step.
 * - Same work, same result - and a different time. The reason is the cache: memory is read in lines of 64 bytes.
 * - Measure as Release.
 */

#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::vector, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `show_row_major` ---
     * Three rows of four: `&grid[1][0]` comes right after `&grid[0][3]`. There are no rows in memory - only the
     * compiler knows where one ends.
     */
    void show_row_major() {
        print_function_header();

        const int grid[3][4]{{0, 1, 2, 3}, {10, 11, 12, 13}, {20, 21, 22, 23}};
        cout << " 1| &grid[0][3]=" << &grid[0][3] << ", &grid[1][0]=" << &grid[1][0] << '\n';
        cout << " 2| distance from [0][0] to [2][1]: " << &grid[2][1] - &grid[0][0] << " elements\n";
    }

    constexpr size_t size{4096};            // 4096 x 4096 `int`s - 64 MB, too big for the stack

    /* --- `sum_by_rows` and `sum_by_columns` ---
     * One block on the heap, indexed by hand, as the compiler does it for `grid[row][column]`.
     */
    long long sum_by_rows(const vector<int>& cells) {
        long long sum{0};
        for (size_t row{0}; row < size; ++row) {
            for (size_t column{0}; column < size; ++column) {
                sum += cells[row * size + column];
            }
        }
        return sum;
    }

    long long sum_by_columns(const vector<int>& cells) {
        long long sum{0};
        for (size_t column{0}; column < size; ++column) {
            for (size_t row{0}; row < size; ++row) {
                sum += cells[row * size + column];
            }
        }
        return sum;
    }

    /* --- `measure_row_or_column` ---
     * Row by row, the next `int` is 4 bytes further, usually in the same cache line - 16 `int`s per line, and the
     * processor fetches the next lines before they are needed. Column by column, the next `int` is 16 KB further: every
     * access needs another line, and by the time the column is done, the first lines have been pushed out of the cache.
     * Try other sizes, e.g. 1024 or 4000: the factor changes - because of the cache sizes of your machine.
     */
    void measure_row_or_column() {
        print_function_header();

        const vector<int> cells(size * size, 1);

        stopwatch watch{};
        const long long by_rows{sum_by_rows(cells)};
        const double rows_ms{watch.elapsed_ms()};

        watch.reset();
        const long long by_columns{sum_by_columns(cells)};
        const double columns_ms{watch.elapsed_ms()};

        cout << " 1| by rows:    " << by_rows << " in " << rows_ms << " ms\n";
        cout << " 2| by columns: " << by_columns << " in " << columns_ms << " ms\n";
    }

}

/* --- `main` --- */
int main() {
    show_row_major();
    measure_row_or_column();

    return EXIT_SUCCESS;
}
