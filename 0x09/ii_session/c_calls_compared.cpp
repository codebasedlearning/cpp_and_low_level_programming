// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Three ways for a function to take "something to call": a template parameter, a function pointer, a `std::function`.
 *   Same source code in the loop, very different machine code.
 * - A template is instantiated once per lambda type - it knows the body, and the compiler inlines it.
 * - A function pointer is an indirect call per element - unless the compiler knows its value.
 * - `std::function` can hold any callable: it keeps a small one inside, a larger one on the heap, and calls it
 *   through a stored pointer - one indirect call per element, and an allocation per copy of a large callable.
 * - Measured: the inlined call is faster - not because the jump is slow, but because inlining removes everything
 *   around it, and lets the optimizer see the whole loop.
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <random>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::vector, std::string, std::function;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The functions up to `sum_triples` are for Compiler Explorer and `nm`, below.
 */

/* --- `sum_with_template`, `sum_with_pointer` and `sum_with_function` ---
 * The same loop three times: call `f` for every element and add up the results. Only the type of `f` differs.
 * - !![#std-function]
 */
template <typename F>
long long sum_with_template(const vector<int>& numbers, const F& f) {
    long long sum{0};
    for (const int x : numbers) {
        sum += f(x);
    }
    return sum;
}

long long sum_with_pointer(const vector<int>& numbers, int (*const f)(int)) {
    long long sum{0};
    for (const int x : numbers) {
        sum += f(x);
    }
    return sum;
}

long long sum_with_function(const vector<int>& numbers, const function<int(int)>& f) {
    long long sum{0};
    for (const int x : numbers) {
        sum += f(x);
    }
    return sum;
}

/* --- `triple` and `twice` --- Two functions for the pointer. */
int triple(const int x) {
    return 3 * x;
}

int twice(const int x) {
    return 2 * x;
}

/* --- `sum_triples` --- The template, instantiated with a lambda. */
long long sum_triples(const vector<int>& numbers) {
    return sum_with_template(numbers, [](const int x) { return 3 * x; });
}

namespace {

    /* --- `instantiate_the_template` ---
     * `sum_with_template` is a recipe (see previous snippets). Every lambda has its own type, so every call with
     * another lambda makes another function: `sum_with_template<{lambda(int)#1}>`, `<{lambda(int)#2}>` - and in each
     * of them, `f(x)` is a call of that very lambda's `operator()`, whose body the compiler knows.
     * With a function pointer, `F` is `int (*)(int)` - one instantiation for all functions of that type, and `f(x)` is
     * an indirect call, as in `sum_with_pointer`. The template is only as good as what it gets.
     */
    void instantiate_the_template() {
        print_function_header();

        const vector<int> numbers{1, 2, 3, 4};
        cout << " 1| triples: " << sum_with_template(numbers, [](const int x) { return 3 * x; }) << ", squares: "
             << sum_with_template(numbers, [](const int x) { return x * x; }) << ", twice: "
             << sum_with_template(numbers, twice) << '\n';
        cout << " 2| sum_with_pointer: " << sum_with_pointer(numbers, triple) << ", sum_with_function: "
             << sum_with_function(numbers, [](const int x) { return 3 * x; }) << '\n';
    }

    /* --- `show_std_function_in_memory` ---
     * A `std::function<int(int)>` can hold any callable - so it cannot know its size in advance. libstdc++ gives it
     * 32 bytes: 16 bytes of storage, and two function pointers - one to call the callable, one to copy and destroy
     * it. A small callable - a function pointer, a lambda with up to 16 bytes of captures that can be copied byte by
     * byte - is stored inside, in the 16 bytes. Anything else is copied into a block on the heap, and the 16 bytes
     * hold its address. Every copy of the `std::function` copies the callable - and for a large one, that is one more
     * allocation.
     * That is the small buffer of unit 0x06 and a table of function pointers of unit 0x08 in one class - "type
     * erasure": the type of the callable is gone, and the two function pointers remember what to do with it.
     * libc++ gives it 48 bytes and room for 24; MSVC 64. Count as Debug.
     * `name` is not `const` here: the lambda's copy of a `const string` would be a `const string`, too - and a `const`
     * member cannot be moved, only copied. The text would be copied once more on its way into the `std::function`.
     */
    void show_std_function_in_memory() {
        print_function_header();

        const double a{1.0};
        const double b{2.0};
        const double c{3.0};
        string name{"a name that is too long for the small string"};
        cout << " 1| sizeof(function<int(int)>)=" << sizeof(function<int(int)>) << '\n';

        heap_watch heap{};
        const function<int(int)> small{[a, b](const int x) { return static_cast<int>(a * x + b); }};
        cout << " 2| [a, b]:       allocations=" << heap.allocations() << '\n';
        heap.reset();
        const function<int(int)> larger{[a, b, c](const int x) { return static_cast<int>(a * x + b * c); }};
        cout << " 3| [a, b, c]:    allocations=" << heap.allocations() << '\n';
        heap.reset();
        const function<int(int)> with_a_name{[name](const int x) { return x + static_cast<int>(name.size()); }};
        cout << " 4| [name]:       allocations=" << heap.allocations() << " - the text, and the lambda\n";
        heap.reset();
        const function<int(int)> copy{with_a_name};
        cout << " 5| a copy of it: allocations=" << heap.allocations() << '\n';
        cout << " 6| called: " << small(1) << ' ' << larger(1) << ' ' << with_a_name(1) << ' ' << copy(1) << '\n';
    }

