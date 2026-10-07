// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A class with two bases contains two sub-objects, one after the other - and with virtual functions in both, two
 *   vptrs.
 * - Only the first base starts at the address of the object. A pointer to the second base is the address plus an
 *   offset: the conversion adds it - and keeps a null pointer null.
 * - A virtual call through the second base reaches the derived function through a thunk, which subtracts the offset
 *   again.
 * - Back from a base to the derived class: `static_cast` subtracts the offset and trusts you, `dynamic_cast` asks the
 *   runtime library - and says `nullptr` if the object is something else.
 * - The diamond: two bases with a common base contain it twice. `virtual` inheritance makes it one - at the price of a
 *   vptr, and of an offset read from the table at every access.
 */

#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ptrdiff_t;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The classes and functions up to `back_to_sprite_checked`, and the devices further down, are for `nm` and Compiler
 * Explorer, below - as in previous snippets, they keep their own names in the object file.
 */

/* --- `printable` and `movable` ---
 * Two interfaces: pure virtual functions, a virtual destructor, no data (see the follow-up). A class may implement
 * both - multiple inheritance of interfaces, the common and harmless case.
 * - !![#multiple-inheritance]
 */
class printable {
public:
    virtual ~printable() = default;
    virtual const char* text() const = 0;
};

class movable {
public:
    virtual ~movable() = default;
    virtual void move_by(int dx, int dy) = 0;
};

/* --- `sprite` --- Something on a screen: it can be printed and moved. */
class sprite : public printable, public movable {
public:
    sprite(const int x, const int y) : x_{x}, y_{y} {}

    const char* text() const override { return "sprite"; }

    void move_by(const int dx, const int dy) override {
        x_ += dx;
        y_ += dy;
    }

    int x() const { return x_; }
    int y() const { return y_; }

private:
    int x_;
    int y_;
};

/* --- `as_movable`, `as_movable_ref` and `move_it` --- Two conversions and a call through the second base. */
movable* as_movable(sprite* s) {
    return s;
}

movable& as_movable_ref(sprite& s) {
    return s;
}

void move_it(movable& m) {
    m.move_by(1, 2);
}

/* --- `back_to_sprite` and `back_to_sprite_checked` --- The way back, without and with a check. */
sprite* back_to_sprite(movable* m) {
    return static_cast<sprite*>(m);
}

sprite* back_to_sprite_checked(movable* m) {
    return dynamic_cast<sprite*>(m);
}

namespace {

    /* --- `offset_of` --- As in the previous snippets: the distance in bytes. */
    ptrdiff_t offset_of(const void* part, const void* object) {
        return static_cast<const unsigned char*>(part) - static_cast<const unsigned char*>(object);
    }

    /* --- `show_two_bases` ---
     * `printable` and `movable` have no data, only a vptr each: 8 bytes. A `sprite` contains both - its `printable`
     * part at offset 0, its `movable` part at offset 8 - and then its two `int`s: 24 bytes, two vptrs. The layout dump
     * shows `(printable vtable pointer)` at 0 and `(movable vtable pointer)` at 8.
     * The first base starts where the sprite starts, as in single inheritance. The second one cannot: a pointer to the
     * `movable` part of `s` is not the address of `s`.
     */
    void show_two_bases() {
        print_function_header();

        const sprite s{10, 20};
        const printable* p{&s};
        const movable* m{&s};
        cout << " 1| sizeof(printable)=" << sizeof(printable) << ", sizeof(movable)=" << sizeof(movable)
             << ", sizeof(sprite)=" << sizeof(sprite) << '\n';
        cout << " 2| &s=" << &s << ", p=" << p << " (offset " << offset_of(p, &s) << "), m=" << m << " (offset "
             << offset_of(m, &s) << ")\n";
    }

    /* --- `convert_to_the_second_base` ---
     * The conversion from `sprite*` to `movable*` is an addition: 8 bytes. But a null pointer must stay null - so the
     * compiler checks for it first. In Compiler Explorer, `-O2`, x86-64 gcc, `as_movable` is
     * `lea rax, 8[rdi]`, `test rdi, rdi` and a `cmove` that takes 0 instead if `rdi` was null (ARM64: `add`, `cmp` and
     * `csel`). `as_movable_ref` is only the `lea`.
     */
    void convert_to_the_second_base() {
        print_function_header();

        sprite s{0, 0};
        sprite* none{nullptr};
        cout << " 1| as_movable(&s)=" << as_movable(&s) << ", as_movable(none)=" << as_movable(none) << '\n';

        /* -- .Q&A -- !![Why does `as_movable` test for null, and `as_movable_ref` does not?](#a-811) */
    }

