// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `unique_ptr` is the owner as a type: it holds the address, and its destructor deletes the object.
 * - `make_unique` instead of `unique_ptr<T>{new T{...}}`: the same `new`, the same machine code - and no `new` in your
 *   code that could be without an owner.
 * - It is as big as a raw pointer, and the machine code is the same - plus the `delete` on the exception path, the
 *   one the raw version forgets.
 * - It cannot be copied: two owners would delete twice - the compiler refuses.
 * - It can be moved: `std::move` hands the ownership on. 8 bytes are copied, a null is left behind, nothing is
 *   allocated.
 * - A function can return a `unique_ptr` - an object that outlives the function, with an owner.
 * - To use an object without owning it, pass `T&` - or `T*` if there may be none.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/tracer.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::runtime_error;


/* ---- Content ---- */

namespace {

    /* `read_config` Something that can go wrong, and does. */
    void read_config() { throw runtime_error{"config not found"}; }

    /* --- `remember_the_idea` ---
     * A `unique_ptr` is a small object on the stack that holds the address of an object on the heap - nothing else:
     * 8 bytes, the size of a raw pointer. `get()` returns that address. The owning costs no memory.
     * - !![#unique-ptr]
     */
    void remember_the_idea() {
        print_function_header();

        cout << " 1| alloc tracer 'p'" << '\n';
        {
            const unique_ptr<tracer> p{new tracer{"p"}};        // see also `use_make_unique`
            cout << " 2| sizeof(unique_ptr<tracer>)=" << sizeof(unique_ptr<tracer>) << '\n';
            cout << " 3| &p=" << &p << " (stack), p.get()=" << p.get() << " (heap)\n";
           // no delete, it is done in the dtor of unique_ptr
        }
        cout << " 4| tracer 'p' gone, RAII in action" << '\n';

        try {
            const unique_ptr<tracer> q{new tracer{"q"}};
            read_config();
        } catch (const runtime_error& e) {
            cout << " 5| caught: " << e.what() << '\n';
        }
        cout << " 6| tracer 'q' gone, even in case of exceptions" << '\n';

        {
            unique_ptr<tracer> r{new tracer{"r"}};
            cout << " 7| &r=" << &r << " (stack), r.get()=" << r.get() << " (heap)\n";

            r.reset();                      // frees and takes another raw ptr if given
            // tracer* pr = r.release();    // returns and removes the ptr; note: memory is not freed here
            cout << " 8| freed?" << '\n';
        }
        cout << " 9| tracer 'r' gone" << '\n';
    }

    /* --- `use_make_unique` ---
     * Two ways to make an owner. By hand: `new` builds the `tracer`, and its address goes straight into the
     * constructor of the `unique_ptr`. With the factory: `make_unique<tracer>("m")` does both in one call - in
     * libstdc++, it is one line, `return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...));`. The same `new`,
     * the same owner, the same machine code. So why the factory?
     * - !![#unique-ptr]
     *
     * - No `new` in your code - and so no `delete` either. Every `new` needs an owner; with the factory, there is
     *   no moment in which the address has none.
     * - The type is written once: `auto m{make_unique<tracer>("m")}` - by hand, it is written twice.
     * - Mistakes it cannot make: one raw address given to two `unique_ptr`s - two owners, the double `delete` of
     *   the previous snippets - or a `unique_ptr<int>` for a `new int[3]`, and `delete` for a block from `new[]`.
     *   `make_unique<int[]>(3)` always pairs `new[]` with `delete[]`.
     * - The old reason: before C++17, `f(unique_ptr<tracer>{new tracer{"x"}}, g())` could leak. The compiler was
     *   allowed to run `new`, then `g()`, and only then the constructor of the `unique_ptr` - if `g` threw, the
     *   `tracer` had no owner. Since C++17, each argument is evaluated completely before the next one starts, and
     *   this leak is gone. The advice stayed.
     * By hand is right where the factory cannot help: a deleter of your own, a constructor that only the class
     * itself may call, or an address that comes from somewhere else - from a C library, say. For `shared_ptr`, the
     * factory makes a real difference: see next snippets.
     *
     * `new tracer{"t"}`, roughly:
     *
     *     tracer* t{operator new(sizeof(tracer))};    // step 1: the bytes - from the class, or the global one
     *     try {
     *         tracer::tracer();                       // step 2: the constructor, with `this` = `t`
     *     } catch (...) {
     *         operator delete(t, sizeof(tracer));     // the constructor threw: the bytes go back
     *         throw;
     *     }
     *     // the result: `t`
     *
     * `delete t`, roughly:
     *
     *     if (t != nullptr) {
     *         tracer::~tracer();                      // step 1: the destructor, with `this` = `t`
     *         operator delete(t, sizeof(tracer));     // step 2: the bytes go back
     *     }
     *
     * `tracer::tracer()` with `this` = `t` is not C++ - it is what the machine does. A constructor is a member
     * function, and every member function gets the address of its object as a hidden first argument: `this`. For
     * the constructor, it is the address `operator new` returned. So inside the constructor, `this` is already
     * there - but the object is not, the constructor makes it. That is why calling just the constructor has no
     * syntax of its own: there is no object yet to call it on. Placement `new` is the way to write it (see below).
     * The destructor can be called by name, `t->~tracer()`, because there the object still exists.
     * Your code never calls `operator new` itself - the compiler does, around the constructor. The next topics
     * change only these two calls.
     */
    void use_make_unique() {
        print_function_header();

        const unique_ptr<tracer> p{new tracer{"p"}};
        const unique_ptr<tracer> m{make_unique<tracer>("m")};
        cout << " 1| p.get()=" << p.get() << ", m.get()=" << m.get() << '\n';
    }

