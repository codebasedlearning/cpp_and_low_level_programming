// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
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
#include <vector>
#include <memory>
#include <cstring>
#include <cstddef>
#include <random>                           // for mt19937, bernoulli_distribution
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::vector, std::unique_ptr, std::make_unique, std::ptrdiff_t, std::size_t;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `shape`, `circle`, `square` and the functions up to `area_of_circle` are for `nm` and Compiler Explorer, below - as
 * in previous snippets, they keep their own names in the object file.
 */

/* --- `plain_shape` --- A shape without `virtual`: one `int`, one member function. */
struct plain_shape {
    double area() const { return 0.0; }
    int id;
};

/* --- `shape` ---
 * The shape of the preparation, with a number instead of a name. `id` is public only to show its address below -
 * normally it would be private.
 */
class shape {
public:
    explicit shape(const int i) : id{i} {}
    virtual ~shape() = default;

    virtual double area() const = 0;
    virtual const char* name() const { return "shape"; }

    int id;
};

/* --- `circle` and `square` ---
 * Two shapes. `square` is `final`: no class can derive from it - so a `square` is always exactly a `square`, see
 * `area_of_square`.
 * - !![#override]
 */
class circle : public shape {
public:
    circle(const int i, const double radius) : shape{i}, radius_{radius} {}

    double area() const override { return 3.14159 * radius_ * radius_; }
    const char* name() const override { return "circle"; }

private:
    double radius_;
};

class square final : public shape {
public:
    square(const int i, const double side) : shape{i}, side_{side} {}

    double area() const override { return side_ * side_; }
    const char* name() const override { return "square"; }

private:
    double side_;
};

/* --- `area_of`, `area_of_square` and `area_of_circle` --- The same call, through three types. */
double area_of(const shape& s) {
    return s.area();
}

double area_of_square(const square& q) {
    return q.area();
}

double area_of_circle(const circle& c) {
    return c.area();
}

namespace {

    /* --- `offset_of` --- As in the previous snippet: the distance in bytes. */
    ptrdiff_t offset_of(const void* member, const void* object) {
        return static_cast<const unsigned char*>(member) - static_cast<const unsigned char*>(object);
    }

    /* --- `vptr_of` ---
     * The first 8 bytes of a shape, as an address - the vptr. That is where gcc, clang and MSVC put it; the standard
     * does not even say that there is one. `memcpy` copies the bytes into a pointer - reading them through a
     * `const void* const*` would break the aliasing rule (see previous snippets). clang warns that "the vtable pointer
     * will be copied" - which is what we want; the cast to `const void*` says so.
     */
    const void* vptr_of(const shape& s) {
        const void* vptr{nullptr};
        std::memcpy(&vptr, static_cast<const void*>(&s), sizeof vptr);
        return vptr;
    }

    /* --- `show_the_hidden_pointer` ---
     * `plain_shape` is its `int`: 4 bytes. `shape` has the same `int` - and 16 bytes: 8 for a pointer the compiler
     * added, 4 for `id`, 4 of padding, so that the next shape in an array starts at a multiple of 8. `id` is at offset
     * 8: the pointer is in front of it, at offset 0 - the layout dump of the preparation showed it.
     * A `circle` adds its `double`, 24 bytes. It has no second pointer: it shares the one of its `shape` part.
     * - !![#vtable]
     */
    void show_the_hidden_pointer() {
        print_function_header();

        const circle c{1, 2.0};
        cout << " 1| sizeof(plain_shape)=" << sizeof(plain_shape) << ", sizeof(shape)=" << sizeof(shape)
             << ", sizeof(circle)=" << sizeof(circle) << '\n';
        cout << " 2| &c=" << &c << ", offset of id " << offset_of(&c.id, &c) << '\n';

        /* -- .Q&A -- !![Ten virtual functions instead of three - how big is a `shape` then?](#a-804) */
    }

    /* --- `global_value` --- Outside of any function: static storage, see previous snippets. */
    int global_value{1};

