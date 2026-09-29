// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The destructor runs exactly at the closing brace - no garbage collector, no delay.
 * - Objects die in reverse order of their birth; members after the destructor body, in reverse declaration order.
 * - A temporary dies at the end of the full expression - at the `;`.
 * - An exception unwinds the stack: frame by frame, every local object is destroyed on the way to the `catch`.
 * - If a constructor throws, the object never existed - only its members built so far are destroyed.
 * - RAII: tie the cleanup to a destructor, and it cannot be forgotten.
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::string, std::vector;
using std::runtime_error;


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

    /* --- `global` ---
     * An object outside of any function. It is born before `main` starts - look at the first lines of the output - and
     * buried after `main` has returned.
     * - !![#lifetime]
     */
    const tracer global{"global"};

    /* --- `show_the_global` ---
     * `global` does not live on the stack: its memory is set aside when the program is loaded, for the whole run -
     * static storage. Compare the addresses: far away from the stack. More on `static`: see future snippets.
     */
    void show_the_global() {
        print_function_header();

        const tracer local{"local"};
        cout << " 1| &global=" << &global << ", &local=" << &local << '\n';
    }

    /* --- `die_at_the_brace` ---
     * Local objects live on the stack, and they die in reverse order - last born, first buried. There is no garbage
     * collector: the compiler inserts the destructor calls at the `}`, where you can find them in the machine code.
     * - !![#destructor]
     */
    void die_at_the_brace() {
        print_function_header();

        const tracer a{"a"};
        {
            const tracer b{"b"};
            const tracer c{"c"};
            cout << " 1| end of the inner block\n";
        }
        cout << " 2| end of function\n";
    }

    /* --- `pair_of_tracers` --- Two members, and a destructor with a body of its own. */
    class pair_of_tracers {
    public:
        pair_of_tracers() : first_{"first"}, second_{"second"} {
            cout << " c|   pair: constructor body\n";
        }

        ~pair_of_tracers() {
            cout << " d|   pair: destructor body\n";
        }

    private:
        tracer first_;
        tracer second_;
    };

    /* --- `die_in_reverse` ---
     * Birth: the members in declaration order, then the constructor body. Death: exactly the other way round - first
     * the destructor body, which may still use the members, then the members, the last one first.
     */
    void die_in_reverse() {
        print_function_header();

        const pair_of_tracers p{};
        cout << " 1| end of function\n";
    }

    /* --- `die_at_the_semicolon` ---
     * A temporary lives until the end of the full expression it was created in. Then it is destroyed - before the next
     * line runs.
     * - !![#temporary]
     */
    void die_at_the_semicolon() {
        print_function_header();

        const size_t length{tracer{"temporary"}.name().size()};
        cout << " 1| length=" << length << " - the temporary is gone already\n";

        // const string& name{tracer{"temporary"}.name()};  // dangles after the `;` - reading `name` is UB
    }

    /* --- `level_1`, `level_2`, `level_3` --- Three frames, each with a local object; the deepest one throws. */
    void level_3() {
        const tracer t3{"t3"};
        throw runtime_error{"thrown in level_3"};
    }

    void level_2() {
        const tracer t2{"t2"};
        level_3();
        cout << " e|   never printed\n";
    }

    void level_1() {
        const tracer t1{"t1"};
        level_2();
        cout << " f|   never printed\n";
    }

    /* --- `unwind_the_stack` ---
     * `throw` leaves `level_3`, `level_2` and `level_1` without finishing them - but not without cleaning up: on the
     * way to the `catch`, every frame is removed and every local object in it destroyed. That is stack unwinding.
     * - !![#stack-unwinding]
     */
    void unwind_the_stack() {
        print_function_header();

        try {
            level_1();
        } catch (const runtime_error& e) {
            cout << " 1| caught: " << e.what() << '\n';
        }

        /* -- .Watch it in the debugger. --
         * Set a breakpoint in the destructor of `tracer`, run until `t3` is destroyed, and look at the frames list:
         * `~tracer`, `level_3`, `level_2`, ... - no trace of the `throw`. The runtime has already found the cleanup
         * code the compiler generated for `level_3`, and jumped there; the debugger marks it at the `}` of `level_3`.
         * Continue: the frame of `level_3` is gone, the next destructor is called from `level_2`.
         */
    }

    /* --- `address` --- Its constructor throws after both members are built. */
    class address {
    public:
        address(const string& street, const string& city) : street_{street}, city_{city} {
            cout << " g|   address: constructor body\n";
            if (street == "Baker Street") {
                throw runtime_error{"Baker Street is fiction"};
            }
        }

        ~address() {
            cout << " h|   address: destructor body\n";
        }

    private:
        tracer street_;
        tracer city_;
    };

    /* --- `throw_in_a_constructor` ---
     * If a constructor throws, the object never existed, so its destructor does not run. But the members built so far
     * are real objects - they are destroyed, in reverse order. Nothing leaks.
     * - !![#constructor]
     */
    void throw_in_a_constructor() {
        print_function_header();

        try {
            const address a{"Baker Street", "London"};
            cout << " 1| never printed\n";
        } catch (const runtime_error& e) {
            cout << " 2| caught: " << e.what() << '\n';
        }

        /* -- .And a destructor that throws? --
         * During stack unwinding, a second exception has nowhere to go - the program ends with `std::terminate`. That
         * is why destructors must not throw; they are `noexcept` without you writing it. To try it: see future
         * snippets.
         */
    }

    /* --- `scope_timer` ---
     * Starts a `stopwatch` when it is born and prints the elapsed time when it dies - wherever and however the scope is
     * left.
     * - !![#raii]
     */
    class scope_timer {
    public:
        explicit scope_timer(const string& label) : label_{label} {}

        // A copy would print a second time for the same scope. `= delete` removes the copy operations - copying a
        // `scope_timer` is a compiler error. More on them: see future snippets.
        scope_timer(const scope_timer&) = delete;
        scope_timer& operator=(const scope_timer&) = delete;

        ~scope_timer() {
            cout << " i|   " << label_ << ": " << watch_.elapsed_ms() << " ms\n";
        }

    private:
        string label_;
        stopwatch watch_{};
    };

    /* --- `sum_up` --- Some work - and, if asked to, an error at the end. */
    long long sum_up(const vector<int>& v, const bool fail) {
        long long sum{0};
        for (const int x : v) {
            sum += x;
        }
        if (fail) {
            throw runtime_error{"failed after sum=" + std::to_string(sum)};
        }
        return sum;
    }

    /* --- `clean_up_with_raii` ---
     * The timer prints in both cases - the normal end of the block and the exception. The cleanup lives next to the
     * resource, not at every exit. That is RAII, Resource Acquisition Is Initialization: `string`, `vector` and every
     * other class that owns something work this way.
     */
    void clean_up_with_raii() {
        print_function_header();

        const vector<int> v(10'000'000, 1);
        {
            const scope_timer timer{"sum without error"};
            const long long sum{sum_up(v, false)};
            cout << " 1| sum=" << sum << '\n';
        }
        try {
            const scope_timer timer{"sum with error"};
            const long long sum{sum_up(v, true)};
            cout << " 2| never printed: sum=" << sum << '\n';
        } catch (const runtime_error& e) {
            cout << " 3| caught: " << e.what() << '\n';
        }

        /* -- .Q&A -- !![Java has `finally` - why does C++ not need it?](#a-304) */
    }

}

/* --- `main` --- */
int main() {
    show_the_global();
    die_at_the_brace();
    die_in_reverse();
    die_at_the_semicolon();
    unwind_the_stack();
    throw_in_a_constructor();
    clean_up_with_raii();

    return EXIT_SUCCESS;
}                                           // `global` is destroyed after this
