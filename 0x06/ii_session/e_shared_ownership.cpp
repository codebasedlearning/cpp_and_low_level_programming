// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `shared_ptr`: several owners, one object. The last owner that dies deletes it.
 * - Two pointers: one to the object, one to the control block, which counts the owners.
 * - `make_shared` is one allocation - the object and its control block together; `shared_ptr{new T}` is two.
 * - Every copy counts up, every destructor counts down - atomically, because the owners may live in different threads.
 * - A cycle keeps itself alive: a leak nobody can delete. `weak_ptr` looks without owning, and breaks the cycle.
 * - Prefer `unique_ptr`. Share only what really has several owners.
 */

#include <iostream>
#include <string>
#include <memory>                           // for shared_ptr, make_shared, weak_ptr
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::unique_ptr, std::shared_ptr, std::make_shared, std::weak_ptr;


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

    /* --- `show_the_size` ---
     * A `shared_ptr` is twice the size of a `unique_ptr`: the address of the object, and the address of the control
     * block. The control block is on the heap, and it holds the counts - `use_count()` reads it.
     * - !![#shared-ptr]
     */
    void show_the_size() {
        print_function_header();

        const shared_ptr<tracer> s{make_shared<tracer>("s")};
        cout << " 1| sizeof(unique_ptr<tracer>)=" << sizeof(unique_ptr<tracer>) << ", sizeof(shared_ptr<tracer>)="
             << sizeof(shared_ptr<tracer>) << ", use_count()=" << s.use_count() << '\n';
    }

    /* --- `count_the_blocks` ---
     * `make_shared` allocates one block for both, the control block and the `tracer` in it - more bytes than
     * `sizeof(tracer)`. `shared_ptr{new tracer{...}}` gets a `tracer` that exists already, and must allocate its
     * control block separately: two blocks, two calls, two places in memory. How big a control block is depends on the
     * library; the difference in the number of allocations does not.
     */
    void count_the_blocks() {
        print_function_header();

        cout << " 1| sizeof(tracer)=" << sizeof(tracer) << '\n';
        heap_watch heap{};
        const shared_ptr<tracer> one{make_shared<tracer>("one")};
        cout << " 2| make_shared:    allocations=" << heap.allocations() << ", bytes=" << heap.bytes() << '\n';

        heap.reset();
        const shared_ptr<tracer> two{new tracer{"two"}};
        cout << " 3| shared_ptr{new}: allocations=" << heap.allocations() << ", bytes=" << heap.bytes() << '\n';
    }

    /* --- `share_and_let_go` ---
     * Copying a `shared_ptr` allocates nothing: it copies the two addresses and counts up. Destroying one counts down.
     * `reset()` gives up one ownership - the `tracer` lives on, because `c` still owns it. The last owner that goes
     * deletes it: here `c` at the `}`, after ` 4|`.
     */
    void share_and_let_go() {
        print_function_header();

        shared_ptr<tracer> a{make_shared<tracer>("shared")};
        const heap_watch heap{};
        {
            const shared_ptr<tracer> b{a};
            cout << " 1| in the block: use_count()=" << a.use_count() << ", a.get()=" << a.get() << ", b.get()="
                 << b.get() << '\n';
        }
        cout << " 2| after the block: use_count()=" << a.use_count() << '\n';

        const shared_ptr<tracer> c{a};
        a.reset();
        cout << " 3| after a.reset(): a is empty: " << (a == nullptr) << ", c.use_count()=" << c.use_count() << '\n';
        cout << " 4| allocations=" << heap.allocations() << '\n';
    }

}

/* --- The count in the machine code ---
 * Two owners in two threads may copy and destroy at the same moment - so the count must be changed atomically: read,
 * add, write, as one step that no other core can interrupt. In Compiler Explorer, x86-64 gcc, `-O2`:
 *
 *      void keep(shared_ptr<int> p);                           // only declared
 *      void share(const shared_ptr<int>& p) { keep(p); }
 *
 * The copy for `keep` is `lock add DWORD PTR 8[rax], 1` (clang: `lock inc`) - `lock` makes the `add` atomic, and
 * `8[rax]` is the count, 8 bytes into the control block (the first 8 hold the address of its table of functions, see
 * future units). After the call, `lock xadd` counts down. An atomic instruction is much slower than a plain one when
 * several cores compete for the same count. gcc's library checks first whether the program has started a thread
 * (`__libc_single_threaded`), and uses a plain `add` while it has not; clang's library always uses `lock`. On ARM64,
 * look for `ldadd` - gcc calls a helper function, `__aarch64_ldadd4_acq_rel`.
 * A `unique_ptr` has nothing to count: one owner, nothing atomic.
 */

namespace {

