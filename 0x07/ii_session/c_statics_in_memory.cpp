// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A `static` data member belongs to the class: one variable for all objects. It is not in the object - `sizeof` does
 *   not change - but in static storage, next to the global variables.
 * - One per class - and one per instantiation of a class template.
 * - A `static` member function has no `this`: the machine code of a free function.
 * - A `static` local variable lives in static storage, too. With a constant initializer, it is ready before `main` - a
 *   global variable with a local name. Otherwise it is initialized at the first call, and a guard variable is tested
 *   at every call.
 * - A `static` object is destroyed after `main` returns.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `registry`, `angle`, `radians_of`, `to_radians_of`, `next_number`, `compute_seed` and `seed` are for `nm` and
 * Compiler Explorer, below - as in previous snippets, they keep their own names in the object file.
 */

/* --- `registry` --- A class template with a static data member. */
template <typename T>
struct registry {
    inline static int count{0};
};

/* --- `angle` ---
 * An angle in degrees. `radians()` is a member function, `to_radians` a static one. `pi_over_180` is a static
 * constant: not in the object either, the compiler puts the value where it is used.
 */
class angle {
public:
    explicit angle(const double degrees) : degrees_{degrees} {}

    double radians() const { return degrees_ * pi_over_180; }

    static double to_radians(const double degrees) { return degrees * pi_over_180; }

private:
    static constexpr double pi_over_180{0.017453292519943295};
    double degrees_;
};

/* --- `radians_of` and `to_radians_of` --- The same computation, through the member and through the static function. */
double radians_of(const angle& a) {
    return a.radians();
}

double to_radians_of(const double degrees) {
    return angle::to_radians(degrees);
}

/* --- `next_number` ---
 * A `static` local variable: created once, it keeps its value from call to call. It is initialized with a constant,
 * 0.
 */
int next_number() {
    static int last{0};
    return ++last;
}

/* --- `compute_seed` and `seed` ---
 * A `static` local variable with an initializer that is not a constant: the value is known only when `compute_seed`
 * has run - at the first call of `seed`.
 */
int compute_seed() {
    cout << " a|   compute_seed runs\n";
    return 23;
}

int seed() {
    static const int value{compute_seed()};
    return value;
}

namespace {

    /* --- `ticket` ---
     * Every ticket gets the next number. The counters belong to the class, not to a ticket: `inline static` data
     * members (C++17), defined in the class. `live` counts the tickets that exist right now. They are public only to
     * show their addresses below - normally they would be private.
     * A ticket is unique: no copies.
     * - !![#static-member]
     */
    class ticket {
    public:
        ticket() : number_{next} {
            ++next;
            ++live;
        }

        ~ticket() {
            --live;
        }

        ticket(const ticket&) = delete;
        ticket& operator=(const ticket&) = delete;

        int number() const { return number_; }

        inline static int next{1};
        inline static int live{0};

    private:
        int number_;
    };

    /* --- `count_tickets` ---
     * The basics first: every constructor counts up, every destructor down - one variable for all tickets.
     * `ticket::live` needs no object.
     */
    void count_tickets() {
        print_function_header();

        const ticket a;
        {
            const ticket b;
            const ticket c;
            cout << " 1| c.number()=" << c.number() << ", live=" << ticket::live << '\n';
        }
        cout << " 2| a.number()=" << a.number() << ", live=" << ticket::live << ", next=" << ticket::next << '\n';
    }

    /* --- `global_value` --- Outside of any function: static storage, see previous snippets. */
    int global_value{1};

    /* --- `show_where_statics_live` ---
     * `sizeof(ticket)` is 4: its `int` number, nothing else. The counters are not in the object - and not on the stack
     * either. `ticket::live` and `ticket::next` have their addresses near `global_value`, far from `a` and `b` on the
     * stack: static storage, from the start of the program to its end. Through `a` or `b`, it is the same variable -
     * one address.
     */
    void show_where_statics_live() {
        print_function_header();

        const ticket a;
        const ticket b;
        cout << " 1| sizeof(ticket)=" << sizeof(ticket) << '\n';
        cout << " 2| &a=" << &a << ", &b=" << &b << '\n';
        cout << " 3| &ticket::live=" << &ticket::live << ", &a.live=" << &a.live << ", &b.live=" << &b.live << '\n';
        cout << " 4| &ticket::next=" << &ticket::next << ", &global_value=" << &global_value << '\n';
    }

    /* --- `count_per_type` ---
     * A template generates code per type (see previous snippets) - and a static data member per type, too.
     * `registry<int>::count` and `registry<double>::count` are two variables with two addresses.
     */
    void count_per_type() {
        print_function_header();

        registry<int>::count = 3;
        registry<double>::count = 5;
        cout << " 1| registry<int>::count=" << registry<int>::count << " at " << &registry<int>::count << '\n';
        cout << " 2| registry<double>::count=" << registry<double>::count << " at " << &registry<double>::count << '\n';
    }

}

