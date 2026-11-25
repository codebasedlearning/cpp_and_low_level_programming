// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A `virtual` function is chosen by the object, a non-virtual one by the type the compiler sees. In Java, every
 *   method is virtual - in C++, only those you mark.
 * - `virtual` adds one pointer to every object - the vptr, 8 bytes, in front of the first member - however many
 *   virtual functions the class has.
 * - The vptr points to a table of addresses of functions: the vtable, one per class, in static storage. All circles
 *   point to the same table. The vptr is the only thing in a circle that says it is a circle.
 * - A virtual call: load the vptr from the object, load the address of the function from the table, jump there. Two
 *   loads and an indirect jump, decided at run time.
 * - If the compiler knows the type - a local object, a `final` class, a call with the class name in front - the call is
 *   direct again, and can be inlined. gcc even guesses.
 * - The jump is cheap as long as the processor predicts where it goes - with mixed types, it guesses wrong.
 */

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <cstddef>
#include <cbl/printing.hpp>

using std::cout, std::string, std::string_view, std::vector, std::unique_ptr, std::make_unique, std::size_t, std::byte;


/* ---- Content ---- */

/*--- namespace java_like  --- */
namespace java_like {

    struct object {
        explicit object(int n) : n{n} {}

        string to_string() const { return (std::stringstream() << "OBJECT(n=" << n << ")").str(); }

        int n;                              // some payload
    };

    struct derived : object {
        explicit derived(int n, int m) : object{n}, m{m} {}     // note: call the base ctor

        string to_string() const { return (std::stringstream() << "DERIVED(n=" << n << ",m=" << m << ")").str(); }

        int m;                              // more payload
    };

    void print_instance(const object& b) {
        cout << " P| to_string: " << b.to_string() << '\n';
    }

    /* --- `compare_behaviour` --- */
    void compare_behaviour() {
        print_function_header();

        // Check, everything is called as expected.
        object o{23};
        cout << " 1| to_string: " << o.to_string() <<  '\n';
        derived d{42,99};
        cout << " 2| to_string: " << d.to_string() << '\n';

        // What would be printed for a similar situation in Java? What is printed here?
        // print_instance(o);
        // print_instance(d);
    }

}

namespace {

    // Same structure, only with virtual functions.
    struct object {
        explicit object(int n) : n{n} {}

        // here it comes: virtual
        virtual string to_string() const { return (std::stringstream() << "OBJECT(n=" << n << ")").str(); }

        // for later
        virtual string sum_as_str(int k) const { return std::to_string(n+k); }

        int n;
    };

    // Same structure, only with virtual/override functions.
    struct derived : object {
        explicit derived(int n, int m) : object{n}, m{m} {}

        string to_string() const { return (std::stringstream() << "DERIVED(n=" << n << ",m=" << m << ")").str(); }

        string sum_as_str(int k) const override { return std::to_string(n+m+k); }

        int m;
    };

    void print_instance(const object& b) {
        cout << " Q| to_string: " << b.to_string() << '\n';
    }

    /* --- `virtual_in_action` --- */
    void virtual_in_action() {
        print_function_header();

        // Check, everything is called as expected.
        object o{23};
        cout << " 1| to_string: " << o.to_string() <<  '\n';
        derived d{42,99};
        cout << " 2| to_string: " << d.to_string() << '\n';

        // What is printed now?
        print_instance(o);
        print_instance(d);

        // classes contain ints
        cout << " 3| sizeof(java_like::object)=" << sizeof(java_like::object)
             << ", sizeof(java_like::derived)=" << sizeof(java_like::derived) << '\n';
        // classes contain ints, ... and more? (remember padding)
        cout << " 4| sizeof(object)=" << sizeof(object)
             << ", sizeof(derived)=" << sizeof(derived) << '\n';
    }

    /* --- `offset_of` --- As in the previous snippet: the distance in bytes. */
    ptrdiff_t offset_of(const void* member, const void* object) {
        return static_cast<const byte*>(member) - static_cast<const byte*>(object);
    }

    /* --- `vptr_of` ---
     * The first 8 bytes of a shape, as an address - the vptr. That is where gcc, clang and MSVC put it; the standard
     * does not even say that there is one. `memcpy` copies the bytes into a pointer - reading them through a
     * `const void* const*` would break the aliasing rule (see previous snippets). clang warns that "the vtable pointer
     * will be copied" - which is what we want; the cast to `const void*` says so.
     */
    const void* vptr_of(const object& s) {
        const void* vptr{nullptr};
        std::memcpy(&vptr, static_cast<const void*>(&s), sizeof(vptr));
        return vptr;
    }

