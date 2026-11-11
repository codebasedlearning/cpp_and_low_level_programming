// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `shared_ptr`: several owners, one object. The last owner that dies deletes it.
 * - Two pointers: one to the object, one to the control block, which counts the owners.
 * - A sketch by hand: a copy counts up, a move changes nothing, a destructor counts down - and the last owner deletes.
 * - `make_shared` is one allocation - the object and its control block together; `shared_ptr{new T}` is two.
 * - Every copy counts up, every destructor counts down - atomically, because the owners may live in different threads.
 * - A cycle keeps itself alive: a leak nobody can delete. `weak_ptr` looks without owning, and breaks the cycle.
 * - Passing it on: the functions that use the object stay the same; by value, a `shared_ptr` is copied - a share for
 *   the time of the call - or moved - a handover.
 * - Prefer `unique_ptr`. Share only what really has several owners.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>                           // for shared_ptr, make_shared, weak_ptr
#include <utility>                          // for swap, forward
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/tracer.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::shared_ptr, std::make_shared, std::weak_ptr;


/* ---- Content ---- */

namespace {

    /* --- `control_block` and `counted_ptr` ---
     * A sketch of `shared_ptr`, without the details. `counted_ptr` is two pointers, as `shared_ptr`: `object_`, the
     * address of the object, and `block_`, the address of a control block on the heap that holds the count. All owners
     * of the same object point to the same block - that is how they know about each other.
     * - The constructor takes an object from `new` and allocates the block: count 1 - the second allocation of
     *   `shared_ptr{new ...}`.
     * - The copy constructor copies both addresses and counts up - no allocation.
     * - The move constructor takes both addresses and leaves nulls: one owner goes, one comes, the count stays.
     * - The destructor counts down. The owner that brings the count to 0 is the last one: it deletes the object and
     *   the block.
     * - The assignment is copy-and-swap (see previous snippets): `other` is a copy - count up - or a move - no change;
     *   after the swap, `other` takes the old object with it, and its destructor counts that one down. `reset()` is
     *   just the assignment of an empty one.
     * What the real one adds: the count is atomic (see below); a second count, for the `weak_ptr`s - the object lives
     * until the first count is 0, the block until both are; a deleter and a table of functions in the block; and
     * `make_shared`, which puts the object into the block. And if the block cannot be allocated, it deletes the
     * object, instead of leaking it as this sketch would. Also, it is not suited for arrays, this would require a
     * specialized version.
     * - !![#shared-ptr]
     */
    struct control_block {
        long owners;
    };

    template <typename T>
    class counted_ptr {
    public:
        counted_ptr() = default;
        explicit counted_ptr(T* object) : object_{object}, block_{new control_block{1}} {}

        counted_ptr(const counted_ptr& other) : object_{other.object_}, block_{other.block_} {
            if (block_ != nullptr) {        // otherwise they both own no object
                ++block_->owners;
            }
        }

        counted_ptr(counted_ptr&& other) noexcept : object_{other.object_}, block_{other.block_} {
            other.object_ = nullptr;
            other.block_ = nullptr;
        }

        counted_ptr& operator=(counted_ptr other) noexcept {
            swap(other);
            return *this;
        }

        ~counted_ptr() {
            if (block_ != nullptr && --block_->owners == 0) {
                delete object_;
                delete block_;
            }
        }

        void swap(counted_ptr& other) noexcept {
            std::swap(object_, other.object_);
            std::swap(block_, other.block_);
        }

        void reset() { *this = counted_ptr{}; }

        T* get() const { return object_; }
        long use_count() const { return block_ != nullptr ? block_->owners : 0; }

    private:
        T* object_{nullptr};
        control_block* block_{nullptr};
    };

    /* --- `make_counted` --- Args&&  are not part of the exam :-)
     * The factory, as `make_unique` for `unique_ptr` (see previous snippets): it calls `new` and hands the address
     * straight to the owner - no `new` in the caller's code, and the type written once:
     * `auto t{make_counted_ptr<tracer>("t")}`. `Args&&... args` takes any number of arguments of any kind, and
     * `std::forward` passes each one on to the constructor as it came - an rvalue as an rvalue (see future snippets).
     * The real `make_shared` does more: it puts the object and its control block into one block - one allocation
     * instead of two. This sketch cannot: its control block is a `new` of its own, so here the factory only saves the
     * `new` in your code.
     */
    template <typename T, typename... Args>
    counted_ptr<T> make_counted(Args&&... args) {
        return counted_ptr<T>{new T(std::forward<Args>(args)...)};
    }

