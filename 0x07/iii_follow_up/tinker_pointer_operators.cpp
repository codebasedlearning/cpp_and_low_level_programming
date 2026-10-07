// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `*` and `->` make a class look like a pointer - that is all `unique_ptr` does on the outside.
 * - `->` drills down: `h->title()` is `h.operator->()->title()`. The compiler applies `->` again until it reaches a raw
 *   pointer.
 * - `explicit operator bool`: is there anything?
 * - Unary `&` can be overloaded, too - and then `&x` is no longer the address. `std::addressof` is, always.
 */

#include <iostream>
#include <string>
#include <memory>                           // for addressof
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

namespace {

    /* --- `track` --- Something to point to. */
    class track {
    public:
        explicit track(const string& title) : title_{title} {}
        const string& title() const { return title_; }

    private:
        string title_;
    };

    /* --- `handle` ---
     * A small owner, as in unit 0x06: it deletes in its destructor and cannot be copied. `operator*` returns the object
     * by reference, `operator->` returns the raw pointer - and the compiler applies the built-in `->` to it.
     * - !![#unique-ptr]
     */
    template <typename T>
    class handle {
    public:
        explicit handle(T* p) : p_{p} {}
        ~handle() { delete p_; }
        handle(const handle&) = delete;
        handle& operator=(const handle&) = delete;

        T& operator*() const { return *p_; }
        T* operator->() const { return p_; }
        explicit operator bool() const { return p_ != nullptr; }

    private:
        T* p_;
    };

    /* --- `use_a_handle` ---
     * Three spellings of the same call. `sizeof(handle<track>)` is the size of one pointer: the operators are
     * functions, not data.
     */
    void use_a_handle() {
        print_function_header();

        const handle<track> h{new track{"Blue in Green"}};
        if (h) {
            cout << " 1| h->title()=" << h->title() << '\n';
            cout << " 2| (*h).title()=" << (*h).title() << '\n';
            cout << " 3| h.operator->()->title()=" << h.operator->()->title() << '\n';
        }
        cout << " 4| sizeof(handle<track>)=" << sizeof(handle<track>) << ", sizeof(track*)=" << sizeof(track*) << '\n';
    }

    /* --- `liar` ---
     * Overloads the unary `&` - and returns something else than its address. Legal, and a bad idea: everybody, and
     * every template, expects `&x` to be the address of `x`.
     */
    class liar {
    public:
        const liar* operator&() const { return nullptr; }
        int value{42};
    };

    /* --- `take_the_real_address` ---
     * `&l` calls the overloaded operator. `std::addressof(l)` bypasses it and returns the real address - that is why
     * the standard library uses it everywhere, and why generic code should, too.
     * - !![#addressof]
     */
    void take_the_real_address() {
        print_function_header();

        const liar l;
        cout << " 1| &l=" << &l << ", std::addressof(l)=" << std::addressof(l) << ", &l.value=" << &l.value << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_a_handle();
    take_the_real_address();

    return EXIT_SUCCESS;
}
