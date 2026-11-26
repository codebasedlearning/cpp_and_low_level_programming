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
#include <string_view>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ptrdiff_t, std::string_view;


/* ---- Content ---- */

namespace {

    /* --- `printable` and `movable` ---
     * `printable` and `movable` have no data, only a vptr each: 8 bytes. A `sprite` contains both - its `printable`
     * part at offset 0, its `movable` part at offset 8 - and then its two `int`s: 24 bytes, two vptrs. The layout dump
     * shows `(printable vtable pointer)` at 0 and `(movable vtable pointer)` at 8.
     * The first base starts where the sprite starts, as in single inheritance. The second one cannot: a pointer to the
     * `movable` part of `s` is not the address of `s`.
     *
     * Two interfaces: pure virtual functions, a virtual destructor, no data (see the follow-up). A class may implement
     * both - multiple inheritance of interfaces, the common and harmless case.
     * - !![#multiple-inheritance]
     *
     * There are no instances of these classes, just showing the multiple interfaces implementations.
     */

    class printable {
    public:
        virtual ~printable() = default;
        virtual string_view text() const = 0;
    };

    class movable {
    public:
        virtual ~movable() = default;
        virtual void move_by(int dx, int dy) = 0;
    };

    /* --- .`sprite` --- Something on a screen: it can be printed and moved. */
    class sprite : public printable, public movable {
    public:
        sprite(const int x, const int y) : x_{x}, y_{y} {}

        string_view text() const override { return "sprite"; }

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

    /* --- .`cloud` --- Another `movable`, but no sprite. */
    class cloud : public movable {
    public:
        void move_by(const int dx, int) override { drift_ += dx; }

    private:
        int drift_{0};
    };


    /* --- .`offset_of` --- As in the previous snippets: the distance in bytes. */
    ptrdiff_t offset_of(const void* part, const void* object) {
        return static_cast<const unsigned char*>(part) - static_cast<const unsigned char*>(object);
    }

    /* --- .`device`, `scanner`, `printer` and `copier` ---
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
        copier(const int s, const int d, const int p) : scanner{s+1, d}, printer{s+2, p} {}
    };

    /* --- `is_it_a_diamond` ---
     * A copier contains a whole scanner and a whole printer - each with its own `device`: two serial numbers, 7 and
     * 8, at offsets 0 and 8. `c.serial` does not compile: which one? The full names say it, `c.scanner::serial`.
     * Sometimes two parts are what you want. For a serial number, they are not.
     */
    void is_it_a_diamond() {
        print_function_header();

        cout << " 1| sizeof(device)=" << sizeof(device)
             << ", sizeof(scanner)=" << sizeof(scanner)
             << ", sizeof(printer)=" << sizeof(printer)
             << ", sizeof(copier)=" << sizeof(copier) << '\n';

        const copier c{23,300,5};
        const scanner& s{c};
        const printer& p{c};
        cout << " 2| &c=" << &c << ", &s=" << &s << ", &p=" << &p << '\n';

        cout << " 3| c.scanner::serial=" << c.scanner::serial
             << ", s.serial=" << s.serial
             << " at offset " << offset_of(&s.serial, &c) << '\n';
        cout << " 4| c.printer::serial=" << c.printer::serial
             << ", s.serial=" << p.serial
             << " at offset " << offset_of(&p.serial, &c) << '\n';
        // cout << c.serial;                // compiler error: `serial` is ambiguous
    }

    /* --- .`scanner_v`, `printer_v` and `copier_v` ---
     * The same diamond, with `virtual` inheritance: `scanner_v` and `printer_v` share their `device` with every other
     * class in the object that inherits it virtually. The most derived class - the one that is actually
     * created - constructs the virtual base: `copier_v` calls `device{7}`, and the `device{1}` and `device{2}`
     * of `scanner_v` and `printer_v` are ignored.
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
        copier_v(const int s, const int d, const int p) : device{s}, scanner_v{-1, d}, printer_v{-2, p} {}
    };

    /* --- `it_is_a_diamond` ---
     * One serial number. But `sizeof(scanner_v)` is 16, not 8: a `scanner_v` has a vptr - without a single virtual
     * function. Look at the offsets: in the copier, 28 bytes behind the start of its `scanner_v` part. The distance
     * depends on the object the scanner is part of - it is in the table.
     * Virtual inheritance is rare - the standard library has one: `std::iostream` derives from `std::istream` and
     * `std::ostream`, which share a virtual base.
     *
     * Layout scanner_v, serial part at the end
     * ---
     * offset  0  ┌ scanner_v part:  vptr               8
     * offset  8  │                  int dpi            4
     * offset 12  │                  (padding)          4   the next part needs an 8-aligned vptr
     * offset 16  ├ printer_v part:  vptr               8
     * offset 24  │                  int pages_per_min  4
     * offset 28  └ device:          int serial         4   one device, shared, at the very end
     */
    void it_is_a_diamond() {
        print_function_header();

        cout << " 1| sizeof(device)=" << sizeof(device)
             << ", sizeof(scanner_v)=" << sizeof(scanner_v)
             << ", sizeof(printer_v)=" << sizeof(printer_v)
             << ", sizeof(copier_v)=" << sizeof(copier_v) << '\n';

        const copier_v c{23,300,5};
        const scanner_v& s{c};
        const printer_v& p{c};
        cout << " 2| &c=" << &c << ", &s=" << &s << ", &p=" << &p << '\n';

        cout << " 3| c.scanner_v::serial=" << c.scanner_v::serial   // here c.serial is ok
             << ", s.serial=" << s.serial
             << " at offset " << offset_of(&s.serial, &c) << '\n';
        cout << " 4| c.printer_v::serial=" << c.printer_v::serial
             << ", s.serial=" << p.serial
             << " at offset " << offset_of(&p.serial, &c) << '\n';

        /* -- .Q&A -- !![Why is the offset of the virtual base in the table, and not in the object?](#a-812) */
    }
}

/* --- `main` --- */
int main() {
    is_it_a_diamond();
    it_is_a_diamond();

    return EXIT_SUCCESS;
}
