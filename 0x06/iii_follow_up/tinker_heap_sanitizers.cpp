// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - AddressSanitizer (ASan) on the heap: it puts poisoned bytes around every block and keeps freed blocks away from
 *   reuse for a while - so a wild access hits poison, and stops the program with a report.
 * - It finds what the session could only comment out: a write past a block, a read after `delete`, a double `delete`,
 *   `delete` for an array from `new[]`.
 * - LeakSanitizer, part of ASan on Linux, lists at the end every block that nobody deleted - and where it was
 *   allocated.
 * - Platforms: gcc and clang on Linux, Apple clang on macOS (without the leak check - use `leaks`, see below). Not with
 *   MinGW; MSVC has `/fsanitize=address`. Valgrind, on Linux, finds most of it without recompiling.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl;

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
 * As in the previous unit: the unit's `CMakeLists.txt` builds this snippet with `-fsanitize=address
 * -fno-omit-frame-pointer` if the toolchain can link it, and without it otherwise. No `heap_watch` here - ASan brings
 * its own `operator new` and `operator delete`.
 */

namespace {

    /* --- Which experiment? ---
     * ASan stops the program at the first error, so there is one experiment per run. Set `experiment` to 1 ... 5,
     * build, run, and read the report.
     */
    constexpr int experiment{1};

    /* --- `write_at`, `read_from` and `release` ---
     * Helpers, so that the compiler cannot see what happens - it warns about some of these in plain sight. ASan does
     * not need to see: it checks at run time.
     */
    void write_at(int* values, const int i, const int value) {
        values[i] = value;
    }

    int read_from(const int* p) {
        return *p;
    }

    void release(const int* p) {
        delete p;
    }

    /* --- `write_past_the_block` ---
     * Report: `heap-buffer-overflow ... WRITE of size 4`, and below: `0 bytes after 12-byte region` - with the stack
     * trace of the `new[]` that allocated it.
     */
    void write_past_the_block() {
        int* values{new int[3]{}};
        write_at(values, 3, 99);
        cout << " 1| values[2]=" << values[2] << endl;
        delete[] values;
    }

    /* --- `read_after_delete` ---
     * Report: `heap-use-after-free ... READ of size 4` - and two more stack traces: where the block was freed, and
     * where it was allocated.
     */
    void read_after_delete() {
        int* p{new int{42}};
        delete p;
        cout << " 1| *p=" << read_from(p) << endl;
    }

    /* --- `delete_twice` ---
     * Report: `attempting double-free`. glibc found this one, too (see previous snippets) - ASan also says where the
     * first `delete` was.
     */
    void delete_twice() {
        int* p{new int{42}};
        release(p);
        release(p);
    }

    /* --- `delete_an_array` ---
     * Report: `alloc-dealloc-mismatch (operator new [] vs operator delete)`. On macOS, ASan checks this only with
     * `ASAN_OPTIONS=alloc_dealloc_mismatch=1` (in CLion: Run | Edit Configurations... | Environment variables).
     */
    void delete_an_array() {
        int* values{new int[3]{}};
        release(values);
    }

    /* --- `leak_a_block` ---
     * The leak of the session, a hundred times: each new address overwrites the one before. Nothing stops here - the
     * program ends normally. Then LeakSanitizer reports `detected memory leaks`, `Direct leak of 400 byte(s) in 100
     * object(s) allocated from:` and the stack trace down to this function, and the exit status is 1.
     * With clang as Debug we got 99 objects: the address of the last block was still lying around in the memory, and
     * LeakSanitizer counts a block as reachable as long as its address can be found anywhere. It may miss a leak - it
     * never reports a block that is still in use. And clang as Release removed the `new`s altogether: no leak to find.
     */
    void leak_a_block() {
        int sum{0};
        for (int i{0}; i < 100; ++i) {
            const int* p{new int{i}};       // a new block, and the address of the one before is lost
            sum += *p;
        }
        cout << " 1| sum=" << sum << endl;
    }

    /* --- `run_an_experiment` ---
     * A leak is harmless without ASan, so experiment 5 runs in every build - for `leaks` and Valgrind, see below. The
     * others only run with ASan.
     */
    void run_an_experiment() {
        print_function_header();

        if (experiment == 5) {
            cout << " 1| experiment 5, AddressSanitizer active: " << CBL_ASAN << endl;
            leak_a_block();
            return;
        }
        if (!CBL_ASAN) {
            cout << " 2| AddressSanitizer is not active in this build - nothing wild happens here.\n";
            cout << " 3| See the CMakeLists of this unit, and the list of platforms at the top.\n";
            return;
        }
        cout << " 4| AddressSanitizer is active, experiment " << experiment << endl;
        switch (experiment) {
            case 1: write_past_the_block(); break;
            case 2: read_after_delete(); break;
            case 3: delete_twice(); break;
            case 4: delete_an_array(); break;
            default: cout << " 5| no such experiment\n";
        }
    }

}

/* --- Leaks on macOS ---
 * Apple's ASan has no leak check. macOS brings a tool of its own: build without ASan, then, in a terminal,
 * `MallocStackLogging=1 leaks --atExit -- ./tinker_heap_sanitizers`, with `experiment` set to 5. It lists the leaked
 * blocks - with the stack trace of the allocation, thanks to `MallocStackLogging`.
 */

/* --- Valgrind ---
 * On Linux, Valgrind runs an unchanged program on a simulated processor and checks every access - slower than ASan,
 * but without recompiling: build without ASan, set `experiment` to 5, and run
 * `valgrind --leak-check=full ./tinker_heap_sanitizers`: at the end, `definitely lost: 400 bytes in 100 blocks`. For
 * the other experiments, it reports `Invalid write`, `Invalid read`, `Invalid free` and
 * `Mismatched free() / delete / delete []` - to try them, remove the `if (!CBL_ASAN)` block for that run.
 */

/* --- `main` --- */
int main() {
    run_an_experiment();

    return EXIT_SUCCESS;
}