    /* --- `transfer_ownership` --- */
    void transfer_ownership() {
        print_function_header();

        // 't' is THE owner of the tracer
        unique_ptr t{make_unique<tracer>("t")};

        // compiler error: a `unique_ptr` cannot be copied (copy-ctor deleted)
        // unique_ptr<tracer> v{t};
        unique_ptr<tracer> v{};

        cout << " 1| 't' at " << t.get() << ", 'v' at " << v.get() << '\n';

        // How to transfer ownership? For the same reason op= is deleted.
        // v = t;

        // Solution: we move it
        v = std::move(t);
        cout << " 2| 't' at " << t.get() << ", 'v' at " << v.get() << '\n';

        // or in the ctor
        unique_ptr<tracer> w{std::move(v)};
        cout << " 3| 't' at " << t.get() << ", 'v' at " << v.get() << ", 'w' at " << w.get() << '\n';

        // So, what does std::move do?
    }

    /* --- `make_text` ---
     * Returns a text too long for the `string` itself: its characters are in a heap block.
     * For the caller, the result is a temporary - an object without a name, which dies at the end of the statement.
     */
    string make_text() {
        string text{"a text too long for the string itself"};
        cout << " M| - make_text: characters at " << static_cast<const void*>(text.data()) << '\n';
        return text;
    }

    /* --- `observe_string_stealing` ---
     * `t = s` copies: `s` has a name and lives on after the `;`, so `t` gets a block of its own, and every character is
     * copied - the addresses in the output differ.
     * `u = make_text()` steals. The result of `make_text()` is a temporary: copying it would mean a new block and
     * every character copied, just before the original is thrown away.
     * `u` takes over the pointer to the block, and the temporary is left empty: output shows the same address.
     * (For a short text there is nothing to steal: its characters live inside the `string` object itself (small
     * string optimization), and a move copies them.)
     */
    void observe_string_stealing() {
        print_function_header();

        string s{"this is a very long text not living in the object"}, t, u;
        cout << " 1| s: characters at " << static_cast<const void*>(s.data()) << '\n';

        t = s;                                          // a named object: a new block, every character copied
        cout << " 2| t: characters at " << static_cast<const void*>(t.data()) << '\n';

        u = make_text();                                // a temporary: its block is taken over, not copied
        cout << " 3| u: characters at " << static_cast<const void*>(u.data()) << '\n';
    }

    /* --- `call_with_ref` ---
     * Two overloads, and the compiler picks one by the kind of the argument.
     * - An lvalue - an object with a name, and an address you can take with `&` - binds only to `const string&`.
     * - An rvalue - a temporary, about to die at the `;`
     * Neither function does anything: they only show which one was chosen, and where the characters are.
     * The move constructor and the move assignment of `string` are exactly such `&&` overloads - that is how the
     * compiler knows when it may steal.
     */
    void call_with_ref(const string& s) {
        cout << " a| - lvalue ref., characters at " << static_cast<const void*>(s.data()) << '\n';
    }
    void call_with_ref(string&& s) {
        cout << " b| - rvalue ref., characters at " << static_cast<const void*>(s.data()) << '\n';
    }

