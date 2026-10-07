// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A processor does not do its loads and stores in the order of the program - it only makes sure that its own
 *   thread cannot tell. Another thread can.
 * - x86-64 changes little: a store may wait in a buffer while a later load goes ahead. ARM64 changes much more.
 * - The memory order of an atomic operation says how much order you need: `relaxed` - none but the operation itself,
 *   `release`/`acquire` - what was written before the release is seen after the acquire, `seq_cst` - one order of all
 *   atomic operations, for all threads.
 * - The store-buffer test: two threads, two variables - and with `release`/`acquire`, both threads may read the old
 *   value. Only `seq_cst` forbids it - and pays with `xchg`.
 * - `ldar` and `stlr` on ARM64 are acquire and release in one instruction.
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::atomic, std::thread, std::memory_order;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `value`, `flag` and the four functions after them are for Compiler Explorer, below.
 */

/* --- `value`, `flag`, `publish` and `consume` --- Message passing: a value, and a flag that says it is there. */
int value{0};
atomic<bool> flag{false};

void publish(const int v) {
    value = v;
    flag.store(true, std::memory_order_release);
}

int consume() {
    while (!flag.load(std::memory_order_acquire)) {}
    return value;
}

/* --- `store_seq_cst` and `store_relaxed` --- The same store, with two orders. */
void store_seq_cst() {
    flag.store(true);
}

void store_relaxed() {
    flag.store(true, std::memory_order_relaxed);
}

namespace {

    /* --- `x`, `y`, `arrived` and `finished` --- Two variables for the test, and two counters to run it in rounds. */
    atomic<int> x{0};
    atomic<int> y{0};
    atomic<int> arrived{0};
    atomic<int> finished{0};
    int seen_by_b{0};

    /* --- `wait_for_each_other` --- Both threads count up `arrived`, then wait until the other one has, too. */
    void wait_for_each_other(const int round) {
        arrived.fetch_add(1);
        while (arrived.load() < 2 * round) {}
    }

    /* --- `count_both_old` ---
     * The store-buffer test, `rounds` times. Thread A: `x = 1`, then read `y`. Thread B: `y = 1`, then read `x`. Both
     * start at the same moment. In any interleaving of the four steps, at least one thread writes before the other
     * reads - so at least one of them must see a 1. Unless a store waits in its core's store buffer, while the load
     * after it already runs: then both read 0. Returns how often that happened.
     */
    template <memory_order store_order, memory_order load_order>
    int count_both_old(const int rounds) {
        arrived = 0;
        finished = 0;
        thread b{[rounds] {
            for (int round{1}; round <= rounds; ++round) {
                wait_for_each_other(round);
                y.store(1, store_order);
                seen_by_b = x.load(load_order);
                finished.store(round);
            }
        }};
        int both_old{0};
        for (int round{1}; round <= rounds; ++round) {
            x.store(0, std::memory_order_relaxed);
            y.store(0, std::memory_order_relaxed);
            wait_for_each_other(round);
            x.store(1, store_order);
            const int seen_by_a{y.load(load_order)};
            while (finished.load() != round) {}
            if (seen_by_a == 0 && seen_by_b == 0) {
                ++both_old;
            }
        }
        b.join();
        return both_old;
    }

    /* --- `run_the_store_buffer_test` ---
     * A million rounds for each order. Run it as Release.
     * - `relaxed` and `release`/`acquire`: both threads see 0 - on our x86-64 machine anywhere between a thousand and
     *   a quarter of a million times in a million, depending on the compiler and the timing. The store is in the
     *   buffer, and the load does not wait for it - x86-64 allows exactly this one reordering. On ARM64 it is allowed,
     *   too; on our Apple silicon Mac, a few hundred times with `relaxed`, and not at all with `release`/`acquire` in
     *   our runs - the window is narrow, and the timing decides.
     * - `seq_cst`: never. The store is an `xchg` now, which waits until the buffer is empty. That is the price of the
     *   default order - and the reason it is the default: it is the only one that behaves as you would expect.
     */
    void run_the_store_buffer_test() {
        print_function_header();

        constexpr int rounds{1'000'000};
        cout << " 1| relaxed:         both saw 0 in "
             << count_both_old<std::memory_order_relaxed, std::memory_order_relaxed>(rounds) << " rounds\n";
        cout << " 2| release/acquire: both saw 0 in "
             << count_both_old<std::memory_order_release, std::memory_order_acquire>(rounds) << " rounds\n";
        cout << " 3| seq_cst:         both saw 0 in "
             << count_both_old<std::memory_order_seq_cst, std::memory_order_seq_cst>(rounds) << " rounds\n";
    }

    /* --- `pass_a_message` ---
     * `publish` and `consume` in two threads: the release store of `flag` and the acquire load that sees it make the
     * write of `value` visible - `value` itself is a plain `int`. That is the pattern of the session's flag, with the
     * weakest order that is still correct.
     */
    void pass_a_message() {
        print_function_header();

        int received{0};
        thread reader{[&received] { received = consume(); }};
        publish(42);
        reader.join();
        cout << " 1| received " << received << '\n';
    }

}

/* --- The instructions ---
 * Paste `value` to `store_relaxed` into Compiler Explorer, `-O2`.
 * - x86-64 gcc: `publish` is two plain `mov`s - a release store needs nothing special on x86-64, stores are not
 *   reordered with each other. `consume` loads `flag` with a plain `movzx`. `store_seq_cst` is `xchg` - the store that
 *   waits for the store buffer. `store_relaxed`: `mov`.
 * - ARM64 gcc: `publish` is `str` for `value`, then `stlrb` - store-release - for the flag. `consume` loads with
 *   `ldarb` - load-acquire. `store_seq_cst` is `stlrb` as well; `store_relaxed` a plain `strb`. ARM64 needs the special
 *   instructions for acquire and release, and gets `seq_cst` from them without an extra barrier.
 * So on x86-64, only `seq_cst` stores cost extra; on ARM64, every order above `relaxed` does. Code that is correct only
 * by the grace of x86-64 breaks on ARM64 - and every phone, and every newer Mac, is ARM64.
 */

/* --- `main` --- */
int main() {
    pass_a_message();
    run_the_store_buffer_test();

    return EXIT_SUCCESS;
}
