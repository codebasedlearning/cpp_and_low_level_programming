// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Before move semantics, C++98 had one smart pointer: `std::auto_ptr`. It handed the ownership on when it was
 *   copied - the "copy" stole the pointer and left a null behind.
 * - A copy that changes its source: every pass by value empties the caller's pointer, and nothing in the call shows
 *   it.
 * - Why `unique_ptr` forbids the copy and asks for `std::move` instead: the same transfer, but visible in the code.
 * - `auto_ptr` was deprecated in C++11 and removed in C++17. This file rebuilds its idea as `stealing_ptr`.
 */

#include <iostream>
#include <memory>
#include <utility>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

namespace {

    /* --- `stealing_ptr` ---
     * Owns an `int` on the heap, as a `unique_ptr<int>` would. The difference is the copy constructor: it takes its
     * source by non-`const` reference - it must, because it sets the source's pointer to `nullptr`. One owner at any
     * time, so one `delete` - that part is right. What is wrong is that it looks like a copy.
     */
    class stealing_ptr {
    public:
        explicit stealing_ptr(int* p) : p_{p} {}
        ~stealing_ptr() { delete p_; }

        stealing_ptr(stealing_ptr& other) : p_{other.p_} {  // a "copy" that steals
            other.p_ = nullptr;
        }

        stealing_ptr& operator=(stealing_ptr& other) {
            if (this != &other) {
                delete p_;
                p_ = other.p_;
                other.p_ = nullptr;
            }
            return *this;
        }

        int* get() const { return p_; }

    private:
        int* p_;
    };

    /* --- `show` --- Takes the pointer by value, as one would for a cheap handle. */
    void show(const stealing_ptr p) {
        cout << " a|   show: " << *p.get() << '\n';
    }

    /* --- `copy_and_lose` ---
     * `b{a}` reads like a copy - and empties `a`. Passing `b` to `show` by value empties `b`: the parameter owns the
     * `int` now, and deletes it at the end of `show`. Back in the caller, both are null, and a `*b.get()` would read
     * through `nullptr` (commented out).
     */
    void copy_and_lose() {
        print_function_header();

        stealing_ptr a{new int{42}};
        stealing_ptr b{a};
        cout << " 1| after b{a}: a.get()=" << a.get() << ", b.get()=" << b.get() << '\n';
        show(b);
        cout << " 2| after show(b): b.get()=" << b.get() << '\n';
        // cout << *b.get();                // reads through nullptr - undefined behavior
    }

    /* --- `move_and_see_it` ---
     * The same transfer with `unique_ptr`: `unique_ptr b{a};` does not compile - the copy is deleted. The transfer has
     * to be written down, `std::move(a)`, and so does the one into the function. A reader sees where the ownership
     * goes.
     */
    void show_unique(const unique_ptr<int> p) {
        cout << " b|   show_unique: " << *p << '\n';
    }

    void move_and_see_it() {
        print_function_header();

        unique_ptr<int> a{make_unique<int>(42)};
        // unique_ptr<int> b{a};            // does not compile: the copy constructor is deleted
        unique_ptr<int> b{std::move(a)};
        cout << " 1| after b{std::move(a)}: a is empty: " << (a == nullptr) << '\n';
        show_unique(std::move(b));
        cout << " 2| after show_unique(std::move(b)): b is empty: " << (b == nullptr) << '\n';
    }

}

/* --- Why it had to go ---
 * The standard containers and algorithms assume that a copy is a copy: equal to its source, and the source unchanged.
 * A `vector<auto_ptr<T>>` did not compile with most libraries, and where a copy slipped through - a sort that copies
 * its pivot element into a local - elements lost their objects. C++11 made the difference between copy and move part
 * of the language (`&&`, see previous snippets), and `unique_ptr` could say what it does: no copy, only a move.
 */

/* --- `main` --- */
int main() {
    copy_and_lose();
    move_and_see_it();

    return EXIT_SUCCESS;
}
