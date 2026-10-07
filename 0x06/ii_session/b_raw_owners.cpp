// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A raw pointer holds an address. It does not say who deletes the object - or whether anybody does.
 * - A leak: the last address of a block is lost. Nothing crashes, nothing warns - but `heap_watch` counts it.
 * - Stack unwinding destroys the pointer, not what it points to: an exception between `new` and `delete` leaks.
 * - Two pointers to one block: after the first `delete`, the second one dangles - reading or deleting through it is
 *   undefined behavior.
 * - The generated copy of a class that owns a block copies the address: two owners, one block, two `delete`s.
 * - The Rule of Three (see previous snippets): a copy constructor and a copy assignment that copy the block itself.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::endl, std::string, std::copy, std::runtime_error, std::size_t;


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

    /* --- `leak_a_block` ---
     * `p` gets a second address, and the first one is gone - nobody can delete that block any more. It stays allocated
     * until the program ends, and the operating system takes back all of its memory. In a program that runs for days,
     * or does it in a loop, the memory grows and grows.
     * - !![#memory-leak]
     */
    void leak_a_block() {
        print_function_header();

        const heap_watch heap{};
        int* p{new int{1}};
        p = new int{2};                     // the address of the first `int` is lost
        delete p;
        cout << " 1| allocations=" << heap.allocations() << ", releases=" << heap.releases() << ", live="
             << heap.live() << '\n';
    }

    /* --- `read_config` --- Something that can go wrong, and does. */
    void read_config() {
        throw runtime_error{"config not found"};
    }

    /* --- `leak_on_the_way_out` ---
     * `delete t` is in the code - but the exception jumps over it. Stack unwinding destroys every local object on the
     * way to the `catch` (see previous snippets), and `t` is a local: a pointer, 8 bytes. Destroying a pointer does
     * nothing to the object it points to. No ` b|` line, one live block.
     */
    void leak_on_the_way_out() {
        print_function_header();

        const heap_watch heap{};
        try {
            tracer* t{new tracer{"t"}};
            read_config();
            delete t;                       // never reached
        } catch (const runtime_error& e) {
            cout << " 1| caught: " << e.what() << '\n';
        }
        cout << " 2| live=" << heap.live() << '\n';
    }

    /* --- `try_two_pointers` ---
     * `p` and `q` hold the same address. After `delete p`, the block belongs to the allocator again - and `q` does not
     * know. Reading through it reads memory that the allocator uses for its own purposes; deleting through it gives the
     * same block back twice.
     * Remove the `//` in front of one line at a time, run it as Debug and as Release - and put the `//` back.
     * - !![#dangling-pointer]
     */
    void try_two_pointers() {
        print_function_header();

        int* p{new int{42}};
        const int* q{p};
        cout << " 1| p=" << p << ", q=" << q << ", *q=" << *q << endl;
        delete p;
        // cout << " 2| *q=" << *q << endl; // undefined behavior: the block was given back
        // delete q;                        // undefined behavior: a double `delete`
        cout << " 3| still alive" << endl;

        /* -- .What we saw. --
         * gcc 13 and clang 18 on x86-64 Linux (glibc 2.39), gcc 11 on ARM64 Linux (glibc 2.35), Debug and Release:
         * - `*q` printed a number like 1453883023, not 42. glibc keeps the blocks that are given back in lists, and
         *   writes the link to the next one into the first bytes of the block - over our `int`.
         * - The second `delete` ended the program: `free(): double free detected in tcache 2`, exit status 134 (128 +
         *   6, the signal SIGABRT). glibc checks this one case - most others it does not.
         * In a smaller program that never printed the address, gcc and clang as Release removed the `new` and both
         * `delete`s altogether, and the program ended normally. Undefined behavior, as in the previous unit.
         */
    }

    /* --- `naive_buffer` ---
     * Owns a block of `int`s: `new[]` in the constructor, `delete[]` in the destructor - RAII, as in unit 0x03. And no
     * copy constructor, so the compiler generates one: member by member. For `data_`, that copies the address.
     */
    class naive_buffer {
    public:
        explicit naive_buffer(const size_t size) : size_{size}, data_{new int[size]{}} {}

        ~naive_buffer() { delete[] data_; }

        size_t size() const { return size_; }
        const int* data() const { return data_; }

    private:
        size_t size_;
        int* data_;
    };

    /* --- `try_to_copy_the_owner` ---
     * `b` is a copy of `a` - and `b.data()` is `a.data()`: two objects, one block, and each believes it is the owner.
     * At the `}`, `b` is destroyed and deletes the block, then `a` deletes it again. With glibc, in all four builds:
     * the two addresses, then `free(): double free detected in tcache 2`, exit status 134.
     * - !![#deep-copy]
     */
    void try_to_copy_the_owner() {
        print_function_header();

        const naive_buffer a{3};
        // const naive_buffer b{a};         // undefined behavior at the `}`: the block is deleted twice
        // cout << " 1| a.data()=" << a.data() << ", b.data()=" << b.data() << endl;
        cout << " 2| a.data()=" << a.data() << ", a.size()=" << a.size() << '\n';
    }

    /* --- `int_buffer` ---
     * The same owner, with the Rule of Three: the copy constructor allocates a block of its own and copies the
     * elements; the copy assignment does the same and then gives its old block back. The order matters: first the new
     * block, then `delete[]` - if `new` throws, the object is unchanged, and `a = a` works.
     * - !![#rule-of-zero]
     */
    class int_buffer {
    public:
        explicit int_buffer(const size_t size) : size_{size}, data_{new int[size]{}} {}

        int_buffer(const int_buffer& other) : size_{other.size_}, data_{new int[other.size_]} {
            copy(other.data_, other.data_ + other.size_, data_);
        }

        int_buffer& operator=(const int_buffer& other) {
            int* block{new int[other.size_]};
            copy(other.data_, other.data_ + other.size_, block);
            delete[] data_;
            data_ = block;
            size_ = other.size_;
            return *this;
        }

        ~int_buffer() { delete[] data_; }

        size_t size() const { return size_; }
        int* data() { return data_; }
        const int* data() const { return data_; }

    private:
        size_t size_;
        int* data_;
    };

    /* --- `copy_deeply` ---
     * Now every copy is an owner of its own block: other addresses, one allocation per copy - and one `delete[]` per
     * block at the end. The price of a deep copy is visible: an allocation and the elements.
     */
    void copy_deeply() {
        print_function_header();

        int_buffer a{3};
        a.data()[0] = 23;
        const heap_watch heap{};
        const int_buffer b{a};
        cout << " 1| a.data()=" << a.data() << ", b.data()=" << b.data() << ", b.data()[0]=" << b.data()[0]
             << ", allocations=" << heap.allocations() << '\n';

        int_buffer c{5};
        c = a;
        cout << " 2| c.size()=" << c.size() << ", allocations=" << heap.allocations() << ", releases="
             << heap.releases() << '\n';

        /* -- .Q&A -- !![Why does `int_buffer` need a copy constructor, but `tracer` does not?](#a-604) */
    }

}

/* --- Who owns this? ---
 * Every `new` needs an owner: exactly one piece of code that deletes it, exactly once, on every path - also the one an
 * exception takes. A raw pointer cannot say whether it is that owner. `int_buffer` can, because it is a class with a
 * destructor - and it took a constructor, an assignment and a destructor to get it right, for one block. The library
 * has that class already, for any type: see next snippets.
 */

/* --- `main` --- */
int main() {
    leak_a_block();
    leak_on_the_way_out();
    try_two_pointers();
    try_to_copy_the_owner();
    copy_deeply();

    return EXIT_SUCCESS;
}