    /* --- `call_through_the_second_base` ---
     * `move_it` gets a `movable&` - the address of the `movable` part of `s`, 8 bytes into the sprite - and calls
     * `move_by` through the vptr there: the same two loads as always (see previous snippets). But `sprite::move_by`
     * expects `this` to be the address of the sprite. So the slot does not hold `sprite::move_by` itself, but a small
     * function in front of it, a thunk: it subtracts 8 from `this` and jumps to `sprite::move_by`. See below.
     */
    void call_through_the_second_base() {
        print_function_header();

        sprite s{10, 20};
        move_it(s);
        cout << " 1| s: x=" << s.x() << ", y=" << s.y() << ", text()=" << s.text() << '\n';
    }

}

/* --- Two tables in one ---
 * The vtable dump (clang, `-Xclang -fdump-vtable-layouts`) shows 11 entries for `sprite`: the table for the sprite and
 * its `printable` part - offset to the top 0, the type information, the destructors, `text` and `move_by` - and a
 * second table behind it, for the `movable` part: `offset_to_top (-8)` - from here, the start of the object is 8 bytes
 * back - the type information again, and three entries with `[this adjustment: -8 non-virtual]`.
 * `nm -C` on the object file lists them as `non-virtual thunk to sprite::move_by(int, int)` and two for the
 * destructors. In Compiler Explorer, x86-64 gcc, `-O0`, the thunk is `sub rdi, 8` and a jump to `sprite::move_by`. With
 * `-O2`, gcc and clang copy the body of `move_by` into the thunk - with the 8 folded into the addresses: clang makes
 * `add dword ptr [rdi + 8], esi` of it.
 */

namespace {

    /* --- `cloud` --- Another `movable`, but no sprite. */
    class cloud : public movable {
    public:
        void move_by(const int dx, int) override { drift_ += dx; }

    private:
        int drift_{0};
    };

    /* --- `cast_back` ---
     * From a `movable*` back to a `sprite*`. `static_cast` subtracts 8, as `back_to_sprite` in Compiler Explorer shows:
     * `lea rax, -8[rdi]`, with the null check. It believes you that `m` points into a sprite. If it points to a cloud,
     * the result is an address 8 bytes in front of the cloud - undefined behavior to use it.
     * `dynamic_cast` checks. `back_to_sprite_checked` is: if `rdi` is null, return null; otherwise
     * `jmp __dynamic_cast`, with `typeinfo for movable`, `typeinfo for sprite` and the 8 as arguments. A function of
     * the runtime library: it follows the vptr to the type information of the object - the entry in front of the slots
     * (see previous snippet) - and walks the classes described there, looking for a `sprite`. For a cloud, it finds
     * none, and returns `nullptr`. For a reference, `dynamic_cast<sprite&>` throws `std::bad_cast` instead.
     * `static_cast`: one instruction and your promise. `dynamic_cast`: a call that searches, and the truth. It works
     * only for classes with virtual functions - it needs a vptr to find the type information.
     * - !![#rtti]
     */
    void cast_back() {
        print_function_header();

        sprite s{1, 2};
        cloud c;
        movable* m{&s};
        movable* n{&c};
        cout << " 1| &s=" << &s << ", back_to_sprite(m)=" << back_to_sprite(m) << '\n';
        // const sprite* wrong{back_to_sprite(n)};  // undefined behavior to use it: `n` points to a cloud
        cout << " 2| back_to_sprite_checked(m)=" << back_to_sprite_checked(m) << ", back_to_sprite_checked(n)="
             << back_to_sprite_checked(n) << '\n';
    }

}

/* --- `device`, `scanner`, `printer` and `copier` ---
 * A copier is a scanner and a printer, and both are devices. No `virtual` anywhere: a diamond on paper - but two
 * `device` parts in the object.
 */
struct device {
    explicit device(const int s) : serial{s} {}
    int serial;
};