/* --- The names in the object file ---
 * `nm -C` on the object file of this snippet lists `registry<int>::count` and `registry<double>::count` - two
 * symbols. With gcc their letter is `u`, with clang `V`: every file that uses them has a copy, and the linker keeps
 * one - as with the `W` of an `inline` function (see previous snippets). `inline static` works the same way.
 * Without `inline`, a static data member is only declared in the class, `static int count;`, and needs a definition
 * in exactly one `.cpp` file: `int ticket::count{0};`. That file gets a `B` (or `D`, if the value is not 0) - and if
 * nobody writes the definition, the linker reports an `undefined reference` (see previous snippets).
 * The locals are there, too: `next_number()::last` with a `b` - local to the file, in the zero-initialized part of
 * static storage - and `seed()::value` together with `guard variable for seed()::value`.
 */

namespace {

    /* --- `call_without_this` ---
     * `a.radians()` passes `a` as `this`. `angle::to_radians(90.0)` passes only the `double`: a static member function
     * has no object. It can be called without one, and it cannot use `degrees_`.
     */
    void call_without_this() {
        print_function_header();

        const angle a{90.0};
        cout << " 1| a.radians()=" << radians_of(a) << ", angle::to_radians(90.0)=" << to_radians_of(90.0) << '\n';
    }

    /* --- `count_the_calls` --- Three calls, one variable. */
    void count_the_calls() {
        print_function_header();

        const int first{next_number()};
        const int second{next_number()};
        const int third{next_number()};
        cout << " 1| " << first << ", " << second << ", " << third << '\n';
    }

    /* --- `initialize_once` ---
     * `compute_seed` runs once - at the first call of `seed`, not before `main`, and not at the second call. `value`
     * keeps the result.
     */
    void initialize_once() {
        print_function_header();

        cout << " 1| first call\n";
        const int first{seed()};
        cout << " 2| second call\n";
        const int second{seed()};
        cout << " 3| " << first << ", " << second << '\n';

        /* -- .Q&A -- !![The guard is tested at every call of `seed`. How do you keep that out of a loop?](#a-708) */
    }

    /* --- `announcer` --- Reports its birth and its death. */
    class announcer {
    public:
        explicit announcer(const string& name) : name_{name} {
            cout << " b|   " << name_ << ": constructed\n";
        }

        ~announcer() {
            cout << " c|   " << name_ << ": destroyed\n";
        }

        const string& name() const { return name_; }

    private:
        string name_;
    };

    /* --- `shared_announcer` --- A `static` local object: constructed at the first call. */
    const announcer& shared_announcer() {
        static const announcer shared{"shared"};
        return shared;
    }

    /* --- `outlive_main` ---
     * A reference to a `static` local is safe to return: the object is not in the frame of `shared_announcer`, it is
     * in static storage. It lives to the end of the program - even longer than `main`: its destructor runs after
     * `main` has returned. Look at the last line of the output - and at the addresses: one object, near the other
     * static variables.
     */
    void outlive_main() {
        print_function_header();

        const announcer& first{shared_announcer()};
        cout << " 1| first: " << first.name() << " at " << &first << '\n';
        const announcer& second{shared_announcer()};
        cout << " 2| second: " << second.name() << " at " << &second << '\n';
    }

}

/* --- The machine code of `static` ---
 * Paste `angle`, `radians_of`, `to_radians_of`, `next_number` and `seed` into Compiler Explorer - `compute_seed` only
 * as a declaration, `int compute_seed();`, so that the compiler cannot look into it. x86-64 gcc:
 * - `radians_of`, `-O0`: the call of `angle::radians() const` gets the address of `a` in `rdi` - that is `this` - and
 *   `radians` loads `degrees_` from `[rax]`. `angle::to_radians(double)` gets the `double` in `xmm0`, and nothing
 *   else. With `-O2`, `to_radians_of` is one `mulsd` - as a free function that multiplies would be.
 * - `next_number`, `-O2`: `mov eax, DWORD PTR next_number()::last[rip]`, `add eax, 1`, `mov` it back - the instructions
 *   for a global variable. No test, no call: the 0 is in place before `main` starts.
 * - `seed`, `-O2`: first `movzx eax, BYTE PTR guard variable for seed()::value[rip]` and `test al, al` - is `value`
 *   initialized? If yes, load it and return. If not, `call __cxa_guard_acquire`, `call compute_seed()`, store,
 *   `call __cxa_guard_release`. The guard makes the initialization thread-safe (C++11): if two threads call `seed`
 *   at the same time, one computes, the other waits.
 * - On ARM64 (ARM64 gcc), the guard is read with `ldar`, a load-acquire: whatever the initializing thread wrote
 *   before it set the guard is visible after it. On x86-64, every load does that anyway - so `movzx` is enough.
 * - A static local with a destructor, like `shared` in `shared_announcer`, also gets a `call __cxa_atexit`: it
 *   registers the destructor, to run after `main`.
 * So the first call pays for the initialization, and every call pays for a load and a branch.
 * - !![#static-storage]
 */

/* --- `main` --- */
int main() {
    count_tickets();
    show_where_statics_live();
    count_per_type();
    call_without_this();
    count_the_calls();
    initialize_once();
    outlive_main();

    return EXIT_SUCCESS;
}
