// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A small buffer: up to N elements inside the object itself, more on the heap - no allocation while it is small.
 * - The price: the object is bigger, and it has to know where its elements are.
 * - When the pointer points into the object itself, the generated copy is wrong - the copy would point into the
 *   original. So a small buffer needs a hand-written copy.
 * - `std::string` does the same for short texts: small string optimization (SSO).
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::vector, std::copy, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `small_ints` ---
     * `data_` points to `inline_` - into the object itself - as long as the elements fit. When the `N + 1`-th element
     * comes, `grow` moves them into a heap block, and `data_` points there. The destructor must know which case it is
     * in: only a heap block is deleted.
     * The copy constructor makes the same decision for the new object: a small copy points into its own `inline_`.
     * The copy assignment and the moves have the same cases, and more code - they are `= delete` here. Try them.
     * - !![#sbo]
     */
    template <size_t N>
    class small_ints {
    public:
        small_ints() = default;

        small_ints(const small_ints& other) : size_{other.size_}, capacity_{other.capacity_} {
            if (!other.is_small()) {
                data_ = new int[capacity_];
            }
            copy(other.data_, other.data_ + other.size_, data_);
        }

        small_ints& operator=(const small_ints&) = delete;

        ~small_ints() {
            if (!is_small()) {
                delete[] data_;
            }
        }

        void push_back(const int value) {
            if (size_ == capacity_) {
                grow();
            }
            data_[size_] = value;
            ++size_;
        }

        size_t size() const { return size_; }
        bool is_small() const { return data_ == inline_; }
        const int* data() const { return data_; }

    private:
        size_t size_{0};
        size_t capacity_{N};
        int inline_[N]{};
        int* data_{inline_};

        void grow() {
            int* block{new int[2 * capacity_]};
            copy(data_, data_ + size_, block);
            if (!is_small()) {
                delete[] data_;
            }
            data_ = block;
            capacity_ *= 2;
        }
    };

    /* --- `stay_small` ---
     * Four `int`s fit, no allocation. The fifth moves everything to the heap. And the object is bigger than a
     * `vector`: 16 bytes of `inline_`, plus the size, the capacity and the pointer - memory that is there even when it
     * is not used.
     */
    void stay_small() {
        print_function_header();

        cout << " 1| sizeof(small_ints<4>)=" << sizeof(small_ints<4>) << ", sizeof(vector<int>)=" << sizeof(vector<int>)
             << '\n';
        const heap_watch heap{};
        small_ints<4> s;
        for (int i{1}; i <= 4; ++i) {
            s.push_back(i);
        }
        cout << " 2| size 4: small=" << s.is_small() << ", allocations=" << heap.allocations() << '\n';
        s.push_back(5);
        cout << " 3| size 5: small=" << s.is_small() << ", allocations=" << heap.allocations() << '\n';
    }

    /* --- `self_pointing` --- The problem in two lines: a pointer to a member of the same object. */
    struct self_pointing {
        int value{0};
        int* p{&value};
    };

    /* --- `copy_a_self_pointer` ---
     * The generated copy copies `p` - the address of `a.value`. So `b.p` points into `a`, not into `b`: write through
     * it, and `a` changes; destroy `a`, and `b.p` dangles. That is what the generated copy of `small_ints` would do.
     * The hand-written one does not: the small copy points into itself.
     */
    void copy_a_self_pointer() {
        print_function_header();

        self_pointing a;
        self_pointing b{a};
        *b.p = 42;
        cout << " 1| &a.value=" << &a.value << ", &b.value=" << &b.value << ", b.p=" << b.p << '\n';
        cout << " 2| a.value=" << a.value << ", b.value=" << b.value << '\n';

        small_ints<4> s;
        s.push_back(1);
        const small_ints<4> t{s};
        cout << " 3| &s=" << &s << ", s.data()=" << s.data() << ", &t=" << &t << ", t.data()=" << t.data() << '\n';
    }

    /* --- `look_at_a_short_string` ---
     * A `string` is a small buffer, too: a short text is inside the object, and `data()` is an address within `s` -
     * between `&s` and `sizeof(s)` bytes later. A long text is on the heap. The libraries differ in how they tell the
     * two cases apart - gcc's library keeps a pointer into the object, as `small_ints` does; clang's looks at a bit,
     * MSVC's at the capacity - but they all needed hand-written copies and moves.
     * - !![#sso]
     */
    void look_at_a_short_string() {
        print_function_header();

        const string short_text{"Kind"};
        const string long_text{"Kind of Blue, recorded in 1959 at Columbia's 30th Street Studio"};
        const void* short_data{short_text.data()};
        const void* long_data{long_text.data()};
        cout << " 1| &short_text=" << &short_text << ", data()=" << short_data << ", sizeof=" << sizeof(short_text)
             << '\n';
        cout << " 2| &long_text=" << &long_text << ", data()=" << long_data << '\n';
    }

}

/* --- `main` --- */
int main() {
    stay_small();
    copy_a_self_pointer();
    look_at_a_short_string();

    return EXIT_SUCCESS;
}
