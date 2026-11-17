// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of unit 0x06 - the things to remember.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/tracer.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::shared_ptr, std::make_shared;


/* ---- Content ---- */

namespace {

    /* --- `recall_the_owner` ---
     * ownership
     * - `unique_ptr` is the owner as a type: as small as a raw pointer, the same machine code - plus the `delete` on the
     *    exception path. It cannot be copied; `std::move` hands the ownership on and leaves a null behind.
     * moving resources
     * - A move steals the pointer instead of copying the elements: a `vector` of a million `int`s is moved by copying
     *   three words - no allocation. The source is empty afterwards, but alive.

     */
    void recall_the_owner() {
        print_function_header();

        unique_ptr u{make_unique<tracer>("u")};                 // or unique_ptr<tracer>
        const unique_ptr v{std::move(u)};
        cout << " 1| sizeof(unique_ptr<tracer>)=" << sizeof(unique_ptr<tracer>) << ", u is empty: " << (u == nullptr)
             << '\n';

        vector source(1'000'000, 1);                            // or vector<int>
        cout << " 2| source.size()=" << source.size() << '\n';
        const vector target{std::move(source)};
        cout << " 3| source.size()=" << source.size() << ", target.size()=" << target.size() <<  '\n';
    }

    /* --- `recall_shared_ownership` ---
     * `shared_ptr` is two pointers - to the object and to the control block, which counts the owners (atomically).
     * The last owner deletes. Prefer `unique_ptr`.
     */
    void recall_shared_ownership() {
        print_function_header();

        shared_ptr s{make_shared<tracer>("s")};           // or shared_ptr<tracer>
        {
            const shared_ptr t{s};
            cout << " 1| sizeof(shared_ptr<tracer>)=" << sizeof(shared_ptr<tracer>)
                 << ", use_count=" << s.use_count() << '\n';
        }
        cout << " 2| use_count=" << s.use_count() << '\n';
        s.reset();
        cout << " 3| dtor called, use_count=" << s.use_count() << '\n';
    }

}

/* --- Rule of Zero, again ---
 * Let the members own - `string`, `vector`, `unique_ptr` - and write none of the five special members. A class that
 * owns a raw block needs all five, and its moves are `noexcept`.
 */

/* --- Operator Teaser ---
 * Since unit 0x01, `cout << x` has called a function named `operator<<`, and since unit 0x03, `a = b` a function
 * named `operator=`. Which function does `a + b` call for a class of your own?
 */

/* --- `main` --- */
int main() {
    recall_the_owner();
    recall_shared_ownership();

    return EXIT_SUCCESS;
}
