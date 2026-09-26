[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)


# Glossary and Background

Shared definitions for the course snippets. 

In Markdown files the **headings are the blocks**, and every entry below carries 
an explicit anchor — `<a id="raii"></a>` — which is what a reference resolves against. 

From a snippet, an entry is addressed by that id:

```
[#raii]      a link; the link text is the entry's headline
![#raii]     an embed, folded until the reader clicks the triangle
!![#raii]    an embed, pinned open
[RAII](doc/glossary.md#raii)     the explicit, path-carrying form
```

The baseline is **C++23** (see `CMAKE_CXX_STANDARD` in the top-level 
`CMakeLists.txt`).


## Building and running

### Translation unit <a id="translation-unit"></a>

One source file after preprocessing — the unit the compiler actually sees, and
the unit the linker later joins with the others. Headers are not compiled on
their own; they become part of every translation unit that includes them.

`static` at file scope gives a name *internal linkage*, meaning it is invisible
to every other translation unit; an [unnamed namespace](#unnamed-namespace)
does the same and is preferred in modern C++, because it also works for types.

*Seen in* `0x07/a_statics.cpp`.

### `#include`                              <a id="include"></a>

This is a special term that is used to include a file into the current code
as if it was written there.

Angle brackets (`#include <iostream>`) search the compiler's include paths,
quotes (`#include "utils.hpp"`) search the current directory first. The
preprocessor performs a literal text substitution — which is why a header must
protect itself against being included twice, see also 
[include guard](#include-guard).

### Include guard                           <a id="include-guard"></a>

Protection against a header being pasted into the same
[translation unit](#translation-unit) twice, which would redefine every class
in it. Either the portable macro form or `#pragma once`, which every compiler
this course targets supports:

```cpp
#pragma once            // ... or:
#ifndef CBL_PRINTING_HPP
#define CBL_PRINTING_HPP
...
#endif
```

### `using`                                 <a id="using"></a>

Three distinct meanings share the keyword: a *using-declaration* pulls one name
into scope (`using std::cout;`), a *using-directive* pulls in a whole namespace
(`using namespace std;` — avoid it outside a small scope), and a *type alias*
names a type (`using catalog_t = std::unordered_map<std::string, book>;`),
which replaces C's `typedef` and is the only form that works with templates.

### `#pragma`                               <a id="pragma"></a>

Compiler instruction, e.g.
- "-Wuninitialized": Ignore warning for uninitialized variables – for didactic purposes here!
Never do this in productive code.
See also [Preprocessor](#preprocessor).

### Preprocessor                            <a id="preprocessor"></a>

The text-substitution pass that runs before compilation proper: it resolves
`#include`, expands macros, and evaluates `#if`. It knows nothing about types,
scopes or C++ syntax, which is why modern C++ replaces macros with
[`constexpr`](#constexpr) constants, `inline` functions and templates wherever
it can.

### Compiler, linker, `main`                <a id="compiler-and-linker"></a>

The compiler turns each [translation unit](#translation-unit) into an object
file; the linker joins the object files and the libraries into one executable
and resolves every name that was only declared. A program has exactly one
`main`, which returns `int` — `return EXIT_SUCCESS;` (from `<cstdlib>`) says
"nothing went wrong" more legibly than `return 0;`, and falling off the end of
`main` means the same thing.

### `main`                                  <a id="main"></a>

The 'main' function serves as the program's entry point and returns an error code. <p>
A return value of 0 (or EXIT_SUCCESS) indicates successful execution, while non-zero
values indicate errors. <p>
The error code can be evaluated in command line operations, such as `make && a.out`. 


### Declaration vs. definition              <a id="declaration-vs-definition"></a>

A *declaration* introduces a name and its type; a *definition* also provides
the body or the storage. A name may be declared many times and defined once,
see [ODR](#odr).

```cpp
double pot(double b, int n);        // declaration - enough to call it
double pot(double b, int n) { … }   // definition
int v1;                             // declaration AND definition (uninitialized!)
```

### One Definition Rule (ODR)               <a id="odr"></a>

Every function or variable used in a program must have exactly one definition
across all [translation units](#translation-unit); classes and templates may be
defined in several, provided the definitions are token-for-token identical —
which is what makes header-only code legal. `inline` marks a definition as
"may appear in several translation units, they are all the same one", which is
why header-only helpers and `inline` variables (C++17) exist.

### Undefined behavior (UB) <a id="undefined-behavior"></a>

Code for which the standard imposes no requirement at all: reading an
uninitialized variable, indexing past the end of an array, dereferencing a
[dangling pointer](#dangling-pointer), signed overflow. The program may crash,
may print garbage, or may appear to work — and the compiler is allowed to
optimize on the assumption that UB never happens, which is why "it worked on my
machine" is not evidence of anything.

```cpp
int v1;                     // no initializer at block scope
std::cout << v1;            // UB - not "some random number", UB
```

*Seen in* `0x01/a_uniform_init.cpp`.

### Build system: make and CMake <a id="build-system"></a>

`make` rebuilds only what changed, driven by rules in a `makefile`. CMake
generates those build files from a portable `CMakeLists.txt` — which is where
this course sets `CMAKE_CXX_STANDARD 23` and where the `cbl_utils` target makes
`<cbl/printing.hpp>` findable from every unit.

*Seen in* `0x00/README.md`, top-level `CMakeLists.txt`.

---

## Types, values, initialization

### Uniform initialization                  <a id="uniform-initialization"></a>

Brace initialization, `T x{…}`, usable everywhere: for primitives, aggregates,
classes and containers. Preferred in this course because it is the one syntax
that works for every type *and* rejects [narrowing](#narrowing-conversion)
instead of warning about it.

```cpp
int    v3{42};          // value 42
int    v4{};            // value-initialized: 0, not garbage
double d{1.0};
std::vector<int> primes{2, 3, 5, 7, 11};
```

The one trap: for `std::vector`, braces prefer the initializer-list
constructor — `std::vector<int> a(5)` is five zeros, `std::vector<int> a{5}` is
one element with value 5.

### Narrowing conversion                    <a id="narrowing-conversion"></a>

A conversion that may lose information: `double` to `int`, a wide integer to a
narrower one, or any value the target type cannot represent.
[Uniform initialization](#uniform-initialization) makes it an **error**, `=`
initialization only a warning.

```cpp
int a = 1.2;        // compiles, a == 1
int b{1.2};         // error: narrowing conversion
```

### Default values                          <a id="default-values"></a>

Default values:
- `int`: 0

### `const` <a id="const"></a>

A promise to the compiler and to the reader that a value will not change
through this name. Applies to variables, parameters, return types and member
functions:

```cpp
const double pi{3.14159};
void print(const std::string& s);          // no copy, no modification
[[nodiscard]] double celsius() const;      // member function: does not touch *this
```

`const` is checked at compile time and costs nothing at runtime — see
[const correctness](#const-correctness) for why it is the default here.

*Seen in* `0x02/a_consts.cpp`, `0x06/a_const.cpp`.

### `constexpr` <a id="constexpr"></a>

*May* be evaluated at compile time. On a variable it means "constant, known at
compile time"; on a function it means "usable in a constant expression, if the
arguments are". Replaces macros for constants.

```cpp
constexpr double zero_celsius_in_kelvin{273.15};
constexpr int square(int n) { return n * n; }
constexpr int nine = square(3);   // computed by the compiler
int m = square(runtime_value);    // same function, computed at runtime
```

*Seen in* `0x02/a_consts.cpp`.

### `consteval` and `constinit` <a id="consteval"></a>

C++20. `consteval` makes compile-time evaluation *mandatory* — an "immediate
function" that cannot be called at runtime at all. `constinit` says a variable
with static storage must be initialized at compile time (no
[static initialization order](#static-storage) surprises), without making it
`const`.

```cpp
consteval int cube(int n) { return n * n * n; }
constexpr int c = cube(3);        // fine
// int r = cube(read_number());   // error: not a constant expression
```

*Seen in* `0x02/a_consts.cpp`.

### `auto` <a id="auto"></a>

Type deduction from the initializer. The type is still static and fixed — the
compiler writes it out, not the programmer. Useful where the type is long,
obvious, or unspeakable (lambdas, iterators).

```cpp
auto i{42};                       // int
auto& r = v[0];                   // reference - without & you get a copy
for (const auto& x : container)   // the workhorse form
```

`auto` drops references and top-level `const` unless you write them back, which
is the single most common source of accidental copies.

*Seen in* `0x02/study_auto.cpp`.

### `decltype` and trailing return type <a id="decltype"></a>

`decltype(expr)` is the declared type of an expression, without evaluating it.
A *trailing return type* moves the return type behind the parameters, where it
can name them:

```cpp
template <class A, class B>
auto add(A a, B b) -> decltype(a + b) { return a + b; }
```

Since C++14 a plain `auto` return type deduces the same thing in most cases;
the trailing form still matters when the type depends on the parameters.

*Seen in* `0x02/study_trailingreturn.cpp`.

### Primitive types and fixed-width integers <a id="primitive-types"></a>

`bool`, `char`, `int`, `double` and friends are built in; their sizes are
implementation-defined, only their minimum ranges are guaranteed. Where the
width matters, use `<cstdint>`: `std::int8_t`, `std::uint32_t`, `std::int64_t`.

The classic trap: `std::int8_t` is `signed char`, so streams print it as a
*character*, not as a number — `static_cast<int>(x)` before printing.

*Seen in* `0x01/c_primitives.cpp`, `0x01/study_int8.cpp`.

### `sizeof`, `size_t`, `ptrdiff_t` <a id="sizeof"></a>

`sizeof(T)` is the size of a type in bytes, evaluated at compile time.
`std::size_t` is the unsigned type it yields and the type every container's
`.size()` returns; `std::ptrdiff_t` is the signed type a pointer subtraction
yields. Mixing `size_t` with `int` in comparisons is a warning worth listening
to: unsigned arithmetic wraps rather than going negative.

*Seen in* `0x05/c_more_pointers.cpp`, `0x06/study_2d_arrays.cpp`.

### `std::string` <a id="string"></a>

An owning, growing sequence of characters with [RAII](#raii) semantics: it
allocates, copies and frees for you. `+` concatenates, `.size()`, `.substr()`,
`.find()` do the obvious, and `.starts_with()` (C++20) / `.contains()` (C++23)
save a comparison against `npos`.

*Seen in* `0x01/d_strings.cpp`, `0x01/study_string.cpp`.


### Small String optimization               <a id="sso"></a>
- SSO (small string optimization): "Kurs" probably never touches the heap, a 40-char
  string does. `sizeof(std::string)` + `s.capacity()` make it visible in two lines.


### `std::string_view` <a id="string-view"></a>

C++17. A **non-owning** view of a character sequence: a pointer and a length.
Copying one costs nothing, so it is the right parameter type for a function
that only *reads* a string — and the wrong type to store, because it does not
keep the characters alive.

```cpp
void print_function_header(std::string_view name);   // no copy of the caller's string
std::string_view bad = std::string{"temp"};          // dangling immediately
```

*Seen in* `0x04/c_string_views.cpp`, `utils/cbl/printing.hpp`.

### Type conversion: `stoi`, `stod`, `to_string` <a id="string-conversion"></a>

`std::stoi` / `std::stod` parse a number out of a `std::string` and **throw**
(`std::invalid_argument`, `std::out_of_range`) rather than returning an error
code — so they belong in a [try/catch](#exception). `std::to_string` goes the
other way; `std::format` (C++20) does it better.

*Seen in* `0x02/study_stoi_stod.cpp`.

### Structured bindings <a id="structured-bindings"></a>

C++17. Names for the members of a pair, tuple, aggregate or map element:

```cpp
for (const auto& [isbn, book] : catalog)
    std::println("{}: {}", isbn, book.title);
```

Reads better than `it->first` / `it->second`, and `auto&` binds without
copying.

*Seen in* `0x04/f_containers.cpp`.

### Aggregate <a id="aggregate"></a>

A `struct` (or class) with no private data, no user-provided constructors, no
virtuals — so it can be initialized member by member with braces, including by
name (*designated initializers*, C++20):

```cpp
struct fraction { int num; int den; };
fraction f{3, 4};
fraction g{.num = 3, .den = 4};      // C++20
```

*Seen in* `0x02/b_structs.cpp`, `0x03/study_aggregate.cpp`.

### Scope <a id="scope"></a>

The region of a program in which a name is visible — a namespace, a class body,
a function, a block. Nothing forces a helper into a class: a static member
function's only advantage over a free function is that it lives in the scope a
reader searches first, and that it may touch the class's private members.

Not to be confused with *lifetime*: `static` at block scope changes the
lifetime of a variable, not the scope in which its name can be used.

### Lifetime <a id="lifetime"></a>

The period during which an object exists and its address is valid: from the end
of its constructor to the start of its destructor. Automatic ("stack") objects
die at the closing brace, dynamic objects when `delete`d or when their
[smart pointer](#unique-ptr) lets go, static objects at program end. Nearly
every pointer bug in this course is a lifetime bug.

*Seen in* `0x03/f_raii.cpp`, `0x04/c_string_views.cpp`.

### Namespace and unnamed namespace         <a id="unnamed-namespace"></a>

A named scope that keeps unrelated names apart (`std::`, `cbl::`). An **unnamed
namespace** gives everything inside it internal linkage — visible only in this
[translation unit](#translation-unit) — and is the modern replacement for
file-scope `static`, because it also works for types.

```cpp
namespace { void define_and_init() { … } }   // helper, private to this file
```

---

## Functions and control flow

### Function overloading <a id="overloading"></a>

Several functions may share a name if their parameter lists differ; the
compiler picks by the argument types (*overload resolution*). The return type
alone is **not** enough to distinguish two overloads. Default arguments
(`void log(std::string_view msg, int level = 0);`) often replace an overload —
and must be given once, in the declaration.

*Seen in* `0x01/f_functions.cpp`.

### Parameter passing <a id="parameter-passing"></a>

Four choices, one decision table:

| Form              | Copy? | Caller's object changed? | Use for                |
|-------------------|-------|--------------------------|------------------------|
| `T v`             | yes   | no                       | small, cheap types     |
| `const T& v`      | no    | no                       | anything bigger, read  |
| `T& v`            | no    | yes                      | out-parameters         |
| `T* v`            | no    | yes, and may be null     | optional out-parameter |

Call-by-value on a `struct` copies every member — measurable as soon as the
struct holds a `std::string`.

*Seen in* `0x02/d_call_by_ref.cpp`, `0x05/e_swap.cpp`.

### Range-based `for` <a id="range-based-for"></a>

Iterates anything with `begin()`/`end()` — arrays, containers, ranges:

```cpp
for (const auto& x : v) …    // read
for (auto& x : v) x *= 2;    // modify in place
for (auto x : v) …           // copy per element - deliberate, or an accident?
```

*Seen in* `0x04/d_iterators.cpp`.

### `switch` and fallthrough <a id="switch"></a>

Selects on an integral or enum value. Without `break`, control *falls through*
to the next label — legal, occasionally intended, and worth marking as such
with `[[fallthrough]];` (C++17) so the reader and the compiler both know it was
on purpose. A `default:` label makes the intent explicit even when it does
nothing.

*Seen in* `0x01/study_switch.cpp`.

### `[[nodiscard]]` <a id="nodiscard"></a>

C++17 attribute: warns when the return value is thrown away. Belongs on
functions whose result *is* the point — a computed value, an error code, a
handle, a [RAII](#raii) wrapper. C++20 allows a reason:
`[[nodiscard("check the error code")]]`.

*Seen in* `0x04/a_nodiscard.cpp`.

### `noexcept` <a id="noexcept"></a>

A promise that a function throws nothing. Not merely documentation: containers
use it to decide whether they may *move* elements while reallocating, so move
constructors and swaps should be `noexcept`. If a `noexcept` function does
throw, `std::terminate` is called — no unwinding, no second chance.

*Seen in* `0x07/d_operators_2.cpp`, `0x0b/c_smart_ptr_v2.cpp`.

### Recursion <a id="recursion"></a>

A function calling itself, with a base case that ends it. Each call gets its
own frame on the [stack](#stack-and-heap) — deep recursion runs out of it, and
C++ makes no guarantee of tail-call elimination.

*Seen in* `0x07/a_statics.cpp` (via the static call counter).

---

## References, pointers, memory

### Reference <a id="reference"></a>

An alias for an existing object: same object, second name. Must be initialized,
can never be rebound, and cannot be null.

```cpp
int  a{1};
int& r{a};      // r IS a
r = 2;          // a == 2
```

`const T&` binds to temporaries as well and extends their lifetime to the
reference's — the reason it is the default parameter type for anything larger
than a pointer.

*Seen in* `0x02/c_references.cpp`.

### Pointer <a id="pointer"></a>

A variable holding an address. Must be dereferenced (`*p`) to reach the object,
can be null, can be re-pointed, and can be compared and printed. Where a
reference says "this object", a pointer says "possibly an object, possibly
none, possibly a different one tomorrow".

```cpp
int  v{1};
int* p{&v};     // p holds the address of v
*p = 2;         // v == 2
p = nullptr;    // legal for a pointer, impossible for a reference
```

*Seen in* `0x05/b_first_pointers.cpp`.

### `nullptr` <a id="nullptr"></a>

C++11. The typed null pointer literal — `NULL` and `0` are integers in
disguise and pick the wrong [overload](#overloading). Dereferencing a null
pointer is [UB](#undefined-behavior); check before you deref, or use a
[reference](#reference) if null is not a case you want to handle.

*Seen in* `0x05/c_more_pointers.cpp`.

### Pointer arithmetic and array decay <a id="pointer-arithmetic"></a>

Adding to a pointer moves it in units of the pointed-to *type*, not bytes, so
`p + 1` and `&a[1]` are the same address. An array converts ("decays") to a
pointer to its first element at the slightest provocation — which is exactly
when it loses its length, and why `std::array` and `std::vector` exist.

```cpp
int  a[5]{};
int* p = a;          // decay - sizeof(p) says nothing about the 5
p[3] = 1;            // same as *(p + 3)
```

*Seen in* `0x05/d_arrays_and_pointers.cpp`, `0x02/f_arrays.cpp`.

### Stack and heap <a id="stack-and-heap"></a>

Automatic variables live on the **stack**: allocation is a pointer bump,
lifetime ends at the closing brace, size must be known at compile time.
Dynamic objects live on the **heap** (free store): allocation is a library
call, lifetime is manual (or [RAII](#raii)-managed), size may be computed at
runtime. Prefer the stack; reach for the heap when the object must outlive the
scope or is too large or too variable for it.

*Seen in* `0x05/a_asm_view.cpp`, `0x06/b_new_delete.cpp`.

### `new` and `delete` <a id="new-delete"></a>

Allocate on the heap and construct; destruct and free. Every `new` needs
exactly one `delete`, every `new[]` exactly one `delete[]`, on every path
including the one an exception takes. This is why modern C++ writes
[`make_unique`](#unique-ptr) instead — `new` in application code is a code
smell in the [Core Guidelines](#the-modern-way) sense.

`new` throws `std::bad_alloc` rather than returning null (unless you ask for
`new (std::nothrow)`).

*Seen in* `0x06/b_new_delete.cpp`.

### `malloc` / `free` <a id="malloc"></a>

The C allocator: raw bytes, no constructor, no destructor, returns null on
failure. Never mix with `new`/`delete`, and never use it on a type with a
constructor. Present in this course to show what `new` adds.

*Seen in* `0x06/study_alloc_and_free.cpp`.

### Memory leak <a id="memory-leak"></a>

Allocated memory that is never freed because the last pointer to it was lost.
Not a crash and not a warning — just a program that grows until it dies. The
cure is ownership: exactly one owner, [RAII](#raii)-managed.

*Seen in* `0x06/b_new_delete.cpp`, `0x06/c_unique_ptr.cpp`.

### Dangling pointer / reference <a id="dangling-pointer"></a>

A pointer or reference to an object whose [lifetime](#lifetime) has ended:
returning the address of a local, keeping a `string_view` of a temporary,
holding an iterator across a `push_back`. Reading through it is
[UB](#undefined-behavior), and it usually "works" for a while, which is the
worst possible failure mode.

*Seen in* `0x04/c_string_views.cpp`, `0x05/c_more_pointers.cpp`.

### `this` <a id="this"></a>

Inside a non-static member function, a pointer to the object the function was
called on — `const` when the function is `const`. `this->x` disambiguates a
member from a parameter of the same name; `*this` is the object, which is what
an assignment operator returns.

C++23 adds *deducing `this`* (`auto&& self` as an explicit first parameter),
which removes a class of const/ref duplication — compiler support is still
uneven, so this course sticks to the classic form.

*Seen in* `0x05/f_this.cpp`.

### Address-of and `std::addressof` <a id="addressof"></a>

`&x` yields the address of `x` — unless the class overloaded `operator&`, in
which case it yields whatever that returns. `std::addressof(x)` gets the real
address regardless, which is what generic code must use.

*Seen in* `0x06/e_addressof.cpp`.

### Const pointers vs. pointers to const <a id="const-pointer"></a>

Read the declaration right to left:

```cpp
const int* p;        // pointer to const int   - *p is fixed, p may move
int* const p{&i};    // const pointer to int   - p is fixed, *p may change
const int* const p{&i};   // both
```

`const_cast` removes the `const` — legal, and [UB](#undefined-behavior) if you
then write through it to an object that really is const.

*Seen in* `0x06/a_const.cpp`.

---

## Classes

### `struct` vs. `class` <a id="struct-vs-class"></a>

The same construct with one difference: members of a `struct` default to
`public`, members of a `class` to `private`. Convention, not rule: `struct` for
plain data ([aggregates](#aggregate)), `class` where there is an invariant to
protect.

*Seen in* `0x02/b_structs.cpp`, `0x03/a_simple_class.cpp`.

### Access modifiers <a id="access-modifiers"></a>

`public` — everyone; `protected` — the class and its derived classes;
`private` — the class and its [friends](#friend). The question to ask first is
*who* wants access: a user of an instance always needs `public`, a derived
class may be served by `protected`.

*Seen in* `0x08/b_modifier.cpp`.

### Constructor <a id="constructor"></a>

The member function that turns raw storage into an object with an invariant. If
it throws, the object never existed — members constructed so far are destroyed,
and no destructor runs for the object itself.

```cpp
explicit Temperature(double celsius) : celsius_{celsius} {}
```

*Seen in* `0x03/a_simple_class.cpp`, `0x03/d_ctor_dtor_throw.cpp`.

### Member initializer list <a id="member-initializer-list"></a>

The `: member_{value}, other_{…}` part between the constructor's signature and
its body. Members are *initialized* here, in declaration order; assigning in
the body instead means default-constructing first and overwriting after —
impossible for `const` members and references, wasteful for everything else.

*Seen in* `0x03/a_simple_class.cpp`.

### Destructor <a id="destructor"></a>

Runs when the object's [lifetime](#lifetime) ends, in reverse order of
construction, and releases whatever the object owns. **A destructor must never
throw** — during stack unwinding a second exception calls `std::terminate`.
Destructors are implicitly `noexcept`.

*Seen in* `0x03/d_ctor_dtor_throw.cpp`, `0x03/f_raii.cpp`.

### `explicit` <a id="explicit"></a>

Forbids a constructor (or conversion operator) from being used for an implicit
conversion. Without it, any one-argument constructor doubles as a silent
conversion — `void f(Temperature); f(20.0);` compiles and probably should not.
The rule of thumb: single-argument constructors are `explicit` unless the
conversion is the point.

*Seen in* `0x03/b_explicit_const.cpp`, `0x09/a_casts.cpp`.

### `const` member function <a id="const-member-function"></a>

`double celsius() const;` — promises not to modify the observable state of the
object, and is the only kind callable on a `const` object or through a
`const&`. Skipping it is what forces callers to drop `const`, one caller at a
time, until nothing is const any more.

See also [`mutable`](#mutable) for the deliberate exception.

*Seen in* `0x03/b_explicit_const.cpp`.

### `mutable` <a id="mutable"></a>

A data member that may change even in a [`const` member
function](#const-member-function) — for caches, lazily computed values, mutexes
and counters, i.e. state that is not part of what the object *means*. (In a
lambda, `mutable` means something related but different: the captured copies
may be modified.)

*Seen in* `0x0a/a_lambdas.cpp`.

### Copy constructor and copy assignment <a id="copy-constructor"></a>

`T(const T&)` builds a new object from an existing one; `T& operator=(const T&)`
overwrites an existing one. The compiler generates both member-wise, which is
correct as long as every member owns itself — and wrong the moment a raw
pointer is involved, see [deep vs. shallow copy](#deep-copy).

*Seen in* `0x03/c_copy_assignment.cpp`.

### Deep vs. shallow copy <a id="deep-copy"></a>

A shallow copy duplicates the pointer, so two objects believe they own the same
memory — double `delete`, or modification through one showing up in the other.
A deep copy duplicates what the pointer points to. Members that own themselves
(`string`, `vector`, [`unique_ptr`](#unique-ptr)) make the question disappear,
which is the [Rule of Zero](#rule-of-zero).

*Seen in* `0x03/c_copy_assignment.cpp`, `0x06/d_smallbuffer_example.cpp`.

### Rule of Zero / Three / Five <a id="rule-of-zero"></a>

- **Zero**: define none of the special members. Let RAII members manage
  resources. This is the goal.
- **Three**: if you define any of destructor, copy constructor, copy
  assignment, you probably need all three (C++98 resource management).
- **Five**: with C++11, add [move constructor](#move-semantics) and move
  assignment to that set.
- **Big Two** (rare): only forbid copying (`= delete`) and provide moves.

*Seen in* `0x03/README.md`, `0x06/d_smallbuffer_example.cpp`.

### `= default` and `= delete` <a id="default-delete"></a>

`= default` asks for the compiler's version explicitly (and keeps the type
trivial where possible); `= delete` removes a function, so using it is a
compile error rather than a surprise. Deleting the copy operations is how a
type says "I am not copyable" — see the `Widget` base in the UI library.

*Seen in* `0x0b/d_ui_lib.hpp`.

### Static member <a id="static-member"></a>

Belongs to the class, not to an instance: one shared variable, or a function
with no `this`. Reachable as `Temperature::is_plausible(20.0)`, and — legally
but pointlessly — through an object. `static constexpr` data members can be
initialized in the class body; other static data members need a definition
(or `inline`, C++17).

*Seen in* `0x07/a_statics.cpp`.

### Three meanings of `static` <a id="static-storage"></a>

One keyword, three jobs:

1. **File scope**: internal linkage — invisible to other
   [translation units](#translation-unit). Superseded by the
   [unnamed namespace](#unnamed-namespace).
2. **Function-local**: one instance, initialized on first use, outliving the
   call. Thread-safe initialization since C++11.
3. **Class member**: belongs to the type, see [static member](#static-member).

*Seen in* `0x07/a_statics.cpp`.

### Named constructor <a id="named-constructor"></a>

A static member function that returns an object, used where a constructor would
be ambiguous or unnamed:

```cpp
static Temperature from_kelvin(double kelvin);   // reads better than Temperature(double, tag)
```

C++ constructors cannot be named or overloaded on intent, only on parameter
types — so the idiom trades `Temperature{300.0, kelvin_tag{}}` for a name a
reader understands. It is the closest counterpart to Python's `@classmethod`
alternative constructor, minus the `cls` parameter.

### `friend` <a id="friend"></a>

Grants one function or class access to the private members of another. Used
sparingly and deliberately — most often for `operator<<`, which cannot be a
member because its left operand is the stream. A friend is part of the class's
interface, not a hole in it.

*Seen in* `0x07/b_friends.cpp`, `0x04/b_stream_insertion.cpp`.

### Operator overloading <a id="operator-overloading"></a>

Giving operators a meaning for your own type. Choose member vs. free function
by the left operand: `a + b` where `a` is yours can be either, `std::cout << a`
must be free (usually a [friend](#friend)). Keep the conventional semantics —
`+` should not print, `[]` should not allocate — and provide `+=` first, then
define `+` in terms of it.

```cpp
number& operator=(const number& rhs);          // member
int3 operator+(int3 a, int3 b) noexcept;       // free, symmetric
double operator()(double x) const;             // call operator: polynomial(x)
T& operator[](std::size_t i);                  // index operator
std::ostream& operator<<(std::ostream&, const T&);
```

C++20's `operator<=>` generates the six comparisons from one definition.

*Seen in* `0x07/c_operators_1.cpp` … `e_operators_3.cpp`.

### Conversion operator <a id="conversion-operator"></a>

`operator double() const;` — lets an object convert to another type. Same
warning as the [`explicit`](#explicit) constructor: without `explicit`, the
conversion happens silently in overload resolution, which can make two
unrelated types compare equal by accident.

*Seen in* `0x02/b_structs.cpp`, `0x09/a_casts.cpp`.

### Getter and setter <a id="getter-setter"></a>

Accessors that keep a member private while exposing its value. Worth it when
they enforce an invariant, validate, or notify; pure pass-throughs are just a
longer `public:`. C++ has no properties — a wrapper class can approximate the
syntax, at a cost.

*Seen in* `0x03/e_getter_setter.cpp`, `0x03/study_properties.cpp`.

### RAII <a id="raii"></a>

*Resource Acquisition Is Initialization*: a resource — memory, file, lock,
socket — is owned by an object, acquired in its [constructor](#constructor) and
released in its [destructor](#destructor). Since destructors run on every exit
path, including the one an exception takes, cleanup cannot be forgotten.

```cpp
{
    std::ofstream f{"log.txt"};      // acquire
    f << "hello";
}                                    // released here, exception or not
```

The whole standard library is built this way: `string`, `vector`,
[`unique_ptr`](#unique-ptr), [`lock_guard`](#lock-guard).

*Seen in* `0x03/f_raii.cpp`.

---

## Inheritance and polymorphism

### Inheritance <a id="inheritance"></a>

`class button : public widget` — a `button` *is a* `widget` and contains one as
a sub-object. Constructors run base-first, destructors derived-first. The
access specifier on the base (`public`, `protected`, `private`) limits how
visible the inherited members are to *users* of the derived class; `public` is
the only one that models "is a".

*Seen in* `0x08/a_single_inheritance.cpp`, `0x08/b_modifier.cpp`.

### `virtual` and dynamic dispatch <a id="virtual"></a>

A `virtual` function is chosen by the **dynamic** type of the object, through a
pointer or reference, not by the static type of the expression. Without
`virtual`, C++ calls what the pointer's type says — unlike Java, where every
method is virtual by default.

```cpp
widget* w = new button{};
w->draw();          // button::draw() only if draw is virtual
```

*Seen in* `0x08/c_virtual.cpp`.

### `override` and `final` <a id="override"></a>

C++11. `override` says "this replaces a base virtual" and turns a typo — a
wrong signature, a missing `const` — into a compile error instead of a second,
never-called function. `final` forbids further overriding (or, on a class,
further derivation).

*Seen in* `0x08/c_virtual.cpp`.

### vtable <a id="vtable"></a>

The usual implementation of [dynamic dispatch](#virtual): each polymorphic
class has a table of function pointers, and each object of it carries a hidden
pointer to that table. Hence the cost — one pointer per object, one indirection
per call — and hence [zero overhead](#zero-overhead): you pay it only for
classes that declare a virtual function.

*Seen in* `0x08/study_vtable.cpp`.

### Abstract class and pure virtual function <a id="abstract-class"></a>

`virtual void draw() = 0;` makes the class abstract: it cannot be instantiated,
only derived from. A class of nothing but pure virtuals is C++'s interface. A
pure virtual *destructor* is legal and still needs a definition, because the
derived destructor calls it.

*Seen in* `0x08/d_abstract.cpp`.

### Virtual destructor <a id="virtual-destructor"></a>

`delete base_ptr;` where the object is really a derived one is
[UB](#undefined-behavior) unless the base destructor is `virtual`. Rule: a base
class intended for polymorphic deletion declares a virtual destructor —
or a protected non-virtual one, if deletion through the base is not allowed.

*Seen in* `0x08/d_abstract.cpp`.

### Object slicing <a id="slicing"></a>

Copying a derived object into a base-typed variable keeps only the base part;
the derived state and the [vtable](#vtable) pointer are lost, so virtual calls
resolve to the base. Passing by value is the usual accident —
`void f(widget w)` instead of `void f(const widget& w)`.

*Seen in* `0x08/a_single_inheritance.cpp`.

### Multiple inheritance and the diamond <a id="multiple-inheritance"></a>

A class may have several bases. If two of them share a base, that base exists
*twice* — the diamond problem — and member access becomes ambiguous.
`class b : virtual public a` makes the shared base a single sub-object, at the
cost of a more complex layout and construction order (the most-derived class
initializes the virtual base).

*Seen in* `0x08/e_multiple_inheritance.cpp`.

### Casts <a id="casts"></a>

Four named casts, each with a job, all greppable — unlike the C-style `(T)x`,
which silently picks whichever of them fits:

- `static_cast<T>` — well-defined conversions: numeric, up/down a hierarchy
  without a check, `void*` back to `T*`.
- `dynamic_cast<T*>` — checked downcast in a polymorphic hierarchy; yields
  `nullptr` (pointers) or throws `std::bad_cast` (references).
- `const_cast<T>` — adds or removes `const`. Writing through a removed `const`
  on a truly const object is [UB](#undefined-behavior).
- `reinterpret_cast<T>` — reinterprets the bits. Portable only in the narrow
  cases the standard lists.

*Seen in* `0x09/a_casts.cpp`.

### `enum` and `enum class` <a id="enum-class"></a>

A plain `enum` leaks its enumerators into the surrounding scope and converts to
`int` on sight. A **scoped** enum (`enum class color { red, green };`, C++11)
does neither: names are `color::red`, and conversion needs an explicit
[`static_cast`](#casts). The underlying type can be fixed
(`enum class flags : std::uint8_t`).

*Seen in* `0x09/b_enums.cpp`.

---

## Templates and generics

### Function and class template <a id="template"></a>

A pattern the compiler instantiates per type used. Templates live in headers,
because the definition must be visible where it is instantiated
([ODR](#odr) permits it).

```cpp
template <class T> void join(const T& container);
template <class T, std::size_t N> class my_array { … };
```

*Seen in* `0x04/e_first_generics.cpp`.

### Template parameters and instantiation <a id="instantiation"></a>

Parameters may be types (`class T` / `typename T` — same thing), non-type
values (`std::size_t N`), or templates. *Instantiation* is the compiler
generating one concrete class or function per distinct argument list — which is
why templates cost compile time and code size but no runtime dispatch, see
[zero overhead](#zero-overhead).

*Seen in* `0x04/study_more_generics.cpp`.

### Specialization <a id="specialization"></a>

Providing a different implementation for particular arguments: *full*
specialization fixes every parameter, *partial* specialization fixes some (only
for class templates; functions overload instead).

```cpp
template <class A, class B> struct my_pair { … };      // primary
template <class A> struct my_pair<A, bool> { … };      // partial
template <> struct my_pair<int, int> { … };            // full
```

*Seen in* `0x09/c_specializations.cpp`.

### Type traits <a id="type-traits"></a>

Compile-time queries and transformations over types, in `<type_traits>`:
`std::is_integral_v<T>`, `std::remove_reference_t<T>`, `std::decay_t<T>`. The
classic use is a [specialization](#specialization) selected by a trait; since
C++17 `if constexpr` often replaces that with a plain branch that is compiled
away.

*Seen in* `0x09/c_specializations.cpp`.

### Concepts and `requires` <a id="concepts"></a>

C++20. Named constraints on template parameters, checked at the call site — so
the error says "not sortable" instead of forty lines from inside the algorithm.

```cpp
template <class T>
concept numeric = std::is_arithmetic_v<T>;

template <numeric T> T twice(T v) { return v + v; }
```

*Seen in* `0x09/c_specializations.cpp`.

---

## Standard library

### Sequence containers <a id="containers"></a>

| Container      | Storage            | Good at                          |
|----------------|--------------------|----------------------------------|
| `std::array`   | fixed, inline      | size known at compile time       |
| `std::vector`  | contiguous, heap   | the default; index and iterate   |
| `std::deque`   | chunked            | push/pop at both ends            |
| `std::list`    | doubly linked      | splice, stable addresses         |

`vector` grows geometrically: `capacity()` may exceed `size()`, and a
reallocation invalidates every iterator and pointer into it.

*Seen in* `0x04/f_containers.cpp`, `0x04/study_vector.cpp`.

### Associative containers <a id="associative-containers"></a>

`std::set` / `std::map` keep keys sorted in a balanced tree — O(log n), ordered
iteration, needs `<`. `std::unordered_set` / `std::unordered_map` hash them —
O(1) on average, no order, needs `==` and a hash. Reach for the unordered ones
unless the order matters.

*Seen in* `0x04/f_containers.cpp`.

### `std::pair` and `std::tuple` <a id="pair-tuple"></a>

Heterogeneous fixed-size bundles: `.first`/`.second` for a pair,
`std::get<0>()` for a tuple, or [structured bindings](#structured-bindings) for
both. Fine as a return type for two values that have no name worth inventing —
a named `struct` beats them the moment they have one.

*Seen in* `0x04/f_containers.cpp`.

### Iterator <a id="iterator"></a>

A generalized pointer: `*it` reads, `++it` advances, `it != end` tests. Every
container provides `begin()`/`end()`, `cbegin()`/`cend()` for const access, and
`rbegin()`/`rend()` for reverse traversal. Half-open ranges throughout: `end()`
points *past* the last element.

*Seen in* `0x04/d_iterators.cpp`, `0x04/study_more_iterators.cpp`.

### Algorithms <a id="algorithms"></a>

`<algorithm>` operates on iterator ranges rather than containers:
`std::sort`, `std::find_if`, `std::for_each`, `std::accumulate` (in
`<numeric>`), `std::transform`. Prefer them to hand-written loops — they say
*what*, not *how*, and they are hard to get wrong.

*Seen in* `0x0a/c_algorithms.cpp`.

### Ranges and views <a id="ranges"></a>

C++20. The same algorithms taking a container directly (`std::ranges::sort(v)`)
plus lazy, composable *views*:

```cpp
for (int x : v | std::views::filter(even) | std::views::take(3)) …
```

A view owns nothing and computes on demand — cheap to copy, and
[dangling](#dangling-pointer) if the underlying range dies first.

*Seen in* `0x0a/c_algorithms.cpp`.

### `std::optional`, `variant`, `expected` <a id="optional"></a>

C++17 brought `std::optional<T>` ("a T or nothing", better than a magic value)
and `std::variant<A, B>` (a type-safe union). C++23 adds
`std::expected<T, E>` ("a T or an error"), the return-value counterpart to
[exceptions](#exception). Compiler support for `expected` is recent — check
your toolchain before relying on it.

*Seen in* `0x09/e_in_out.cpp`.

### Streams <a id="streams"></a>

`std::cout`, `std::cin`, `std::cerr` for the console; `std::ifstream` /
`std::ofstream` for files ([RAII](#raii): they close themselves);
`std::istringstream` / `std::ostringstream` for parsing and building strings in
memory. `std::endl` flushes — `'\n'` usually suffices and is faster.

*Seen in* `0x01/b_console_io.cpp`, `0x09/e_in_out.cpp`.

### `std::format`, `std::print`, `std::println` <a id="format"></a>

`std::format("{:.2f}", x)` (C++20) builds a string from a pattern; `std::print`
and `std::println` (C++23, header `<print>`) write one directly, without the
`<<` chain. Type-safe, and the format string is checked at compile time.

```cpp
std::println("{}| {} = {:.3f}", i, name, value);
```

Needs a recent libstdc++/libc++ — where it is missing, `std::format` plus
`std::cout` is the fallback.

*Seen in* `0x01/study_println.cpp`, `0x09/e_in_out.cpp`.

---

## Errors and exceptions

### Exception <a id="exception"></a>

An error signalled by `throw` and handled by the nearest matching `catch`,
unwinding the stack — and running every destructor on the way — in between.

```cpp
try {
    auto n = std::stoi(text);
} catch (const std::invalid_argument& e) {   // specific first
    std::println("not a number: {}", e.what());
} catch (...) {                              // catch-all last
    throw;                                   // rethrow, keep the type
}
```

Catch by `const&` (copying slices, see [slicing](#slicing)), order handlers
from specific to general, and let the exception carry the message —
`e.what()`.

*Seen in* `0x02/e_exception.cpp`.

### Standard exception types <a id="std-exception"></a>

All derive from `std::exception`: `std::logic_error` (`invalid_argument`,
`out_of_range`, `domain_error`) for bugs the caller could have prevented,
`std::runtime_error` (`range_error`, `system_error`) for conditions only
discovered while running, plus `std::bad_alloc` and `std::bad_cast`. Derive
your own from `std::runtime_error` rather than from `std::exception` directly —
you get the `what()` machinery for free.

*Seen in* `0x02/e_exception.cpp`.

### Exception safety <a id="exception-safety"></a>

What a function guarantees when it throws: *basic* (nothing leaks, invariants
hold), *strong* (the operation either completes or changes nothing — see
[copy-and-swap](#copy-and-swap)), or *nothrow* ([`noexcept`](#noexcept)).
[RAII](#raii) is what makes the basic guarantee automatic rather than a matter
of discipline.

*Seen in* `0x03/d_ctor_dtor_throw.cpp`, `0x0b/b_smart_ptr_v1.cpp`.

### `assert` and `static_assert` <a id="assert"></a>

`assert(cond)` checks a *precondition* at runtime and aborts if it fails
(disabled by `NDEBUG` in release builds) — for bugs, not for user input.
`static_assert(cond, "message")` checks at compile time and belongs in
templates, where it turns an invalid instantiation into a readable error.

*Seen in* `0x04/e_first_generics.cpp`.

---

## Move semantics and value categories

### lvalue and rvalue <a id="value-categories"></a>

An **lvalue** has a name and an address you can take (`v`, `v[0]`, `*p`); an
**rvalue** is a temporary about to expire (`42`, `f()`, `std::move(x)`).
`T&` binds to lvalues, `const T&` to both, and `T&&` binds only to rvalues —
which is what lets a function know its argument is disposable.

*Seen in* `0x0b/c_smart_ptr_v2.cpp`.

### Move semantics <a id="move-semantics"></a>

C++11. Transferring the guts of an expiring object instead of copying them: the
move constructor `T(T&&) noexcept` and move assignment `T& operator=(T&&)
noexcept` take the resource and leave the source in a valid but unspecified
state (typically null/empty).

```cpp
smart_ptr(smart_ptr&& other) noexcept : p_{other.p_} { other.p_ = nullptr; }
```

Cheap for anything that owns a heap allocation — `string`, `vector`,
`unique_ptr` — and identical to a copy for a plain `int`.

*Seen in* `0x0b/c_smart_ptr_v2.cpp`.

### `std::move` <a id="std-move"></a>

Moves nothing: it is a cast to an rvalue reference, saying "I am done with
this, take it if you can". After moving from an object, only assign to it or
destroy it. Do **not** `std::move` a return value of a local — it blocks
[copy elision](#copy-elision).

*Seen in* `0x0b/c_smart_ptr_v2.cpp`.

### Copy elision, RVO <a id="copy-elision"></a>

The compiler constructing the result directly in the caller's storage, so
neither a copy nor a move happens. Since C++17 this is *guaranteed* when a
function returns a prvalue (`return Temperature{20.0};`) and optional but
routine for a named local (NRVO). Which is why "return by value is slow" has
been folklore for a decade.

*Seen in* `0x0b/c_smart_ptr_v2.cpp`.

### Copy-and-swap <a id="copy-and-swap"></a>

An assignment operator written as: take the parameter **by value** (the copy or
move happens there, and may be elided), then swap with `*this`. Self-assignment
works, the [strong exception guarantee](#exception-safety) follows from the
copy happening before any state changes, and copy and move assignment collapse
into one function.

```cpp
smart_ptr& operator=(smart_ptr other) noexcept { swap(*this, other); return *this; }
```

*Seen in* `0x0b/b_smart_ptr_v1.cpp`.

---

## Smart pointers and ownership

### Ownership <a id="ownership"></a>

The answer to "who calls `delete`, and when". A raw pointer says nothing about
it, which is why modern C++ encodes it in the type: `unique_ptr` (exactly one
owner), `shared_ptr` (several), `weak_ptr` (none, observes), plain `T*` or
`T&` (a non-owning *reference*, valid only as long as the owner says).

*Seen in* `0x06/c_unique_ptr.cpp`.

### `std::unique_ptr` <a id="unique-ptr"></a>

The default smart pointer: sole ownership, zero overhead over a raw pointer,
non-copyable and movable, deletes on destruction. Create it with
`std::make_unique<T>(args…)`, hand it around with `std::move`, pass the
*object* (`const T&`) to functions that only use it.

```cpp
auto w = std::make_unique<widget>(42);
use(*w);                       // borrow
auto w2 = std::move(w);        // transfer; w is now null
```

*Seen in* `0x06/c_unique_ptr.cpp`.

### `std::shared_ptr` and `weak_ptr` <a id="shared-ptr"></a>

`shared_ptr` counts owners in a control block and deletes when the count hits
zero — pay for it only when ownership really is shared. Two objects pointing at
each other keep the count above zero forever: a *cycle*, i.e. a
[leak](#memory-leak). `weak_ptr` observes without owning and must be `lock()`ed
into a `shared_ptr` before use, which is how the observer pattern and
parent-pointers in trees are built.

Prefer `std::make_shared<T>(…)`: one allocation for object and control block.

*Seen in* `0x09/d_more_smartptrs.cpp`.

### Small Buffer Optimization (SBO) <a id="sbo"></a>

Storing small payloads inline in the object and only allocating on the heap
when they outgrow it — what most `std::string` implementations do for short
strings. Trades object size for allocations, and forces the class to implement
the [Rule of Five](#rule-of-zero) by hand, since the inline buffer cannot
simply be pointer-swapped.

*Seen in* `0x06/d_smallbuffer_example.cpp`.

---

## Functional C++

### Lambda expression <a id="lambda"></a>

An anonymous function object written where it is used:

```cpp
auto add = [](int a, int b) { return a + b; };
std::ranges::sort(v, [](const auto& a, const auto& b) { return a.size() < b.size(); });
```

`[](){}` in full: *capture*, *parameters*, *body*. Generic lambdas take
`auto` parameters (C++14); `mutable` allows modifying captured copies.

*Seen in* `0x0a/a_lambdas.cpp`.

### Lambda capture <a id="capture"></a>

What the lambda takes from the enclosing scope: `[x]` by copy, `[&x]` by
reference, `[=]` / `[&]` everything (avoid: it hides which), `[this]` the
enclosing object, `[x = std::move(y)]` an init-capture (C++14). A
by-reference capture that outlives its scope is a
[dangling reference](#dangling-pointer) — the standard bug in a lambda handed
to a thread.

*Seen in* `0x0a/a_lambdas.cpp`.

### Function pointer and `std::function` <a id="std-function"></a>

A function pointer (`double (*f)(double)`) stores exactly one kind of callable
and costs nothing. `std::function<double(double)>` is a type-erased wrapper
that stores *any* callable — function, lambda, functor — at the price of an
indirection and possibly an allocation. Use a template parameter where the type
can be deduced, `std::function` where it must be stored.

C++23 adds `std::move_only_function` for callables that cannot be copied.

*Seen in* `0x0a/b_functions.cpp`, `0x0b/d_ui_lib.hpp`.

---

## Concurrency

### `std::thread` <a id="thread"></a>

Starts a function on a new thread of execution; must be `join()`ed or
`detach()`ed before it is destroyed, or the program calls `std::terminate`.
Arguments are **copied** — wrap in `std::ref` to pass a reference.

```cpp
std::thread t{[&]{ work(); }};
t.join();
```

C++20's `std::jthread` joins in its own destructor ([RAII](#raii)) and supports
cooperative cancellation — prefer it where available.

*Seen in* `0x0a/d_threads.cpp`.

### Race condition <a id="race-condition"></a>

Two threads touching the same data with at least one writing, without
synchronization: the result depends on timing, and the standard calls it
[UB](#undefined-behavior) rather than "sometimes wrong". Not fixable by
reordering statements or adding sleeps — only by a
[mutex](#mutex), an atomic, or not sharing.

*Seen in* `0x0a/e_mutex.cpp`.

### `std::mutex` <a id="mutex"></a>

Mutual exclusion: one thread at a time inside the critical section.
`lock()`/`unlock()` by hand works until an exception or an early `return`
skips the unlock — which is why the manual form appears once in this course and
never again.

*Seen in* `0x0a/e_mutex.cpp`.

### `lock_guard`, `unique_lock`, `scoped_lock` <a id="lock-guard"></a>

[RAII](#raii) wrappers for a [mutex](#mutex): `std::lock_guard` locks on
construction and unlocks on destruction, full stop. `std::unique_lock` can also
unlock and relock, which [condition variables](#condition-variable) require.
`std::scoped_lock` (C++17) takes several mutexes at once, deadlock-free — the
sane default for multiple locks.

*Seen in* `0x0a/e_mutex.cpp`, `0x0b/a_condition_variables.cpp`.

### Condition variable <a id="condition-variable"></a>

Lets a thread wait until another signals that something changed, without
busy-waiting. Always used with a `unique_lock` **and** a predicate, because
waits may wake up for no reason (*spurious wakeup*):

```cpp
cv.wait(lock, []{ return done; });      // re-checks the predicate on every wake
```

The waiting side must hold the lock while testing the condition; the notifying
side sets it under the lock and then calls `notify_one`/`notify_all`.

*Seen in* `0x0b/a_condition_variables.cpp`.

### Deadlock <a id="deadlock"></a>

Two threads each holding what the other needs, both waiting forever. Avoid by
locking in a fixed global order, holding one lock at a time, or taking them
together with `std::scoped_lock`.

---

## Principles and style

## Background Narrowing Conversion
When converting between data types with potential loss of precision
(narrowing conversion) classical style initialization gives a warning
while uniform initialization gives an error; e.g.
- `int v2 = 123456789012345;`
- `int v3{123456789012345};`
  or
- `int a = 1.2;`
- `int b{1.2};`



### Zero overhead                           <a id="zero-overhead"></a>

C++'s central bargain: you do not pay for what you do not use, and what you do
use you could not have hand-coded better. No garbage collector, no universal
base class, no [vtable](#vtable) unless a class declares a virtual function,
no exception cost on the path that does not throw. The corollary is the
unforgiving half — no bounds checks either, unless you ask for `.at()`.

### Const correctness                       <a id="const-correctness"></a>

Marking everything that does not modify observable state as
[`const`](#const) — parameters, locals, member functions — so the compiler
enforces intent and the reader knows what a call can do. It also composes:
a `const` object is only usable through `const` member functions, so a single
missing `const` propagates outward until someone reaches for
[`const_cast`](#casts).

Const-correctness is a core building block of C++ design: express intent by
defaulting to const and relaxing only when mutation is required.

const ≠ immutable
– const applies to the object you see, not necessarily the whole reachable state. It is
a compile-time qualifier preventing certain writes through a particular handle.
– Immutability means the entire logical state of the object never changes after construction.
It is a stronger design property about an object’s whole (logical) state over time.

### Value semantics <a id="value-semantics"></a>

Objects behave like values: copying makes an independent object, assignment
overwrites, equality compares contents. Java-style reference semantics happen
in C++ only where you ask for them (pointers, references,
[`shared_ptr`](#shared-ptr)). Most bugs from other languages arrive here — the
copy that was expected to be an alias, or the alias expected to be a copy.

*Seen in* `0x03/c_copy_assignment.cpp`, `0x08/a_single_inheritance.cpp`.

### Code Style <a id="code-style"></a>

The points that matter most in this course:

- four spaces, no tabs; braces on the same line as the statement
- `lower_snake_case` for functions and variables, `_` suffix for data members
- `[[nodiscard]]` on functions whose result is the whole point
- `const` by default — on member functions, parameters and locals
- prefer uniform initialization (`double x{1.0}`) over `=` where it reads clearly
- one demo function per topic, called from `main`

### The Modern Way <a id="the-modern-way"></a>

Solving a problem the way the standard library and the
[Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
suggest rather than the way C would: values and references over raw pointers,
RAII over manual cleanup, range-based `for` over index arithmetic, `constexpr`
over macros, unnamed namespaces over file-scope `static`. Shortest definition:
code that makes an experienced C++ reader nod instead of reach for a debugger.
