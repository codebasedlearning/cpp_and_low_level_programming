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
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::shared_ptr, std::make_shared;


/* ---- Content ---- */

namespace {

    /* --- `tracer` --- Reports its birth and its death. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed\n";
        }

        ~tracer() {
            cout << " b|   " << name_ << ": destroyed\n";
        }

    private:
        string name_;
    };

    /* --- `recall_new_and_delete` ---
     * Every `new` needs an owner. `new` is two steps - `operator new` asks the allocator for bytes, the constructor
     * turns them into an object - and `delete` is the same backwards. A heap block costs more than its bytes: with
     * glibc, an `int` takes a block of 32 bytes. `heap_watch` counts the blocks - as Debug.
     */
    void recall_new_and_delete() {
        print_function_header();

        const heap_watch heap{};
        tracer* t{new tracer{"t"}};
        delete t;
        cout << " 1| allocations=" << heap.allocations() << ", releases=" << heap.releases() << '\n';
    }

    /* --- `recall_the_owner` ---
     * `unique_ptr` is the owner as a type: as small as a raw pointer, the same machine code - plus the `delete` on the
     * exception path. It cannot be copied; `std::move` hands the ownership on and leaves a null behind.
     */
    void recall_the_owner() {
        print_function_header();

        unique_ptr<tracer> u{make_unique<tracer>("u")};
        const unique_ptr<tracer> v{std::move(u)};
        cout << " 1| sizeof(unique_ptr<tracer>)=" << sizeof(unique_ptr<tracer>) << ", u is empty: " << (u == nullptr)
             << '\n';
    }

    /* --- `recall_the_move` ---
     * A move steals the pointer instead of copying the elements: a `vector` of a million `int`s is moved by copying
     * three words - no allocation. The source is empty afterwards, but alive.
     */
    void recall_the_move() {
        print_function_header();

        vector<int> source(1'000'000, 1);
        const heap_watch heap{};
        const vector<int> target{std::move(source)};
        cout << " 1| target.size()=" << target.size() << ", source.size()=" << source.size() << ", allocations="
             << heap.allocations() << '\n';
    }

    /* --- `recall_shared_ownership` ---
     * `shared_ptr` is two pointers - to the object and to the control block, which counts the owners (atomically).
     * The last owner deletes. Prefer `unique_ptr`.
     */
    void recall_shared_ownership() {
        print_function_header();

        const shared_ptr<tracer> s{make_shared<tracer>("s")};
        {
            const shared_ptr<tracer> t{s};
            cout << " 1| sizeof(shared_ptr<tracer>)=" << sizeof(shared_ptr<tracer>) << ", use_count=" << s.use_count()
                 << '\n';
        }
        cout << " 2| use_count=" << s.use_count() << '\n';
    }

}

/* --- Rule of Zero, again ---
 * Let the members own - `string`, `vector`, `unique_ptr` - and write none of the five special members. A class that
 * owns a raw block needs all five, and its moves are `noexcept`.
 */

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`).
 * - The debugger: breakpoints, the memory view, stepping and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 * - `heap_watch` and `heap_log` - count allocations, as Debug.
 * - `nm`, `nm -C` and `c++filt` - which functions are in which object file, and under which name.
 * - `g++ -S` and Compiler Explorer (godbolt.org) - the machine code, x86-64 and ARM64.
 * - AddressSanitizer (`-fsanitize=address`), where the toolchain has it.
 */

/* --- Teaser ---
 * Since unit 0x01, `cout << x` has called a function named `operator<<`, and since unit 0x03, `a = b` a function
 * named `operator=`. Which function does `a + b` call for a class of your own - and what is left of that call as
 * Release?
 */

/* --- `main` --- */
int main() {
    recall_new_and_delete();
    recall_the_owner();
    recall_the_move();
    recall_shared_ownership();

    return EXIT_SUCCESS;
}
