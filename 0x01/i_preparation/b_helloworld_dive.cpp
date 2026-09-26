// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// A single comment line.

/*
 * A comment block.
 *
 * This is the famous C++ program that prints `Hello world!` to the console.
 */

/*
 * `#include` is a preprocessor step that pastes the whole header's text into this file before
 * the compiler sees it. The declarations in that text are what make `cout` and `endl` known.
 * `<iostream>` is the standard header for input/output streams.
 */
#include <iostream>

/*
 * Execution starts here.
 * Like Java's `main`, it's the entry point, but it returns an `int` exit status
 * (0 indicates success) to the OS instead of `void`.
 * `return 0;` is optional here: `main` is the one function where reaching the closing `}`
 * means `return 0;`.
 *
 * Try it: run the program, then ask the shell for its exit status:
 *     ./b_helloworld_dive.out; echo $?
 * Change `return 0;` to `return 42;`, rebuild with `make` and check again.
 */
int main() {
    /*
     * Using `cout` for console output, we connect the output items with `<<` operators
     * and use `endl` for line breaks (with flush) or `\n` for line break only - here we use `\n`.
     * Since these objects live in the `std` namespace we use the full name here.
     */
    std::cout << "Hello world - Deep dive!\n";
    return 0;
}
