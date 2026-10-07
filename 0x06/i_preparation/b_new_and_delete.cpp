// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `new` creates an object on the heap and returns its address; `delete` destroys it and gives the memory back.
 * - An object on the heap does not die at a `}` - it lives until somebody deletes it.
 * - `new int[n]` creates an array whose size is known only at run time; it is given back with `delete[]`.
 * - `std::make_unique` creates an object on the heap together with its owner: a `unique_ptr` that deletes it at its
 *   `}`.
 */

#include <iostream>
#include <string>
#include <memory>                           // for unique_ptr, make_unique
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

namespace {

    /* --- `allocate_an_int` ---
     * `new int{23}` creates an `int` on the heap - a part of memory that belongs to no function - and returns its
     * address. You reach the `int` only through that address: `*p`. The address is far away from the local `n`: the
     * stack and the heap are different areas of memory.
     * `delete p` destroys the `int` and gives its memory back. `p` still holds the address - of nothing, now.
     * - !![#new-delete]
     */
    void allocate_an_int() {
        print_function_header();

        const int n{15};
        int* p{new int{23}};
        cout << " 1| &n=" << &n << ", p=" << p << ", *p=" << *p << '\n';
        *p = 42;
        cout << " 2| *p=" << *p << '\n';
        delete p;
        // cout << *p;                      // undefined behavior: the `int` is gone
    }

    /* --- `tracer` --- Reports its birth and its death, as in unit 0x03. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed at " << this << '\n';
        }

        ~tracer() {
            cout << " b|   " << name_ << ": destroyed at " << this << '\n';
        }

        const string& name() const { return name_; }

    private:
        string name_;
    };

    /* --- `allocate_an_object` ---
     * `new` calls the constructor, `delete` calls the destructor - the same two as for a local object, only the moment
     * is yours. `->` reaches a member through the pointer, as in unit 0x05.
     */
    void allocate_an_object() {
        print_function_header();

        tracer* t{new tracer{"t"}};
        cout << " 1| t=" << t << ", t->name()=" << t->name() << '\n';
        delete t;
        cout << " 2| end of function\n";
    }

    /* --- `make_tracer` ---
     * Creates a `tracer` on the heap and returns its address. The object survives the `}` of `make_tracer` - it was
     * never in its stack frame. The caller gets the address, and with it the duty to delete the object.
     */
    tracer* make_tracer(const string& name) {
        return new tracer{name};
    }

    /* --- `outlive_the_function` ---
     * The function has returned, its frame is gone, the object is still there. Compare with the address of a local
     * variable, returned from a function (see previous snippets): that one dangled.
     */
    void outlive_the_function() {
        print_function_header();

        tracer* m{make_tracer("m")};
        cout << " 1| back in the caller: " << m->name() << '\n';
        delete m;
    }

    /* --- `allocate_an_array` ---
     * The size of a C array must be known at compile time (see previous snippets). `new int[count]` takes it at run
     * time - here it is a parameter, it could as well be an input - and returns the address of the first element: only
     * an address, as after the decay. An array from `new[]` is given back with `delete[]`, not with `delete`. Why that
     * matters: see next snippets.
     */
    void allocate_an_array(const size_t count) {
        print_function_header();

        int* fibs{new int[count]};
        fibs[0] = 1;
        fibs[1] = 1;
        for (size_t i{2}; i < count; ++i) {
            fibs[i] = fibs[i - 2] + fibs[i - 1];
        }
        cout << " 1| count=" << count << ", fibs[" << count - 1 << "]=" << fibs[count - 1] << '\n';
        delete[] fibs;

        /* -- .And `std::vector`? --
         * It does the same - a block of `int`s on the heap, with a size chosen at run time - and gives it back by
         * itself. In real code, it is the better choice. Here, `new[]` shows what a `vector` does inside (see previous
         * snippets: 24 bytes on the stack, the elements on the heap).
         */
    }

    /* --- `own_with_a_unique_ptr` ---
     * `make_unique<tracer>("u")` creates a `tracer` on the heap - the arguments go to its constructor - and returns a
     * `unique_ptr<tracer>`: an object that holds the address and owns the `tracer`. It is used like a pointer, `*u` and
     * `u->name()`. There is no `delete`: when `u` dies at the `}`, its destructor deletes the `tracer`.
     * - !![#unique-ptr]
     */
    void own_with_a_unique_ptr() {
        print_function_header();

        const unique_ptr<tracer> u{make_unique<tracer>("u")};
        cout << " 1| u->name()=" << u->name() << ", address " << u.get() << '\n';
        cout << " 2| end of function\n";

        /* -- .Q&A -- !![Who deletes the `tracer` - and in which line of the output can you see it?](#a-601) */
    }

}

/* --- `main` --- */
int main() {
    allocate_an_int();
    allocate_an_object();
    outlive_the_function();
    allocate_an_array(12);
    own_with_a_unique_ptr();

    return EXIT_SUCCESS;
}