struct scanner : device {
    scanner(const int s, const int d) : device{s}, dpi{d} {}
    int dpi;
};

struct printer : device {
    printer(const int s, const int p) : device{s}, pages_per_minute{p} {}
    int pages_per_minute;
};

struct copier : scanner, printer {
    copier() : scanner{7, 600}, printer{8, 30} {}
};

namespace {

    /* --- `build_a_diamond` ---
     * A copier contains a whole scanner and a whole printer - each with its own `device`: two serial numbers, 7 and
     * 8, at offsets 0 and 8. `c.serial` does not compile: which one? The full names say it, `c.scanner::serial`.
     * Sometimes two parts are what you want. For a serial number, they are not.
     */
    void build_a_diamond() {
        print_function_header();

        const copier c{};
        const scanner& s{c};
        const printer& p{c};
        cout << " 1| sizeof(device)=" << sizeof(device) << ", sizeof(scanner)=" << sizeof(scanner)
             << ", sizeof(copier)=" << sizeof(copier) << '\n';
        cout << " 2| c.scanner::serial=" << c.scanner::serial << " at offset " << offset_of(&s.serial, &c)
             << ", c.printer::serial=" << c.printer::serial << " at offset " << offset_of(&p.serial, &c) << '\n';
        // cout << c.serial;                // compiler error: `serial` is ambiguous
    }

}

/* --- `scanner_v`, `printer_v` and `copier_v` ---
 * The same diamond, with `virtual` inheritance: `scanner_v` and `printer_v` share their `device` with every other class
 * in the object that inherits it virtually. The most derived class - the one that is actually created - constructs the
 * virtual base: `copier_v` calls `device{7}`, and the `device{1}` and `device{2}` of `scanner_v` and `printer_v` are
 * ignored. `serial_of` is for Compiler Explorer.
 */
struct scanner_v : virtual device {
    scanner_v(const int s, const int d) : device{s}, dpi{d} {}
    int dpi;
};

struct printer_v : virtual device {
    printer_v(const int s, const int p) : device{s}, pages_per_minute{p} {}
    int pages_per_minute;
};

struct copier_v : scanner_v, printer_v {
    copier_v() : device{7}, scanner_v{1, 600}, printer_v{2, 30} {}
};

int serial_of(const scanner_v& s) {
    return s.serial;
}

namespace {

    /* --- `share_the_base` ---
     * One serial number, 7. But `sizeof(scanner_v)` is 16, not 8: a `scanner_v` has a vptr - without a single virtual
     * function. Look at the offsets: in a scanner on its own, `device` is 12 bytes behind the start of the scanner; in
     * the copier, 28 bytes behind the start of its `scanner_v` part. The distance depends on the object the scanner is
     * part of - so it cannot be written into the machine code. It is in the table.
     * `serial_of` in Compiler Explorer, `-O2`, x86-64 gcc: `mov rax, QWORD PTR [rdi]` - the vptr -
     * `mov rax, QWORD PTR -24[rax]` - the offset of `device`, in front of the offset to the top -
     * `mov eax, DWORD PTR [rdi+rax]` - the member. Three loads for one `int`, at every access. ARM64: three `ldr`.
     * Virtual inheritance is rare - the standard library has one: `std::iostream` derives from `std::istream` and
     * `std::ostream`, which share a virtual base.
     */
    void share_the_base() {
        print_function_header();

        const scanner_v alone{3, 300};
        const copier_v c{};
        const scanner_v& s{c};
        cout << " 1| sizeof(scanner_v)=" << sizeof(scanner_v) << ", sizeof(copier_v)=" << sizeof(copier_v) << '\n';
        cout << " 2| c.serial=" << c.serial << ", serial_of(c)=" << serial_of(c) << ", serial_of(alone)="
             << serial_of(alone) << '\n';
        cout << " 3| device in alone at offset " << offset_of(&alone.serial, &alone) << ", in c's scanner_v part at "
             << offset_of(&s.serial, &s) << '\n';

        /* -- .Q&A -- !![Why is the offset of the virtual base in the table, and not in the object?](#a-812) */
    }

}

/* --- `main` --- */
int main() {
    show_two_bases();
    convert_to_the_second_base();
    call_through_the_second_base();
    cast_back();
    build_a_diamond();
    share_the_base();

    return EXIT_SUCCESS;
}
