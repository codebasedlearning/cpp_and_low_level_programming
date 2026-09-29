// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - How every snippet in this course is structured. The program itself is deliberately trivial - it is the Hello World
 *   from before.
 */

/* --- Reading guide ---
 * - Most comments are written for the Codebook, a plugin for CLion (how to install it: see the unit README). It folds
 *   them away in the editor, so the code stays readable, and shows them in a side panel instead: the sections and
 *   topics as an outline, the explanation for the code at the cursor, and every `!![...]` link opened in place.
 * - Without the Codebook, everything still compiles and runs, and the comments stay readable - the markers below just
 *   look a bit unusual, and the links are plain text you look up in `docs/`.
 * - Short notes are plain `//` comments: they belong to the line next to them and are meant to be read in the code,
 *   with or without the Codebook.
 *
 * - `---- X ----` marks a section, `--- X ---` a topic, `-- .X --` a detail (not shown in the Codebook outline).
 * - Every snippet has the same two sections, Preamble and Content. Under Preamble you find the 'Teaching Focus',
 *   all 'includes' and more 'intro' stuff.
 * - `!![#id]` links into the glossary, `!![question](#a-nnn)` to an answer. Try to answer first, then click.
 * - Output lines are numbered ` <n>| ` so you can map them back to the code.
 */

/* --- #include ---
 * `#include` is a preprocessor step that pastes the whole header's text into this file before the compiler sees it.
 * The declarations in that text are what make `cout` known.
 * `<iostream>` is the standard header for input/output streams.
 * - !![#include]
 */
#include <iostream>
#include <cstdlib>                          // for EXIT_SUCCESS, see below
#include <cbl/printing.hpp>                 // for `print_function_header`

/* --- `using` ---
 * When using objects from the `std` namespace like `cout`, you need to either use the full namespace path (`std::cout`)
 * or import them with `using`. This allows omitting the `std::` prefix for the specified items.
 * - !![#using]
 */
using std::cout;


/* ---- Content ---- */

/* --- Namespaces ---
 * All snippet functions are embedded in an unnamed namespace, thus visible only in this file. In a single-file program
 * like this one it makes no difference yet - it is a habit we will need once programs consist of several files.
 * - !![#unnamed-namespace]
 */
namespace {

    /* --- `greet_the_world` --- Print text to console. */
    void greet_the_world() {
        /* -- .`print_function_header` --
         * Prints the name of the current function as a heading; the Codebook plugin uses it to link the output to this
         * function. Treat it as a black box for now - it is built from `source_location`, `constexpr` and
         * `string_view`, which we will take apart later in the course.
         */
        print_function_header();

        /* -- .`cout` with line info. --
         * Using `cout` for console output, we connect the output items with `<<` operators.
         * - Short version used without `std::`, thanks to the `using` declaration above.
         * - House rule: `'\n'` ends a line. `endl` ends the line and also flushes the output buffer, which costs
         *   time - we use it only where a flush is really wanted.
         * - Output is usually of the form ` <n>| <text>`. This numbering helps to find the correct line.
         * - When using the 'cbl Codebook' in CLion a so-called Gutter icon appears next to the function. This jumps
         *   to the corresponding output in the 'Run'-tool-window.
         */
        cout << " 1| Hello world - Codebook style!\n";

        /* -- .Q&A -- !![What is the type of `cout`?](#a-101) */
    }

}

/* --- `main` ---
 * The entry point; its return value is the exit status for the OS.
 * - !![#main]
 *
 * Usually we just call the functions we discuss from `main`. There is no (or very little) logic found here.
 */
int main() {
    greet_the_world();

    return EXIT_SUCCESS;                    // A portable spelling of 0.
}
