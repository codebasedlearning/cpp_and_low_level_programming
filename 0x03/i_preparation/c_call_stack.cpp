// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: stepping through a program in the debugger.
 * - The call stack: which function called which - one frame per call.
 * - Constructors and destructors are functions, too - with frames of their own.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `room` --- */
    class room {
    public:
        room(const int number) {
            number_ = number;                                       // again, later we see other ways for initializing
            cout << " a|   room " << number_ << " built\n";         // breakpoint here
        }

        ~room() {
            cout << " b|   room " << number_ << " torn down\n";     // and here
        }

    private:
        int number_;
    };

    /* --- `house` ---
     * A house has two rooms - two members of type `room`. Each gets its number right at the declaration, as in
     * `int n{1};` - so the constructor of `room` is called with 1 and 2.
     */
    class house {
    public:
        house() {
            cout << " c|   house built\n";
        }

        ~house() {
            cout << " d|   house torn down\n";
        }

    private:
        room kitchen_{1};
        room bath_{2};
    };

    /* --- `build_a_house` ---
     * One line of code - and five functions run: two `room` constructors, the `house` constructor, and after `1|` the
     * destructors.
     */
    void build_a_house() {
        print_function_header();

        const house h{};
        cout << " 1| the house is standing\n";
    }

}

/* --- Stepping ---
 * Set a breakpoint on the line `const house h{};` and start with 'Debug'. The debugger stops before the line runs.
 * - Step Over runs the whole line, including every constructor it calls, and stops at the next line.
 * - Step Into goes into the first function the line calls - here a constructor. Keep stepping into, and you see the
 *   members being built before the body of `house` runs.
 * - Step Out runs the rest of the current function and stops in the caller.
 * If you step into `print_function_header` or into `cout` by accident: Step Out.
 */

/* --- The call stack ---
 * Set a breakpoint in the constructor of `room` and run. The frames list shows the call stack, the newest call on
 * top: `room::room`, `house::house`, `build_a_house`, `main` - possibly with an `(anonymous namespace)::` in front.
 * Click on a frame to see where that function currently is, and its local variables.
 * - Which `room` is built first? Swap the two lines `room kitchen_{1};` and `room bath_{2};` and try again.
 */

/* --- Who calls the destructor? ---
 * Set a breakpoint in the destructor of `room`, run, and click on the frame of `build_a_house` in the frames list.
 * Which line of your code is marked as the caller?
 */

/* --- `main` --- */
int main() {
    build_a_house();

    return EXIT_SUCCESS;
}
