// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A function is code, and code is in memory, like data: every function has an address - in the part of memory
 *   where the program's machine code lives.
 * - A function pointer holds that address: 8 bytes. A call through it is an indirect call - the address is taken from
 *   a register, not from the instruction.
 * - A table of function pointers picks the code by a number - the `switch` table and the vtable of the previous units,
 *   written by hand.
 * - C passes behavior as a function pointer: `qsort` calls the comparison through its address, once per comparison.
 * - When the compiler knows the value of a function pointer, the indirect call disappears - the call is direct, and
 *   inlined.
 */

/* --- Warm-up ---
 * Did you work through the preparation? Answer without looking it up.
 * - `capture_by_value` found 12, not 21 - although `limit` was 20 by then. Why?
 * - What did C++ Insights make of `[&count]` - which member, of which type?
 * - Which of the five lambdas in C++ Insights got a conversion to a function pointer?
 * - Your prediction: how many bytes does an object of the class of `sort_descending` take?
 */

#include <iostream>
#include <array>
#include <memory>
#include <random>
#include <cstdlib>                          // for qsort
#include <cbl/printing.hpp>

using std::cout, std::array, std::unique_ptr, std::make_unique;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `square`, `cube`, `call_through`, `call_square` and `apply_square` are for Compiler Explorer, below - as in previous
 * snippets, they keep their own names in the assembly.
 */

/* --- `square` and `cube` --- Two ordinary functions, `double` to `double`. */
double square(const double x) {
    return x * x;
}

double cube(const double x) {
    return x * x * x;
}

/* --- `unary` ---
 * The type "pointer to a function that takes a `double` and returns a `double`". The `(*)` makes it a pointer; without
 * the parentheses, `double* (double)` would be a function that returns a `double*`. With `using`, the type gets a name
 * that can be read.
 * - !![#std-function]
 */
using unary = double (*)(double);

/* --- `call_through`, `call_square` and `apply_square` --- A call through a pointer, a direct call, and a pointer the
 * compiler can see. */
double call_through(const unary f, const double x) {
    return f(x);
}

double call_square(const double x) {
    return square(x);
}

double apply_square(const double x) {
    return call_through(square, x);
}

namespace {

    /* --- `global_value` --- Outside of any function: static storage. */
    int global_value{1};

    /* --- `show_where_functions_live` ---
     * When the program starts, the operating system loads its machine code into memory - into a region that can be
     * read and executed, but not written: the text segment. So a function has an address, like a variable: the address
     * of its first instruction. Here it is next to `global_value` - both come from the program file - and far from the
     * stack and the heap.
     * `cout << square` would not print the address: there is no `<<` for function pointers, but a conversion to
     * `bool` - it prints 1 (gcc and clang warn: the address is never null). So the address is converted to
     * `const void*` first, with a `reinterpret_cast` (see previous snippets) - allowed on every platform with gcc,
     * clang and MSVC, although the standard only calls it "conditionally-supported".
     */
    void show_where_functions_live() {
        print_function_header();

        const int local{0};
        const unique_ptr<int> on_the_heap{make_unique<int>(0)};
        cout << " 1| square: " << reinterpret_cast<const void*>(&square) << ", cube: "
             << reinterpret_cast<const void*>(&cube) << '\n';
        cout << " 2| &global_value=" << &global_value << ", &local=" << &local << ", heap: " << on_the_heap.get()
             << '\n';
        // cout << square;                  // prints 1 - a warning with -Wall
    }

    /* --- `store_a_function_pointer` ---
     * `f` is a variable of type `unary`: 8 bytes, an address - that of `square`, then that of `cube`. `f(3.0)` calls
     * whatever `f` points to at that moment. The name of a function becomes its address by itself - `square` and
     * `&square` are the same, as the name of an array becomes the address of its first element (see previous snippets).
     * A function pointer can be compared, and it can be null - calling a null pointer is undefined behavior.
     */
    void store_a_function_pointer() {
        print_function_header();

        unary f{square};
        cout << " 1| sizeof(f)=" << sizeof(f) << ", f=" << reinterpret_cast<const void*>(f) << ", f(3)=" << f(3.0)
             << '\n';
        f = &cube;
        cout << " 2| f=" << reinterpret_cast<const void*>(f) << ", f(3)=" << f(3.0) << ", f == cube: " << (f == cube)
             << '\n';

        const unary nothing{nullptr};
        cout << " 3| nothing is null: " << (nothing == nullptr) << '\n';
        // nothing(3.0);                    // undefined behavior: a call to address 0
    }

}

