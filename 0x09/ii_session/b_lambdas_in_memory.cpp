// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A lambda is an object of a class the compiler writes - the closure type. The same class can be written by hand:
 *   same size, same result, same machine code.
 * - Its data members are the captures: `sizeof` of a lambda is the size of what it captured - a copy for `[n]`, an
 *   address for `[&n]`.
 * - A captured copy lives inside the lambda object; a captured reference holds the address of the original.
 * - `mutable` state lives in the object, too - and a copy of the lambda copies it.
 * - Every lambda has a type of its own, even two with the same text.
 * - Only a lambda without captures converts to a function pointer - there is nothing to carry along.
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <type_traits>                      // for is_same_v
#include <typeinfo>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * `in_range`, `count_by_hand` and `count_by_lambda` are for Compiler Explorer and `nm`, below.
 */

/* --- `in_range` ---
 * The class a lambda `[low, high](const int v) { ... }` stands for, written by hand: two data members, and a `const`
 * member function called `operator()` - the function call operator (see previous snippets). An object of it can be
 * called like a function: `r(5)` is `r.operator()(5)`. A class like this is called a function object, or functor.
 * - !![#lambda]
 */
struct in_range {
    int low;
    int high;

    bool operator()(const int v) const { return low <= v && v <= high; }
};

/* --- `count_by_hand` and `count_by_lambda` --- The same count, with the class and with a lambda. */
std::ptrdiff_t count_by_hand(const vector<int>& numbers, const int low, const int high) {
    return std::count_if(numbers.begin(), numbers.end(), in_range{low, high});
}

std::ptrdiff_t count_by_lambda(const vector<int>& numbers, const int low, const int high) {
    return std::count_if(numbers.begin(), numbers.end(), [low, high](const int v) { return low <= v && v <= high; });
}

namespace {

    /* --- `write_the_class_by_hand` ---
     * The functor and the lambda: both 8 bytes - two `int`s -, both count the same. For the compiler, they are the
     * same thing: the lambda is only shorter to write, and the compiler invents the class name.
     */
    void write_the_class_by_hand() {
        print_function_header();

        const vector<int> numbers{5, 12, 8, 21, 9, 3};
        const in_range by_hand{4, 10};
        const auto by_lambda = [low = 4, high = 10](const int v) { return low <= v && v <= high; };
        cout << " 1| by_hand(5)=" << by_hand(5) << ", by_lambda(5)=" << by_lambda(5) << '\n';
        cout << " 2| sizeof: by_hand " << sizeof(by_hand) << ", by_lambda " << sizeof(by_lambda) << '\n';
        cout << " 3| counted: " << count_by_hand(numbers, 4, 10) << " and " << count_by_lambda(numbers, 4, 10) << '\n';
    }

    /* --- `show_the_captures` ---
     * Predict first - you did it for C++ Insights. Every capture is a data member, laid out like the members of a
     * struct (see previous snippets):
     * - `[]` - no member, but every object takes at least one byte (see previous snippets): 1.
     * - `[n]` - one `int`: 4. `[n, d]` - an `int` and a `double`: 16, with 4 bytes of padding.
     * - `[&n]` - a reference, which is an address (see previous snippets): 8. `[&n, &d, &k]` - three addresses, 24.
     *   The standard leaves open how a reference capture is stored; gcc and clang store one address per variable.
     * - `[s]` - a copy of the `string`: `sizeof(string)` - 32 with libstdc++, 24 with libc++ ... and if the text is
     *   long, the copy has its own block on the heap.
     * `n`, `d` and `k` are not `const` here, for once: a `const int` with a constant value need not be captured at
     * all - the compiler uses the value, and clang warns that the capture is not required.
     * - !![#capture]
     */
    void show_the_captures() {
        print_function_header();

        int n{1};
        double d{2.0};
        int k{3};
        const string s{"a string"};
        const auto nothing = [] { return 0; };
        const auto one_int = [n] { return n; };
        const auto int_and_double = [n, d] { return n + d; };
        const auto one_reference = [&n] { return n; };
        const auto three_references = [&n, &d, &k] { return n + d + k; };
        const auto a_string = [s] { return s.size(); };
        cout << " 1| []: " << sizeof(nothing) << ", [n]: " << sizeof(one_int) << ", [n, d]: " << sizeof(int_and_double)
             << '\n';
        cout << " 2| [&n]: " << sizeof(one_reference) << ", [&n, &d, &k]: " << sizeof(three_references) << ", [s]: "
             << sizeof(a_string) << '\n';
        cout << " 3| called: " << nothing() << ' ' << one_int() << ' ' << int_and_double() << ' ' << one_reference()
             << ' ' << three_references() << ' ' << a_string() << '\n';
    }

    /* --- `address_inside` ---
     * The first 8 bytes of an object, as an address - `memcpy`, as for the vptr in unit 0x08. Here for a lambda that
     * holds a reference.
     */
    const void* address_inside(const void* object) {
        const void* address{nullptr};
        std::memcpy(&address, object, sizeof address);
        return address;
    }