    /* --- `person` ---
     * Two people who know each other, and each keeps the other alive: `partner_` is a `shared_ptr`.
     */
    class person {
    public:
        explicit person(const string& name) : name_{name} {}
        ~person() { cout << " c|   " << name_ << ": destroyed\n"; }

        void meet(const shared_ptr<person>& other) { partner_ = other; }
        const string& name() const { return name_; }

    private:
        string name_;
        shared_ptr<person> partner_;
    };

    /* --- `try_a_cycle` ---
     * `ann` owns her `person`, and `bob`'s `person` owns it, too - count 2; the same the other way round. At the `}`,
     * `ann` and `bob` die, both counts go down to 1 - and stay there: each `person` is owned by the other. No
     * destructor runs, and nobody has the addresses any more. `heap_watch` sees two blocks that nobody will ever
     * delete: a leak, with smart pointers only.
     */
    void try_a_cycle() {
        print_function_header();

        const heap_watch heap{};
        {
            const shared_ptr<person> ann{make_shared<person>("Ann")};
            const shared_ptr<person> bob{make_shared<person>("Bob")};
            ann->meet(bob);
            bob->meet(ann);
            cout << " 1| use_count: ann=" << ann.use_count() << ", bob=" << bob.use_count() << '\n';
        }
        cout << " 2| after the block: live=" << heap.live() << '\n';
    }

    /* --- `friendly_person` ---
     * The same, but `partner_` is a `weak_ptr`: it knows the object and its control block, but it does not own the
     * object - it does not count as an owner. To use the object, `lock()` gives a `shared_ptr`: an owner for the
     * moment, or an empty one if the object is gone.
     */
    class friendly_person {
    public:
        explicit friendly_person(const string& name) : name_{name} {}
        ~friendly_person() { cout << " d|   " << name_ << ": destroyed\n"; }

        void meet(const shared_ptr<friendly_person>& other) { partner_ = other; }

        string partner_name() const {
            const shared_ptr<friendly_person> partner{partner_.lock()};
            return partner != nullptr ? partner->name_ : "nobody";
        }

    private:
        string name_;
        weak_ptr<friendly_person> partner_;
    };

    /* --- `break_the_cycle` ---
     * Now each object has one owner, the counts are 1, and both die at the `}`. The pointers back do not keep anybody
     * alive.
     * - !![#ownership]
     */
    void break_the_cycle() {
        print_function_header();

        const heap_watch heap{};
        {
            const shared_ptr<friendly_person> ann{make_shared<friendly_person>("Ann")};
            const shared_ptr<friendly_person> bob{make_shared<friendly_person>("Bob")};
            ann->meet(bob);
            bob->meet(ann);
            cout << " 1| use_count: ann=" << ann.use_count() << ", bob=" << bob.use_count() << ", ann's partner: "
                 << ann->partner_name() << '\n';
        }
        cout << " 2| after the block: live=" << heap.live() << '\n';
    }

    /* --- `watch_a_weak_ptr` ---
     * A `weak_ptr` outlives the object: after `s.reset()`, the `tracer` is destroyed - and `lock()` returns an empty
     * `shared_ptr`. But the block does not go yet: the control block must stay as long as some `weak_ptr` asks it, and
     * with `make_shared`, the `tracer`'s bytes are in the same block. The destructor has run, the memory is still
     * allocated - until the last `weak_ptr` is gone.
     */
    void watch_a_weak_ptr() {
        print_function_header();

        const heap_watch heap{};
        shared_ptr<tracer> s{make_shared<tracer>("watched")};
        weak_ptr<tracer> w{s};
        cout << " 1| expired: " << w.expired() << ", live=" << heap.live() << '\n';
        s.reset();
        cout << " 2| expired: " << w.expired() << ", lock() is empty: " << (w.lock() == nullptr) << ", live="
             << heap.live() << '\n';
        w.reset();
        cout << " 3| no weak_ptr: live=" << heap.live() << '\n';

        /* -- .Q&A -- !![With `shared_ptr{new tracer{...}}` instead - what stays after `s.reset()`?](#a-607) */
    }

}

/* --- One owner, if you can ---
 * `shared_ptr` costs: twice the size, a control block, an atomic count on every copy, and the risk of cycles. And it
 * makes the question "who deletes this, and when?" hard to answer by reading the code. Most objects have one owner -
 * take `unique_ptr`; it converts to a `shared_ptr` later, if the need arises. Share what really has several owners,
 * with lifetimes nobody can predict: a cache entry, a document open in several windows, data several threads use.
 */

/* --- `main` --- */
int main() {
    show_the_size();
    count_the_blocks();
    share_and_let_go();
    try_a_cycle();
    break_the_cycle();
    watch_a_weak_ptr();

    return EXIT_SUCCESS;
}