/* --- The machine code of a call through a pointer ---
 * Paste `square` to `apply_square` into Compiler Explorer, x86-64 gcc.
 * - `call_square`, `-O0`: `call square(double)` - the address of the function is part of the instruction. The linker
 *   filled it in.
 * - `call_through`, `-O0`: `f` is stored in the stack frame, then `mov rdx, QWORD PTR [rbp-8]` and `call rdx` - the
 *   address comes from a register, loaded from memory. An indirect call, as the virtual call of the previous unit, but
 *   with one load less: there is no object and no table, the pointer is the address.
 * - `call_through`, `-O2`: `jmp rdi` - the pointer arrives in `rdi`, the `double` in `xmm0`, and since the call is the
 *   last thing to do, it jumps: `square` returns directly to the caller of `call_through`.
 * - ARM64 gcc: `bl square(double)` for the direct call, `blr x0` for the one through the pointer (`-O0`); with `-O2`,
 *   `mov x16, x0` and `br x16` - the jump through a register.
 * - `apply_square`, `-O2`: `mulsd xmm0, xmm0`, `ret`. The compiler inlined `call_through`, saw that `f` is `square`,
 *   and inlined that, too. The function pointer is gone. It only stays where the compiler cannot know its value - a
 *   pointer chosen at run time, or handed to code that was compiled elsewhere.
 */

namespace {

    /* --- `half` --- One more function for the table. */
    double half(const double x) {
        return x / 2.0;
    }

    /* --- `pick_from_a_table` ---
     * An array of function pointers: three addresses of code, 24 bytes. `operations[i]` picks one by a number, and the
     * call goes to wherever it points. That is the jump table of a `switch` over an enum (unit 0x07), and a vtable
     * (unit 0x08) - only that here you made the table yourself. A C program that needs "virtual functions" does exactly
     * this: a struct of function pointers.
     */
    void pick_from_a_table() {
        print_function_header();

        constexpr array<unary, 3> operations{square, cube, half};
        constexpr array<const char*, 3> names{"square", "cube", "half"};
        cout << " 1| sizeof(operations)=" << sizeof(operations) << '\n';
        for (std::size_t i{0}; i < operations.size(); ++i) {
            cout << ' ' << i + 2 << "| " << names[i] << "(5)=" << operations[i](5.0) << '\n';
        }
    }

    /* --- `comparisons` and `compare_ints` ---
     * The comparison for `qsort`, as C wants it: two addresses of elements, as `const void*` - `qsort` does not know
     * the type of the elements, only their size. The result: negative, zero or positive. `comparisons` counts the
     * calls.
     */
    int comparisons{0};

    int compare_ints(const void* a, const void* b) {
        ++comparisons;
        const int x{*static_cast<const int*>(a)};
        const int y{*static_cast<const int*>(b)};
        return (x > y) - (x < y);

        /* -- .Q&A -- !![Why not simply `return x - y;`?](#a-902) */
    }

    /* --- `sort_the_c_way` ---
     * `qsort` from the C library (see previous snippets) sorts any array: the address of the first element, the
     * number of elements, the size of one - and the address of a function that compares two of them. `qsort` was
     * compiled long before your program, into the C library: it cannot know the comparison, it can only call it,
     * through the pointer - thousands of times for a thousand numbers, and every one a real call through a register,
     * with two `const void*` to convert back.
     * The price of this flexibility: see the session's next snippets, and task 'Silver Lake'.
     */
    void sort_the_c_way() {
        print_function_header();

        array<int, 1000> numbers{};
        std::mt19937 generator{23};
        std::uniform_int_distribution<int> pick{0, 999'999};
        for (int& n : numbers) {
            n = pick(generator);
        }

        comparisons = 0;
        std::qsort(numbers.data(), numbers.size(), sizeof(int), compare_ints);
        cout << " 1| sorted: " << numbers[0] << ", " << numbers[1] << ", ..., " << numbers[999] << '\n';
        cout << " 2| comparisons: " << comparisons << " - for " << numbers.size() << " numbers\n";
    }

}

/* --- `main` --- */
int main() {
    show_where_functions_live();
    store_a_function_pointer();
    pick_from_a_table();
    sort_the_c_way();

    return EXIT_SUCCESS;
}
