// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Forget `virtual` for a moment. A derived object is the bytes of its base, followed by its own members - the base
 *   is a sub-object at the start.
 * - So a pointer to the base part is the address of the object itself: converting a `ring*` to a `shape*` costs
 *   nothing.
 * - A non-virtual member function is chosen by the static type - the type the compiler sees - at compile time. A
 *   function of the same name in the derived class hides the one of the base; both exist.
 * - An empty base class takes no space - an empty member does.
 * - A copy into a base object takes the base part, and nothing else.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - In which order do the constructors of `shape` and `circle` run, in which order the destructors - and why must
 *   `circle` call `shape{name}`?
 * - `print_shape(c)` for a circle: which of `describe` and `area` gave the circle's answer, and why?
 * - Why does `const shape s{"s"};` not compile?
 * - What was `sizeof(shape)` in the layout snippet - and what did the layout dump show at offset 0?
 */

#include <iostream>
#include <memory>
#include <cstddef>                          // for ptrdiff_t
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ptrdiff_t, std::unique_ptr;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `shape`, `circle`, `ring`, `as_shape`, `id_of` and `describe_as_shape` are for Compiler Explorer, below - as in
 * previous snippets, they keep their own names in the assembly.
 */

/* --- `shape`, `circle` and `ring` ---
 * Three levels, no `virtual` anywhere: a ring is a circle with a hole, a circle is a shape. Structs with public data
 * members, so that we can print their addresses. `describe` exists twice - in `shape` and in `circle`.
 * - !![#inheritance]
 */
struct shape {
    explicit shape(const int i) : id{i} {}
    const char* describe() const { return "some shape"; }
    int id;
};

struct circle : shape {
    circle(const int i, const double r) : shape{i}, radius{r} {}
    const char* describe() const { return "a circle"; }
    double radius;
};

struct ring : circle {
    ring(const int i, const double r, const double in) : circle{i, r}, inner{in} {}
    double inner;
};

/* --- `as_shape`, `id_of` and `describe_as_shape` --- A conversion, a member of the base, and a call. */
const shape* as_shape(const ring* r) {
    return r;
}

int id_of(const ring& r) {
    return r.id;
}

const char* describe_as_shape(const ring& r) {
    const shape& s{r};
    return s.describe();
}

namespace {

    /* --- `offset_of` ---
     * How many bytes `member` lies behind the start of `object`. Two addresses as `const unsigned char*` - pointers to
     * bytes - and their difference counts bytes (see previous snippets).
     */
    ptrdiff_t offset_of(const void* member, const void* object) {
        return static_cast<const unsigned char*>(member) - static_cast<const unsigned char*>(object);
    }

    /* --- `show_the_layout` ---
     * A `ring` begins with a `circle`, which begins with a `shape`: `id` at offset 0, then 4 bytes of padding, then
     * `radius` at 8 - the members of `circle` - and then `inner` at 16, the member of `ring`. Every level appends its
     * members to the bytes of its base. Nothing else is in the object: no type, no name of the class, no pointer to the
     * base - `sizeof(ring)` is the sum of the members plus padding, as for a plain struct (see previous snippets).
     * `private` and `protected` would change nothing here: access is checked by the compiler, the machine does not know
     * it.
     */
    void show_the_layout() {
        print_function_header();

        const ring r{1, 2.0, 1.5};
        cout << " 1| sizeof(shape)=" << sizeof(shape) << ", sizeof(circle)=" << sizeof(circle) << ", sizeof(ring)="
             << sizeof(ring) << '\n';
        cout << " 2| &r=" << &r << ", offsets: id " << offset_of(&r.id, &r) << ", radius " << offset_of(&r.radius, &r)
             << ", inner " << offset_of(&r.inner, &r) << '\n';

        /* -- .Q&A -- !![Why not `offsetof(ring, radius)`, as for a struct in unit 0x02?](#a-802) */
    }

    /* --- `convert_to_the_base` ---
     * The `shape` part of `r` is at offset 0 - so a pointer to it is the address of `r`, and so is a pointer to its
     * `circle` part. Converting a pointer or a reference from a derived class to a base class - an upcast - happens
     * silently, and it costs nothing: the address stays what it was, only the type changes. See the machine code below.
     */
    void convert_to_the_base() {
        print_function_header();

        const ring r{2, 3.0, 2.0};
        const circle* c{&r};
        const shape* s{&r};
        const shape& ref{r};
        cout << " 1| &r=" << &r << ", c=" << c << ", s=" << s << ", &ref=" << &ref << '\n';
        cout << " 2| s->id=" << s->id << '\n';
        // cout << s->radius;               // compiler error: a `shape` has no `radius`
    }

