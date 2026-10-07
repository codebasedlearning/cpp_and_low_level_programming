// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The vtable, read by hand: the vptr, the slots it points to, and the two entries in front of them - the offset to
 *   the top and the type information.
 * - A virtual call, made by hand: take the address from slot 2, and call it with the object as its first argument -
 *   the hidden `this`.
 * - None of this is standard C++. It follows the Itanium C++ ABI - the rules gcc and clang use on Linux and macOS, and
 *   MinGW on Windows. MSVC has its own layout, and there this snippet only says so. For the curious - never in a real
 *   program.
 */

#include <iostream>
#include <cstring>
#include <cstddef>
#include <typeinfo>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ptrdiff_t, std::type_info;


/* ---- Content ---- */

namespace {

    /* --- `shape` and `circle` --- As in the session: a destructor, `area`, `name`. */
    class shape {
    public:
        virtual ~shape() = default;
        virtual double area() const = 0;
        virtual const char* name() const { return "shape"; }
    };

    class circle : public shape {
    public:
        explicit circle(const double radius) : radius_{radius} {}

        double area() const override { return 3.14159 * radius_ * radius_; }
        const char* name() const override { return "circle"; }

    private:
        double radius_;
    };

    /* --- `read_address` ---
     * The 8 bytes at `where`, as an address. `memcpy`, so that no pointer of the wrong type reads the memory (see
     * previous snippets).
     */
    const void* read_address(const void* where) {
        const void* address{nullptr};
        std::memcpy(&address, where, sizeof address);
        return address;
    }

    /* --- `entry` --- The address of entry `index` of the table - negative numbers count backwards. */
    const void* entry(const void* vptr, const ptrdiff_t index) {
        return static_cast<const unsigned char*>(vptr) + index * static_cast<ptrdiff_t>(sizeof(void*));
    }

#if !defined(_MSC_VER)

    /* --- `read_the_table` ---
     * The vptr is the first 8 bytes of `c`. From where it points: `[0]` and `[1]` the two destructors, `[2]` `area`,
     * `[3]` `name` - addresses in the code. In front: `[-1]` the address of the type information - the same object
     * `typeid(circle)` refers to - and `[-2]` the offset to the top, 0 for a single base.
     */
    void read_the_table() {
        print_function_header();

        const circle c{2.0};
        const void* vptr{read_address(&c)};
        cout << " 1| vptr=" << vptr << '\n';
        for (ptrdiff_t i{0}; i < 4; ++i) {
            cout << " 2| [" << i << "]=" << read_address(entry(vptr, i)) << '\n';
        }

        const void* rtti{read_address(entry(vptr, -1))};
        ptrdiff_t offset_to_top{0};
        std::memcpy(&offset_to_top, entry(vptr, -2), sizeof offset_to_top);
        cout << " 3| [-1]=" << rtti << ", &typeid(circle)=" << static_cast<const void*>(&typeid(circle)) << '\n';
        cout << " 4| [-2]=" << offset_to_top << '\n';

        const type_info* info{static_cast<const type_info*>(rtti)};
        cout << " 5| the name in the type information: " << info->name() << '\n';
    }

    /* --- `call_by_hand` ---
     * The address in slot 2 is the function `circle::area`. It is a member function, but in the machine a member
     * function is a function with `this` as its first argument (see previous snippets). So a pointer to a function that
     * takes a `const shape*` and returns a `double` can call it. The address is copied into such a pointer with
     * `memcpy` - a cast from an address of data to an address of code is not portable, and `-pedantic` says so.
     * The result is what `c.area()` computes - a virtual call, done by hand.
     */
    void call_by_hand() {
        print_function_header();

        using area_function = double (*)(const shape*);

        const circle c{2.0};
        const void* vptr{read_address(&c)};
        area_function area{nullptr};
        std::memcpy(&area, entry(vptr, 2), sizeof area);
        cout << " 1| by hand: " << area(&c) << ", virtual call: " << c.area() << '\n';
    }

#else

    void read_the_table() {
        print_function_header();
        cout << " 1| MSVC lays out its tables differently - this snippet reads them the Itanium way only\n";
    }

    void call_by_hand() {
        print_function_header();
        cout << " 1| see above\n";
    }

#endif

}

/* --- `main` --- */
int main() {
    read_the_table();
    call_by_hand();

    return EXIT_SUCCESS;
}
