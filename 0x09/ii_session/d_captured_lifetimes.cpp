// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A lambda is an object, and it can live longer than the function that made it - returned, stored in a
 *   `std::function`, handed to a list of callbacks. It carries what it captured.
 * - A captured copy travels with it - safe. A captured reference is an address, and the lambda keeps it after the
 *   variable is gone: a dangling pointer, with nicer syntax.
 * - `[this]` is the address of an object. If the object moves - a `vector` that grows - the lambda still points to
 *   the old place.
 * - An init-capture can move a `unique_ptr` into a lambda: the lambda owns it, and cannot be copied any more.
 * - Every line with undefined behavior below is commented out. Remove the `//` one at a time, run it as Debug and as
 *   Release - and put the `//` back.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <utility>                          // for move
#include <version>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::vector, std::function, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

namespace {

    /* --- `make_multiplier` ---
     * Returns a lambda - by value, as any other object. `auto` is the only way to write the return type. The lambda
     * holds a copy of `factor`: when the function is gone, the copy is still there, inside the returned object.
     */
    auto make_multiplier(const int factor) {
        return [factor](const int x) { return x * factor; };
    }

    /* --- `return_a_lambda` ---
     * `times3` is an object of the lambda's class, 4 bytes: its copy of `factor`. `times5` is a second object of the
     * same class - both lambdas come from the same lambda expression - with another copy.
     */
    void return_a_lambda() {
        print_function_header();

        const auto times3 = make_multiplier(3);
        const auto times5 = make_multiplier(5);
        cout << " 1| times3(7)=" << times3(7) << ", times5(7)=" << times5(7) << ", sizeof(times3)=" << sizeof(times3)
             << '\n';
    }

    /* --- `make_dangling_multiplier` ---
     * The same with `[&factor]`: the lambda holds the address of the parameter `factor` - a place in the frame of
     * `make_dangling_multiplier`. The frame is gone when the function returns, and the address in the lambda points to
     * whatever the next call puts there. The pointer version of this bug was in unit 0x05 - the same bug, but hidden
     * in a `&` between two brackets.
     * clang warns: "address of stack memory associated with parameter 'factor' returned". gcc says nothing here -
     * with `-O2` and `-Wall`, it warns where the lambda is called: "'factor' is used uninitialized".
     * - !![#dangling-pointer]
     */
    // auto make_dangling_multiplier(const int factor) {
    //     return [&factor](const int x) { return x * factor; };
    // }

    /* --- `return_a_dangling_lambda` --- Remove the `//`s here and above. */
    void return_a_dangling_lambda() {
        print_function_header();

        // const auto times3 = make_dangling_multiplier(3);
        // cout << " 1| times3(7)=" << times3(7) << '\n';     // undefined behavior: `factor` is gone
        cout << " 2| a lambda that captured a reference must not outlive what it refers to\n";

        /* -- .Q&A -- !![Remove the `//`s: what do gcc and clang print, as Debug and as Release?](#a-906) */
    }

    /* --- `counter` ---
     * An object that hands out a callback: `on_click` returns a lambda that counts the clicks of this very counter.
     * `[this]` captures the address of the object - `clicks_` in the body is `this->clicks_`.
     */
    class counter {
    public:
        function<void()> on_click() {
            return [this] { ++clicks_; };
        }

        int clicks() const { return clicks_; }

    private:
        int clicks_{0};
    };

    /* --- `capture_this` ---
     * The callbacks are stored in a list, as a button would store them, and called later. As long as the counters stay
     * where they are, every click reaches its counter.
     * Then a third counter is added, and the `vector` grows: it allocates a new block, moves the counters there, and
     * frees the old block (see previous snippets). The callbacks still hold the old addresses. A click now writes into
     * freed memory - where the heap keeps its list of free blocks, or into a block it has handed out since: unnoticed,
     * or a crash much later, somewhere else.
     * The object in a callback must stay where it is, as long as the callback can be called: `reserve` in advance, own
     * the objects through `unique_ptr` (the object stays, only the pointer moves), or make the class impossible to copy
     * or move - see the follow-up.
     */
    void capture_this() {
        print_function_header();

        vector<counter> counters(2);
        vector<function<void()>> callbacks;
        callbacks.push_back(counters[0].on_click());
        callbacks.push_back(counters[1].on_click());
        callbacks[0]();
        callbacks[1]();
        callbacks[1]();
        const void* registered{&counters[0]};
        cout << " 1| clicks: " << counters[0].clicks() << ", " << counters[1].clicks() << ", &counters[0]="
             << registered << '\n';

        counters.emplace_back();
        cout << " 2| after the vector has grown: &counters[0]=" << &counters[0] << ", the callback still points to "
             << registered << '\n';
        // callbacks[0]();                  // undefined behavior: writes to the freed block

        /* -- .Q&A -- !![In a member function, `[=] { return clicks_; }` - what does the lambda copy?](#a-907) */
    }

