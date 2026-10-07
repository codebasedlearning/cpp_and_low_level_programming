// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: counting what a program asks from the heap - how many blocks, how many bytes.
 * - Not only your own `new` counts: `string`, `vector` and `list` allocate, too - some often, some never.
 * - `heap_log` prints every allocation and every release while it lives.
 * - Count as Debug.
 */

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>               // for heap_watch, heap_log

using std::cout, std::string, std::vector, std::list;


/* ---- Content ---- */

namespace {

    /* --- `count_an_int` ---
     * `heap_watch` counts from the moment it is created - like `stopwatch`, it is a black box for now. One `new`, one
     * allocation of 4 bytes; one `delete`, one release.
     */
    void count_an_int() {
        print_function_header();

        const heap_watch watch{};
        int* p{new int{23}};
        cout << " 1| p=" << p << ", *p=" << *p << ", allocations=" << watch.allocations() << ", bytes=" << watch.bytes()
             << '\n';
        delete p;
        cout << " 2| releases=" << watch.releases() << '\n';
    }

    /* --- `count_in_strings` ---
     * A short text fits into the `string` object itself (see previous snippets: SSO) - no allocation. A long one gets
     * a block on the heap. Where "long" begins depends on the library: more than 15 characters with gcc's library and
     * with MSVC, more than 22 with clang's library, the default on macOS.
     */
    void count_in_strings() {
        print_function_header();

        const heap_watch watch{};
        const string title{"Kind of Blue"};
        cout << " 1| short: allocations=" << watch.allocations() << '\n';
        const string line{"So What, Freddie Freeloader, Blue in Green"};
        cout << " 2| long:  allocations=" << watch.allocations() << ", bytes=" << watch.bytes() << '\n';
        cout << " 3| " << title.size() << " and " << line.size() << " characters\n";
    }

    /* --- `count_in_containers` ---
     * A `vector` keeps its elements in one block. When the block is full, `push_back` allocates a bigger one, moves
     * the elements over and releases the old block (see previous snippets: `size` and `capacity`). gcc's and clang's
     * libraries double the capacity, so 1000 `int`s take 11 blocks: 1, 2, 4, ..., 1024 `int`s; MSVC grows by half and
     * needs more. After `reserve`, one block is enough.
     * A `list` allocates one node per element - 1000 elements, 1000 blocks.
     */
    void count_in_containers() {
        print_function_header();

        heap_watch watch{};
        vector<int> grown;
        for (int i{0}; i < 1000; ++i) {
            grown.push_back(i);
        }
        cout << " 1| vector, push_back:   allocations=" << watch.allocations() << ", releases=" << watch.releases()
             << '\n';

        watch.reset();
        vector<int> reserved;
        reserved.reserve(1000);
        for (int i{0}; i < 1000; ++i) {
            reserved.push_back(i);
        }
        cout << " 2| vector, reserve:     allocations=" << watch.allocations() << '\n';

        watch.reset();
        list<int> linked;
        for (int i{0}; i < 1000; ++i) {
            linked.push_back(i);
        }
        cout << " 3| list, push_back:     allocations=" << watch.allocations() << ", bytes=" << watch.bytes() << '\n';

        /* -- .Q&A -- !![A `list` node for an `int` takes 24 bytes. What else is in it?](#a-602) */
    }

    /* --- `log_the_heap` ---
     * While a `heap_log` lives, every allocation prints a ` +|` line, every release a ` -|` line. Watch the `vector`
     * grow: a new block, the old one released - and a block that is given back may be handed out again.
     * Your addresses will differ; the pattern will not.
     */
    void log_the_heap() {
        print_function_header();

        const heap_log log{};
        vector<int> v;
        for (int i{0}; i < 5; ++i) {
            cout << " 1| push_back(" << i << ")\n";
            v.push_back(i);
        }
        cout << " 2| end of function\n";
    }

}

/* --- How does it know? ---
 * Every `new` - your own, and those inside `string`, `vector`, `list` and the rest of the library - gets its memory
 * from a function called `operator new`, and gives it back through `operator delete`. A program may replace both with
 * its own versions, and `utils/cbl/heap_watch.hpp` does exactly that: they count, print if a `heap_log` lives, and
 * pass the work on to the C functions `malloc` and `free`.
 * Two rules come with it: include the header in exactly one `.cpp` file of a program, and do not combine it with
 * AddressSanitizer, which replaces the same functions.
 */

/* --- Count as Debug ---
 * The compiler may remove a `new` and its `delete` if it can prove that nobody needs the memory - the standard allows
 * it. clang does so as Release, e.g. for a `new int` whose address and value nobody uses - then there is nothing to
 * count. As Debug, every `new` is a call.
 */

/* --- `main` --- */
int main() {
    count_an_int();
    count_in_strings();
    count_in_containers();
    log_the_heap();

    return EXIT_SUCCESS;
}
