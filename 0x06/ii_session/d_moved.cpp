// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A move steals the pointer instead of copying the elements: the move constructor takes over the block and leaves
 *   the source empty.
 * - `T&&`, an rvalue reference, binds to what is about to die: a temporary, or an object handed over with `std::move`.
 * - `std::move` moves nothing - it only allows the move. The moved-from object is still alive: empty, and destroyed
 *   as usual.
 * - Moving a `vector` copies three words, whatever its size.
 * - A growing `vector` moves its elements only if their move constructor is `noexcept` - otherwise it copies them.
 * - The Rule of Five - and back to the Rule of Zero, with a `unique_ptr<int[]>` member.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::copy, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `int_buffer` ---
     * The owner of the previous snippets, now with all five special members.
     * - The move constructor takes an `int_buffer&&` - an object that is about to die, or that its owner has given up.
     *   It copies the address and the size, and sets the source to empty, so that its destructor deletes nothing.
     * - The move assignment gives its own block back first, then does the same. `a = std::move(a)` must not destroy
     *   the block, hence the test.
     * - Both are `noexcept`: they allocate nothing, so nothing can fail. Why the word matters: see below.
     * - !![#move-semantics]
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

        int_buffer(int_buffer&& other) noexcept : size_{other.size_}, data_{other.data_} {
            other.size_ = 0;
            other.data_ = nullptr;
        }

        int_buffer& operator=(int_buffer&& other) noexcept {
            if (this != &other) {
                delete[] data_;
                data_ = other.data_;
                size_ = other.size_;
                other.data_ = nullptr;
                other.size_ = 0;
            }
            return *this;
        }

        ~int_buffer() { delete[] data_; }       // `delete[]` of a `nullptr` does nothing

        size_t size() const { return size_; }
        const int* data() const { return data_; }

    private:
        size_t size_;
        int* data_;
    };

    /* --- `copy_or_move` ---
     * The copy gets a block of its own: one allocation, 4000 bytes, and the elements copied. The move gets the block
     * of `a`: no allocation, and `c.data()` is the address `a.data()` had. What is left of `a` is an empty
     * `int_buffer` - still an object, destroyed at the `}` like every other.
     */
    void copy_or_move() {
        print_function_header();

        int_buffer a{1000};
        const void* before{a.data()};
        heap_watch heap{};
        const int_buffer b{a};
        cout << " 1| copy: b.data()=" << b.data() << ", a.data()=" << a.data() << ", allocations="
             << heap.allocations() << ", bytes=" << heap.bytes() << '\n';

        heap.reset();
        const int_buffer c{std::move(a)};
        cout << " 2| move: c.data()=" << c.data() << " (was " << before << "), allocations=" << heap.allocations()
             << '\n';
        cout << " 3| a after the move: a.data()=" << a.data() << ", a.size()=" << a.size() << '\n';
    }

    /* --- `describe` --- Two overloads: one for any object, one only for objects that are about to die. */
    void describe(const int_buffer& b) {
        cout << " a|   const int_buffer& - somebody else's, size " << b.size() << " - may be copied\n";
    }

    void describe(int_buffer&& b) {
        cout << " b|   int_buffer&& - about to die, size " << b.size() << " - may be moved from\n";
    }

    /* --- `bind_to_what_dies` ---
     * A named object is an lvalue: it has an address, and it lives on after the call - the `const&` overload. A
     * temporary is an rvalue: it dies at the `;` - the `&&` overload. `std::move(a)` turns `a` into an rvalue - no
     * code, only another type - and says: I do not need it any more. That is the whole trick of move semantics: the
     * overload tells the function whether it may steal.
     * - !![#value-categories]
     */
    void bind_to_what_dies() {
        print_function_header();

        int_buffer a{3};
        describe(a);
        describe(int_buffer{5});
        describe(std::move(a));             // nothing is moved - `describe` only looks
        cout << " 1| a.size()=" << a.size() << '\n';

        /* -- .Q&A -- !![`std::move(a)` - and `a` still has its elements. Why?](#a-606) */
    }

    /* --- `move_a_vector` ---
     * A `vector` is three words on the stack and a block on the heap (see previous snippets). Its copy allocates a new
     * block and copies ten million `int`s. Its move copies the three words and sets the source to empty - the block
     * stays where it is, and changes its owner. Measure as Release.
     */
    void move_a_vector() {
        print_function_header();

        vector<int> numbers(10'000'000, 1);
        const void* block{numbers.data()};

        heap_watch heap{};
        stopwatch watch{};
        const vector<int> copied{numbers};
        const double copy_ms{watch.elapsed_ms()};
        const size_t copy_allocations{heap.allocations()};

        heap.reset();
        watch.reset();
        const vector<int> moved{std::move(numbers)};
        const double move_ms{watch.elapsed_ms()};

        cout << " 1| copy: " << copy_ms << " ms, " << copy_allocations << " allocation, copied.size()=" << copied.size()
             << '\n';
        cout << " 2| move: " << move_ms << " ms, " << heap.allocations() << " allocations, moved.data()="
             << moved.data() << " (was " << block << ")\n";
        cout << " 3| numbers.size()=" << numbers.size() << '\n';
    }

    /* --- `record` and `sure_record` ---
     * Two classes that report copies and moves - the same, except for one word: the move constructor of `sure_record`
     * is `noexcept`. `std::move(other.name_)` moves the `string` member: its characters change the owner, too.
     */
    class record {
    public:
        explicit record(const string& name) : name_{name} {}
        record(const record& other) : name_{other.name_} { cout << " c|   copied " << name_ << '\n'; }
        record(record&& other) : name_{std::move(other.name_)} { cout << " d|   moved " << name_ << '\n'; }

    private:
        string name_;
    };

    class sure_record {
    public:
        explicit sure_record(const string& name) : name_{name} {}
        sure_record(const sure_record& other) : name_{other.name_} { cout << " e|   copied " << name_ << '\n'; }
        sure_record(sure_record&& other) noexcept : name_{std::move(other.name_)} {
            cout << " f|   moved " << name_ << '\n';
        }

    private:
        string name_;
    };

    /* --- `grow_a_vector` ---
     * When a `vector` is full, it allocates a bigger block and brings its elements over. With `record`, it copies them
     * - although a move constructor is there. With `sure_record`, it moves them.
     * The reason is a promise of `push_back`: if something throws, the `vector` stays as it was. A copy leaves the old
     * block intact; if a copy fails, the new block is thrown away. A move empties the old elements - if the third move
     * threw, two elements would be gone, and there is no way back. So the `vector` moves only what cannot throw:
     * `noexcept`, checked at compile time.
     * - !![#noexcept]
     */
    void grow_a_vector() {
        print_function_header();

        vector<record> records;
        for (const string name : {"a", "b", "c"}) {
            cout << " 1| emplace_back(" << name << ")\n";
            records.emplace_back(name);
        }

        vector<sure_record> sure_records;
        for (const string name : {"a", "b", "c"}) {
            cout << " 2| emplace_back(" << name << ")\n";
            sure_records.emplace_back(name);
        }
    }

    /* --- `smart_buffer` ---
     * The Rule of Zero again: a member that owns itself, and no special member at all. The generated move moves the
     * `unique_ptr` - the block changes its owner - and copies `size_`. The generated copy is gone, because a
     * `unique_ptr` cannot be copied: you get a compiler error instead of a double `delete`.
     * - !![#rule-of-zero]
     */
    class smart_buffer {
    public:
        explicit smart_buffer(const size_t size) : size_{size}, data_{make_unique<int[]>(size)} {}

        size_t size() const { return size_; }
        const int* data() const { return data_.get(); }

    private:
        size_t size_;
        unique_ptr<int[]> data_;
    };

    /* --- `move_with_zero_rules` ---
     * Moving works - and look at `a` afterwards: no block, but `size()` still says 1000. The generated move copies an
     * `int`, it does not know that it belongs to the block. A moved-from object is "valid but unspecified": you may
     * destroy it or assign to it, nothing else. Where the members must agree, write the moves yourself - or use a
     * `vector`, which keeps the size and the block together.
     */
    void move_with_zero_rules() {
        print_function_header();

        smart_buffer a{1000};
        const heap_watch heap{};
        const smart_buffer b{std::move(a)};
        // const smart_buffer c{b};         // compiler error: the copy constructor is deleted
        cout << " 1| b.data()=" << b.data() << ", b.size()=" << b.size() << ", allocations=" << heap.allocations()
             << '\n';
        cout << " 2| a.data()=" << a.data() << ", a.size()=" << a.size() << '\n';
    }

}

/* --- Rule of Five ---
 * A class that manages a resource by hand needs five special members: destructor, copy constructor, copy assignment,
 * move constructor, move assignment - or `= delete` for those it should not have. Declare one, think about all five.
 * Better: let members that own themselves - `unique_ptr`, `vector`, `string` - do the work, and write none. The
 * mantra of this unit: every `new` needs an owner - and the best owner is one you did not have to write.
 */

/* --- `main` --- */
int main() {
    copy_or_move();
    bind_to_what_dies();
    move_a_vector();
    grow_a_vector();
    move_with_zero_rules();

    return EXIT_SUCCESS;
}