    /* --- `show_where_the_copy_lives` ---
     * Inside a lambda, the name of a variable captured by copy means the data member - not the original. So
     * `&n` in `by_value` is the address of the copy: the same address as the lambda object itself - the copy is its
     * first, and only, member. The lambda object is a local of this function, so the copy is in this frame, next to
     * `n`, but in another place.
     * `&n` in `by_reference` is the address of the original. And the 8 bytes of the object `by_reference` hold exactly
     * that address - a reference capture is an address, stored in the lambda.
     */
    void show_where_the_copy_lives() {
        print_function_header();

        const int n{23};
        const auto by_value = [n] { return static_cast<const void*>(&n); };
        const auto by_reference = [&n] { return static_cast<const void*>(&n); };
        cout << " 1| &n=" << &n << '\n';
        cout << " 2| &by_value=" << &by_value << ", inside: &n=" << by_value() << '\n';
        cout << " 3| &by_reference=" << &by_reference << ", inside: &n=" << by_reference() << ", its 8 bytes: "
             << address_inside(&by_reference) << '\n';

        /* -- .Q&A -- !![`std::sort` takes its comparison by value. Where is the lambda while `sort` runs?](#a-903) */
    }

    /* --- `keep_state_in_the_object` ---
     * `[count = 0]` creates a member `count` with the value 0 - an init-capture, for a member that is not a copy of a
     * variable. `mutable` allows `operator()` to change it: every call counts up, and the value stays in the object
     * from one call to the next - a function with a memory, and it is 4 bytes.
     * `copy` is a copy of the object - and of its `count`. From then on, there are two counters.
     */
    void keep_state_in_the_object() {
        print_function_header();

        auto next = [count = 0]() mutable { return ++count; };
        cout << " 1| next: " << next();
        cout << ' ' << next();
        cout << ' ' << next() << ", sizeof(next)=" << sizeof(next) << '\n';

        auto copy = next;
        cout << " 2| copy: " << copy();
        cout << ", next: " << next();
        cout << ", copy: " << copy() << '\n';
    }

    /* --- `compare_lambda_types` ---
     * Every lambda expression makes a new class, even when the text is the same - so `a` and `b` have different
     * types, and one cannot be assigned to the other. The names are made up by the compiler, and `typeid` shows them
     * mangled: gcc ends them with `UliE_` and `UliE0_` - the first and the second lambda taking an `int` in this
     * function -, clang with `$_0` and `$_1`. Nobody can write that type in the program - hence `auto`.
     * A distinct type per lambda is what makes them fast: a template instantiated with it knows exactly which
     * `operator()` it calls - see the next snippet.
     */
    void compare_lambda_types() {
        print_function_header();

        auto a = [](const int x) { return x + 1; };
        const auto b = [](const int x) { return x + 1; };
        cout << " 1| same type: " << std::is_same_v<decltype(a), decltype(b)> << '\n';
        cout << " 2| typeid(a).name()=" << typeid(a).name() << ", typeid(b).name()=" << typeid(b).name() << '\n';
        // a = b;                           // compiler error: no conversion from one lambda type to the other
        cout << " 3| a(1)=" << a(1) << ", b(1)=" << b(1) << '\n';
    }

    /* --- `convert_to_a_function_pointer` ---
     * A lambda without captures carries no data - its `operator()` needs nothing from the object. So the compiler gives
     * it a conversion to a function pointer (C++ Insights showed it): the address of a static function that does the
     * same. Such a lambda can go wherever C expects a function pointer - to `qsort`.
     * A lambda with a capture cannot: a function pointer is nothing but the address of code. Where would `count` come
     * from, when `qsort` calls it? C libraries solve that with an extra `void*` argument for "user data"; C++ passes
     * the object.
     */
    void convert_to_a_function_pointer() {
        print_function_header();

        int numbers[]{5, 2, 8, 1, 9, 3};
        int (*const compare)(const void*, const void*){[](const void* a, const void* b) {
            const int x{*static_cast<const int*>(a)};
            const int y{*static_cast<const int*>(b)};
            return (x > y) - (x < y);
        }};
        std::qsort(numbers, std::size(numbers), sizeof(int), compare);
        cout << " 1| sorted:";
        for (const int n : numbers) {
            cout << ' ' << n;
        }
        cout << ", compare=" << reinterpret_cast<const void*>(compare) << '\n';

        int count{0};
        const auto counting = [&count](const void*, const void*) { ++count; return 0; };
        // int (*const f)(const void*, const void*){counting};  // compiler error: no conversion - it captures
        cout << " 2| sizeof(counting)=" << sizeof(counting) << " - it carries an address, a function pointer cannot\n";

        /* -- .Q&A -- !![`auto f = +[](int x) { return x; };` compiles. What does the `+` do?](#a-904) */
    }

}

/* --- The class in the machine ---
 * Paste `in_range`, `count_by_hand` and `count_by_lambda` into Compiler Explorer, with `#include <vector>` and
 * `#include <algorithm>`, `-O2`. With x86-64 clang, the two functions are the same instructions, line by line -
 * vectorized, even: `pcmpgtd` compares four numbers at a time. gcc makes a plain loop for both, and the same one - it
 * only tests the two bounds in the other order. There is nothing left of either class: `low` and `high` are in
 * registers, and `operator()` is inlined into the loop of `count_if`.
 * With `-O0`, both call their `operator()`, and the lambda's has a name, too: `nm -C` on the object file of this
 * snippet (see previous snippets) lists
 *     count_by_lambda(...)::{lambda(int)#1}::operator()(int) const
 * the first lambda taking an `int` in `count_by_lambda` - `_ZZ15count_by_lambda...ENKUliE_clEi` before `c++filt`;
 * clang calls the class `$_0`. And `std::count_if` twice: once instantiated for `in_range`, once for the lambda's class
 * (see previous snippets: a template generates code per type).
 */

/* --- `main` --- */
int main() {
    write_the_class_by_hand();
    show_the_captures();
    show_where_the_copy_lives();
    keep_state_in_the_object();
    compare_lambda_types();
    convert_to_a_function_pointer();

    return EXIT_SUCCESS;
}
