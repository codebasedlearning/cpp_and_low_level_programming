// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `new T{...}` is two steps, and both can be done by hand: `operator new` for the bytes, placement `new` for the
 *   object. `delete` backwards: an explicit destructor call, then `operator delete`.
 * - That is how a `vector` works: its capacity is raw memory, its size the number of objects built in it.
 * - The C way below all of it: `malloc`, `calloc`, `realloc` and `free` - bytes only, no constructors, no destructors,
 *   and `nullptr` instead of an exception.
 * - When there is no memory: `new` throws `std::bad_alloc`, `new (std::nothrow)` returns `nullptr` - and on Linux, a
 *   program rarely sees either.
 */

#include <iostream>
#include <string>
#include <memory>                           // for allocator, construct_at, destroy_at
#include <new>                              // for placement new, bad_alloc, nothrow
#include <limits>
#include <cstdlib>                          // for malloc, calloc, realloc, free
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::allocator, std::construct_at, std::destroy_at, std::bad_alloc, std::nothrow,
      std::numeric_limits;


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

    /* --- `new_in_two_steps` ---
     * `operator new(size)` is the function behind every `new`: it returns raw bytes, a `void*` - no object yet.
     * `new (raw) tracer{"t"}` is placement `new`: it allocates nothing, it only runs the constructor at the address you
     * give it. Backwards: `t->~tracer()` runs the destructor - the one place where you call it yourself - and
     * `operator delete` gives the bytes back. The log shows the same four steps as for `new` and `delete` (see previous
     * snippets), in your hands.
     */
    void new_in_two_steps() {
        print_function_header();

        const heap_log log{};
        void* raw{operator new(sizeof(tracer))};
        cout << " 1| raw=" << raw << '\n';
        tracer* t{new (raw) tracer{"t"}};
        cout << " 2| t=" << t << '\n';
        t->~tracer();
        operator delete(raw);
    }

    /* --- `build_in_a_capacity` ---
     * `allocator<tracer>::allocate(3)` returns memory for three `tracer`s - typed, but with no `tracer` in it: a
     * capacity of 3, a size of 0. `construct_at` builds one in a slot - placement `new` with a nicer name - and
     * `destroy_at` destroys it. This is what a `vector` does: `reserve` allocates, `push_back` constructs at the end,
     * `pop_back` destroys, and only the destructor of the `vector` deallocates.
     * - !![#placement-new]
     */
    void build_in_a_capacity() {
        print_function_header();

        allocator<tracer> alloc;
        const heap_log log{};
        tracer* slots{alloc.allocate(3)};
        cout << " 1| capacity for 3 at " << slots << ", size 0\n";
        construct_at(slots, "first");
        construct_at(slots + 1, "second");
        cout << " 2| size 2\n";
        destroy_at(slots + 1);
        destroy_at(slots);
        alloc.deallocate(slots, 3);
    }

    /* --- `use_the_c_allocator` ---
     * `malloc` returns bytes as a `void*`, or `nullptr` if there are none - no exception. In C, a `void*` converts to
     * any pointer by itself; C++ asks for a cast: `static_cast<int*>(raw)` says "treat these bytes as `int`s" - casts
     * are the topic of a future unit. The `int`s have no values yet, as after `new int[4]`. `calloc` sets all bytes to
     * 0. `realloc` makes a block bigger - in place if there is room, otherwise it allocates a new one, copies the bytes
     * and frees the old one: then the address changes, and every old pointer dangles. And `heap_watch`? It counts
     * nothing here: `malloc` is not `operator new` - it is the layer below it.
     * - !![#malloc]
     */
    void use_the_c_allocator() {
        print_function_header();

        const heap_watch heap{};
        int* numbers{static_cast<int*>(std::malloc(4 * sizeof(int)))};
        if (numbers == nullptr) {
            return;
        }
        for (int i{0}; i < 4; ++i) {
            numbers[i] = i;
        }
        cout << " 1| numbers=" << numbers << ", numbers[3]=" << numbers[3] << '\n';

        const void* before{numbers};
        int* more{static_cast<int*>(std::realloc(numbers, 1000 * sizeof(int)))};
        if (more == nullptr) {              // `numbers` is still valid - and still ours to free
            std::free(numbers);
            return;
        }
        cout << " 2| more=" << more << ", more[3]=" << more[3] << " - the address "
             << (more == before ? "stayed" : "changed") << '\n';
        std::free(more);

        int* zeros{static_cast<int*>(std::calloc(4, sizeof(int)))};
        if (zeros != nullptr) {
            cout << " 3| zeros[3]=" << zeros[3] << '\n';
            std::free(zeros);
        }
        cout << " 4| heap_watch counted " << heap.allocations() << " allocations\n";
    }

    /* --- `catch_a_failed_new` ---
     * A quarter of all addresses a 64-bit pointer can hold - far more than any address space, so the allocator gives
     * up at once. `new` cannot return "nothing": it throws `bad_alloc`, and the constructor never runs.
     * `new (nothrow)` asks for the C behavior instead and returns `nullptr` - then the check is yours, as after
     * `malloc`. The addresses are printed on purpose: a compiler may drop a `new` whose result is never looked at.
     * - !![#new-delete]
     */
    void catch_a_failed_new() {
        print_function_header();

        // Not `const`, on purpose: clang rejects a constant this large already while compiling, "array is too large".
        std::size_t absurd{numeric_limits<std::size_t>::max() / 4};
        try {
            char* p{new char[absurd]};
            const void* got{p};
            cout << " 1| got memory at " << got << " - surprise\n";
            delete[] p;
        } catch (const bad_alloc& e) {
            cout << " 1| new threw: " << e.what() << '\n';
        }

        char* q{new (nothrow) char[absurd]};
        const void* got{q};
        cout << " 2| new (nothrow) returned " << got << (q == nullptr ? " - nullptr" : " - surprise") << '\n';
        delete[] q;                         // `delete[]` of a `nullptr` does nothing
    }

    /* -- .Rarely seen on Linux. --
     * Ask Linux for more memory than is free, and it says yes: it hands out addresses, not memory, and finds the memory
     * page by page when the program first touches it (overcommit). By default it refuses only a request larger than all
     * of its memory and swap together. If it runs out later, there is no exception to throw - the kernel's
     * out-of-memory killer ends a process, maybe yours. So a `bad_alloc` mostly means an absurd size, often a negative
     * number converted to `size_t`. Windows does not overcommit: it commits the memory, against RAM and page file,
     * when `new` asks - and a failure is a `bad_alloc` there.
     */

}

/* --- Never mix ---
 * `free` only what `malloc`, `calloc` or `realloc` returned, `delete` only what `new` returned, `delete[]` only what
 * `new[]` returned. With glibc, `operator new` happens to call `malloc` - but that is not a promise, and `free` never
 * calls a destructor. `malloc` belongs in C code and at the border to C libraries; in C++, a `vector` does the job.
 */

/* --- `main` --- */
int main() {
    new_in_two_steps();
    build_in_a_capacity();
    use_the_c_allocator();
    catch_a_failed_new();

    return EXIT_SUCCESS;
}