    /* --- `two_kinds_of_refs` ---
     * The same choice, made visible: `call_with_ref(s)` gets the `const string&` overload, `call_with_ref(make_text())`
     * the `string&&` one. And `call_with_ref(std::move(s))` gets the `&&` overload, too - with the characters of `s`,
     * still at their place. `std::move` moved nothing, it only made `s` look like a temporary. The stealing is done by
     * a function that takes from its `&&` argument: a move constructor, or a move assignment.
     * - !![#std-move]
     *
     * So no `std::move` is needed for the string example earlier - the right side is a temporary, and the compiler
     * picks the move assignment by itself.
     */
    void two_kinds_of_refs() {
        print_function_header();

        string s{"another very long text not living in the object"};
        cout << " 1| s: characters at " << static_cast<const void*>(s.data()) << '\n';

        call_with_ref(s);                   // located value => lvalue
        call_with_ref(make_text());         // not an lvalue => rvalue

        call_with_ref(std::move(s));        // it does not steal but makes it a rvalue

        // Remark, here nothing is move out of `s`, it is just an example for
        // making an lvalue-ref an rvalue-ref.
        // cout << " 2| s: " << s << '\n';
    }

    /* --- `steal_with_move` ---
     * The same for a `unique_ptr` - only that it has no copy to fall back on: its copy constructor and copy assignment
     * are deleted, so from a temporary, as in `u = make_unique<tracer>("u")`, the move assignment is the only one there
     * is. A named object does not die at the `;`, so the compiler never steals from it on its own. `std::move(t)` moves
     * nothing: it is a cast, `static_cast<unique_ptr<tracer>&&>(t)` - casts are the topic of a future unit. It only
     * says "treat `t` like a temporary": the result is an rvalue reference, `unique_ptr<tracer>&&`. For that type,
     * the compiler picks the other constructor, `unique_ptr(unique_ptr&&)`, or the other `operator=` - and they do the
     * stealing, not `std::move`. More on `&&` in the next snippets.
     * The code of `transfer_ownership`, seen in this light: in `v = std::move(t)`, `std::move` makes `t` look like a
     * temporary, so the `&&` overload of `operator=` is picked - and it steals: `v` takes the address, `t` is left
     * empty. `w{std::move(v)}` is the same for the constructor. The address wanders from `t` to `v` to `w`, a null is
     * left behind each time, and only one `tracer` is ever built.
     * - !![#std-move]
     */
    void steal_with_move() {
        print_function_header();

        unique_ptr t{make_unique<tracer>("t")};
        cout << " 1| t.get()=" << t.get() << '\n';

        unique_ptr<tracer> v{};
        // using move-assign-op=, it steals from 't'
        v = std::move(t);
        cout << " 2| t.get()=" << t.get() << ", v.get()=" << v.get() << '\n';

        // using move-ctor, it steals from 'v'
        unique_ptr<tracer> w{std::move(v)};
        cout << " 3| v.get()=" << v.get() << ", w.get()=" << w.get() << '\n';
    }

    /* --- .`make_tracer` --- an object on the heap outlives the function that creates it - and the caller its owner. */
    unique_ptr<tracer> make_tracer(const string& name) {
        cout << " a| - make_tracer: " << name << '\n';
        return make_unique<tracer>(name);
    }

    /* --- .`show` --- uses the object - it does not care who owns it, or whether it is on the heap at all. */
    void show(const tracer& t) {
        cout << " b| - show: " << t.name() << '\n';
    }

    /* --- .`show_if_any` --- the same, but "none" is allowed: `nullptr`. */
    void show_if_any(const tracer* t) {
        cout << " c| - show_if_any: " << (t != nullptr ? t->name() : "none") << '\n';
    }

    /* --- .`peek` --- gets the owner, not the object - it works, but ties `peek` to `unique_ptr` for no reason. */
    void peek(const unique_ptr<tracer>& t) {
        cout << " d| - peek: " << (t != nullptr ? t->name() : "none") << '\n';
    }