    /* --- `shared_ptr_the_idea` --- */
    void shared_ptr_the_idea() {
        print_function_header();

        cout << " 1| alloc tracer" << '\n';
        {
            const counted_ptr<tracer> t{make_counted<tracer>("t")};
            cout << " 2| sizeof(counted_ptr<tracer>)=" << sizeof(counted_ptr<tracer>) << '\n';
            cout << " 3| t.get()=" << t.get() << ", t.use_count()=" << t.use_count() << '\n';
            const counted_ptr<tracer> u{t};
            cout << " 4| t.get()=" << t.get() << ", t.use_count()=" << t.use_count() << '\n';
            cout << " 5| u.get()=" << u.get() << ", u.use_count()=" << u.use_count() << '\n';
            // no delete, it is done in the dtor of counted_ptr
        }
        cout << " 6| tracer gone, RAII in action" << '\n';

        counted_ptr<tracer> v{make_counted<tracer>("v")};
        counted_ptr<tracer> w{};
        cout << " 7| v.get()=" << v.get() << ", v.use_count()=" << v.use_count() << '\n';
        cout << " 8| w.get()=" << w.get() << ", w.use_count()=" << w.use_count() << '\n';

        w = v;
        // w = std::move(v);                // What happens here?
        cout << " 9| v.get()=" << v.get() << ", v.use_count()=" << v.use_count() << '\n';
        cout << "10| w.get()=" << w.get() << ", w.use_count()=" << w.use_count() << '\n';

        v.reset();
        w.reset();
        cout << "11| w.get()=" << w.get() << ", w.use_count()=" << w.use_count() << '\n';
    }

    /* --- `shared_ptr_the_original` --- */
    void shared_ptr_the_original() {
        print_function_header();

        cout << " 1| alloc tracer" << '\n';
        {
            const shared_ptr<tracer> t{make_shared<tracer>("t")};
            cout << " 2| sizeof(counted_ptr<tracer>)=" << sizeof(counted_ptr<tracer>) << '\n';
            cout << " 3| t.get()=" << t.get() << ", t.use_count()=" << t.use_count() << '\n';
            const shared_ptr<tracer> u{t};
            cout << " 4| t.get()=" << t.get() << ", t.use_count()=" << t.use_count() << '\n';
            cout << " 5| u.get()=" << u.get() << ", u.use_count()=" << u.use_count() << '\n';
            // no delete, it is done in the dtor of counted_ptr
        }
        cout << " 6| tracer gone, RAII in action" << '\n';

        shared_ptr<tracer> v{make_shared<tracer>("v")};
        shared_ptr<tracer> w{};
        cout << " 7| v.get()=" << v.get() << ", v.use_count()=" << v.use_count() << '\n';
        cout << " 8| w.get()=" << w.get() << ", w.use_count()=" << w.use_count() << '\n';

        w = v;
        // w = std::move(v);                // What happens here?
        cout << " 9| v.get()=" << v.get() << ", v.use_count()=" << v.use_count() << '\n';
        cout << "10| w.get()=" << w.get() << ", w.use_count()=" << w.use_count() << '\n';

        v.reset();
        w.reset();
        cout << "11| w.get()=" << w.get() << ", w.use_count()=" << w.use_count() << '\n';
    }

    /* --- .`share` --- the caller and the function both own the object during the call */
    void share(const shared_ptr<tracer> s) {
        cout << " a| - share: " << s->name() << ", use_count()=" << s.use_count() << '\n';
    }

    /* --- `working_with_shared_pointers` ---
     * The same ideas as in the previous unique_ptr-snippet, i.e.
     * `show`, `show_if_any`, `peek`, and `reseat` work as expected.
     * - !![#shared-ptr]
     */
    void working_with_shared_pointers() {
        print_function_header();

        shared_ptr<tracer> s{make_shared<tracer>("s")};
        cout << " 1| transferred: " << s->name() << '\n';

        share(s);                           // both are owners
        cout << " 2| after share: u is empty: " << (s == nullptr) << '\n';

        share(std::move(s));                // hands the ownership over: nothing counted
        cout << " 3| after share: u is empty: " << (s == nullptr) << '\n';
    }

