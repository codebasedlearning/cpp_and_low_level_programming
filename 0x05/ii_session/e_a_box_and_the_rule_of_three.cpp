// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A class that creates an object with `new` owns it: its destructor has to delete it.
 * - The generated copy copies member by member - the pointer, not what it points to. Two boxes, one `int`, and two
 *   destructors that delete it.
 * - The Rule of Three (see previous snippets): who writes a destructor, writes the copy constructor and the copy
 *   assignment as well - or forbids the copies with `= delete`.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl;


/* ---- Content ---- */

namespace {

    /* --- `naive_box` ---
     * One `int` on the heap: the constructor creates it, the destructor deletes it. No copy constructor, no copy
     * assignment - the compiler generates them, and they copy `value_`: an address.
     *
     * Think of a large box, maybe a movie or something with much data.
     */
    class naive_box {
    public:
        explicit naive_box(const int value) : value_{new int{value}} {}
        ~naive_box() { delete value_; }

        [[nodiscard]] const int* address() const { return value_; }

    private:
        int* value_;
    };

    /* --- `try_to_copy_a_naive_box` ---
     * `b` gets a copy of the address in `a` - two owners, one `int`. At the `}`, `b` deletes it, then `a` deletes it
     * again: a double `delete`, undefined behavior. With glibc, the program aborts with `free(): double free detected`.
     * Nobody warns: generating the copy next to a destructor of your own is deprecated since C++11, but only
     * `-Wdeprecated` (clang) or `-Wdeprecated-copy-dtor` (gcc) say so.
     * Remove the `//`s, run it - and put them back. `endl` on purpose: what is still in the buffer is lost when the
     * program is killed.
     */
    void try_to_copy_a_naive_box() {
        print_function_header();

        const naive_box a{23};
        cout << " 1| a.address()=" << a.address() << endl;
        // What happens here?
        // const naive_box b{a};
        // cout << " 2| b.address()=" << b.address() << " - the same `int`" << endl;
    }

    /* --- `box` ---
     * The same box with the Rule of Three. The copy constructor creates an `int` of its own; the copy assignment only
     * copies the value - both boxes already have their `int`.
     * - !![#rule-of-zero]
     *
     * Think of a large box, maybe a movie or something with much data.
     */
    class box {
    public:
        explicit box(const int value) : value_{new int{value}} {}
        box(const box& other) : value_{new int{*other.value_}} {}
        box& operator=(const box& other) {
            *value_ = *other.value_;
            return *this;
        }
        ~box() {
            cout << " a|   ~box: delete " << value_ << '\n';
            delete value_;
        }

        [[nodiscard]] const int* address() const { return value_; }

    private:
        int* value_;
    };

    /* --- `copy_a_box` ---
     * Three boxes, three `int`s, three addresses - and at the `}`, each `int` is deleted once.
     */
    void copy_a_box() {
        print_function_header();

        const box a{23};
        const box b{a};                     // copy constructor
        box c{42};
        c = a;                              // copy assignment
        cout << " 1| a: " << a.address() << ", b: " << b.address() << ", c: " << c.address() << '\n';
        cout << " 2| *b=" << *b.address() << ", *c=" << *c.address() << '\n';
    }

}

/* --- Three, five, zero ---
 * - Three: destructor, copy constructor, copy assignment - all three or none. Or `box(const box&) = delete;`: then
 *   `b{a}` is a compiler error instead of a crash.
 * - Five: two more, the moves - instead of a new `int`, take over the address of a box that is about to die anyway.
 *   See future snippets.
 * - Zero: let a member own - a `unique_ptr<int>` deletes by itself and cannot be copied (see previous snippets).
 */

/* --- `main` --- */
int main() {
    try_to_copy_a_naive_box();
    copy_a_box();

    return EXIT_SUCCESS;
}
