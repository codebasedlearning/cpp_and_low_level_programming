// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - What the compiler makes of `new` and `delete`, written out.
 * - `new` is two steps - and the first one can be swapped: a class can bring its own `operator new` and
 *   `operator delete`, and `new` and `delete` use them for that class.
 * - Underneath, both can be as simple as `malloc` and `free`: raw bytes, no constructor, no destructor.
 * - A pool: a class can take its blocks from an array of its own - no `malloc`, and no size in front of a block. The
 *   free slots hold the list of free slots, as in a real heap.
 * - `::new` and `::delete` skip the class and use the global pair. The pairs must match - a pointer does not remember
 *   which `operator new` it came from, so the `delete` has to say it.
 * - Placement `new` is one more `operator new`: it allocates nothing and returns the address it gets. Both steps by
 *   hand are `malloc` and placement `new` - and an `operator new` in the class hides the placement one.
 */

#include <iostream>
#include <new>                              // for bad_alloc, placement new
#include <cstddef>                          // for byte
#include <cstdlib>                          // for malloc, free
#include <cbl/printing.hpp>
#include <cbl/tracer.hpp>

using std::cout, std::size_t, std::byte, std::bad_alloc;


/* ---- Content ---- */

namespace {

    /* --- `recall_new_and_delete` ---
     * The two steps once more, for the smallest class there is. `sizeof(empty)` is 1, not 0: every object needs an
     * address of its own, so even an empty one takes a byte - and `operator new` is asked for 1 byte. `empty` has no
     * `operator new` of its own, so the compiler takes the global one. The constructor and the destructor have nothing
     * to do: the compiler writes no call for them, not even as Debug.
     */
    void recall_new_and_delete() {
        print_function_header();

        cout << " 1| decompose new and delete" << '\n';
        tracer* t{new tracer{"t"}};
        cout << " 2| t=" << t << '\n';
        delete t;

        /* -- .What the compiler makes of it, roughly. --
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
    }

    /* --- .`point` ---
     * A little struct with an `operator new` and an `operator delete` of its own. As members they are `static`, even
     * without the keyword: when `operator new` runs, there is no object yet - and when `operator delete` runs, there
     * is none any more. Both hand the work on to the C allocator: `malloc` returns raw bytes, a `void*`, or `nullptr`
     * if there are none - and `free` takes them back.
     * The pseudo code of `empty` stays as it is - only the `operator new` and `operator delete` in it are these two
     * now.
     */
    struct point {
        int x;
        int y;

        point(const int x0, const int y0) : x{x0}, y{y0} {
            cout << " a| - point constructed at " << this << '\n';
        }

        ~point() {
            cout << " b| - point destroyed at " << this << '\n';
        }

        static void* operator new(const size_t size) {
            void* raw{std::malloc(size)};   // `malloc` (memory allocation) reserves `size` bytes on the heap
            if (raw == nullptr) {
                throw bad_alloc{};          // `new` never returns `nullptr` - `malloc` does
            }
            cout << " c| - point::operator new got " << size << " bytes from malloc at " << raw << '\n';
            return raw;
        }

        static void operator delete(void* raw, const size_t size) noexcept {
            cout << " d| - point::operator delete put " << size << " bytes to free at " << raw << '\n';
            std::free(raw);                 // `free` gives the memory back to the heap
        }
    };

    /* --- `handle_new_with_malloc` ---
     * The same two steps, but the first one is ours now. For `new tracer{}`, the compiler looked into `tracer`, found
     * no `operator new`, and took the global one. For `new point{1, 2}`, it looks into `point` first, finds its
     * `operator new`, and calls it with `sizeof(point)`. The address `malloc` returns is `this` in the constructor -
     * the output line show the same address - and the one `p` gets. `delete p` goes backwards: the
     * destructor, then `point::operator delete`, and `free`. The compiler even hands the size back, it knows the type.
     * `free` does not need it: the allocator keeps the size of each block itself (see previous snippets).
     * - !![#new-delete]
     */
    void handle_new_with_malloc() {
        print_function_header();

        cout << " 1| get a new point, sizeof(point)=" << sizeof(point) << '\n';
        point* p{new point{1, 2}};
        cout << " 2| p=" << p << ", p->x=" << p->x << ", p->y=" << p->y << '\n';
        delete p;

        /* -- .Why a class brings its own. --
         * The global `operator new` can be replaced, too - once, for every type of the program, as the counting tool of
         * this unit does. A class does it for itself: to get its blocks from somewhere else - a pool of slots of equal
         * size (next topic), an arena that is given back in one piece, memory with a special alignment - or just to
         * count its own objects. The size is a parameter because a derived class inherits `operator new` and asks for
         * more bytes. And `new point[2]` does not use it: arrays go to `operator new[]`, and a class must bring that
         * one, too.
         */
    }

    /* --- .`connection` ---
     * A connection to a server, say - and a server allows only so many at a time. Two `int`s, an `operator new` and an
     * `operator delete` of its own - declared here, defined below the slots: the slots need `sizeof(connection)`, so
     * the struct must be complete first. Outside the class, the definitions are written without `static`.
     */
    struct connection {
        int id;
        int port;

        static void* operator new(size_t size);
        static void operator delete(void* raw) noexcept;
    };

    /* --- .`pool` ---
     * Room for `N` objects of type `T` - no `malloc`, and no size in front of a block: all slots have the same size,
     * and the pool knows it. `pool` is a class template (see previous snippets): the compiler writes a class for every
     * `T` and `N` it is used with - here once, `pool<connection, max_connections>`. Neither is stored: `N` is the size
     * of the array, `T` the size and alignment of a slot, both known while compiling. A pool without slots would write
     * outside its array in the constructor, so `static_assert` refuses it - while compiling, too.
     * A free slot holds no `T`, so it holds something useful instead: the address of the next free slot. `first_free_`
     * is the start of this chain, the free list, and the last free slot holds `nullptr`.
     * `slot` is a `union`, private to the pool: its members share the same bytes - `next` while the slot is free,
     * `bytes` while a `T` lives in it - and it is as big as the bigger of the two, at least a pointer. `byte` is the
     * type for raw memory: no number, no character, just bits - and an array of `byte` may hold objects of any other
     * type. `alignas` gives it the alignment of a `T`.
     * The bytes do not say which of the two they are - a `union` keeps no record of that. Who holds the slot says it:
     * a slot in the list belongs to the pool, and only the pool reads its `next`; a slot handed out by `allocate`
     * belongs to its owner, and only the owner reads the `T` in it. `allocate` and `deallocate` hand a slot from one
     * side to the other - and they are the only ones that touch the list: `slots_` and `first_free_` are private.
     * Every slot is on one side or the other - so at most `N` are in use, without counting.
     * - `allocate` takes the head of the list - the slot given back last, or at first slot 0 - and its `next` becomes
     *   the head. If the list is empty, all `N` slots are taken: `bad_alloc`.
     * - `deallocate` puts the slot back in front: its first bytes - where the `T` was - now hold the link to the old
     *   head. `static_cast<slot*>(raw)` says "this address is a slot" - casts are the topic of a future unit.
     *   `nullptr` is no slot: nothing to do.
     * No search - a few instructions, however many slots there are. The names are those of `std::allocator` (see
     * future snippets): a class that brings its own `operator new` usually hands the work on to such an object.
     * At the start, all slots are free: the constructor pushes them onto the empty list, as `deallocate` would - the
     * last one first, so that slot 0 ends up at the head, and the chain runs slot 0, 1, ... to `nullptr`. No special
     * case for the end: the last slot gets its `nullptr` from the empty list, like the first slot ever given back.
     * The constructor is `constexpr`, and `constinit` says: this variable must be initialized while compiling - if the
     * compiler cannot do that, it is an error, not a constructor call at run time. So the loop runs in the compiler,
     * the finished chain is part of the program's data, and there is no code for it in the program - and no question
     * of whether `connections` is ready before its first `new`.
     * - !![#consteval]
     */
    template <typename T, int N>
    class pool {
        static_assert(N > 0, "a pool needs at least one slot");

        union slot {
            slot* next;
            alignas(T) byte bytes[sizeof(T)];
        };

    public:
        // runs while compiling - see `constinit` below
        constexpr pool() : slots_{}, first_free_{nullptr} {
            for (int i{N - 1}; i >= 0; --i) {
                slots_[i].next = first_free_;
                first_free_ = &slots_[i];
            }
        }

        void* allocate() {
            if (first_free_ == nullptr) throw bad_alloc{};      // guard
            slot* s{first_free_};
            first_free_ = s->next;
            cout << " P| - allocate slot " << s - slots_ << " at " << s << '\n';
            return s;
        }

        void deallocate(void* raw) noexcept {
            if (raw == nullptr) return;                         // guard
            slot* s{static_cast<slot*>(raw)};
            cout << " P| - deallocate slot " << s - slots_ << " back to the free list\n";
            s->next = first_free_;
            first_free_ = s;
        }

    private:
        slot slots_[N];
        slot* first_free_;
    };

    constexpr int max_connections{5};
    constinit pool<connection, max_connections> connections{};     // initialized while compiling, or an error

    /* --- .connection new and delete ---
     * Only a redirection: `connection` says where its memory comes from, the pool manages it. Neither knows the
     * other's business - the pool knows nothing about connections, `connection` nothing about lists.
     */
    void* connection::operator new(const size_t) {        // no name: the size is ignored, see below
        return connections.allocate();
    }

    void connection::operator delete(void* raw) noexcept {
        connections.deallocate(raw);
    }

    /* -- .Kept simple - and what it risks. --
     * The size is ignored: a class derived from `connection` inherits its `operator new` and asks for more bytes than a
     * slot has. A real pool checks `size`, and hands larger requests on to `::operator new`.
     * The pool trusts its users, as every heap does - and the `union` makes that trust cheap and dangerous at once:
     * the bookkeeping lives in the very bytes the users get.
     * - Use after `delete`: the slot is back in the list, its first bytes are the link. Remove the `//` in
     *   `use_a_pool`: `id` is the first 4 bytes of the slot - on x86-64 and ARM64 the lower half of the address in
     *   `next` - so the link becomes something like `0x000055f10000002a`. The next `new` still gets slot 3; the one
     *   after it follows the broken link and reads `next` there - a segmentation fault, exit status 139, inside
     *   `pool::allocate`, far away from the line that did it.
     * - A double `delete`: the slot is in the list twice, and two `new`s return the same slot - two connections in the
     *   same 8 bytes, and nothing crashes.
     * - A foreign address: `deallocate` takes any address for a slot - one from `::new`, or from the middle of a
     *   connection - and the next `new` hands it out.
     * The professional ones spend a little to catch some of it. glibc puts a second value next to the link in a free
     * block, a random key: a block that already carries it is probably freed twice (see previous snippets); and it
     * stores the link scrambled with its own address, so that a stray write like the `42` is usually noticed on the
     * next `malloc` ("safe-linking"). jemalloc keeps the state outside the slots, one bit per slot in a bitmap - a
     * stray write cannot fake it, and a double free is certain. Neither sees the write itself: that takes a check on
     * every access - AddressSanitizer, with a table of its own for every 8 bytes, and freed blocks kept out of use for
     * a while.
     * Chaining all slots at the start is fine for five. A real heap does not do it: writing into every slot makes every
     * page of it resident - the system must give it physical memory (see future snippets) - even if the program never
     * needs most of it. glibc carves new blocks from the untouched rest only when they are asked for, and only blocks
     * that were given back go into its lists.
     * A real connection pool goes one step further: it keeps the connections themselves open and hands them out
     * again. Here, only the memory for them is pooled - and the limit is real.
     */

    /* --- `handle_new_with_pool` ---
     * Five `new`s take all slots: the addresses are `sizeof(connection)` apart, 8 bytes - with glibc, an `int` alone
     * takes 32 (see previous snippets).
     * - !![#new-delete]
     */
    void handle_new_with_pool() {
        print_function_header();

        cout << " 1| get a new connection, sizeof(connection)=" << sizeof(connection) << '\n';
        connection* c1{new connection{1, 8001}};
        connection* c2{new connection{2, 8002}};
        cout << " 2| c1=" << c1 << ", c1->id=" << c1->id << ", c1->port=" << c1->port << '\n';
        cout << " 3| c2=" << c2 << ", c2->id=" << c2->id << ", c2->port=" << c2->port << '\n';
        delete c1;
        connection* c3{new connection{3, 8003}};
        cout << " 4| c3=" << c3 << ", c3->id=" << c3->id << ", c3->port=" << c3->port << '\n';
        delete c2;
        delete c3;
    }

    /* --- more new ---
     *
     * Global `::new` and `::delete`
     * - `::new` starts the lookup in the global namespace, past the class: the global `operator new` gets the bytes.
     * - And it must go back to the global `operator delete`: that is what `::delete` says. Don't confuse them with
     *   the class versions.
     *
     * Placement `new`
     *      void* raw{std::malloc(sizeof(point))};
     *      point* q{::new (raw) point{5, 6}};
     *      ...
     *      q->~point();
     *      std::free(raw);
     * - Both steps by hand: `malloc` for the bytes, placement `new` for the object. The parentheses after `new` are
     *   extra arguments for `operator new`: `new (raw) point{5, 6}` calls `operator new(sizeof(point), raw)`. The
     *   version in `<new>` allocates nothing, it just returns `raw` - so the constructor runs at exactly that address.
     * - There is no `delete` for it: the destructor is called by hand, the one place where you do that, and the bytes
     *   go back to whoever owns them - here, to `free`. The bytes can come from anywhere: a slot of the pool above
     *   would do.
     * - !![#placement-new]
     */
}

/* --- `main` --- */
int main() {
    recall_new_and_delete();
    handle_new_with_malloc();
    handle_new_with_pool();

    return EXIT_SUCCESS;
}