    /* --- `destroyed_nodes` and `node` ---
     * A node of a list that runs both ways: `next` owns the node after it, `prev` points back to the one before it.
     * As written, `prev` is a `shared_ptr`, too - and that is the bug: two nodes linked both ways own each other.
     * Replace `shared_ptr` by `weak_ptr` in this one line, and nothing else has to change: assigning a `shared_ptr` and
     * `use_count()` work with both. A `weak_ptr` knows the node and its control block, but it is not an owner - it
     * does not count.
     * Which link is the weak one follows from the structure: ownership runs forward, along `next`, and looking back
     * only looks. The same holds for a tree - a parent owns its children, a child only knows its parent.
     * `destroyed_nodes` counts the destructors that ran.
     */
    int destroyed_nodes{0};

    struct node {
        explicit node(const string& name) : name{name} {}

        ~node() {
            ++destroyed_nodes;
            cout << " a| - " << name << ": destroyed\n";
        }

        string name;
        shared_ptr<node> next;          // owns the node after it
        ///shared_ptr<node> prev;          // the bug: owns the node before it, too - make it a `weak_ptr`
        weak_ptr<node> prev;
    };

    /* --- `link` ---
     * Sets both links, from outside: `first` owns `second`, and `second` points back to `first`. A free function on
     * purpose: it needs both nodes as `shared_ptr`s. A member function `first->link(second)` would only have `this`, a
     * raw pointer - and `shared_ptr<node>{this}` would be the classic bug: a second control block for the same node,
     * a second count, and in the end a second `delete`.
     */
    void link(const shared_ptr<node>& first, const shared_ptr<node>& second) {
        first->next = second;
        second->prev = first;
    }

    /* --- `link_the_nodes` ---
     * As written: `a` owns `b`, `b` owns `a` and `c`, `c` owns `b` - the counts are 2, 3 and 2. At the `}`, the locals
     * die, and every count goes down by one - and none reaches 0: each node is owned by a neighbor. No destructor
     * runs, nobody has the addresses any more - `destroyed_nodes=0`: a leak, with smart pointers only.
     *
     * With `weak_ptr<node> prev`: the counts are 1, 2 and 2, and all nodes die at the `}` - `destroyed_nodes=3`, in the
     * order a, b, c, although the locals go c, b, a: `c`'s and `b`'s locals go first, but `b->next` and `a->next` still
     * hold them. Then `a`'s local goes, `a` dies, its `next` lets go of `b` - and `b`, dying, lets go of `c`.
     * Walking the list. Forward, along `next`, a plain `const node*` is enough: the `shared_ptr`s in `next` own the
     * nodes, and keep every one alive during the walk. Backward, along `prev`, the links are `weak_ptr`s - they do
     * not own, so they cannot be used directly, there is no `->`. `lock()` asks: is the node still there? Then it
     * returns a `shared_ptr`, an owner for the moment - while `n` holds it, the node cannot die under our hands. If it
     * is gone - or at the start of the list - `lock()` returns an empty one, and the loop ends.
     * The backward walk compiles only with `weak_ptr<node> prev` - a `shared_ptr` has no `lock()`. After the fix,
     * remove its `//`.
     * - !![#ownership]
     */
    void link_the_nodes() {
        print_function_header();

        {
            const shared_ptr<node> a{make_shared<node>("a")};
            const shared_ptr<node> b{make_shared<node>("b")};
            const shared_ptr<node> c{make_shared<node>("c")};
            link(a, b);
            link(b, c);
            cout << " 1| use_count: a=" << a.use_count() << ", b=" << b.use_count() << ", c=" << c.use_count() << '\n';

            for (const node* n{a.get()}; n != nullptr; n = n->next.get()) {     // forward: along the owners
                cout << " 2| forward: " << n->name << '\n';
            }
            for (shared_ptr<node> n{c}; n != nullptr; n = n->prev.lock()) {  // backward: with `weak_ptr<node> prev`
                cout << " 3| backward: " << n->name << '\n';
            }
        }
        cout << " 4| after the block: destroyed nodes=" << destroyed_nodes << '\n';
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
    shared_ptr_the_idea();
    shared_ptr_the_original();
    working_with_shared_pointers();
    link_the_nodes();

    return EXIT_SUCCESS;
}
