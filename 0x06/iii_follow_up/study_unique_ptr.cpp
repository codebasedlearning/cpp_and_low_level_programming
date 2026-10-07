// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The everyday operations of `unique_ptr`: `get`, `reset`, `release`, and the test for empty.
 * - `release` hands the duty back: the object is yours again - and so is the `delete`.
 * - `unique_ptr<T[]>` and `make_unique<T[]>(n)`: an owner for an array, with `delete[]`.
 * - A `vector<unique_ptr<T>>`: owners in a container - moved in, never copied.
 * - Five ways to pass a `unique_ptr`, or what it owns - and what each one tells the caller.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::make_unique_for_overwrite;


/* ---- Content ---- */

namespace {

    /* --- `tracer` --- Reports its birth and its death. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed\n";
        }

        ~tracer() {
            cout << " b|   " << name_ << ": destroyed\n";
        }

        const string& name() const { return name_; }

    private:
        string name_;
    };

    /* --- `use_the_basics` ---
     * A default-constructed `unique_ptr` owns nothing - it holds `nullptr`, and it tests as `false`. Assigning a new
     * owner deletes the old object first. `reset()` deletes the object and leaves the `unique_ptr` empty.
     */
    void use_the_basics() {
        print_function_header();

        unique_ptr<tracer> p;
        cout << " 1| empty: " << (p == nullptr) << ", !p: " << !p << '\n';
        p = make_unique<tracer>("first");
        if (p) {
            cout << " 2| owns " << p->name() << " at " << p.get() << '\n';
        }
        p = make_unique<tracer>("second");  // `first` is deleted here
        p.reset();                          // and `second` here
        cout << " 3| empty again: " << (p == nullptr) << '\n';
    }

    /* --- `take_the_duty` --- Old code that takes the ownership through a raw pointer, and deletes. */
    void take_the_duty(const tracer* t) {
        cout << " c|   take_the_duty: deletes " << t->name() << '\n';
        delete t;
    }

    /* --- `release_the_duty` ---
     * `release()` gives up the ownership without deleting: it returns the address and leaves the `unique_ptr` empty.
     * From then on, the object is a raw `new` again - somebody must delete it. Use it only to hand the object to code
     * that takes the ownership through a raw pointer: a C library, or old code. Mixing it up with `reset()` is a
     * classic leak.
     */
    void release_the_duty() {
        print_function_header();

        unique_ptr<tracer> p{make_unique<tracer>("released")};
        const tracer* raw{p.release()};
        cout << " 1| p is empty: " << (p == nullptr) << ", raw=" << raw << '\n';
        take_the_duty(raw);
    }

    /* --- `own_an_array` ---
     * `unique_ptr<int[]>` - with `[]` - calls `delete[]`, and offers `a[i]` instead of `*` and `->`. It does not know
     * its size: it holds an address, as after the decay (see previous snippets). `make_unique<int[]>(5)` sets all
     * elements to 0; `make_unique_for_overwrite<int[]>(5)` does not - for a large buffer that is filled anyway, that
     * saves a pass over the memory.
     * `unique_ptr<int>` for an array would call `delete`, not `delete[]` - undefined behavior. `make_unique` gets it
     * right from the type.
     */
    void own_an_array() {
        print_function_header();

        const heap_watch heap{};
        const unique_ptr<int[]> a{make_unique<int[]>(5)};
        a[2] = 7;                           // `a` is const - the owner, not the elements
        cout << " 1| a[0]=" << a[0] << ", a[2]=" << a[2] << ", bytes=" << heap.bytes() << '\n';

        const unique_ptr<int[]> b{make_unique_for_overwrite<int[]>(5)};
        int value{0};
        for (size_t i{0}; i < 5; ++i) {
            b[i] = value;
            value += 10;
        }
        cout << " 2| b[4]=" << b[4] << '\n';
    }

    /* --- `own_in_a_vector` ---
     * The `vector` owns the `unique_ptr`s, and they own the `tracer`s. `push_back` takes a temporary - or what
     * `std::move` hands over. When the `vector` grows, it moves its elements: 8 bytes each, and `noexcept`. Copying the
     * `vector` is a compiler error: it would copy the owners. `erase` destroys an element - and with it its `tracer`.
     * When the `vector` dies, all `tracer`s die.
     */
    void own_in_a_vector() {
        print_function_header();

        vector<unique_ptr<tracer>> owners;
        owners.push_back(make_unique<tracer>("a"));
        unique_ptr<tracer> b{make_unique<tracer>("b")};
        owners.push_back(std::move(b));
        owners.push_back(make_unique<tracer>("c"));
        // const vector<unique_ptr<tracer>> copy{owners};  // compiler error: a `unique_ptr` cannot be copied

        for (const auto& owner : owners) {
            cout << " 1|   " << owner->name() << '\n';
        }
        owners.erase(owners.begin());
        cout << " 2| size()=" << owners.size() << ", end of function\n";
    }

    /* --- `use`, `use_if_any`, `sink`, `replace` and `make` ---
     * The signature says what happens to the ownership.
     * - `use(const tracer&)` - "I only use it, and it must exist." The caller passes `*p`. Most functions look like
     *   this: they do not care who owns the object, or whether it is on the heap at all.
     * - `use_if_any(const tracer*)` - the same, but "none" is allowed. The caller passes `p.get()`.
     * - `sink(unique_ptr<tracer>)` - "I take it." The caller passes `std::move(p)`, and `p` is empty afterwards.
     * - `replace(unique_ptr<tracer>&)` - "I may change your owner": reset it, move out of it, or give it another
     *   object.
     * - `make(...)` returning `unique_ptr<tracer>` - "you own what I create."
     * And `const unique_ptr<tracer>&`? It says no more than `const tracer*`, but forces the caller to own the object
     * through a `unique_ptr`. Rarely the right choice.
     */
    void use(const tracer& t) {
        cout << " d|   use: " << t.name() << '\n';
    }

    void use_if_any(const tracer* t) {
        cout << " e|   use_if_any: " << (t != nullptr ? t->name() : "none") << '\n';
    }

    void sink(const unique_ptr<tracer> t) {
        cout << " f|   sink: " << t->name() << '\n';
    }

    void replace(unique_ptr<tracer>& t) {
        t = make_unique<tracer>("replacement");
    }

    unique_ptr<tracer> make(const string& name) {
        return make_unique<tracer>(name);
    }

    /* --- `pass_a_unique_ptr` --- All five, one after the other. */
    void pass_a_unique_ptr() {
        print_function_header();

        unique_ptr<tracer> p{make("made")};
        use(*p);
        use_if_any(p.get());
        replace(p);
        cout << " 1| now p owns " << p->name() << '\n';
        sink(std::move(p));
        use_if_any(p.get());
        cout << " 2| end of function\n";
    }

}

/* --- `main` --- */
int main() {
    use_the_basics();
    release_the_duty();
    own_an_array();
    own_in_a_vector();
    pass_a_unique_ptr();

    return EXIT_SUCCESS;
}
