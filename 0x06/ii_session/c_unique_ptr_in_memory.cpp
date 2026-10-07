// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `unique_ptr` is the owner as a type: it holds the address, and its destructor deletes the object.
 * - It is as big as a raw pointer, and the machine code is the same - plus the `delete` on the exception path, the
 *   one the raw version forgets.
 * - It cannot be copied: two owners would delete twice - the compiler refuses.
 * - It can be moved: `std::move` hands the ownership on. 8 bytes are copied, a null is left behind, nothing is
 *   allocated.
 * - A function can return a `unique_ptr` - an object that outlives the function, with an owner.
 * - Its price: a class with a destructor is never passed in a register. By value it travels in memory, and the caller
 *   destroys the empty shell after the call.
 * - To use an object without owning it, pass `T&` - or `T*` if there may be none.
 */

#include <iostream>
#include <string>
#include <memory>
#include <utility>                          // for move
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::unique_ptr, std::make_unique, std::runtime_error;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `use`, `raw_owner` and `smart_owner` are for Compiler Explorer, below - as in previous snippets, they stay under
 * their own names in the assembly.
 */

/* --- `use` --- Uses an `int` it gets by address - and could throw, as far as a caller knows. */
void use(const int* p) {
    cout << " a|   use: *p=" << *p << '\n';
}

/* --- `raw_owner` and `smart_owner` --- The same work, with a raw pointer and with a `unique_ptr`. */
void raw_owner() {
    int* p{new int{42}};
    use(p);
    delete p;
}

void smart_owner() {
    const unique_ptr<int> p{make_unique<int>(42)};
    use(p.get());
}

namespace {

    /* --- `tracer` --- Reports its birth and its death. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " b|   " << name_ << ": constructed at " << this << '\n';
        }

        ~tracer() {
            cout << " c|   " << name_ << ": destroyed at " << this << '\n';
        }

        const string& name() const { return name_; }

    private:
        string name_;
    };

    /* --- `show_the_size` ---
     * A `unique_ptr` is a small object on the stack that holds the address of an object on the heap - nothing else:
     * 8 bytes, the size of a raw pointer. `get()` returns that address. The owning costs no memory.
     * - !![#unique-ptr]
     */
    void show_the_size() {
        print_function_header();

        const unique_ptr<tracer> u{make_unique<tracer>("u")};
        cout << " 1| sizeof(tracer*)=" << sizeof(tracer*) << ", sizeof(unique_ptr<tracer>)="
             << sizeof(unique_ptr<tracer>) << '\n';
        cout << " 2| &u=" << &u << " (stack), u.get()=" << u.get() << " (heap)\n";
    }

    /* --- `read_config` --- Something that can go wrong, and does. */
    void read_config() {
        throw runtime_error{"config not found"};
    }

    /* --- `own_on_the_way_out` ---
     * The leak of the previous snippet, with an owner. Stack unwinding destroys `t` on the way to the `catch` - and
     * now `t` is an object with a destructor, and the destructor deletes the `tracer`. The ` c|` line comes before
     * `caught`, and nothing is left.
     */
    void own_on_the_way_out() {
        print_function_header();

        const heap_watch heap{};
        try {
            const unique_ptr<tracer> t{make_unique<tracer>("t")};
            read_config();
        } catch (const runtime_error& e) {
            cout << " 1| caught: " << e.what() << '\n';
        }
        cout << " 2| live=" << heap.live() << '\n';
    }

    /* --- `compare_the_machine_code` ---
     * Both functions do the same. Which one is slower? Look before you guess - see below.
     */
    void compare_the_machine_code() {
        print_function_header();

        raw_owner();
        smart_owner();
    }

}

/* --- The machine code of an owner ---
 * Paste `raw_owner` and `smart_owner` into Compiler Explorer, with `#include <memory>` and
 * `using std::unique_ptr, std::make_unique;` in front, and only a declaration of `use`: `void use(const int* p);` -
 * so that the compiler cannot look into it. Compile with x86-64 gcc, `-O2`.
 * - Both start the same: `mov edi, 4`, `call operator new(unsigned long)`, `mov DWORD PTR [rax], 42`, then
 *   `call use(int const*)`, then `operator delete(void*, unsigned long)` with the size, 4. The `unique_ptr` has
 *   disappeared - there is only the address, in a register (and a copy of it in the frame, for the case below).
 * - `smart_owner` has one more part, marked `[clone .cold]`: it runs only if `use` throws. It calls the destructor
 *   of the `unique_ptr` - the `delete` - and then `_Unwind_Resume`, which continues the unwinding. clang does the
 *   same without a separate name.
 * - `raw_owner` has no such part: if `use` throws, the `int` leaks.
 * So the only extra instructions are the ones the raw version is missing. That is zero overhead - you pay for what you
 * use, and here you use the cleanup.
 * - !![#zero-overhead]
 */

namespace {

    /* --- `try_to_copy` ---
     * Remove the `//`: `use of deleted function` - the copy constructor of `unique_ptr` is `= delete` (see previous
     * snippets). A copy would be a second owner of the same object, and at the `}` both would delete it: the
     * `naive_buffer` of the previous snippet, prevented by the compiler.
     */
    void try_to_copy() {
        print_function_header();

        const unique_ptr<tracer> u{make_unique<tracer>("u")};
        // const unique_ptr<tracer> v{u};   // compiler error: a `unique_ptr` cannot be copied
        cout << " 1| one owner: " << u->name() << '\n';
    }

