// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A raw pointer holds an address. It does not say who deletes the object - or whether anybody does.
 * - A leak: the last address of a block is lost. Nothing crashes, nothing warns.
 * - Stack unwinding destroys the pointer, not what it points to: an exception between `new` and `delete` leaks.
 * - Two pointers to one block: after the first `delete`, the second one dangles - reading or deleting through it is
 *   undefined behavior.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/tracer.hpp>

using std::cout, std::endl, std::string, std::copy, std::runtime_error, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `leak_a_block` ---
     * `t` gets a second address, and the first one is gone - nobody can delete that block any more. It stays allocated
     * until the program ends, and the operating system takes back all of its memory. In a program that runs for days,
     * or does it in a loop, the memory grows and grows.
     * - !![#memory-leak]
     */
    void leak_a_block() {
        print_function_header();

        cout << " 1| forgot to delete" << '\n';
        tracer* t{new tracer{"t"}};
        t = new tracer{"t´"};               // the address of the first `tracer` is lost, and so is the tracer itself
        delete t;
    }

    /* --- .`read_config` --- Something that can go wrong, and does. */
    void read_config() {
        throw runtime_error{"config not found"};
    }

    /* --- `leak_on_the_way_out` ---
     * `delete t` is in the code - but the exception jumps over it. Stack unwinding destroys every local object on the
     * way to the `catch` (see previous snippets), and `t` is a local: a pointer, 8 bytes. Destroying a pointer does
     * nothing to the object it points to. No `destroyed` line, one live block.
     */
    void leak_on_the_way_out() {
        print_function_header();

        cout << " 1| skip a delete" << '\n';
        try {
            tracer* t{new tracer{"t"}};
            read_config();
            delete t;                       // never reached
        } catch (const runtime_error& e) {
            cout << " 2| caught: " << e.what() << '\n';
        }
    }

    /* --- `try_two_pointers` ---
     * `t` and `u` hold the same address. After `delete t`, the block belongs to the allocator again - and `u` does not
     * know. Reading through it reads memory that the allocator uses for its own purposes; deleting through it gives the
     * same block back twice.
     * Remove the `//` in front of one line at a time, run it as Debug and as Release - and put the `//` back.
     * - !![#dangling-pointer]
     */
    void try_two_pointers() {
        print_function_header();

        tracer* t{new tracer{"t"}};
        const tracer* u{t};
        cout << " 1| t=" << t << ", u=" << u << '\n';

        delete t;
        // delete u;                        // undefined behavior: a double `delete`
        cout << " 3| still alive" << endl;

        /* -- .What we saw. --
         * - The second `delete` ended the program: `free(): double free detected in tcache 2`, exit status 134 (128 +
         *   6, the signal SIGABRT). glibc checks this one case - most others it does not.
         * In a smaller program that never printed the address, gcc and clang as Release removed the `new` and both
         * `delete`s altogether, and the program ended normally. Undefined behavior, as in the previous unit.
         */
    }

}

/* --- `main` --- */
int main() {
    leak_a_block();
    leak_on_the_way_out();
    try_two_pointers();

    return EXIT_SUCCESS;
}
