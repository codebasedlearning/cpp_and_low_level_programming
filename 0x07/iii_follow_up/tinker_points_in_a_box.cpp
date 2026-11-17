// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Placement `new` needs bytes, not a heap: an array of `byte` inside an object will do.
 * - A box for up to five `point`s: raw bytes for five, and a count of the living ones. `push_back` constructs in the
 *   next free slot, `pop_back` destroys the last one, the destructor of the box destroys the rest.
 * - No allocation at all - the points live where the box lives, here on the stack.
 * - C++26 has this container for any type: `std::inplace_vector`.
 */

#include <iostream>
#include <new>                              // for placement new, bad_alloc
#include <cstddef>                          // for byte
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::byte, std::bad_alloc;


/* ---- Content ---- */

namespace {

    /* --- `point` --- Two `int`s that report their birth and their death, with their address. */
    struct point {
        int x;
        int y;

        point(const int x0, const int y0) : x{x0}, y{y0} {
            cout << " a|   point " << x << ": constructed at " << this << '\n';
        }

        ~point() {
            cout << " b|   point " << x << ": destroyed at " << this << '\n';
        }
    };

    /* --- `point_box` ---
     * `slots_` is raw memory for five `point`s - no `point` in it, and no constructor runs when a box is created.
     * `push_back` builds one with placement `new` in the next free slot and keeps the address it returns; `pop_back`
     * calls the destructor of the last one by hand. `size_` is the number of living points - the bytes behind them are
     * capacity, as in a `vector`. The destructor of the box destroys the living points, the last one first; the bytes
     * need no `delete`, they are part of the box.
     * The box is full after five: `push_back` throws `bad_alloc`, as `std::inplace_vector` does. `pop_back` on an
     * empty box is not checked - as for a `vector`, it is undefined behavior.
     */
    class point_box {
    public:
        static constexpr int capacity{5};

        point_box() = default;
        point_box(const point_box&) = delete;
        point_box& operator=(const point_box&) = delete;

        ~point_box() {
            while (size_ > 0) {
                pop_back();
            }
        }

        point& push_back(const int x, const int y) {
            if (size_ == capacity) {
                throw bad_alloc{};
            }
            point* p{::new (slots_[size_]) point{x, y}};
            items_[size_] = p;
            ++size_;
            return *p;
        }

        void pop_back() {
            --size_;
            items_[size_]->~point();
            items_[size_] = nullptr;
        }

        int size() const { return size_; }
        point& operator[](const int i) { return *items_[i]; }

    private:
        alignas(point) byte slots_[capacity][sizeof(point)];
        point* items_[capacity]{};
        int size_{0};
    };

    /* -- .Kept simple. --
     * `items_` keeps the addresses that placement `new` returns - five pointers, 40 bytes, only to avoid a cast. A real
     * container computes the address from `slots_` and the index, and turns it into a `point*` with a cast - casts
     * are the topic of a future unit. The copy is deleted: the generated one would copy the bytes and the pointers,
     * and the pointers of the copy would point into the original - and copying bytes copies no object, no constructor
     * runs. The moves are left out for the same reason.
     */

    /* --- `show_points_without_heap` ---
     * Three points, and `heap_watch` counts no allocation. The box is a local, so the points are on the stack: the
     * address of `box[0]` is the address of the box - `slots_` is its first member. `sizeof(point_box)` is the bytes
     * for five points, the five pointers, and the count, rounded up to a multiple of 8.
     */
    void show_points_without_heap() {
        print_function_header();

        const heap_watch heap{};
        point_box box{};
        box.push_back(1, 1);
        box.push_back(2, 2);
        box.push_back(3, 3);
        cout << " 1| &box=" << &box << ", &box[0]=" << &box[0] << ", &box[2]=" << &box[2] << '\n';
        cout << " 2| sizeof(point_box)=" << sizeof(point_box) << ", size=" << box.size() << ", allocations="
             << heap.allocations() << '\n';
    }

    /* --- `pop_and_push_again` ---
     * `pop_back` destroys the point in the last slot - the bytes stay where they are, and the next `push_back` builds
     * a new point at the same address.
     */
    void pop_and_push_again() {
        print_function_header();

        point_box box{};
        box.push_back(1, 1);
        box.push_back(2, 2);
        box.pop_back();
        cout << " 1| size=" << box.size() << '\n';
        const point& p{box.push_back(20, 20)};
        cout << " 2| size=" << box.size() << ", p.x=" << p.x << " at " << &p << '\n';
    }

    /* --- `try_a_sixth_point` ---
     * Five fill the box, the sixth `push_back` throws - before placement `new`, so no point is half built. At the
     * `}`, the destructor of the box destroys the five, the last one first.
     */
    void try_a_sixth_point() {
        print_function_header();

        point_box box{};
        for (int i{1}; i <= point_box::capacity; ++i) {
            box.push_back(i, i);
        }
        try {
            box.push_back(6, 6);
        } catch (const bad_alloc&) {
            cout << " 1| the sixth push_back threw: the box is full, size=" << box.size() << '\n';
        }
    }

}

/* --- `main` --- */
int main() {
    show_points_without_heap();
    pop_and_push_again();
    try_a_sixth_point();

    return EXIT_SUCCESS;
}