    /* --- .`reseat` --- gets the owner to change it; the old `tracer` dies, the caller owns another object afterwards. */
    void reseat(unique_ptr<tracer>& t) {
        t = make_unique<tracer>("r");
        cout << " e|   reseat: now " << t->name() << '\n';
    }

    /* --- .`take` --- takes the ownership: the object dies at the end - a sink.
     * Note, call-by-value has always meant 'construct a new object, the parameter, from the argument'.
     * What C++11 changed is the number of ways to do that construction:
     * - lvalue: take(u)                -> copy constructor, T(const T&)
     * - rvalue: take(std::move(u))     -> move constructor, T(T&&)
     * - prvalue: there are more... not today.
     */
    void take(const unique_ptr<tracer> t) {
        cout << " f|   take: " << t->name() << '\n';
        // Core Guidelines R.32
    }

    /* --- `working_with_unique_pointers` ---
     * The caller decides what to pass:
     * - `*u` - the object,
     * - `u.get()` - its address,
     * - `u` - the owner, or
     * - `std::move(u)` - the ownership. After that `u` is empty.
     */
    void working_with_unique_pointers() {
        print_function_header();

        const unique_ptr<tracer> m{make_tracer("m")};
        cout << " 1| transferred: " << m->name() << '\n';

        unique_ptr<tracer> u{make_unique<tracer>("u")};

        show(*u);                           // borrows the object
        show_if_any(u.get());               // borrows the object - or none
        peek(u);                            // access to the owner, to look - `u` stays the owner
        reseat(u);                          // access to the owner, to change it - `u` owns "r" now
        take(std::move(u));                 // hands the ownership over
        cout << " 2| after take: u is empty: " << (u == nullptr) << '\n';
        show_if_any(u.get());
    }

    /* --- `using_unique_ptr_in_a_vector` ---
     * A `vector` of owners: every element is a `unique_ptr`, the `vector` owns the owners - and so the `tracer`s,
     * too.
     * In: a temporary from `make_unique` goes in by itself; a named owner needs `std::move`, and is empty afterwards.
     * When the `vector` grows, it moves its elements - 8 bytes each, and the `tracer`s stay where they are.
     * Out: `std::move(owners.back())` steals from the element - the `vector` still has it, but empty, and `pop_back()`
     * removes the empty shell. Taken from the front, the null stays behind: a hole that every loop over the `vector`
     * now has to expect. At the `}`, the locals die in reverse order - `first` with "a", `last` with "c" - and then
     * the `vector`, with what is left in it, "b". No `delete` anywhere.
     * - !![#unique-ptr]
     */
    void using_unique_ptr_in_a_vector() {
        print_function_header();

        cout << " 1| fill the vector" << '\n';
        {
            vector<unique_ptr<tracer>> owners;
            owners.push_back(make_unique<tracer>("a"));         // a temporary: moved in by itself
            unique_ptr<tracer> b{make_unique<tracer>("b")};
            owners.push_back(std::move(b));                     // a named owner: hand it over
            owners.push_back(make_unique<tracer>("c"));
            cout << " 1| size()=" << owners.size() << ", b is empty: " << (b == nullptr) << '\n';

            unique_ptr<tracer> mid{std::move(owners[1])};
            // unique_ptr last{std::move(owners.back())};       // out: steal from the last element ...
            // owners.pop_back();                               // ... and remove its empty shell
            cout << " 2| size()=" << owners.size() << ", mid=" << mid->name() << ", [1] is empty: " << (owners[1].get() == nullptr) << '\n';

            // owners[1] = std::move(x);                        // x is empty afterwards
            owners[1] = make_unique<tracer>("x");               // a temporary: no std::move needed
            cout << " 3| size()=" << owners.size() << ", [1]=" << owners[1]->name() << ", [1] is empty: " << (owners[1].get() == nullptr) << '\n';

            /* Preview operators:
             * - operator[] returns unique_ptr<tracer>&, a reference to the element
             * - operator->() returns the stored tracer*, the same as get().
             */
        }
    }

}

/* --- `main` --- */
int main() {
    remember_the_idea();
    use_make_unique();
    transfer_ownership();
    observe_string_stealing();
    two_kinds_of_refs();
    steal_with_move();
    working_with_unique_pointers();
    using_unique_ptr_in_a_vector();

    return EXIT_SUCCESS;
}
