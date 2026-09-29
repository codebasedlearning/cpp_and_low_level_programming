// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A destructor is `noexcept` without you writing it: an exception that leaves it ends the program.
 * - `noexcept(false)` allows it - until the destructor runs during stack unwinding. Two exceptions at once: the end.
 * - This program crashes on purpose. Look at the exit status.
 */

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl, std::runtime_error;


/* ---- Content ---- */

namespace {

    /* --- `grumpy` ---
     * Its destructor throws. `noexcept(false)` switches off the implicit `noexcept` - without it, the program would end
     * right in the first function (and the compiler warns: `-Wterminate` in gcc, `-Wexceptions` in clang).
     * - !![#noexcept]
     */
    class grumpy {
    public:
        ~grumpy() noexcept(false) {
            cout << " a|   ~grumpy: throwing" << endl;
            throw runtime_error{"thrown in a destructor"};
        }
    };

    /* --- `throw_from_a_destructor` ---
     * Without a second exception, this even works: the destructor throws at the `}`, and the `catch` gets it.
     */
    void throw_from_a_destructor() {
        print_function_header();

        try {
            const grumpy g{};
            cout << " 1| end of the block\n";
        } catch (const runtime_error& e) {
            cout << " 2| caught: " << e.what() << '\n';
        }
    }

    /* --- `throw_during_unwinding` ---
     * Now the block is left by an exception - and on the way, the destructor of `g` throws a second one. C++ cannot
     * handle two exceptions at once, so it calls `std::terminate`: the program is aborted, the `catch` never runs.
     * - !![#stack-unwinding]
     */
    void throw_during_unwinding() {
        print_function_header();

        try {
            const grumpy g{};
            // `endl` on purpose: flush now - when the program is aborted, output still in the buffer is lost.
            cout << " 1| throwing the first exception" << endl;
            throw runtime_error{"the first one"};
        } catch (const runtime_error& e) {
            cout << " 2| never printed: " << e.what() << '\n';
        }
    }

}

/* --- Exit status ---
 * Run it in the terminal and then `echo $?`: 134 on Linux and macOS - 128 plus 6, the number of the signal `SIGABRT`
 * that `std::terminate` raises through `abort()`. With MSVC, `abort()` ends the program with exit code 3 (a Debug build
 * shows a dialog first).
 */

/* --- `main` --- */
int main() {
    throw_from_a_destructor();
    throw_during_unwinding();

    cout << "never printed\n";
    return EXIT_SUCCESS;
}
