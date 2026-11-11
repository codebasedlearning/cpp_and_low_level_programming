// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A move steals the pointer instead of copying the elements: the move constructor takes over the block and leaves
 *   the source empty.
 * - Recap (see previous snippets): `&&` binds to what is about to die, and `std::move` only makes an object look like
 *   that. What a move does, the class decides - and the moved-from object is still alive: empty, and destroyed as
 *   usual.
 * - Copy-and-swap: one assignment operator, by value, for copy and move - shorter, safe when an object is assigned to
 *   itself, and all or nothing if the copy fails.
 * - Two compromises for speed: a copy assignment that reuses the block when it fits, a move assignment that only swaps.
 * - The Rule of Five - and back to the Rule of Zero, with a `unique_ptr<int[]>` member. The order of choice: no special
 *   member at all, then copy-and-swap, and tuned assignments only where a measurement asks for them.
 */

#include <iostream>
#include <memory>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::unique_ptr, std::make_unique, std::copy, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `int_buffer` ---
     * The owner of the previous snippets, now with all five special members.
     * - The copy assignment allocates and copies first, and gives its old block back last - that order makes it all
     *   or nothing if `new` throws, and correct for `a = a`. The test in front only saves the allocation for `a = a`;
     *   why it is not the fix, see below.
     * - The move constructor takes an `int_buffer&&` - an object that is about to die, or that its owner has given up.
     *   It copies the address and the size, and sets the source to empty, so that its destructor deletes nothing.
     * - The move assignment gives its own block back first, then does the same. `a = std::move(a)` must not destroy
     *   the block, hence the test.
     * - Both are `noexcept`: they allocate nothing, so nothing can fail.
     *   A growing vector moves its elements only if their move constructor is noexcept - otherwise it copies them.
     * - !![#move-semantics]
     *
     * noexcept is a promise in the function’s declaration that no exception will leave it: the compiler doesn’t check
     * the promise, but if an exception tries to leave anyway, the program ends with std::terminate.
     * And because the promise can be asked for at compile time, with noexcept(expr) or is_nothrow_move_constructible,
     * other code can rely on it, as a growing vector does when it decides to move instead of copy.
     */
    class int_buffer {
    public:
        explicit int_buffer(const size_t size) : size_{size}, data_{new int[size]{}} {}

        int_buffer(const int_buffer& other) : size_{other.size_}, data_{new int[other.size_]} {
            copy(other.data_, other.data_ + other.size_, data_);
        }

        int_buffer& operator=(const int_buffer& other) {
            if (this != &other) {           // only saves work: the order below already makes `a = a` correct
                int* block{new int[other.size_]};
                copy(other.data_, other.data_ + other.size_, block);
                delete[] data_;
                size_ = other.size_; data_ = block;
            }
            return *this;
        }

        int_buffer(int_buffer&& other) noexcept : size_{other.size_}, data_{other.data_} {
            other.size_ = 0; other.data_ = nullptr;
        }

        int_buffer& operator=(int_buffer&& other) noexcept {
            if (this != &other) {
                delete[] data_;
                size_ = other.size_; data_ = other.data_;
                other.size_ = 0; other.data_ = nullptr;
            }
            return *this;
        }

        ~int_buffer() { delete[] data_; }   // `delete[]` of a `nullptr` does nothing

        size_t size() const { return size_; }
        const int* data() const { return data_; }

    private:
        size_t size_;
        int* data_;
    };

    /* -- .Wrong version . --
     * A version that looks just as careful - and is wrong:
     *
     *     int_buffer& operator=(const int_buffer& other) {
     *         if (this != &other) {
     *             delete[] data_;                         // the old block is gone ...
     *             data_ = new int[other.size_];           // ... and if this throws, `data_` dangles
     *             copy(other.data_, other.data_ + other.size_, data_);
     *             size_ = other.size_;
     *         }
     *         return *this;
     *     }
     *
     * Here the test is needed: without it, `a = a` would free the very block it is about to copy from. But a `new`
     * that throws `bad_alloc` leaves the object with the address of a freed block, and its destructor frees it a
     * second time. The rule of thumb: if an assignment needs the test to be correct, it is not exception-safe either.
     * The order of `int_buffer` - allocate and copy first, free last - makes both right, and the test only saves
     * work. In the move assignment the test does more: without it, `a = std::move(a)` would free its own block, then
     * take the freed address - and set it to `nullptr`, an empty `a`.
     * `a = a` rarely stands in the code like this - the compiler warns. It comes through two names for one object:
     * `v[i] = v[j]` with `i == j`, or `*p = *q`.
     * - !![#exception-safety]
     */

    /* --- `swap_buffer` ---
     * The same owner as `int_buffer` - with one assignment operator instead of two. Compare the code: the copy
     * assignment of `int_buffer` allocates, copies, gives its old block back, and has to get the order right; its
     * move assignment needs the test for `a = std::move(a)`. Here, `operator=` takes its argument by value, and the
     * work is done before it even starts: an lvalue argument is copied into `other` - the copy constructor, one
     * allocation - an rvalue is moved into it - the move constructor, none. Then `*this` and `other` swap their
     * members, and `other` takes the old elements with it to its destructor, at the `}`.
     * - If the copy fails (`bad_alloc`), it fails before the operator starts: `*this` is untouched - all or nothing,
     *   the strong exception guarantee.
     * - `a = a` needs no test: a copy of `a` is swapped in, and the old block is freed. It costs an allocation, but
     *   assigning an object to itself is rare.
     * - `swap` exchanges two pointers and two sizes - four words, nothing allocated, nothing that can throw, hence
     *   `noexcept`.
     * The price: a move assignment is now a move constructor and a swap - a few instructions more than by hand - and
     * a copy never reuses the block the object already has, even when it would fit. `int_buffer`, as written, does not
     * either - but a hand-written assignment could: see `tuned_buffer`.
     * - !![#copy-and-swap]
     */
    class swap_buffer {
    public:
        explicit swap_buffer(const size_t size) : size_{size}, data_{new int[size]{}} {}

        swap_buffer(const swap_buffer& other) : size_{other.size_}, data_{new int[other.size_]} {
            copy(other.data_, other.data_ + other.size_, data_);
        }

        swap_buffer(swap_buffer&& other) noexcept : size_{other.size_}, data_{other.data_} {
            other.size_ = 0; other.data_ = nullptr;
        }

        swap_buffer& operator=(swap_buffer other) noexcept {    // by value: the copy or the move happens here
            swap(other);
            return *this;
        }

        ~swap_buffer() { delete[] data_; }

        void swap(swap_buffer& other) noexcept {
            std::swap(size_, other.size_);
            std::swap(data_, other.data_);
        }

        size_t size() const { return size_; }
        const int* data() const { return data_; }

    private:
        size_t size_;
        int* data_;
    };

    /* --- `tuned_buffer` ---
     * Two compromises between `int_buffer` and `swap_buffer` - more code than copy-and-swap, less work at run time.
     * - The copy assignment reuses the block when it fits: the same size, so just copy the elements - no allocation,
     *   no `delete[]`. Only for another size, copy-and-swap: a copy, `tmp`, then a swap. Copying `int`s cannot throw,
     *   so it is still all or nothing. With elements whose copy can throw, the reuse gives only the basic guarantee:
     *   valid, but maybe half copied - `vector` makes the same trade. And `a = a` needs its test again: `copy` must
     *   not copy a range onto itself.
     * - The move assignment only swaps: no extra object, and no `delete[]` here. The source gets the old block and
     *   frees it in its own destructor - "valid but unspecified" allows that, and ` 3|` shows it.
     * - !![#copy-and-swap]
     */
    class tuned_buffer {
    public:
        explicit tuned_buffer(const size_t size) : size_{size}, data_{new int[size]{}} {}

        tuned_buffer(const tuned_buffer& other) : size_{other.size_}, data_{new int[other.size_]} {
            copy(other.data_, other.data_ + other.size_, data_);
        }

        tuned_buffer(tuned_buffer&& other) noexcept : size_{other.size_}, data_{other.data_} {
            other.size_ = 0; other.data_ = nullptr;
        }

        tuned_buffer& operator=(const tuned_buffer& other) {
            if (size_ == other.size_) {             // the block fits: reuse it
                if (this != &other) {
                    copy(other.data_, other.data_ + other.size_, data_);
                }
            } else {                                // it does not: copy-and-swap
                tuned_buffer tmp{other};
                swap(tmp);
            }
            return *this;
        }

        tuned_buffer& operator=(tuned_buffer&& other) noexcept {    // just swap: `other` takes the old block
            swap(other);
            return *this;
        }

        ~tuned_buffer() { delete[] data_; }

        void swap(tuned_buffer& other) noexcept {
            std::swap(size_, other.size_);
            std::swap(data_, other.data_);
        }

        size_t size() const { return size_; }
        const int* data() const { return data_; }

    private:
        size_t size_;
        int* data_;
    };

    /* --- `move_only_buffer` ---
     * The Rule of Zero again: a member that owns itself, and no special member at all. The generated move moves the
     * `unique_ptr` - the block changes its owner - and copies `size_`.
     * Because a `unique_ptr` cannot be copied, you get a compiler error. The rule is:
     * - If a generated version (here copy-ctor, copy-assignment), would be ill-formed, it’s defined as deleted.
     * - !![#rule-of-zero]
     */
    class move_only_buffer {
    public:
        explicit move_only_buffer(const size_t size) : size_{size}, data_{make_unique<int[]>(size)} {}

        size_t size() const { return size_; }
        const int* data() const { return data_.get(); }

    private:
        size_t size_;
        unique_ptr<int[]> data_;
    };

    /* --- `test_buffers` --- Check all. */
    void test_buffers() {
        print_function_header();

        int_buffer a{1000};
        swap_buffer b{1000};
        tuned_buffer c{1000};
        move_only_buffer d{1000};

        cout << " 1| feel free to modify..." << '\n';
    }

}

/* --- Rule of Five ---
 * A class that manages a resource by hand needs five special members: destructor, copy constructor, copy assignment,
 * move constructor, move assignment - or `= delete` for those it should not have. Declare one, think about all five.
 * The order of choice:
 * - The Rule of Zero first: members that own themselves - `unique_ptr`, `vector`, `string` - do the work, and you
 *   write none. A `vector` member even reuses its block on a copy assignment.
 * - If a class must own by hand: copy-and-swap, as `swap_buffer` - short, and correct without thinking about the
 *   order.
 * - Tuned assignments, as `tuned_buffer`, only where a measurement says that the assignment matters.
 * The mantra of this unit: every `new` needs an owner - and the best owner is one you did not have to write.
 */

/* --- `main` --- */
int main() {
    test_buffers();

    return EXIT_SUCCESS;
}