    /* --- `call_an_empty_function` ---
     * A `std::function` can be empty - it holds nothing, and says so when converted to `bool`. Calling it anyway is not
     * undefined behavior, as for a null function pointer: it throws `std::bad_function_call`. For that, every call
     * checks first - see the machine code below.
     */
    void call_an_empty_function() {
        print_function_header();

        const function<int(int)> nothing;
        cout << " 1| holds something: " << static_cast<bool>(nothing) << '\n';
        try {
            cout << nothing(1);
        } catch (const std::bad_function_call& e) {
            cout << " 2| caught: " << e.what() << '\n';
        }
    }

}

/* --- The three loops in the machine ---
 * Paste everything from `sum_with_template` to `sum_triples` into Compiler Explorer, with `#include <vector>` and
 * `#include <functional>`, x86-64 gcc, `-O2`.
 * - `sum_with_pointer`: in the loop, the element goes to `edi`, then `call rbp` (or another register) - an indirect
 *   call per element, the sum in a register that must survive the call.
 * - `sum_with_function`: a test first - `cmp QWORD PTR [rbp+16], 0`, is the `std::function` empty? - then the element
 *   is stored in the stack frame, its address goes to `rsi`, and `call [QWORD PTR [rbp+24]]` - the call through the
 *   pointer stored at offset 24 of the object. One indirect call per element, plus the test, plus a detour through
 *   memory for the argument.
 * - `sum_triples`: no call at all. The lambda is inlined: `lea edx, [rdx+rdx*2]` - `x + 2 * x` - is all that is left
 *   of it, and the loop is a handful of instructions per element. The instantiation `sum_with_template<...>` has
 *   disappeared into `sum_triples`; with `-O0`, `nm -C` finds it:
 *   `long long sum_with_template<sum_triples(...)::{lambda(int)#1}>(...)`.
 * ARM64 gcc: `blr x21` for the pointer; `ldr x2, [x20, 24]` and `blr x2` for the `std::function`; and for the lambda
 * `add w1, w1, w1, lsl 1` - `x + (x << 1)`, the multiplication by three in one instruction.
 */

namespace {

    /* --- `measure_the_calls` ---
     * Ten million numbers, four ways to triple them and add them up:
     * - a lambda through the template,
     * - a function pointer whose value the compiler knows - `triple`,
     * - a function pointer chosen at run time - `triple` or `twice`, by a random number,
     * - the lambda in a `std::function`.
     * Run it as Release. The lambda is the fastest: inlined, three instructions of work per element. The pointer
     * chosen at run time and the `std::function` take longer - two to three times as long on our x86-64 machines,
     * one and a half times on an Apple silicon Mac: a call per element - the jump there, the jump back, the
     * registers that must survive it, and for `std::function` the test and the detour through memory. The jump is
     * predicted perfectly, it always goes to the same place; it is everything around it that costs. And inlining is
     * what lets the optimizer do more - vectorize a loop, or compute it at compile time.
     * The known pointer depends on the compiler: clang inlines `sum_with_pointer` into this function, sees `triple`,
     * and inlines that, too - as fast as the lambda; gcc keeps the call - as slow as the pointer chosen at run time.
     * The compiler may see through a pointer; with a lambda in a template, it always can.
     * - !![#zero-overhead]
     */
    void measure_the_calls() {
        print_function_header();

        vector<int> numbers(10'000'000);
        std::mt19937 generator{23};
        std::uniform_int_distribution<int> pick{0, 999};
        for (int& n : numbers) {
            n = pick(generator);
        }
        int (*const chosen)(int){pick(generator) < 1000 ? triple : twice};
        const auto lambda = [](const int x) { return 3 * x; };
        const function<int(int)> wrapped{lambda};

        stopwatch watch{};
        const long long by_template{sum_with_template(numbers, lambda)};
        const double template_ms{watch.elapsed_ms()};
        watch.reset();
        const long long by_known_pointer{sum_with_pointer(numbers, triple)};
        const double known_ms{watch.elapsed_ms()};
        watch.reset();
        const long long by_chosen_pointer{sum_with_pointer(numbers, chosen)};
        const double chosen_ms{watch.elapsed_ms()};
        watch.reset();
        const long long by_function{sum_with_function(numbers, wrapped)};
        const double function_ms{watch.elapsed_ms()};

        cout << " 1| template, lambda:       " << template_ms << " ms, sum=" << by_template << '\n';
        cout << " 2| pointer, known:         " << known_ms << " ms, sum=" << by_known_pointer << '\n';
        cout << " 3| pointer, at run time:   " << chosen_ms << " ms, sum=" << by_chosen_pointer << '\n';
        cout << " 4| std::function:          " << function_ms << " ms, sum=" << by_function << '\n';

        /* -- .Q&A -- !![`chosen` is always `triple`, the number is below 1000. Why is it slow anyway?](#a-905) */
    }

}

/* --- `main` --- */
int main() {
    instantiate_the_template();
    show_std_function_in_memory();
    call_an_empty_function();
    measure_the_calls();

    return EXIT_SUCCESS;
}
