// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x09 - the things to remember.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector, std::function;


/* ---- Content ---- */

namespace {

    /* --- `twice` --- A plain function. */
    int twice(const int x) {
        return 2 * x;
    }

    /* --- `recall_functions_have_addresses` ---
     * A function is code, at an address among the program's code; a function pointer holds that address, 8 bytes. A
     * call through it is an indirect call - the address comes from a register. C passes behavior this way: `qsort`
     * calls its comparison through a pointer, once per comparison.
     */
    void recall_functions_have_addresses() {
        print_function_header();

        int (*const f)(int){twice};
        cout << " 1| f=" << reinterpret_cast<const void*>(f) << ", f(21)=" << f(21) << ", sizeof(f)=" << sizeof(f)
             << '\n';
    }

    /* --- `recall_lambdas_are_objects` ---
     * A lambda is an object of a class the compiler writes. Its captures are its data members - `sizeof` is what it
     * captured: a copy for `[limit]`, an address for `[&count]`. The body is a `const` `operator()`. Only a lambda
     * without captures converts to a function pointer.
     */
    void recall_lambdas_are_objects() {
        print_function_header();

        int limit{10};
        int count{0};
        const auto above = [limit](const int n) { return n > limit; };
        const auto counting = [&count](const int n) { count += n; };
        counting(5);
        cout << " 1| sizeof: [limit] " << sizeof(above) << ", [&count] " << sizeof(counting) << ", above(12)="
             << above(12) << ", count=" << count << '\n';
    }

    /* --- `recall_the_price_of_a_call` ---
     * A template takes the lambda's own type, sees its body and inlines it. A function pointer is an indirect call,
     * unless the compiler knows its value. `std::function` holds anything - 32 bytes with libstdc++, a small callable
     * inside, a large one on the heap - and every call goes through a stored pointer.
     */
    void recall_the_price_of_a_call() {
        print_function_header();

        vector<int> numbers{5, 2, 8, 1};
        std::sort(numbers.begin(), numbers.end(), [](const int a, const int b) { return a > b; });
        const function<int(int)> wrapped{twice};
        cout << " 1| first=" << numbers.front() << ", sizeof(function<int(int)>)=" << sizeof(wrapped) << '\n';
    }

    /* --- `make_counter` --- A lambda that owns its state: an init-capture, and `mutable`. */
    auto make_counter() {
        return [count = 0]() mutable { return ++count; };
    }

    /* --- `recall_captured_lifetimes` ---
     * A lambda can outlive the function that made it. A captured copy travels along - safe. A captured reference is an
     * address, and if the variable is gone, the lambda dangles. `[this]` is the address of an object, and if the object
     * moves, the lambda points to the old place.
     */
    void recall_captured_lifetimes() {
        print_function_header();

        auto next = make_counter();
        cout << " 1| next: " << next();
        cout << ' ' << next() << '\n';
    }

}

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`).
 * - The debugger: breakpoints, the memory view, stepping and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 * - `heap_watch` and `heap_log` - count allocations, as Debug.
 * - `nm`, `nm -C` and `c++filt` - which functions are in which object file, and under which name.
 * - `g++ -S` and Compiler Explorer (godbolt.org) - the machine code, x86-64 and ARM64.
 * - C++ Insights (cppinsights.io) - the code the way the compiler reads it, lambdas included.
 * - The class layout: clang's `-Xclang -fdump-record-layouts` and `-fdump-vtable-layouts`, gcc's `-fdump-lang-class`.
 * - AddressSanitizer (`-fsanitize=address`), where the toolchain has it.
 */

/* --- Teaser ---
 * Two threads - two things a program does at the same time, on two cores - run the same lambda:
 *     [&counter] { for (int i{0}; i < 1'000'000; ++i) { ++counter; } }
 * Both lambdas hold the address of the same `counter`. Where does each thread keep its `i`? And what is `counter`
 * when both are done - two million? Think of what `++counter` is in the machine.
 */

/* --- `main` --- */
int main() {
    recall_functions_have_addresses();
    recall_lambdas_are_objects();
    recall_the_price_of_a_call();
    recall_captured_lifetimes();

    return EXIT_SUCCESS;
}
