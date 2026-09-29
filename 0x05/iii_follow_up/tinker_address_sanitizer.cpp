// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - AddressSanitizer (ASan): the compiler adds a check to every memory access, and a runtime library keeps track of
 *   which bytes belong to a living object. A wild access stops the program - with a report that names the object.
 * - It finds what the session could only comment out: out of bounds, a dangling pointer to a local, the address of a
 *   local returned, a pointer into a `vector` that has moved.
 * - The price: roughly twice the time and more memory - a tool for testing, not for the release.
 * - Platforms: gcc and clang on Linux, Apple clang on macOS. Not with MinGW; MSVC has its own switch,
 *   `/fsanitize=address`.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl, std::vector;

// gcc and MSVC define `__SANITIZE_ADDRESS__` when ASan is on, clang reports it via `__has_feature`.
#if defined(__SANITIZE_ADDRESS__)
#define CBL_ASAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define CBL_ASAN 1
#endif
#endif
#ifndef CBL_ASAN
#define CBL_ASAN 0
#endif


/* ---- Content ---- */

/* -- .Switched on in the CMakeLists. --
 * The unit's `CMakeLists.txt` builds this snippet with `-fsanitize=address -fno-omit-frame-pointer` if the toolchain
 * can link it (`check_linker_flag`) - and without it otherwise. Then this program only says so, and does nothing wild.
 */

namespace {

    /* --- Which experiment? ---
     * ASan stops the program at the first error, so there is one experiment per run. Set `experiment` to 1, 2, 3 or 4,
     * build, run, and read the report.
     */
    constexpr int experiment{1};

    /* --- `write_at` and `read_from` ---
     * Helpers that pass addresses around - they cannot know what is there, and often neither can the compiler. That is
     * where ASan earns its keep: the warnings mostly stop at function borders, ASan does not.
     */
    void write_at(int* values, const int i, const int value) {
        values[i] = value;
    }

    int read_from(const int* p) {
        return *p;
    }

    const int* address_of(const int& x) {
        return &x;
    }

    /* --- `write_out_of_bounds` ---
     * Report: `stack-buffer-overflow ... WRITE of size 4`, the line, and a picture of the frame - `'a'` and
     * `Memory access at offset ... overflows this variable`.
     */
    void write_out_of_bounds() {
        int a[3]{10, 20, 30};
        write_at(a, 3, 99);
        cout << " 1| a[2]=" << a[2] << endl;
    }

    /* --- `read_after_the_block` ---
     * Report: `stack-use-after-scope ... READ of size 4`. The block of `inner` has ended - ASan marks its bytes as dead
     * at the `}`.
     */
    void read_after_the_block() {
        const int* p{nullptr};
        {
            const int inner{7};
            p = address_of(inner);
        }
        cout << " 1| *p=" << read_from(p) << endl;
    }

    /* --- `read_after_the_move` ---
     * Report: `heap-use-after-free ... READ of size 4` - and where the block was freed: inside `push_back`, when the
     * `vector` moved its elements.
     */
    void read_after_the_move() {
        vector<int> v{1, 2, 3};
        const int* first{&v[0]};
        v.push_back(4);
        cout << " 1| *first=" << read_from(first) << endl;
    }

    /* --- `address_of_a_local` and `read_a_returned_local` ---
     * Report: `stack-use-after-return ... READ of size 4`. The classic from the session, through a helper, so that no
     * compiler warns. With some versions, ASan checks this only with the runtime option
     * `ASAN_OPTIONS=detect_stack_use_after_return=1` (in CLion: Run | Edit Configurations... | Environment variables).
     */
    const int* address_of_a_local() {
        const int n{42};
        return address_of(n);
    }

    void read_a_returned_local() {
        const int* p{address_of_a_local()};
        cout << " 1| *p=" << read_from(p) << endl;
    }

    /* --- `run_an_experiment` --- */
    void run_an_experiment() {
        print_function_header();

        if (!CBL_ASAN) {
            cout << " 1| AddressSanitizer is not active in this build - nothing wild happens here.\n";
            cout << " 2| See the CMakeLists of this unit, and the list of platforms at the top.\n";
            return;
        }
        cout << " 1| AddressSanitizer is active, experiment " << experiment << endl;
        switch (experiment) {
            case 1: write_out_of_bounds(); break;
            case 2: read_after_the_block(); break;
            case 3: read_after_the_move(); break;
            case 4: read_a_returned_local(); break;
            default: cout << " 2| no such experiment\n";
        }
    }

}

/* --- Reading the report ---
 * The report goes to the error output, the program ends with exit status 1. The first lines say what happened and
 * where: the kind of error, `READ` or `WRITE`, the size, and a stack trace with file and line (`-g` - a Debug build -
 * makes them readable). Then where the memory came from: the frame and its variables, or the allocation and the free
 * of a heap block.
 * `endl` in the experiments on purpose: ASan ends the program, and unflushed output would be lost.
 */

/* --- `main` --- */
int main() {
    run_an_experiment();

    return EXIT_SUCCESS;
}
