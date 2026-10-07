// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A pointer to a member function, `&shape::area`, is not an address of code: it must also work for virtual
 *   functions, and for a base class that is not at offset 0.
 * - gcc and clang (the Itanium ABI) make it 16 bytes: a number for the function, and the adjustment of `this`.
 * - For a non-virtual function, the number is its address. For a virtual one, it is the offset of its slot in the
 *   vtable - on x86-64 plus 1, an odd number, which says "virtual"; on ARM64, the "virtual" is in the adjustment.
 * - It is called with `.*` or `->*` - or `std::invoke`.
 * - Itanium ABI only: gcc and clang on Linux and macOS. MSVC has other sizes - 8 bytes for a class with one base.
 */

#include <iostream>
#include <array>
#include <cstdint>                          // for uintptr_t
#include <cstring>
#include <functional>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::array, std::uintptr_t;


/* ---- Content ---- */

namespace {

    /* --- `shape`, `named` and `circle` ---
     * A base with a virtual destructor - two slots, 0 and 8 - and two virtual functions in the slots 16 and 24, plus a
     * non-virtual one. `named` has no virtual function and is the second base of `circle`: it starts behind the
     * `shape` part.
     */
    struct shape {
        virtual ~shape() = default;
        virtual double area() const { return 0.0; }
        virtual double perimeter() const { return 0.0; }
        int id() const { return 7; }
    };

    struct named {
        const char* label() const { return label_; }
        const char* label_{"a circle"};
    };

    struct circle : shape, named {
        double area() const override { return 3.14; }
        double radius{1.0};
    };

#if !defined(_MSC_VER)

    /* --- `raw` ---
     * The 16 bytes of a pointer to a member function, as two numbers - copied out with `memcpy`, as the vptr in unit
     * 0x08.
     */
    template <typename P>
    array<uintptr_t, 2> raw(const P p) {
        static_assert(sizeof(P) == 2 * sizeof(uintptr_t), "Itanium ABI expected");
        array<uintptr_t, 2> words{};
        std::memcpy(words.data(), &p, sizeof p);
        return words;
    }

    /* --- `print_raw` --- The two numbers, in hex. */
    template <typename P>
    void print_raw(const char* line, const char* name, const P p) {
        const array<uintptr_t, 2> words{raw(p)};
        cout << line << name << ": {0x" << std::hex << words[0] << ", " << std::dec << static_cast<long long>(words[1])
             << "}\n";
    }

    /* --- `look_inside` ---
     * - `&shape::id` - non-virtual: the address of `shape::id`, and 0.
     * - `&shape::area`, `&shape::perimeter` - virtual: which function is called depends on the object, so the pointer
     *   holds the slot. x86-64: 16 + 1 = 0x11 and 24 + 1 = 0x19, and 0 - the odd number says "look it up in the
     *   vtable"; the compiler places member functions at even addresses, so that an odd one cannot be a function.
     *   ARM64: 16 and 24, and 1 - the lowest bit of the
     *   adjustment says "virtual", the rest is the adjustment times two.
     * - `&named::label` converted to a pointer to a member of `circle`: the function belongs to the `named` part,
     *   which is 8 bytes into a circle. So the adjustment is 8 - on ARM64 16, twice as much. The call adds it to `this`
     *   before it calls `label`.
     * All four are 16 bytes - a function pointer is 8.
     */
    void look_inside() {
        print_function_header();

        cout << " 1| sizeof(&shape::id)=" << sizeof(&shape::id) << ", sizeof(a function pointer)="
             << sizeof(&look_inside) << '\n';
        print_raw(" 2| ", "&shape::id       ", &shape::id);
        print_raw(" 3| ", "&shape::area     ", &shape::area);
        print_raw(" 4| ", "&shape::perimeter", &shape::perimeter);
        const char* (circle::*label)() const{&named::label};
        print_raw(" 5| ", "label in circle  ", label);
    }

#else

    void look_inside() {
        print_function_header();
        cout << " 1| MSVC stores pointers to members differently - this snippet reads them the Itanium way only\n";
    }

#endif

    /* --- `call_through_them` ---
     * `.*` for an object, `->*` for a pointer to one - with parentheses, because `()` binds tighter than `.*`.
     * `std::invoke` spares you the syntax. `&shape::area` called for a circle reaches `circle::area` - it is virtual,
     * and the pointer holds the slot, not the function.
     */
    void call_through_them() {
        print_function_header();

        const circle c;
        const shape* s{&c};
        double (shape::*area)() const{&shape::area};
        const char* (circle::*label)() const{&named::label};
        cout << " 1| (c.*area)()=" << (c.*area)() << ", (s->*area)()=" << (s->*area)() << ", (c.*label)()="
             << (c.*label)() << '\n';
        cout << " 2| invoke: " << std::invoke(area, c) << ", " << std::invoke(&shape::id, c) << '\n';
    }

}

/* --- `main` --- */
int main() {
    look_inside();
    call_through_them();

    return EXIT_SUCCESS;
}
