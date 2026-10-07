// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The vptr is the dynamic type of an object - and the constructors write it. Each constructor writes the address of
 *   the table of its own class, the base first. While the base constructor runs, the object is a base.
 * - So a virtual call in a constructor reaches the version of the class being constructed, not the override - unlike
 *   Java. A pure virtual one aborts the program. The destructors write the vptr back, in reverse.
 * - A virtual destructor has two slots; `delete` through a base pointer calls the one that destroys and frees. Without
 *   a virtual destructor, the derived destructor never runs.
 * - A copy into a base object gets the vptr of the base - its constructor writes it. Slicing makes a base, never half
 *   a derived object.
 * - `typeid` reads the type information through the vptr - for a class with virtual functions. For all others, it is
 *   the static type.
 * - Every line with undefined behavior below is commented out. Remove the `//` one at a time, run it as Debug and as
 *   Release - and put the `//` back.
 */

#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <typeinfo>                         // for typeid, type_info
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

namespace {

    /* --- `vptr_of` ---
     * The first 8 bytes of an object, as an address - as in the previous snippet, but for any object, so that the
     * constructors below can pass `this`.
     */
    const void* vptr_of(const void* object) {
        const void* vptr{nullptr};
        std::memcpy(&vptr, object, sizeof vptr);
        return vptr;
    }

    /* --- `shape` and `circle` ---
     * Both constructors and both destructors print the vptr of the object - and the result of the virtual call
     * `name()`. `shape` is not abstract here: `name` has a body.
     */
    class shape {
    public:
        explicit shape(const int i) : id_{i} {
            cout << " a|   shape(): vptr=" << vptr_of(this) << ", name()=" << name() << '\n';
        }

        virtual ~shape() {
            cout << " d|   ~shape(): vptr=" << vptr_of(this) << ", name()=" << name() << '\n';
        }

        virtual const char* name() const { return "shape"; }
        int id() const { return id_; }

    private:
        int id_;
    };

    class circle : public shape {
    public:
        circle(const int i, const double radius) : shape{i}, radius_{radius} {
            cout << " b|   circle(): vptr=" << vptr_of(this) << ", name()=" << name() << '\n';
        }

        ~circle() override {
            cout << " c|   ~circle(): vptr=" << vptr_of(this) << ", name()=" << name() << '\n';
        }

        const char* name() const override { return "circle"; }
        double radius() const { return radius_; }

    private:
        double radius_;
    };

    /* --- `watch_the_vptr_being_written` ---
     * One circle, two vptrs over its lifetime. The constructor of `shape` runs first. Before its body - before `id_`
     * even - it writes the address of the table of `shape` into the object: while `shape()` runs, the object is a
     * shape. The circle part does not exist yet - `radius_` is not initialized - so a virtual call must not reach
     * `circle::name`, and it does not. Here, the compiler calls `shape::name` directly - it knows what the object is at
     * this point. A function that does not know, and calls through the table, gets the same answer: the table is the
     * one of `shape`. Then the constructor of `circle` writes the address of its own table - from now on, the object is
     * a circle.
     * The destructors run the other way round: `~circle` finds its own table, `~shape` writes the table of `shape`
     * back - the circle part is already destroyed.
     * The machine code of the constructor, below, shows the write.
     */
    void watch_the_vptr_being_written() {
        print_function_header();

        {
            const circle c{1, 2.0};
            cout << " 1| c: vptr=" << vptr_of(&c) << ", name()=" << c.name() << '\n';
        }
        cout << " 2| gone\n";

        /* -- .Q&A -- !![In Java, the base constructor would reach the override. What would it see?](#a-808) */
    }

}

/* --- The machine code of a constructor ---
 * In Compiler Explorer, x86-64 gcc, `-O0`, the constructor of a `circle` (without the `cout` lines) is:
 *     call    shape::shape(int)
 *     lea     rdx, vtable for circle[rip+16]
 *     mov     QWORD PTR [rax], rdx
 * then the `double` for `radius_`. First the base, which writes its own vptr, then the vptr of `circle` - 16 bytes into
 * its table - to offset 0 of the object, then the members. As Release, when `new circle{...}` is inlined, the compiler
 * drops the first write: nobody can see the object in between - here, the `cout` in the constructor could.
 */

namespace {

    /* --- `task` and `backup` ---
     * `task` calls `start()` in its constructor, and `start` calls the pure virtual `title()` - a virtual call in a
     * constructor, one step removed.
     */
    class task {
    public:
        task() { start(); }
        virtual ~task() = default;

        void start() const { cout << " a|   starting " << title() << '\n'; }
        virtual const char* title() const = 0;
    };

    class backup : public task {
    public:
        const char* title() const override { return "backup"; }
    };

    /* --- `try_a_pure_virtual_call` ---
     * While `task()` runs, the vptr points to the table of `task` - and its slot for `title` holds
     * `__cxa_pure_virtual` (see previous snippet): a function of the runtime library that prints a message -
     * `pure virtual method called`, with gcc's library - and calls `std::terminate`. Exit status 134, as for a
     * destructor that throws (see previous snippets).
     * gcc and clang warn about a pure virtual call right in a constructor. One step removed, through `start`, they see
     * nothing.
     */
    void try_a_pure_virtual_call() {
        print_function_header();

        // const backup b;                  // undefined behavior: pure virtual call - the program aborts
        cout << " 1| no backup started\n";
    }