    /* --- `show_the_vptr` ---
     * Two circles, one vptr: both point to the same table - the one of `circle`. The square points to another one. The
     * table is not in the object, and not on the heap or the stack: its address is near `global_value`, in static
     * storage. One table per class, made by the compiler, there from the start of the program to its end.
     * That is all a circle knows about its class: 8 bytes, the address of a table. Nothing else in its bytes says
     * "circle".
     */
    void show_the_vptr() {
        print_function_header();

        const circle c1{1, 1.0};
        const circle c2{2, 2.0};
        const square q{3, 3.0};
        cout << " 1| vptr: c1=" << vptr_of(c1) << ", c2=" << vptr_of(c2) << ", q=" << vptr_of(q) << '\n';

        const int local{0};
        const unique_ptr<int> on_the_heap{make_unique<int>(0)};
        cout << " 2| &global_value=" << &global_value << ", &local=" << &local << ", heap: " << on_the_heap.get()
             << '\n';
    }

}

/* --- What is in the table ---
 * The vtable dump of the preparation showed six entries for `circle`: the offset to the top, the type information
 * (RTTI - run-time type information), the destructor twice, `area`, `name`. The vptr points to the third entry, so from
 * there, `[0]` and `[1]` are the destructors, `[2]` is `area` - 16 bytes into the functions - and `[3]` is `name`. The
 * two entries in front of it are used by `typeid` and `dynamic_cast`, see future snippets.
 * `nm -C` on the object file of this snippet lists them by name: `vtable for circle`, `vtable for square` and
 * `vtable for shape`, and `typeinfo for circle` with `typeinfo name for circle` - the type information the table points
 * to. `shape` has a table, too, although there are no `shape` objects: every circle is a shape while it is being
 * constructed - see future snippets. Its slot for `area` holds `__cxa_pure_virtual`, which `nm` lists as well.
 * On Linux, the tables are in `.data.rel.ro`: written once, when the program is loaded - the addresses of the functions
 * are known only then - and read-only afterwards.
 */

namespace {

    /* --- `call_through_the_table` ---
     * `area_of` knows nothing but a `const shape&`, and it gets the `area` of a circle and of a square. See below how.
     */
    void call_through_the_table() {
        print_function_header();

        const circle c{1, 1.0};
        const square q{2, 3.0};
        cout << " 1| area_of(c)=" << area_of(c) << ", area_of(q)=" << area_of(q) << '\n';

        /* -- .Q&A -- !![Why is `area` in slot 2 of the table, and not in slot 0?](#a-805) */
    }

}

/* --- The machine code of a virtual call ---
 * Paste everything from `shape` to the end of `area_of_circle` into Compiler Explorer, x86-64 gcc, `-O2`:
 *     area_of(shape const&):
 *         mov     rax, QWORD PTR [rdi]
 *         jmp     [QWORD PTR [rax+16]]
 * `rdi` holds the address of the shape. The first `mov` loads the vptr from offset 0 of the object. The `jmp` loads
 * slot 2 - 16 bytes behind where the vptr points - and jumps to the address it finds there: `circle::area` or
 * `square::area`. A `jmp`, not a `call`, because the call is the last thing `area_of` does: `area` returns directly to
 * the caller of `area_of`, with the result in `xmm0`. clang: the same two instructions. ARM64 gcc:
 *     ldr     x1, [x0]
 *     ldr     x1, [x1, 16]
 *     mov     x16, x1
 *     br      x16
 * Two loads, a jump to a register. With `-O0`, the same loads, and a `call rdx` - an indirect call, through a register.
 * Compare with the non-virtual call of the previous snippet: `call shape::describe() const` - there, the address of the
 * function is in the instruction.
 * And compare with the `switch` over an enum (see previous snippets): there, the table of addresses belongs to the
 * function, and the number that selects the entry comes from the enum. Here, the table belongs to the class - and the
 * object brings the address of its table along.
 * - !![#virtual]
 */

namespace {

    /* --- `devirtualize` ---
     * When the compiler knows the exact type, it does not need the table.
     * - `area_of_square`: `square` is `final` - a `const square&` refers to a square, never to something derived from
     *   it. `-O2` makes it `movsd`, `mulsd`, `ret`: `square::area`, inlined. No vptr is read.
     * - `area_of_circle`: `circle` is not `final` - a class derived from `circle` might override `area`, so clang makes
     *   the virtual call. gcc guesses: it loads slot 2, compares it with the address of `circle::area` - if equal, it
     *   runs the inlined multiplication, otherwise it jumps. Speculative devirtualization.
     * - `c.circle::area()`: with the class name in front, a call is never virtual - you ask for exactly that function.
     * - `local.area()` for a local `circle`: the compiler constructed `local` itself and knows its type - a direct
     *   call, as Release inlined.
     */
    void devirtualize() {
        print_function_header();

        const circle local{1, 2.0};
        const square q{2, 2.0};
        cout << " 1| local.area()=" << local.area() << ", local.circle::area()=" << local.circle::area()
             << ", area_of_square(q)=" << area_of_square(q) << ", area_of_circle(local)=" << area_of_circle(local)
             << '\n';

        /* -- .Q&A -- !![Why is a virtual call "in the way of inlining" - what is lost besides the jump?](#a-806) */
    }

