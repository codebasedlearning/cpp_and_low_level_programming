// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `++counter` is three steps in the machine: load, add, store. Two threads can interleave them - and an increment
 *   is lost.
 * - As Release, x86-64 makes it one instruction, `add` to memory - still three steps inside the core, and still not
 *   atomic. ARM64 keeps the three: it can only compute in registers.
 * - A data race - two threads, the same variable, at least one writes, no synchronization - is undefined behavior.
 *   The compiler may assume that it does not happen: it turns a loop of increments into one `add`, and removes a
 *   loop that waits for a plain `bool`.
 * - So the results of a race are not "sometimes wrong": they depend on the compiler, the options, the processor, and
 *   the moment. `volatile` does not help.
 * - Every line with undefined behavior below is commented out. Remove the `//` one at a time, run it as Debug and as
 *   Release - and put the `//` back.
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::thread;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `counter`, `increment`, `count_up`, `data`, `ready` and `wait_for_ready` are for Compiler Explorer, below.
 */

/* --- `counter`, `increment` and `count_up` --- A plain `int`, counted up by one, and by `n`. */
int counter{0};

void increment() {
    ++counter;
}

void count_up(const int n) {
    for (int i{0}; i < n; ++i) {
        ++counter;
    }
}

/* --- `data`, `ready` and `wait_for_ready` ---
 * A value and a flag, both plain: one thread writes `data`, then sets `ready`; the other waits until `ready` is set,
 * then reads `data`.
 */
int data{0};
bool ready{false};

int wait_for_ready() {
    while (!ready) {}
    return data;
}

namespace {

    /* --- `count_in_one_thread` --- A million increments, one thread: a million. */
    void count_in_one_thread() {
        print_function_header();

        counter = 0;
        count_up(1'000'000);
        cout << " 1| counter=" << counter << '\n';
    }

    /* --- `count_in_two_threads` ---
     * Two threads, a million increments each - on two cores, at the same time, with the same `counter`. Remove the
     * `//`s.
     * - As Debug: less than two million, a different number in every run - ours were between 1.0 and 1.4 million on
     *   x86-64 and ARM64, and once in a while exactly two million, when the threads happened not to overlap. See below
     *   for why.
     * - As Release: two million, every time. Correct? No - the compiler turned each loop into one `add` of a million
     *   (see below): two additions instead of two million, and the chance that they overlap is small. The race is
     *   still there; it only became rare. A program with a rare race is worse than one with a frequent one: it passes
     *   the tests.
     * - !![#race-condition]
     */
    void count_in_two_threads() {
        print_function_header();

        counter = 0;
        // thread a{count_up, 1'000'000};
        // thread b{count_up, 1'000'000};
        // a.join();                        // undefined behavior: a data race on `counter`
        // b.join();
        cout << " 1| counter=" << counter << '\n';

        /* -- .Q&A -- !![Two threads, a million `++counter` each: what is the smallest possible result?](#a-1003) */
    }

}

/* --- The machine code of `++counter` ---
 * Paste `counter` to `wait_for_ready` into Compiler Explorer.
 * - `increment`, x86-64 gcc, `-O0`:
 *       mov     eax, DWORD PTR counter[rip]
 *       add     eax, 1
 *       mov     DWORD PTR counter[rip], eax
 *   Load into a register, add, store. Two threads on two cores: both load 41, both add 1, both store 42 - two
 *   increments, and the counter went up by one. Nothing in these instructions stops the other core in between.
 * - `increment`, `-O2`: `add DWORD PTR counter[rip], 1` (clang: `inc`) - one instruction. But inside the core, it is
 *   still a load, an add and a store, and another core can load between the load and the store. One instruction is
 *   not one step - only a `lock` in front of it makes it one (see the next snippet).
 * - ARM64 gcc, `-O2`: `ldr w0, [x1, ...]`, `add w0, w0, 1`, `str w0, [x1, ...]` - three instructions, always: ARM64
 *   computes only in registers, a load-store architecture.
 * - `count_up`, `-O2`, both: no loop - one load, one `add` of `n`, one store. Allowed: without a race, nobody could see
 *   the difference - and a race, the compiler may assume, does not happen.
 */

namespace {

    /* --- `wait_for_a_plain_flag` ---
     * A second thread waits for `ready`, then reads `data`; `main` sleeps, writes 42 to `data`, and sets `ready`.
     * Remove the `//`s.
     * - As Debug: 42 - the loop loads `ready` again and again, until it is `true`.
     * - As Release: 0, at once - the waiting thread does not wait at all. Look at `wait_for_ready` in Compiler
     *   Explorer, `-O2`: gcc and clang make it `ret`, and the loop is gone. The reasoning: no other thread may write
     *   `ready` while this one reads it - that would be a race. So `ready` cannot change during the loop: if it is
     *   `false`, the loop never ends. But a loop without side effects must end, says the standard - so `ready` must be
     *   `true`, and the loop has nothing to do. Every step is legal; the result is nonsense - because the program broke
     *   the rule first.
     * - With `volatile bool ready`, the loop loads `ready` every time, and on x86-64 the program seems to work. It is
     *   still a race, and still undefined behavior: `volatile` says "this memory may change by itself" - a device
     *   register, not another thread. It does not make an access atomic, and it does not order the write of `data`
     *   before the write of `ready` - on ARM64, the other core may see them in the other order.
     */
    void wait_for_a_plain_flag() {
        print_function_header();

        data = 0;
        ready = false;
        int seen{-1};
        // thread waiter{[&seen] { seen = wait_for_ready(); }};
        // std::this_thread::sleep_for(std::chrono::milliseconds{100});
        // data = 42;
        // ready = true;                    // undefined behavior: a data race on `ready` and `data`
        // waiter.join();
        cout << " 1| the waiting thread saw data=" << seen << '\n';
    }

}

/* --- Data races ---
 * Two threads, the same memory location, at least one of them writes, and nothing orders the accesses: a data race,
 * and undefined behavior - not "a wrong number now and then". The compiler optimizes every thread as if it were alone,
 * and the processor may reorder loads and stores as long as its own thread cannot tell. What orders them: an atomic,
 * a mutex - both in the next snippet - or not sharing at all. A tool finds races while the program runs:
 * ThreadSanitizer, see the follow-up.
 */

/* --- `main` --- */
int main() {
    count_in_one_thread();
    count_in_two_threads();
    wait_for_a_plain_flag();

    return EXIT_SUCCESS;
}
