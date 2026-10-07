// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - UndefinedBehaviorSanitizer (UBSan): the compiler adds a check to every operation that can be undefined - signed
 *   arithmetic, division, and more. When a check fails, a runtime library reports it, with file and line.
 * - Every UB claim of this unit - "signed overflow is undefined, the program goes on with a wrong value" - turns from
 *   "trust me" into a message.
 * - Unlike most sanitizers, UBSan reports and goes on: one run shows every problem.
 * - The price: a sanitized build is a different program - slower, and the optimizer cannot exploit the UB any more.
 * - Platforms: gcc and clang on Linux, Apple clang on macOS. Not with MinGW, not with MSVC.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl;

// `CBL_UBSAN` is set to 1 by the CMakeLists if this snippet is built with UBSan - a preprocessor switch, as `#include`.
#ifndef CBL_UBSAN
#define CBL_UBSAN 0
#endif


/* ---- Content ---- */

/* -- .Switched on in the CMakeLists. --
 * The unit's `CMakeLists.txt` builds this snippet with `-fsanitize=undefined` if the toolchain can link it
 * (`check_linker_flag`) - and without it otherwise. Then this program only says so: one of the experiments below would
 * run forever as Release.
 */

namespace {

    /* --- `factorial` --- As in the session, with a loop. */
    int factorial(const int n) {
        int result{1};
        for (int i{2}; i <= n; ++i) {
            result *= i;
        }
        return result;
    }

    /* --- `divide` --- Integer division, no check. */
    int divide(const int a, const int b) {
        return a / b;
    }

    /* --- `show_an_overflow` ---
     * Report: `runtime error: signed integer overflow: 479001600 * 13 cannot be represented in type 'int'` - with the
     * file, the line and the column of the `*=`. And then the program goes on, with the same wrong value as without
     * UBSan.
     */
    void show_an_overflow() {
        print_function_header();

        cout << " 1| 13!=" << factorial(13) << endl;
    }

    /* --- `loop_until_overflow` ---
     * The loop of the task 'AI': `i` counts up until it is no longer greater than 0 - which only an overflow can do.
     * Report: `signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'`. It takes a few seconds.
     * Without UBSan, the Release build of gcc runs this loop forever: the optimizer assumes that `++i` never overflows,
     * so `i > 0` is always true. With UBSan, every `++i` is checked, the optimizer cannot assume anything, and the loop
     * ends - the sanitized program is a different program. A test with UBSan finds the UB; it does not show what the
     * Release build will do with it.
     */
    void loop_until_overflow() {
        print_function_header();

        int n{0};
        for (int i{1}; i > 0; ++i) {
            ++n;
        }
        cout << " 1| n=" << n << endl;
    }

    /* --- `try_to_divide_by_zero` ---
     * Report: `runtime error: division by zero`. What happens next depends on the build and the processor. As Debug,
     * the division is executed: on ARM64 it returns 0 and the program goes on, on x86-64 it traps and the program ends
     * with exit status 136 (the signal `SIGFPE`). As Release, gcc already knows the 0 while compiling and puts a trap
     * instruction right behind the report - the program ends there (on Linux: exit status 133, `SIGTRAP`, on ARM64;
     * 132, `SIGILL`, on x86-64).
     * That is why this experiment comes last.
     */
    void try_to_divide_by_zero() {
        print_function_header();

        const int zero{factorial(0) - 1};
        cout << " 1| 7/0=" << divide(7, zero) << endl;
    }

}

/* --- What UBSan does not find ---
 * An uninitialized variable: reading it is UB, but UBSan does not track which bytes were written - that is the job of
 * MemorySanitizer (clang on Linux only) or Valgrind, and of the warning `-Wuninitialized`. Wild pointers and dangling
 * references are the job of AddressSanitizer, see future snippets.
 */

/* --- Reading the report ---
 * The reports go to the error output, one line each: file, line and column, then what went wrong, with the values.
 * `endl` in the experiments on purpose: `cout` and the error output are two streams, and a flush keeps them in order.
 * To stop at the first report instead of going on, build with `-fno-sanitize-recover=undefined`.
 */

/* --- `main` --- */
int main() {
    if (!CBL_UBSAN) {
        cout << " 1| UndefinedBehaviorSanitizer is not active in this build - no experiments.\n";
        cout << " 2| See the CMakeLists of this unit, and the list of platforms at the top.\n";
        return EXIT_SUCCESS;
    }
    show_an_overflow();
    loop_until_overflow();
    try_to_divide_by_zero();

    return EXIT_SUCCESS;
}