    /* --- `sum_direct` --- The areas of circles, by name - no virtual call. */
    double sum_direct(const vector<circle>& circles) {
        double sum{0.0};
        for (const circle& c : circles) {
            sum += c.circle::area();
        }
        return sum;
    }

    /* --- `sum_virtual` --- The areas of shapes, through the table. */
    double sum_virtual(const vector<const shape*>& shapes) {
        double sum{0.0};
        for (const shape* s : shapes) {
            sum += s->area();
        }
        return sum;
    }

    /* --- `measure_the_jump` ---
     * A million circles and a million squares, and four sums:
     * - `direct`: the circles, by name - no table.
     * - `one type`: the same circles, through `const shape*` - a million virtual calls, and every one jumps to
     *   `circle::area`.
     * - `mixed`: a million pointers, each one to a circle or a square, at random - as in a real program.
     * - `sorted`: the same pointers as `mixed` - the same objects, the same calls - but first all circles, then all
     *   squares.
     * Run it as Release. `direct` and `one type` are about the same: when the jump goes to the same place every time,
     * the processor predicts it, and it costs almost nothing. `mixed` takes much longer: the processor must guess where
     * each jump goes before the table is even loaded - and with random types, it guesses wrong half of the time. Every
     * wrong guess throws away work already started. `sorted` makes the same calls in another order - and is fast again.
     * (On some machines still a little slower than `one type`: it reads the objects from two vectors instead of one.)
     * So the jump per call is cheap - as long as it is predictable.
     * - !![#branch-prediction]
     */
    void measure_the_jump() {
        print_function_header();

        constexpr int n{1'000'000};
        vector<circle> circles;
        vector<square> squares;
        circles.reserve(n);
        squares.reserve(n);
        for (int i{0}; i < n; ++i) {
            circles.emplace_back(i, 1.0);
            squares.emplace_back(i, 2.0);
        }

        vector<const shape*> one_type;
        vector<const shape*> mixed;
        vector<const shape*> sorted;
        vector<const shape*> squares_later;
        std::mt19937 generator{23};
        std::bernoulli_distribution coin{0.5};
        for (size_t i{0}; i < circles.size(); ++i) {
            one_type.push_back(&circles[i]);
            if (coin(generator)) {
                mixed.push_back(&circles[i]);
                sorted.push_back(&circles[i]);
            } else {
                mixed.push_back(&squares[i]);
                squares_later.push_back(&squares[i]);
            }
        }
        sorted.insert(sorted.end(), squares_later.begin(), squares_later.end());

        stopwatch watch{};
        const double direct{sum_direct(circles)};
        const double direct_ms{watch.elapsed_ms()};
        watch.reset();
        const double virtual_one_type{sum_virtual(one_type)};
        const double one_type_ms{watch.elapsed_ms()};
        watch.reset();
        const double virtual_mixed{sum_virtual(mixed)};
        const double mixed_ms{watch.elapsed_ms()};
        watch.reset();
        const double virtual_sorted{sum_virtual(sorted)};
        const double sorted_ms{watch.elapsed_ms()};

        cout << " 1| direct:   " << direct_ms << " ms, sum=" << direct << '\n';
        cout << " 2| one type: " << one_type_ms << " ms, sum=" << virtual_one_type << '\n';
        cout << " 3| mixed:    " << mixed_ms << " ms, sum=" << virtual_mixed << '\n';
        cout << " 4| sorted:   " << sorted_ms << " ms, sum=" << virtual_sorted << '\n';

        /* -- .Q&A -- !![`mixed` and `sorted` load the same vptrs and the same slots. Why is one slower?](#a-807) */
    }

}

/* --- `main` --- */
int main() {
    show_the_hidden_pointer();
    show_the_vptr();
    call_through_the_table();
    devirtualize();
    measure_the_jump();

    return EXIT_SUCCESS;
}