    /* --- `call_by_the_static_type` ---
     * `describe` is not `virtual`, so the compiler decides which one to call, from the type it sees: `r` is a `ring` -
     * `ring` has no `describe` of its own, `circle` has one, that is it. `s` is a `const shape*`, so it is the one of
     * `shape` - although `s` points to the very same ring. The decision is made at compile time, and it is final.
     * `circle::describe` does not replace `shape::describe` - it hides it: both functions exist, and
     * `r.shape::describe()` calls the hidden one by its full name.
     */
    void call_by_the_static_type() {
        print_function_header();

        const ring r{3, 1.0, 0.5};
        const shape* s{&r};
        cout << " 1| r.describe()=" << r.describe() << ", s->describe()=" << s->describe() << '\n';
        cout << " 2| r.shape::describe()=" << r.shape::describe() << '\n';

        /* -- .Q&A -- !![`shape` gets a second `describe(int)`. Can you call `r.describe(1)`?](#a-803) */
    }

}

/* --- The machine code of an upcast and a non-virtual call ---
 * Paste `shape`, `circle`, `ring` and the three functions after them into Compiler Explorer, x86-64 gcc.
 * - `as_shape`, `-O2`: `mov rax, rdi` - the pointer goes back as it came. The conversion is no instruction at all.
 * - `id_of`, `-O2`: `mov eax, DWORD PTR [rdi]` - the `id` of the base is at offset 0 of the ring.
 * - `describe_as_shape`, `-O0`: `call shape::describe() const`, with the address of the ring in `rdi` - that is `this`.
 *   The address of the function is written in the instruction: nothing is looked up at run time.
 * - `describe_as_shape`, `-O2`: `lea rax, .LC0[rip]` - the call is inlined, and the address of the text "some shape"
 *   is all that is left.
 * A derived class without `virtual` costs exactly what its members cost.
 * - !![#zero-overhead]
 */

namespace {

    /* --- `empty` and two counters ---
     * `empty` has no data members. `counter_with_member` has one as a member, `counter_with_base` derives from it.
     */
    struct empty {};

    struct counter_with_member {
        empty policy;
        int count;
    };

    struct counter_with_base : empty {
        int count;
    };

    /* --- `show_the_empty_base` ---
     * An object takes at least 1 byte - two objects must have two addresses (see previous snippets). As a member,
     * `empty` gets its byte, and the `int` after it 3 bytes of padding: 8 bytes. As a base class, it may take no space
     * at all: its sub-object shares the address of the counter, and `sizeof` is 4 - the empty base optimization.
     * The standard library uses it: `unique_ptr<int>` holds a pointer and a deleter, `std::default_delete<int>` - an
     * empty class. Stored so that it takes no space - as a base, or with `[[no_unique_address]]` (see previous
     * snippets) - the deleter costs nothing, and `sizeof(unique_ptr<int>)` is 8.
     */
    void show_the_empty_base() {
        print_function_header();

        const counter_with_base c{};
        const empty& e{c};
        cout << " 1| sizeof(empty)=" << sizeof(empty) << ", sizeof(counter_with_member)=" << sizeof(counter_with_member)
             << ", sizeof(counter_with_base)=" << sizeof(counter_with_base) << '\n';
        cout << " 2| &c=" << &c << ", &e=" << &e << ", &c.count=" << &c.count << '\n';
        cout << " 3| sizeof(unique_ptr<int>)=" << sizeof(unique_ptr<int>) << '\n';
    }

    /* --- `name_of_a_copy` --- Takes a shape by value - a copy. */
    const char* name_of_a_copy(const shape s) {
        return s.describe();
    }

    /* --- `slice_a_copy` ---
     * `const shape copy{r};` makes a `shape` from a ring: the copy constructor of `shape` copies the `shape` part - the
     * first 4 bytes - and nothing else. `radius` and `inner` do not fit into a `shape`; they are cut off - slicing. The
     * same happens when a ring is passed by value to a function that takes a `shape`. Here, nothing is lost that a
     * `shape` could hold. With `virtual`, it gets more interesting - see future snippets.
     * - !![#slicing]
     */
    void slice_a_copy() {
        print_function_header();

        const ring r{4, 2.0, 1.0};
        const shape copy{r};
        cout << " 1| sizeof(copy)=" << sizeof(copy) << ", copy.id=" << copy.id << ", copy.describe()="
             << copy.describe() << '\n';
        cout << " 2| name_of_a_copy(r)=" << name_of_a_copy(r) << ", &r=" << &r << ", &copy=" << &copy << '\n';
    }

}

/* --- `main` --- */
int main() {
    show_the_layout();
    convert_to_the_base();
    call_by_the_static_type();
    show_the_empty_base();
    slice_a_copy();

    return EXIT_SUCCESS;
}