    /* --- `show_the_hidden_pointer` ---
     * the pointer is in front of it, at offset 0 - the layout dump of the preparation showed it.
     * - !![#vtable]
     */
    void show_the_hidden_pointer() {
        print_function_header();

        // detect the place
        const java_like::object j{1};
        cout << " 1| &j=" << &j << ", &j.n=" << &j.n << ", offset n: " << offset_of(&j.n, &j) << '\n';
        const object o{23};
        cout << " 2| &o=" << &o << ", &o.n=" << &o.n << ", offset n: " << offset_of(&o.n, &o) << '\n';
        derived d{42,99};
        cout << " 3| &d=" << &d << ", &d.n=" << &d.n << ", offset n: " << offset_of(&d.n, &d) << '\n';

        // read it
        cout << " 4| vptr o: " << vptr_of(o) << '\n';

        // different to o but the same for d and e
        derived e{123,456};
        cout << " 5| vptr d: " << vptr_of(d) << ", vptr e: " << vptr_of(e) << '\n';
    }

    /* --- function pointers --- `to_string_function` and `sum_as_str_function`
     * One type per slot: the types of the two virtual functions, with the hidden `this` as the first parameter.
     */
    using to_string_function = string (*)(const object*);
    using sum_as_str_function = string (*)(const object*, int);

    /* --- `entry_of` ---
     * Entry `index` of the table of `o`, as a pointer of type `Function`. The vptr comes from `vptr_of`; the slots are
     * 8 bytes each, the size of an address - but not of one type: every slot holds a function of its own type, and
     * the two entries in front of slot 0 are no functions at all (see future snippets). So the table is no array of
     * one type, and the arithmetic is done in bytes, with a `const byte*` - memory, not numbers. A `const void*` does
     * not turn into one by itself, as it does in C: `static_cast`. Which type a slot holds, only the caller knows - so
     * it is a template parameter, given explicitly: `entry_of<to_string_function>(o, 0)`. Nothing in the arguments
     * says it, the compiler cannot deduce it.
     * The entry is copied with `memcpy`, as the vptr in `vptr_of`: no pointer of the wrong type reads the memory. And
     * the table holds addresses of code - a cast from an address of data to an address of code is not portable,
     * `memcpy` does not ask.
     */
    template <typename Function>
    Function entry_of(const object& o, const size_t index) {
        const byte* table{static_cast<const byte*>(vptr_of(o))};
        Function entry{nullptr};
        std::memcpy(&entry, table + index * sizeof(void*), sizeof(entry));
        return entry;
    }

    /* --- `call_through_the_vtable` ---
     * A virtual call, written by hand: take the vptr from the object, take the entry with the number of the function
     * - in the order of declaration, `to_string` is entry 0 and `sum_as_str` entry 1 - and call it, with the address
     * of the object as its first argument. In the machine, a member function is a function with `this` as its first
     * parameter - that is all the hidden `this` is. So a plain function pointer (see previous snippets) that takes a
     * `const object*` can call it, and the `int` of `sum_as_str` comes after it. The `string` comes back the same way
     * for both: through a hidden pointer to the place for the result.
     * For `o`, entry 1 is `object::sum_as_str`; for `d`, it is `derived::sum_as_str`. The same index, the same call -
     * and different code: the object decides, by its table. `r.to_string()` is what the compiler makes of it: two
     * loads - the vptr, then the entry - and an indirect call.
     * None of this is C++: the standard knows no vtable. It is the Itanium C++ ABI, the rules of gcc and clang on
     * Linux and macOS - a demo, never code for a real program. Not shown: in front of entry 0, the table has two more
     * entries, and a `virtual` destructor takes two of its own (see future snippets).
     * - !![#vtable]
     *
     * If it does not run, then the table layout of the compiler might be different (try offset 2 instead of 0).
     */
    void call_through_the_vtable() {
        print_function_header();

        const object o{1};
        const derived d{2,3};
        cout << " 1| o " << vptr_of(o) << '\n';
        cout << " 2| d " << vptr_of(d) << '\n';

        cout << " 3| o.to_string by hand: " << entry_of<to_string_function>(o, 0)(&o) << '\n';
        cout << " 4| d.to_string by hand: " << entry_of<to_string_function>(d, 0)(&d) << '\n';

        cout << " 5| o.sum_as_str by hand: " << entry_of<sum_as_str_function>(o, 1)(&o, 10) << '\n';
        cout << " 6| d.sum_as_str by hand: " << entry_of<sum_as_str_function>(d, 1)(&d, 20) << '\n';
    }
}

/* --- `main` --- */
int main() {
    java_like::compare_behaviour();
    virtual_in_action();
    show_the_hidden_pointer();
    call_through_the_vtable();

    return EXIT_SUCCESS;
}