    /* --- `move_into_a_lambda` ---
     * An init-capture can move: `[p = std::move(p)]` creates a member `p` in the lambda and moves the `unique_ptr`
     * into it - the lambda owns the `int` now, and the old `p` is empty. The `int` lives as long as the lambda does,
     * and is deleted at its `}`.
     * A `unique_ptr` cannot be copied, so neither can the lambda. And `std::function` must be able to copy what it
     * holds - it refuses a lambda like this. C++23 has `std::move_only_function` for it; libstdc++ has it since gcc 12,
     * libc++ not yet in version 18 - the snippet asks the library first.
     */
    void move_into_a_lambda() {
        print_function_header();

        heap_watch heap{};
        {
            unique_ptr<int> p{make_unique<int>(42)};
            const auto owner = [p = std::move(p)] { return *p; };
            cout << " 1| owner()=" << owner() << ", p is empty: " << (p == nullptr) << ", sizeof(owner)="
                 << sizeof(owner) << '\n';
            // const auto copy = owner;     // compiler error: the copy constructor of the lambda is deleted
            // function<int()> f{owner};    // compiler error: std::function needs a copyable callable
#if defined(__cpp_lib_move_only_function)
            unique_ptr<int> q{make_unique<int>(23)};
            std::move_only_function<int()> f{[q = std::move(q)] { return *q; }};
            cout << " 2| move_only_function: f()=" << f() << '\n';
#else
            cout << " 2| no std::move_only_function in this library\n";
#endif
        }
        cout << " 3| allocations=" << heap.allocations() << ", released=" << heap.releases() << '\n';
    }

    /* --- `capture_a_big_vector` ---
     * `[big]` copies the whole vector into the lambda: 24 bytes in the lambda - the three words of a `vector` - and a
     * copy of the million `int`s on the heap. `[&big]` is an address: 8 bytes, nothing copied - but the lambda must not
     * outlive `big`. `[big = std::move(big)]` moves: the lambda takes over the block, nothing is copied, and `big` is
     * empty afterwards - the lambda owns the data, and can take it wherever it goes. `by_reference` still looks at
     * `big` itself - so it counts 0 now.
     */
    void capture_a_big_vector() {
        print_function_header();

        vector<int> big(1'000'000, 1);
        heap_watch heap{};
        const auto by_copy = [big] { return big.size(); };
        cout << " 1| [big]:            sizeof=" << sizeof(by_copy) << ", allocations=" << heap.allocations()
             << ", bytes=" << heap.bytes() << '\n';
        heap.reset();
        const auto by_reference = [&big] { return big.size(); };
        cout << " 2| [&big]:           sizeof=" << sizeof(by_reference) << ", allocations=" << heap.allocations()
             << '\n';
        heap.reset();
        const auto by_move = [big = std::move(big)] { return big.size(); };
        cout << " 3| [big = move(big)]: sizeof=" << sizeof(by_move) << ", allocations=" << heap.allocations()
             << ", big.size() now " << big.size() << '\n';
        cout << " 4| called: " << by_copy() << ' ' << by_reference() << ' ' << by_move() << '\n';
    }

}

/* --- Keeping lambdas safe ---
 * - Capture by copy what the lambda needs later; capture by reference only if the lambda is used before the `}` of
 *   what it refers to - in an algorithm, say.
 * - Name what you capture: `[factor]`, `[&count]` - not `[=]` or `[&]`, which hide it (see the follow-up).
 * - A lambda that captures `this` depends on the object: the object must not move or die while the lambda can be
 *   called.
 * - Hand ownership to a lambda with an init-capture and `std::move`.
 * - Let the tools look: clang's `-Wreturn-stack-address`, and AddressSanitizer, which reports the click into freed
 *   memory as `heap-use-after-free`.
 */

/* --- `main` --- */
int main() {
    return_a_lambda();
    return_a_dangling_lambda();
    capture_this();
    move_into_a_lambda();
    capture_a_big_vector();

    return EXIT_SUCCESS;
}