    /* --- `hand_it_on` ---
     * `std::move(u)` says: take it, I do not need it any more. Then the move constructor of `unique_ptr` does the work
     * - it copies the address into `v` and sets `u` to `nullptr`. One owner before, one owner after, no allocation, and
     * the `tracer` stays where it is. An empty `unique_ptr` tests as `false`.
     * The move assignment `w = std::move(v)` does the same - but `w` owned a `tracer` already, and deletes it first.
     * - !![#std-move]
     */
    void hand_it_on() {
        print_function_header();

        unique_ptr<tracer> u{make_unique<tracer>("u")};
        const heap_watch heap{};
        unique_ptr<tracer> v{std::move(u)};
        cout << " 1| u.get()=" << u.get() << ", v.get()=" << v.get() << ", u is empty: " << (u == nullptr)
             << ", allocations=" << heap.allocations() << '\n';

        unique_ptr<tracer> w{make_unique<tracer>("w")};
        cout << " 2| w = std::move(v);\n";
        w = std::move(v);
        cout << " 3| w->name()=" << w->name() << ", v is empty: " << !v << '\n';

        /* -- .Q&A -- !![After `std::move(u)`, `u` is empty. What happens at its `}`?](#a-605) */
    }

    /* --- `make_tracer` ---
     * The answer to the teaser: an object on the heap outlives the function that creates it - and the `unique_ptr`
     * that is returned makes the caller its owner. No `std::move` in the `return`: the result is built in the caller's
     * place (see previous snippets), and nothing is copied or moved.
     */
    unique_ptr<tracer> make_tracer(const string& name) {
        return make_unique<tracer>(name);
    }

    /* --- `return_an_owner` --- The object survives `make_tracer`, and dies with `m`. */
    void return_an_owner() {
        print_function_header();

        const unique_ptr<tracer> m{make_tracer("m")};
        cout << " 1| back in the caller: " << m->name() << '\n';
    }

    /* --- `show`, `show_if_any` and `take` ---
     * Three ways to hand an object to a function, three different promises.
     * - `show(const tracer&)` uses the object - it does not care who owns it, or whether it is on the heap at all.
     * - `show_if_any(const tracer*)` the same, but "none" is allowed: `nullptr`.
     * - `take(unique_ptr<tracer>)` takes the ownership: the object dies at the end of `take` - a sink.
     */
    void show(const tracer& t) {
        cout << " d|   show: " << t.name() << '\n';
    }

    void show_if_any(const tracer* t) {
        cout << " e|   show_if_any: " << (t != nullptr ? t->name() : "none") << '\n';
    }

    void take(const unique_ptr<tracer> t) {
        cout << " f|   take: " << t->name() << '\n';
    }

    /* --- `pass_to_functions` ---
     * The caller decides what to pass: `*u` - the object, `u.get()` - the address, or `std::move(u)` - the ownership.
     * After `take`, `u` is empty, and the `tracer` is gone before ` 1|`.
     */
    void pass_to_functions() {
        print_function_header();

        unique_ptr<tracer> u{make_unique<tracer>("u")};
        show(*u);
        show_if_any(u.get());
        take(std::move(u));
        cout << " 1| after take: u is empty: " << (u == nullptr) << '\n';
        show_if_any(u.get());
    }

}

/* --- By value, in memory ---
 * Now the price. In Compiler Explorer, x86-64 gcc, `-O2`, with `#include <memory>` and
 * `using std::unique_ptr, std::make_unique;` in front:
 *
 *      int read_raw(const int* p) { return *p; }
 *      int read_smart(const unique_ptr<int> p) { return *p; }
 *      int read_elsewhere(unique_ptr<int> p);              // only declared
 *      int call_it() { return read_elsewhere(make_unique<int>(3)); }
 *
 * - `read_raw` is `mov eax, DWORD PTR [rdi]` - the address of the `int` arrives in `rdi`.
 * - `read_smart` needs two loads, `mov rax, QWORD PTR [rdi]` first: `rdi` holds the address of the `unique_ptr`, not
 *   its content - the same code as for a `const unique_ptr<int>&`. The rule of the previous unit: a type with a
 *   destructor of its own is always passed via an address, whatever its size.
 * - `call_it` puts the `unique_ptr` into its own frame, `mov QWORD PTR 8[rsp], rax`, and passes its address,
 *   `lea rdi, 8[rsp]`. After the call, the caller destroys the parameter: `test rdi, rdi` and `operator delete`. In a
 *   sink that took the ownership, the parameter is empty by then, and the `test` skips the `delete`.
 * ARM64 gcc does the same: `x0` holds the address of the `unique_ptr`, and `cbz` tests it after the call.
 * That is the one cost of `unique_ptr`: a pointer in memory instead of a register, and a test after the call. It
 * matters only for a sink - and a sink is called to take something.
 * - !![#calling-convention]
 */

/* --- `main` --- */
int main() {
    show_the_size();
    own_on_the_way_out();
    compare_the_machine_code();
    try_to_copy();
    hand_it_on();
    return_an_owner();
    pass_to_functions();

    return EXIT_SUCCESS;
}
