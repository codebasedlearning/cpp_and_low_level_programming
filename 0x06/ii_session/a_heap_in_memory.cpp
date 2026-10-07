// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Three areas of memory, three kinds of lifetime: static storage (the whole run), the stack (until the `}`), the
 *   heap (until the `delete`).
 * - `new` is two steps: `operator new` gets raw bytes from the allocator, the constructor turns them into an object.
 *   `delete` is the same, backwards.
 * - A heap block costs more than its bytes: the allocator rounds it up and keeps its size in front of it.
 * - And it costs time: a million blocks are much slower than one block of a million elements.
 * - `new[]` for a type with a destructor puts the count in front of the elements - that is why an array needs
 *   `delete[]`.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `new tracer{"t"}` and `delete t`: which member functions of `tracer` run, and when?
 * - `make_tracer` returns the address of a `tracer` it created with `new`. Why does the object survive the `}` of
 *   `make_tracer` - and who must delete it?
 * - `make_unique<tracer>("u")`: who deletes the `tracer`, and when?
 * - How many allocations did `heap_watch` count for a short and for a long `string`? And for 1000 `push_back`s into a
 *   `vector`, with and without `reserve`?
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::vector, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `tracer` --- Reports its birth and its death, with its address. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed at " << this << '\n';
        }

        ~tracer() {
            cout << " b|   " << name_ << ": destroyed at " << this << '\n';
        }

    private:
        string name_;
    };

    /* --- `global_value` --- Outside of any function: static storage, see previous snippets. */
    int global_value{1};

    /* --- `show_three_areas` ---
     * Three variables, three areas. `global_value` has its place from the start of the program to its end. `local` has
     * its place in the stack frame, until the `}`. The `int` from `new` has its place on the heap, until the `delete` -
     * `p` itself is a local on the stack, holding a heap address.
     * The addresses show three different areas: the heap usually close to the static data, the stack far away.
     * - !![#stack-and-heap]
     */
    void show_three_areas() {
        print_function_header();

        const int local{2};
        int* p{new int{3}};
        cout << " 1| static: " << &global_value << ", heap: " << p << ", stack: " << &local << " (p itself: " << &p
             << ")\n";
        delete p;
    }

    /* --- `show_new_in_two_steps` ---
     * The log shows what `new` hides. First `operator new` is asked for `sizeof(tracer)` bytes - raw memory, no object
     * yet. Then the constructor runs at exactly that address. `delete` goes backwards: first the destructor, then the
     * bytes go back to the allocator.
     * A `tracer` with a long name needs a second block: the constructor copies the name into its `string` member, and
     * the member allocates its characters - one `new`, two blocks. The destructor body runs first, then the member
     * gives its characters back, then the `tracer`'s own bytes go.
     * - !![#new-delete]
     */
    void show_new_in_two_steps() {
        print_function_header();

        const string long_name{"u, with a name too long for the string itself"};
        const heap_log log{};
        cout << " 1| sizeof(tracer)=" << sizeof(tracer) << '\n';
        tracer* t{new tracer{"t"}};
        delete t;

        cout << " 2| a long name\n";
        tracer* u{new tracer{long_name}};
        delete u;
    }

    /* --- `show_the_neighbors` ---
     * Three `int`s, 4 bytes each - and the addresses are further apart. The allocator must be able to give every block
     * back later, so it keeps the size of each block in front of it, and it hands out addresses that suit every type,
     * multiples of 16. With glibc, the allocator of Linux, a block for 1 to 24 bytes takes 32 bytes: fresh blocks are
     * 0x20 apart. If an address jumps, the allocator has handed out a block that was given back before - it keeps them
     * for exactly that. Other allocators have other rules - on macOS and Windows, look for yourself.
     */
    void show_the_neighbors() {
        print_function_header();

        int* a{new int{1}};
        int* b{new int{2}};
        int* c{new int{3}};
        cout << " 1| a=" << a << ", b=" << b << ", c=" << c << '\n';
        delete a;
        delete b;
        delete c;

        /* -- .Memory view. --
         * Set a breakpoint on ` 1|`, and look at `b` in the memory view - and at the 8 bytes in front of it. With
         * glibc: `21 00 00 00 00 00 00 00` - the size of the block, 0x20, plus 1 for "the block in front of me is in
         * use" - then the 4 bytes of the `int`, `02 00 00 00`, and the rest of the block up to the next size field.
         */
    }

    constexpr size_t count{1'000'000};

    /* --- `sum_of_blocks` --- A million `int`s, each in a block of its own. */
    long long sum_of_blocks() {
        vector<int*> blocks(count);
        for (size_t i{0}; i < count; ++i) {
            blocks[i] = new int{1};
        }
        long long sum{0};
        for (const int* p : blocks) {
            sum += *p;
        }
        for (const int* p : blocks) {
            delete p;
        }
        return sum;
    }

    /* --- `sum_of_one_block` --- A million `int`s in one block. */
    long long sum_of_one_block() {
        const vector<int> values(count, 1);
        long long sum{0};
        for (const int x : values) {
            sum += x;
        }
        return sum;
    }

    /* --- `measure_a_million_blocks` ---
     * The same million `int`s, the same sum. A million blocks means a million calls of `operator new` and of
     * `operator delete`, and 32 MB of memory for 4 MB of `int`s, plus the pointers - and a million places to fetch
     * them from. One block is one allocation, and the `int`s lie one after the other.
     * Measure as Release - and count as Debug.
     */
    void measure_a_million_blocks() {
        print_function_header();

        heap_watch heap{};
        stopwatch watch{};
        const long long blocks{sum_of_blocks()};
        const double blocks_ms{watch.elapsed_ms()};
        const size_t blocks_allocations{heap.allocations()};

        heap.reset();
        watch.reset();
        const long long one_block{sum_of_one_block()};
        const double one_block_ms{watch.elapsed_ms()};

        cout << " 1| a million blocks: sum=" << blocks << ", " << blocks_allocations << " allocations, " << blocks_ms
             << " ms\n";
        cout << " 2| one block:        sum=" << one_block << ", " << heap.allocations() << " allocation, "
             << one_block_ms << " ms\n";

        /* -- .Q&A -- !![Why is a heap block slower than a local variable?](#a-603) */
    }

    /* --- `show_the_array_cookie` ---
     * `new tracer[3]` asks for more than three `tracer`s: 8 bytes more, and the address you get is 8 bytes after the
     * start of the block. In these 8 bytes, `new[]` stores the count, 3. `delete[]` reads it to know how many
     * destructors to call - there is no other place to find it: the array has decayed to an address.
     * `new int[3]` asks for exactly 12 bytes. An `int` has no destructor, so there is nothing to count, and no cookie.
     * The size of the whole block, the allocator knows anyway.
     * - !![#array-cookie]
     */
    void show_the_array_cookie() {
        print_function_header();

        const heap_log log{};
        tracer* ts{new tracer[3]{tracer{"x"}, tracer{"y"}, tracer{"z"}}};
        cout << " 1| ts=" << ts << ", 3 * sizeof(tracer)=" << 3 * sizeof(tracer) << '\n';
        delete[] ts;
        // delete ts;                       // undefined behavior: one destructor, and the wrong address to give back

        int* numbers{new int[3]{1, 2, 3}};
        cout << " 2| numbers=" << numbers << '\n';
        delete[] numbers;

        /* -- .`delete` instead of `delete[]`. --
         * Swap the two lines with `delete[] ts`. gcc (with `-Wall`) and clang warn. With glibc, one destructor runs,
         * then `operator delete` gets the address 8 bytes after the start of the block - not a block it knows: the
         * program aborts with `munmap_chunk(): invalid pointer`, exit status 134. For `new int[3]` and `delete`, it
         * seems to work - and is undefined behavior all the same.
         * The size of the cookie belongs to the platform's ABI: 8 bytes on Linux and Windows. On Apple silicon, the
         * ABI puts the size of an element in front, too - 16 bytes. Check what your machine asks for.
         */
    }

}

/* --- `main` --- */
int main() {
    show_three_areas();
    show_new_in_two_steps();
    show_the_neighbors();
    measure_a_million_blocks();
    show_the_array_cookie();

    return EXIT_SUCCESS;
}