    /* --- `delete_through_the_base` ---
     * `p` is a `unique_ptr<shape>` - at its `}`, it calls `delete` on a `shape*`. `delete` for a class with a virtual
     * destructor is a virtual call: slot 1 of the table, the deleting destructor. It runs `~circle` - which runs
     * `~shape` - and then gives the 24 bytes of the circle back to `operator delete`. Slot 0 is the complete
     * destructor: it destroys, but does not free - for circles that are not on the heap. In Compiler Explorer,
     * `void delete_it(const shape* p) { delete p; }` is, with `-O2`: `test rdi, rdi` - `delete` of a null pointer does
     * nothing - then `mov rax, QWORD PTR [rdi]`, `jmp [QWORD PTR [rax+8]]`: slot 1.
     * - !![#virtual-destructor]
     */
    void delete_through_the_base() {
        print_function_header();

        const heap_watch heap{};
        {
            const unique_ptr<shape> p{make_unique<circle>(3, 1.0)};
            cout << " 1| p->name()=" << p->name() << '\n';
        }
        cout << " 2| allocations=" << heap.allocations() << ", releases=" << heap.releases() << '\n';

        /* -- .Q&A -- !![Why does a `shared_ptr<shape>` destroy a circle even without a virtual `~shape`?](#a-809) */
    }

    /* --- `record` and `named_record` ---
     * No virtual function, no virtual destructor. `named_record` adds a `string` long enough to need the heap (see
     * previous snippets).
     */
    struct record {
        int id{0};
    };

    struct named_record : record {
        string name{"a name that is too long for the small buffer"};
    };

    /* --- `delete_without_a_virtual_destructor` ---
     * `delete r` through a `record*`: `record` has no vptr, so `delete` cannot find out what `r` points to. It calls
     * `~record` - the static type - and never `~named_record`: the `string` is never destroyed, and its block is lost.
     * Remove the `//`: `heap_watch` counts 2 allocations - the object and the characters of the name - and 1 release.
     * And that is only what happened here: the standard calls it undefined behavior.
     * gcc and clang warn about a `delete` like this (`-Wdelete-non-virtual-dtor`, in `-Wall`) - but only for a class
     * with virtual functions. `record` has none, so here: not a word.
     */
    void delete_without_a_virtual_destructor() {
        print_function_header();

        const heap_watch heap{};
        // const record* r{new named_record{}};
        // delete r;                        // undefined behavior: `record` has no virtual destructor
        cout << " 1| allocations=" << heap.allocations() << ", releases=" << heap.releases() << '\n';
    }

    /* --- `slice_a_circle` ---
     * `const shape copy{c};` - the copy constructor of `shape`, generated by the compiler, copies `id_`. And, as every
     * constructor of `shape`, it writes the vptr of `shape`: the vptr is not a member that is copied, it is written by
     * the constructor - it says which constructor made the object. So `copy` is a shape through and through, and
     * `copy.name()` is "shape". `ref` is no copy: it refers to the circle, and has its vptr.
     * Slicing never makes half a circle - it makes a shape. Which is exactly what you asked for - and almost never what
     * you wanted.
     * - !![#slicing]
     */
    void slice_a_circle() {
        print_function_header();

        const circle c{4, 1.0};
        const shape copy{c};
        const shape& ref{c};
        cout << " 1| c: vptr=" << vptr_of(&c) << ", copy: vptr=" << vptr_of(&copy) << ", ref: vptr=" << vptr_of(&ref)
             << '\n';
        cout << " 2| copy.name()=" << copy.name() << ", ref.name()=" << ref.name() << '\n';
    }

    /* --- `ask_for_the_type` ---
     * `typeid(ref)` for a `const shape&` that refers to a circle is `typeid(circle)`: the type of the object, found at
     * run time - through the vptr, in the entry in front of the slots, where the vtable dump showed `circle RTTI`. In
     * Compiler Explorer, `const std::type_info& type_of(const shape& s) { return typeid(s); }` is two loads:
     * `mov rax, QWORD PTR [rdi]`, `mov rax, QWORD PTR -8[rax]`.
     * A `record&` that refers to a `named_record` has no vptr to ask: `typeid` gives the static type, `record` - at
     * compile time. And the sliced copy is a shape, as `typeid` confirms.
     * - !![#rtti]
     */
    void ask_for_the_type() {
        print_function_header();

        const circle c{5, 1.0};
        const shape& ref{c};
        const shape copy{c};
        cout << " 1| typeid(ref) == typeid(circle): " << (typeid(ref) == typeid(circle))
             << ", typeid(copy) == typeid(shape): " << (typeid(copy) == typeid(shape)) << '\n';

        const named_record n{};
        const record& r{n};
        cout << " 2| typeid(r) == typeid(record): " << (typeid(r) == typeid(record)) << '\n';
        cout << " 3| typeid(ref).name()=" << typeid(ref).name() << ", typeid(r).name()=" << typeid(r).name() << '\n';

        /* -- .Q&A -- !![`N12_GLOBAL__N_16circleE` - what kind of name is that?](#a-810) */
    }

}

/* --- `main` --- */
int main() {
    watch_the_vptr_being_written();
    try_a_pure_virtual_call();
    delete_through_the_base();
    delete_without_a_virtual_destructor();
    slice_a_circle();
    ask_for_the_type();

    return EXIT_SUCCESS;
}
