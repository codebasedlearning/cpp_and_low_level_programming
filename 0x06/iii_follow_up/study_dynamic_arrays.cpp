// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `new int[n]` leaves the elements uninitialized, `new int[n]{}` sets them to 0, `new int[n]{1, 2}` sets the first
 *   ones and the rest to 0.
 * - A 2D array whose size is known only at run time, as one block: indexed by hand, `row * columns + column` - what the
 *   compiler does for `int grid[3][4]` (see previous snippets).
 * - Or as one block per row and an array of row pointers: `rows + 1` allocations, and the rows anywhere in memory.
 * - `unique_ptr<int[]>` owns such a block; a `vector<int>` does the same - and knows its size.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::vector, std::unique_ptr, std::make_unique, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `initialize_the_elements` ---
     * Without braces, the `int`s of `new int[4]` have whatever bytes were in the block - reading them is undefined
     * behavior, as for an uninitialized local. With braces, they are set: the listed ones as listed, all others to 0.
     * For a class type, every element is built by its default constructor either way.
     */
    void initialize_the_elements() {
        print_function_header();

        int* unset{new int[4]};             // four `int`s, no values yet
        int* zeros{new int[4]{}};
        int* some{new int[4]{1, 2}};
        unset[0] = 7;                       // writing is fine - reading before writing is not
        cout << " 1| unset[0]=" << unset[0] << ", zeros[3]=" << zeros[3] << ", some: " << some[0] << ' ' << some[1]
             << ' ' << some[2] << ' ' << some[3] << '\n';
        delete[] unset;
        delete[] zeros;
        delete[] some;
    }

    /* --- `use_one_block` ---
     * `rows * columns` `int`s in one block, owned by a `unique_ptr<int[]>`. Element `[row][column]` is at
     * `row * columns + column` - the rows lie one after the other, and the end of one row is the start of the next.
     * One allocation.
     */
    void use_one_block(const size_t rows, const size_t columns) {
        print_function_header();

        const heap_watch heap{};
        const unique_ptr<int[]> cells{make_unique<int[]>(rows * columns)};
        int value{0};
        for (size_t row{0}; row < rows; ++row) {
            for (size_t column{0}; column < columns; ++column) {
                cells[row * columns + column] = value;
                ++value;
            }
        }
        for (size_t row{0}; row < rows; ++row) {
            cout << " 1|   row " << row << " at " << &cells[row * columns] << ", first element "
                 << cells[row * columns] << '\n';
        }
        cout << " 2| allocations=" << heap.allocations() << '\n';
    }

    /* --- `use_one_block_per_row` ---
     * An array of `rows` pointers, and for each row a block of its own. `grid[row][column]` looks like a 2D array,
     * but it is two steps: load the address of the row, then index it (see previous snippets: `int**`). The rows are
     * wherever the allocator puts them - often close together, but not guaranteed, and not in one piece.
     * Every block needs its own `delete[]`: first the rows, then the array of pointers - the other way round, the row
     * addresses would be gone.
     */
    void use_one_block_per_row(const size_t rows, const size_t columns) {
        print_function_header();

        const heap_watch heap{};
        int** grid{new int*[rows]};
        for (size_t row{0}; row < rows; ++row) {
            grid[row] = new int[columns]{};
        }
        grid[1][2] = 12;
        for (size_t row{0}; row < rows; ++row) {
            cout << " 1|   row " << row << " at " << grid[row] << '\n';
        }
        cout << " 2| grid[1][2]=" << grid[1][2] << ", allocations=" << heap.allocations() << '\n';

        for (size_t row{0}; row < rows; ++row) {
            delete[] grid[row];
        }
        delete[] grid;
    }

    /* --- `grid` ---
     * The one block with a size and a way to index it - owned by a `vector<int>`, the Rule of Zero. It can be copied
     * and moved, and nothing leaks.
     */
    class grid {
    public:
        grid(const size_t rows, const size_t columns) : columns_{columns}, cells_(rows * columns) {}

        int& at(const size_t row, const size_t column) { return cells_[row * columns_ + column]; }
        size_t size() const { return cells_.size(); }

    private:
        size_t columns_;
        vector<int> cells_;
    };

    /* --- `use_a_grid` --- One allocation, as with `unique_ptr<int[]>` - and the size is kept. */
    void use_a_grid(const size_t rows, const size_t columns) {
        print_function_header();

        const heap_watch heap{};
        grid g{rows, columns};
        g.at(1, 2) = 12;
        cout << " 1| g.at(1, 2)=" << g.at(1, 2) << ", size()=" << g.size() << ", allocations=" << heap.allocations()
             << '\n';

        /* -- .Q&A -- !![3 x 4 `int`s: how much heap memory does each of the three versions take?](#a-608) */
    }

}

/* --- `main` --- Three rows, four columns - imagine they were typed in. */
int main() {
    initialize_the_elements();
    use_one_block(3, 4);
    use_one_block_per_row(3, 4);
    use_a_grid(3, 4);

    return EXIT_SUCCESS;
}
