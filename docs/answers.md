# Answers

## A-101                                    <a id="a-101"></a>
`cout` is of type `ostream`.

## A-102                                    <a id="a-102"></a>
It is unknown and random. The variable just uses a specific memory location. Reading the value of an uninitialized local
variable, here of type `int`, is undefined behavior (UB) in C++
<br> - !![UB](glossary.md#undefined-behavior).

## A-103                                    <a id="a-103"></a>
`init_variable_on_stack` wrote 23 into its stack frame. When it returned, the frame was released, but not cleared. The
second call of `define_variable_without_init` gets a frame at the same place, and `v` happens to use the same slot - so
it "finds" the 23. This is an observation about one build, not a guarantee: with `-O2` or another compiler the output
differs. Reading `v` is still undefined behavior.
<br> - !![Stack and heap](glossary.md#stack-and-heap)

## A-104                                    <a id="a-104"></a>
`33 22 11 00` - the least significant byte comes first, at the lowest address. This is called little-endian, and it is
what x86 and (usually) ARM do. Big-endian systems, and most network protocols, store the bytes in the order you write
them: `00 11 22 33`. The value is the same, only its order in memory differs.

## A-105                                    <a id="a-105"></a>
The compiler decides where each variable lives in the stack frame - the order in the source is no promise. It places
each variable at an address that suits its type (alignment): an `int` at a multiple of 4, a `double` at a multiple of 8.
Bytes that are left over stay unused (padding). Order and gaps can change with another compiler or with `-O2`.
<br> - !![sizeof](glossary.md#sizeof)

## A-106                                    <a id="a-106"></a>
- `string hello{"Hello!"};` creates the object itself, here on the stack - no `new`, no reference.
- A C++ `string` is mutable (`+=`, `replace`, ...), a Java `String` is not.
- Assignment copies the text (value semantics); in Java it copies the reference.
- There is no garbage collector: the memory is released when `hello` goes out of scope.
- `size()` counts bytes, not characters - it makes a difference for text beyond ASCII.

## A-107                                    <a id="a-107"></a>
No, both are equally big. `sizeof(string)` belongs to the type and is fixed at compile time - 32 bytes with gcc's
standard library (libstdc++, also used by clang on Linux), 24 with Apple clang's (libc++). The object holds only the
bookkeeping: where the characters are, the size, the capacity - and, for short texts, room for the characters
themselves. Long texts live outside the object, on the heap.
<br> - !![Small string optimization](glossary.md#sso)

## A-108                                    <a id="a-108"></a>
`s2` is a variable on the stack, so it stays where it is. Its text no longer fit into the object, so the string
requested memory on the heap, copied the characters there and now refers to them. That is the cost: an allocation and a
copy, invisible in the source code. Appending in a loop can trigger it again and again - `reserve()` helps if you know
the final size.

## A-109                                    <a id="a-109"></a>
2,692,537. Every call with `n > 1` makes two more calls, so the number of calls grows like the Fibonacci numbers
themselves - exponentially: `fib(n)` needs `2*fib(n+1) - 1` calls. Each of them creates and removes a stack frame. A
loop needs 30 steps and no extra frames.

## A-110                                    <a id="a-110"></a>
It depends on the build. With `-O0` every call gets its own stack frame; a million of them do not fit into the usual 8
MB stack, and the program crashes with a segmentation fault (exit status 139). With `-O2`, gcc turns the recursion into
a loop: no crash at all, and it prints `0` - the product overflowed long before, and after enough factors of 2 all its
bits are 0. Same source code, two completely different results - that is undefined behavior in practice.
<br> - !![UB](glossary.md#undefined-behavior)

## A-201                                    <a id="a-201"></a>
On the stack, like `n` - a pointer is an ordinary variable whose value happens to be an address. It has an address of
its own (`&p`) and a size of its own: 8 bytes on a 64-bit system, no matter what it points to. So there are two
variables here, `n` and `p`, and `p` contains `&n`.
<br> - !![Pointer](glossary.md#pointer)

## A-202                                    <a id="a-202"></a>
Here, no: the compiler simply uses `n` wherever `m` appears - `&m` is `&n`, and `sizeof(m)` is `sizeof(int)`. The
standard leaves open whether a reference needs storage at all. When a reference is passed to a function or stored in a
struct, it is usually implemented as an address - 8 bytes on a 64-bit system - that the code follows.
<br> - !![Reference](glossary.md#reference)

## A-203                                    <a id="a-203"></a>
No. For small types like `int`, `double` or a small struct, a copy is as cheap as passing an address - and the function
then works on its own local value instead of reaching into the caller's memory. Rule of thumb: pass cheap-to-copy types
by value, everything else that is only read by `const&`, and use `&` only if the function should change the argument. By
value is also right when the function needs its own copy anyway.
<br> - !![Parameter passing](glossary.md#parameter-passing)

## A-204                                    <a id="a-204"></a>
To the memory where the first element was before. `push_back` found the vector full (capacity 3), reserved a new block,
copied the elements over and released the old one. `first` still refers to the old place - it dangles. Reading it is
undefined behavior: it may print 1, some other value, or crash. The same holds for every reference or address into a
`vector`: after an operation that may reallocate (`push_back`, `insert`, `resize`, ...), take it again.
<br> - !![Dangling](glossary.md#dangling-pointer)

## A-205                                    <a id="a-205"></a>
Because C++ promises that members are laid out in declaration order - each member has a larger address than the one
before. Code relies on this: binary file formats, network protocols, hardware registers, and interfaces to C, where both
sides must agree on the layout. So the order is your decision - and so is the padding. Some languages, e.g. Rust,
reorder by default.
<br> - !![sizeof](glossary.md#sizeof)

## A-301                                    <a id="a-301"></a>
No. `private` is a rule the compiler checks when it translates your code - in the object file there is no trace of it,
and the bytes of a private member are ordinary memory like any other. The debugger shows them, and any code with the
object's address could read or write them. `private` protects against mistakes, not against someone who wants to get
in: it is about who may use a member in the source code, not about security.
<br> - !![Access modifiers](glossary.md#access-modifiers)

## A-302                                    <a id="a-302"></a>
Because there is only one destructor, but there may be many constructors. The destructor destroys the members in reverse
order of their construction - so that order must be the same for every constructor, and the only order all of them
share is the declaration order. If each constructor could choose its own order, the destructor would have to know which
constructor built the object.
<br> - !![Member initializer list](glossary.md#member-initializer-list)

## A-303                                    <a id="a-303"></a>
The caller reserves the memory for `m` in its own frame and passes its address to `make_tracer` - a hidden argument,
like `this` (on x86-64 Linux and macOS in register `rdi`, on ARM64 in `x8`). `make_tracer` constructs the `tracer`
directly at that address, so there is nothing left to copy. Since C++17 this is guaranteed when a function returns an
unnamed temporary, as in `return tracer{name};`. For a named local, `tracer t{name}; return t;`, compilers usually do
the same (NRVO), but they do not have to - that is the difference `-fno-elide-constructors` makes visible.
<br> - !![Copy elision](glossary.md#copy-elision)

## A-304                                    <a id="a-304"></a>
Because the destructor does the job - and does it better. In Java, every user of a resource must remember a `finally`
block, at every place where the resource is used. In C++, the cleanup is written once, in the destructor of the class
that owns the resource, and the compiler calls it at every exit of the scope: the normal end, `return`, `break`, and
stack unwinding. It cannot be forgotten. Java's `try`-with-resources and C#'s `using` follow the same idea - but there,
the user of the resource still has to remember to write them.
<br> - !![RAII](glossary.md#raii)

## A-305                                    <a id="a-305"></a>
Because the One Definition Rule treats classes differently from functions. A function or a variable must be defined
exactly once in the whole program - its definition becomes code or memory in one object file, and the linker would not
know which of two to take. A class definition produces no code of its own; it only describes the layout and the members
for the compiler, so it may appear in every translation unit that needs it, as long as all of them are identical. That
is exactly what including the header does. The same holds for member functions defined inside the class body - they
are implicitly `inline`.
<br> - !![ODR](glossary.md#odr)

## A-306                                    <a id="a-306"></a>
When the object dies - the reference points to the member, and the member is part of the object.
`const string& n{ada.name()};` is fine as long as `ada` lives. The classic trap is a temporary:
`const string& n{person{"Ada"}.name()};` dangles right after the `;`, because the `person` is destroyed at the end of
the full expression. Reading `n` is undefined behavior. When in doubt, take a copy: `const string n{...};`.
<br> - !![Dangling](glossary.md#dangling-pointer)

## A-401                                    <a id="a-401"></a>
Nowhere. A mangled name encodes the name, its namespace or class, `const`, and the parameter types - not the return
type. The linker only needs to tell different functions apart, and in C++ two functions cannot differ in the return
type alone: `int f(double)` and `double f(double)` in one program are an error, and they would get the same symbol,
`_Z1fd`. The exception are instances of function templates: `largest<int>` has the return type in its name (`T_` after
the template arguments), because two instances may really differ only in it.
<br> - !![Symbols](glossary.md#symbols)

## A-402                                    <a id="a-402"></a>
No - by value. A `string_view` is two words, and on x86-64 (Linux, macOS) and ARM64 two words are passed in two
registers: `where_by_view` gets the size in `rdi` and the address in `rsi` (libstdc++ order). With
`const string_view&`, the caller has to put the view into memory first and pass its address - one more indirection, to
save nothing. (The Windows x64 convention passes everything larger than 8 bytes via a hidden address anyway, so there
it makes no difference.) The same holds for `span` - and in general for small types that are cheap to copy.
<br> - !![Parameter passing](glossary.md#parameter-passing)

## A-403                                    <a id="a-403"></a>
Something between the right text and garbage - undefined behavior. With gcc and clang on Linux, the view pointed into
the heap block of the temporary `string`. After the `;` the block was released, but not cleared: typically the first
16 bytes are overwritten by the allocator's bookkeeping and the rest still reads `...he Miles Davis album from 1959`. As
Release, clang printed only garbage. None of that is a guarantee - the next allocation may reuse the block at any
moment. "It prints almost the right text" is the dangerous case: the bug goes unnoticed. AddressSanitizer
(`-fsanitize=address`, Linux and macOS) reports it as `heap-use-after-free`.
<br> - !![Dangling](glossary.md#dangling-pointer)

## A-404                                    <a id="a-404"></a>
From the type, at compile time. `vector<int>::iterator` and `list<int>::iterator` are two different classes, each
with its own `operator++`: one adds 4 to the address, the other loads the address of the next node from the current
one. The compiler picks the function by the static type of `it` - there is no tag in the iterator and no check at
runtime. That is also why `print_all` is compiled once per container: each instance calls another `++`.
<br> - !![Iterator](glossary.md#iterator)

## A-405                                    <a id="a-405"></a>
Because most of it depends on `T`. When the template is defined, the compiler checks what does not depend on `T` - the
syntax, names that must exist anyway. Whether `value_ * 2` makes sense can only be decided when `T` is known, at the
instantiation - and member functions of a class template are only instantiated when they are called. So
`maybe<string>` is fine as long as nobody calls `twice`. The price: an error in rarely used template code shows up
late, and far from where it is caused - hence the "required from" lines. Concepts (C++20) state the requirements up
front: see future snippets.
<br> - !![Instantiation](glossary.md#instantiation)

## A-406                                    <a id="a-406"></a>
One value: `nullptr`. `maybe<int*>{nullptr}` says `has_value() == false`, although a value was given - "empty" and "a
null pointer" are the same bytes now. For most uses of an optional pointer that is fine, and it saves 8 bytes; but it
changes the meaning for one value, and a user of the template may not expect that. `std::optional<int*>` does not do
it: it keeps its flag, and costs 16 bytes.
<br> - !![Specialization](glossary.md#specialization)

## A-407                                    <a id="a-407"></a>
One of them - in practice, with the GNU and LLVM linkers, the first one it sees in the order of the object files; the
other copies are dropped. It does not matter which, because they were compiled from the same definition in the same
header, and the ODR guarantees they are the same. If they are not - two different definitions under one name - it does
matter, and nobody warns you: see the extension of task 'Fox Hollow'. As Release, there may be no copy left at all,
because every call was inlined.
<br> - !![ODR](glossary.md#odr)

## A-501                                    <a id="a-501"></a>
The copies. `swap_by_value` got its own `a` and `b` - copies of 1 and 2, in its own stack frame (as Release: in
registers). It swapped them correctly - and at its `}` the frame was released, with the swapped copies in it. The
caller's `n` and `m` were never touched. The other two versions got addresses instead of values - once visibly, `&n`,
once invisibly, as a reference - and wrote through them into the caller's frame.
<br> - !![Parameter passing](glossary.md#parameter-passing)

## A-502                                    <a id="a-502"></a>
Because another translation unit might call it. `square` is not in the unnamed namespace, so it can be used from other
object files (a `T` in `nm`, see unit 0x04). The compiler of this file cannot know whether one does - so it keeps a
callable copy, although it inlined every call in this file. Move `square` into an unnamed namespace, and it disappears
from the Release assembly: nobody outside can call it, and nobody inside does any more.
<br> - !![Symbols](glossary.md#symbols)

## A-503                                    <a id="a-503"></a>
Because the machine is not the only one who reads the program.

- Not every machine has flat addresses. With segmented memory (the 8086, some microcontrollers), an address is a segment
  and an offset, and `p - 1` at the start of a segment wraps or traps. On CHERI processors, a pointer carries its
  bounds, and leaving them is an error in hardware. The rule keeps C and C++ portable to all of them.
- The optimizer relies on it: a pointer that stays inside its array cannot wrap around, so `p + n > p` for `n > 0`, and
  a loop from `first` to `last` has a known number of steps - the basis for unrolling and vectorization.
- Checking tools rely on it: they can report an out-of-range pointer the moment it is created, not only when it is used.

The address one past the end is the exception, because every loop needs it as its end.
<br> - !![UB](glossary.md#undefined-behavior)

## A-504                                    <a id="a-504"></a>
Because `operator<<` has an overload for `const char*`, and none for `const int*`. The `const char*` overload treats its
argument as a C string - that is what a `char*` usually is in C - and prints the characters up to the `'\0'`. `numbers`
decays to a `const int*`, and the best match for it is the overload for `const void*`, which prints the address. So an
address of characters must be converted to `const void*` to be printed as an address. And a `char*` that does not point
to a C string makes `cout` read on until it happens to find a zero byte.
<br> - !![C string](glossary.md#c-string)

## A-505                                    <a id="a-505"></a>
In this file, probably neither: as Release, both are small enough to be inlined, and after inlining the compiler keeps
the minutes and seconds in registers either way. The difference shows where a real call remains - in another translation
unit, or for a bigger function. Then `to_duration` returns both results in `rax` (ARM64: `x0`), while `split_seconds`
writes them to memory through two addresses: the caller must give `minutes` and `seconds` a place in memory, because
their addresses are taken, and load them again after the call - plus the null check. So the returned struct is at least
as fast, usually a little faster, and simpler to use. Out-parameters pay off when the caller wants to reuse an existing
object, e.g. a large buffer.
<br> - !![Parameter passing](glossary.md#parameter-passing)

## A-506                                    <a id="a-506"></a>
With gcc 13 and clang 18 on x86-64 Linux, in all four builds: ` 1| p=0`, and then the program is killed -
`Segmentation fault`, exit status 139 (128 + 11, the number of the signal SIGSEGV); CLion shows the exit code and names
the signal. ` 2|` never appears. No memory is mapped at address 0, the processor refuses the write, and the operating
system ends the program. On Windows the same is an access violation, exit code `0xC0000005` (not tested here). That is
the kind case. Do not rely on the crash either: in another program of ours, clang as Release dropped a line that
dereferenced a null pointer it could see, and the program ran on as if nothing had happened.
<br> - !![nullptr](glossary.md#nullptr)

## A-507                                    <a id="a-507"></a>
gcc returns a null pointer. The address of `n` is useless to the caller anyway, so gcc returns `nullptr` instead - even
as Debug (gcc 11 and 13, x86-64 and ARM64): `number` prints as `0`, and a dereference crashes at once instead of reading
garbage. clang returns the real address of `n` - an address in the released part of the stack: a dangling pointer.
Printing it is harmless; dereferencing it reads whatever the next call left there. Both are allowed, because the
behavior is undefined; gcc's choice makes the bug visible. Both compilers warn without any option.
<br> - !![Dangling](glossary.md#dangling-pointer)

## A-508                                    <a id="a-508"></a>
That depends on the compiler, the options, the platform - and on the rest of the function. Our runs on x86-64 Linux:
clang as Debug placed `guard1` directly behind `a` and printed `guard1=99`. gcc as Debug and as Release, and clang as
Release, printed both guards unchanged: `a[3]` hit padding or another slot of the frame. gcc on ARM64 Linux, in a
similar program: unchanged as well. In a slightly different program, gcc as Debug hit the guard value of the stack
protector, and the program ended with `*** stack smashing detected ***`. The addresses printed in ` 1|` tell you who is
behind `a` - not what the compiler makes of it: even if the write hits a guard, a Release build may print the old value,
because the compiler assumes that nobody changes `guard1` but through its name. For a `const` guard it would print the
constant in any case - that is why they are not `const`.
<br> - !![UB](glossary.md#undefined-behavior)

## A-509                                    <a id="a-509"></a>
`memset` sets bytes, not elements. `memset(b, 1, sizeof(b))` writes the byte `01` sixteen times, so every `int` is
`01 01 01 01` - `0x01010101`, which is 16843009. It gives the expected `int`s only for 0 (all bytes 0) and -1 (all bytes
`ff`, in two's complement). For any other value, `std::fill(std::begin(b), std::end(b), 1)` sets the elements.
<br> - !![C string](glossary.md#c-string)

## A-510                                    <a id="a-510"></a>
Nowhere. `const` is a promise the compiler checks; the machine code of a function that only reads through a `const int*`
is the same as with an `int*`, and so is the address. What `const` can change is what the optimizer may assume - a
`const int` with a known value may be used as a constant. And the linker puts `const` data and string literals into a
read-only section (`.rodata` on Linux); a write there - after a cast that removes the `const` - is stopped by the
processor, like a write to address 0. That protection comes from the memory page, not from the type.
<br> - !![Const pointers](glossary.md#const-pointer)

## A-511                                    <a id="a-511"></a>
Because there is no pointer in `grid` that an `int**` could point to. `grid` is six `int`s in one block. It decays to a
pointer to its first element - and its first element is a row: `grid` becomes a `const int (*)[3]`, a pointer to an
array of three `int`s, and `grid[i][j]` is computed from that one address, `i * 3 + j` elements further. With an
`int**`, `p[i]` loads the `i`-th pointer from memory and then indexes it - which is what `rows` needs. Same syntax, two
different machines. A function for such a 2D array takes `const int grid[][3]`, i.e. `const int (*grid)[3]` - the number
of columns is part of the type.
<br> - !![Pointer arithmetic](glossary.md#pointer-arithmetic)

---

# Comprehension Check

What you should be able to say for each point of the Comprehension Check at the end of the tasks, and our results for
the AI warm-ups. Try first, then read. The outputs were checked with gcc on Linux; where your platform may differ, it
says so.

## Unit 0x01 <a id="check-0x01"></a>

**I can create, build and run a C++ program, with CMake and with `make`.**

The compiler translates each source file into an object file, the linker joins the object files and the libraries
into an executable, and the operating system runs it, starting at `main`. `g++ hello.cpp -o hello` does compiling and
linking in one go. `make` rebuilds only what changed, driven by a `makefile`; CMake generates such build files from a
`CMakeLists.txt`, and CLion runs it for you. A typo is a compiler error, an `undefined reference` a linker error.

**I can define and initialize variables, and I know why we prefer `{}`.**

`int v{42};` works for every type, `int v{};` gives the default value 0, and braces reject conversions that lose
information: `int b{1.2};` does not compile, `int a = 1.2;` silently gives 1.

**I can explain what an uninitialized variable contains and why reading it is UB.**

Whatever was in that memory before - often a value left behind in the stack frame of an earlier call. Reading it is
undefined behavior: the standard promises nothing, and the optimizer may assume it never happens, so Debug (`-O0`) and
Release (`-O2`) can show completely different results.

**I know that the sizes of types depend on the platform, and how to find them out.**

The standard only fixes minimum sizes and their order; `long`, for example, has 8 bytes on Linux and macOS, but 4 on
Windows. `sizeof(T)` gives the size, `std::numeric_limits<T>::max()` the largest value. If the number of bits matters,
use the fixed-width types from `<cstdint>`, e.g. `std::int64_t`.

**I know where the characters of a `string` live.**

The `string` object has a fixed size (32 bytes with gcc, 24 with Apple clang) and sits where the variable is, e.g. on
the stack. A short text fits into the object itself (small string optimization, up to 15 or 22 characters), a longer
one lives on the heap. `c_str()` shows where. When a string grows beyond its capacity, the characters move: an
allocation and a copy you do not see in the code.

**I know the difference between `s[i]` and `s.at(i)`, and what `string::npos` means.**

`s[i]` does not check the index - out of range it is undefined behavior. `s.at(i)` checks and throws
`std::out_of_range`; you pay for the check only if you ask for it. `find` returns a position, or `string::npos` for
"not found": the largest value of the unsigned size type, 18446744073709551615 = 2^64 - 1 on a 64-bit system - no valid
position can have it.

**I can declare and define functions, and I know what a call does on the stack.**

A declaration gives the signature, which is enough to call the function; the definition adds the body, exactly once.
Every call gets its own stack frame with its parameters and local variables, which is released when the function
returns - recursion makes that visible: `factorial(5)` means five frames, and too deep a recursion runs out of stack.

**I can predict what happens on integer overflow.**

Signed overflow is undefined behavior: no error, no exception - usually a wrong value, but the optimizer may also
assume it never happens, and then e.g. a loop does not end. Unsigned types wrap around, well-defined: 255 + 1 is 0 in
8 bits. Checking before is possible without overflowing: `if (result > max / b)`.

**I know the control structures `if`, `for`, `while`, `do`-`while` and `switch` - and what a `switch` without `break`
does.**

They work as in Java; C++ adds `if (init; condition)`, whose variable lives only inside the `if`/`else`. `switch`
works on integral and enumeration values with constant labels - not on a `string`. Without `break`, execution falls
through into the next `case`; `[[fallthrough]]` marks it as intended.

**I can test with `assert`, and I know why an `assert` must never do work the program needs.**

`assert(condition)` does nothing if the condition holds; otherwise it prints file, line and condition and aborts -
exit status 134 on Linux and macOS. In a release build (`NDEBUG` defined) every `assert` disappears, including the
code inside it: `assert(++count < 10);` counts in Debug and not in Release.

### 'AI' - Two Opinions <a id="ai-0x01"></a>

Whether the loop ends depends on the build, because signed overflow is undefined behavior. With gcc: at `-O0` it
prints `done, n=2147483647` after about 2 s; at `-O2` it never ends - the optimizer assumes that `i` never overflows,
so `i > 0` is always true, and gcc even warns: "iteration 2147483646 invokes undefined behavior". Other compilers or
versions may do something else again.

Answer A:

- "counts up to 2147483647, the largest `int`" - right, on all platforms of this course.
- "wraps around to -2147483648" and "it ends" - that is what happens at `-O0`, but nothing guarantees it: it is
  undefined behavior, and at `-O2` it does not end. A describes one build as if it were the language.

Answer B:

- "Signed overflow is undefined behavior", "the compiler may assume that `i > 0` always holds" - right, the key point.
- "never prints anything" - wrong as a general statement: at `-O0` it does print. "May" in the first sentence was right,
  "never" in the second is not.
- "use `long long`, then the loop ends correctly" - wrong: the overflow is only postponed, it is still undefined
  behavior - and at about a billion steps per second, 2^63 steps take centuries. A real fix is a condition that does
  not rely on overflow, e.g. `i < std::numeric_limits<int>::max()`, or an unsigned type, whose wrap-around is defined.

B is more useful - it names the real problem - but both sound equally sure of themselves, and both contain a wrong
statement. That is the point: fluency is not correctness.

## Unit 0x02 <a id="check-0x02"></a>

**I can explain what a reference is, and why it is not a copy.**

A reference is a second name for an existing object: same object, same address (`&m == &n`), no copy. It must be
initialized and cannot be rebound - an assignment through it changes the object. Where a reference must exist at
runtime, as a parameter or a member, the compiler usually implements it as an address.

**I can choose between passing by value, by reference and by `const&` - and justify it.**

By value copies the argument into the function's stack frame: fine for small types like `int`, `double` or a small
`struct`, expensive for a `vector` or a long `string`. `const&` passes the object itself, read-only - the default for
anything bigger that is only read. `&` only if the function should change the argument. By value is also right when
the function needs its own copy anyway.

**I can say where the elements of a `std::array` and of a `std::vector` live.**

A `std::array` is its elements - `sizeof` is exactly their size, and a local array lies completely on the stack. A
`std::vector` object (24 bytes: where, how many, how many fit) sits on the stack, its elements on the heap; `data()`
gives their address. A `vector` has no small buffer - even three elements are on the heap.

**I can explain `size` and `capacity`, and why a reference into a `vector` can dangle.**

`size` is the number of elements, `capacity` the number that fit into the memory already reserved. When it is full,
`push_back` reserves a larger block (by a factor of about 1.5 or 2), copies the elements over and releases the old
block. Every reference, pointer or iterator into the old block then dangles - reading through it is undefined
behavior. `reserve` avoids the moves if you know the size in advance.

**I can estimate the memory of a `struct`, including padding.**

The members lie in declaration order, each at an address that is a multiple of its alignment (for the primitive types
usually their size). Gaps between them are padding, and the whole `struct` is padded to a multiple of its largest
alignment, so that in an array the next element is aligned, too. `char, double, char` needs 24 bytes, `double, char,
char` only 16 - order the members from large to small. `offsetof` shows where each member starts.

**I can measure the cost of a copy instead of guessing it.**

Put a `stopwatch` around the work, build as Release (`-O2`) - otherwise you measure the missing optimization -, repeat
the work often enough to get more than a few milliseconds, and use the result, e.g. print it, so the optimizer cannot
drop it. Then compare, e.g. passing a big `vector` by value and by `const&`.

**I know what `auto`, `auto&` and `const auto&` deduce, especially in a range-based `for`.**

`auto` deduces the type from the initializer and drops references and top-level `const`: it always makes a copy.
`auto&` refers to the element and may change it, `const auto&` refers to it read-only - the good default in a
range-based `for`, except for small types like `int`, where a copy is just as cheap. `auto x{v.size()};` is a
`std::size_t`, not an `int`.

**I can throw, catch and rethrow standard exceptions, and I know why the order of the `catch` blocks matters.**

Throw objects of types derived from `std::exception`, e.g. `throw std::runtime_error{"file not found"};`, and catch
them by `const&`. The `catch` blocks are tried from top to bottom, and the first matching one wins - so a derived type
must come before its base, otherwise the base catches everything. `throw;` passes the same exception on. An exception
that nobody catches ends the program with `std::terminate` - exit status 134 on Linux and macOS.

**I can convert between text and numbers with `stoi`, `stod` and `to_string`, and handle what goes wrong.**

`stoi` and `stod` read a number from the start of a string: they throw `std::invalid_argument` if there is none and
`std::out_of_range` if it does not fit, and they stop at the first character that does not fit - `stoi("12abc")` is 12;
the optional second parameter tells how many characters were used. `to_string` goes the other way, for a `double` with
six decimal places: `to_string(1.5)` is "1.500000".

### 'AI' - Two Opinions <a id="ai-0x02"></a>

| | `sizeof(std::string)` | `sizeof(std::vector<int>)` | characters before the heap |
|:--|:--|:--|:--|
| gcc (libstdc++) | 32 | 24 | 15 |
| Apple clang on Apple Silicon (libc++) | 24 | 24 | 22 |
| MSVC, Release | 32 | 24 | 15 |

Answer A:

- "32 bytes, a pointer, the length, a 16-byte buffer, up to 15 characters" - right for gcc (and MSVC in Release).
- "on every 64-bit system" - wrong: with libc++, e.g. on a Mac, it is 24 bytes and 22 characters. A generalizes from one
  library to all of them.
- "`vector<int>` is 24 - three pointers" - right (MSVC Debug builds can be larger, because of added checks).

Answer B:

- The `string` part is right, including that it depends on the library.
- "`vector<int>` has 24 bytes everywhere" - right for Release builds.
- "like a `string` it keeps a few elements inside the object" - wrong: `std::vector` has no small buffer, its elements
  are always on the heap. `v.data()` of a vector with three elements is far away from `&v` - as in the session.

A is wrong about the platforms, B about `vector` - each is mostly right, and each has one confident statement that
does not hold. Even the layouts differ: on Apple Silicon the characters start at the object's address and the size is
in the last byte; with gcc a pointer comes first.

## Unit 0x03 <a id="check-0x03"></a>

**I can explain what an object is in memory, and what `sizeof` of a class includes - and what not.**

An object is its data members, in declaration order, with padding - exactly like a `struct`. Member functions,
constructors, the destructor and `private` add nothing: the code of a member function exists once in the program, not
in each object. `sizeof` of a class with two `double`s is 16, with or without twenty member functions. A hidden pointer
appears only with `virtual` functions - that comes later.

**I can explain what `this` is, how a member function knows its object, and what a `const` member function promises.**

A member function is an ordinary function with a hidden first parameter: the address of the object it was called on,
available as `this`. `p.scale(2.0)` compiles to the same machine code as a free function `scale(&p, 2.0)` - on x86-64
the address arrives in `rdi`, on ARM64 in `x0`. Inside, `x_` is short for `this->x_`. A `const` after the parameter
list makes `this` a pointer to a `const` object: the function promises not to change the object, and only such
functions can be called on a `const` object or through a `const&`.

**I can write constructors with a member initializer list, and I know in which order the members are initialized.**

The member initializer list, `: begin_{begin}, end_{begin_ + length}`, initializes the members before the body runs;
assigning in the body instead means initializing first and overwriting after - impossible for `const` members and
references. The members are initialized in declaration order, whatever the order of the list; the compiler warns
(`-Wreorder`) if the two differ. Default member initializers, `int id_{-1};`, apply to every constructor that does not
set the member itself.

**I know when a constructor should be `explicit`, and what an implicit conversion creates.**

A constructor with one argument is also a conversion: with `tower(int)`, `eiffel = 4;` compiles, and the compiler
builds a temporary `tower` from the 4, assigns it, and destroys it at the `;` - an object nobody wrote. `explicit`
forbids that; the temporary must then be written, `tower{4}`. Rule of thumb: one-argument constructors are `explicit`,
unless the conversion is the point.

**I can tell a copy construction from a copy assignment, and I know what the generated versions do.**

A copy construction creates a new object - `tracer b{a};`, `tracer c = a;`, and every pass by value. A copy
assignment overwrites an existing one - `d = a;`. The generated versions copy or assign member by member, each member
with its own copy operation: 4 bytes for an `int`, a new heap block for a `string`. So whether a copy is deep or
shallow is decided by the members.

**I can say exactly when a destructor runs: at the `}`, at the `;`, and during stack unwinding.**

A local object is destroyed at the closing brace of its block, in reverse order of construction; the compiler inserts
the calls there. A temporary is destroyed at the end of the full expression, the `;`. A member is destroyed after the
destructor body of its object, in reverse declaration order. When an exception passes through a function, its frame is
removed and its local objects are destroyed on the way to the `catch` - stack unwinding.

**I know what happens when a constructor throws, and why a destructor must not throw.**

If a constructor throws, the object never existed: its destructor does not run, but the members that were already
constructed are destroyed, in reverse order - nothing leaks. A destructor that throws during stack unwinding would
create a second exception while the first is still in flight; the program ends with `std::terminate` (exit status 134
on Linux and macOS). That is why destructors are implicitly `noexcept`.

**I can explain RAII, and why C++ needs no `finally`.**

Resource Acquisition Is Initialization: a resource - memory, a file, a lock, a measurement - is owned by an object,
acquired in its constructor and released in its destructor. Since the destructor runs at every exit of the scope,
including stack unwinding, the cleanup cannot be forgotten, and it is written once, in the class, instead of in a
`finally` at every use. `string`, `vector` and the `scope_timer` of the session work this way.

**I can split a class into a header and a source file, and I can read the typical linker errors.**

The header holds the class definition - data members and declarations of the member functions - protected by
`#pragma once` or an include guard; the source file includes it and defines the member functions as
`double temperature::celsius() const { ... }`. All `.cpp` files go into the `add_executable`, the header does not. The
typical linker errors, as gcc on Linux shows them (Apple clang says the same in other words):
- A missing `.cpp` file, or a declared but never defined function:
  `undefined reference to 'fraction::fraction(int, int)'` (Apple clang: `Undefined symbols for architecture arm64`).
- A function body in a header that is included in two `.cpp` files: `multiple definition of 'twice(int)'; ... first
  defined here` (Apple clang: `duplicate symbol`). Fix: move the body into the `.cpp` file, or mark it `inline`.
- Without `#pragma once`, a header included twice in the same file is not a linker error but a compiler error:
  `redefinition of 'class fraction'` - the class appears twice in one translation unit.

**I can write getters that neither copy needlessly nor give away the members.**

A getter is `const`. For small members it returns a copy; for bigger ones, e.g. a `string`, a `const&` avoids the copy -
but the reference is only valid while the object lives. A getter that returns a non-`const` reference hands out write
access without any check - as good as a public member. Better than a setter for every member: member functions with a
purpose, which keep the rules of the class.

**I know which special member functions the compiler generates, and how to `= default` or `= delete` them.**

Default constructor, copy constructor, copy assignment and destructor (and since C++11 the two move operations): the
compiler generates them member by member, unless you declare them yourself - and the default constructor disappears as
soon as you declare any constructor. `= default` asks for the generated version explicitly, `= delete` forbids a
function, e.g. the copy operations of a timer. Rule of Zero: with members that clean up after themselves, write none of
them. Rule of Three: if you need a destructor, you need the copy operations, too - or delete them.

### 'AI' - Two Opinions <a id="ai-0x03"></a>

| | `sizeof(point)` | with `~point()` | empty class |
|:--|:--|:--|:--|
| gcc, clang (Linux, macOS), MSVC | 16 | 16 | 1 |

Answer A:

- "Member functions are stored once, in the code, not in every object" - right.
- "only the data members count", "16 bytes" - right.
- "because it has a user-defined destructor, the compiler adds a hidden pointer ... 24 bytes" - wrong: a destructor is
  a function like any other, it adds nothing. A hidden pointer, the vptr, comes only with `virtual` functions -
  including a `virtual` destructor. A mixes up two things it has heard about.

Answer B:

- "16 bytes", "neither member functions nor constructors nor destructors take space", "neither does `private`" - right.
- "a class without data members takes no memory at all: its `sizeof` is 0" - wrong: `sizeof` is at least 1, because
  two different objects must have different addresses - in an array, for example. (As a base class or with
  `[[no_unique_address]]`, an empty class may take no space - but that is another story.)

A is wrong about the destructor, B about the empty class - each follows a correct rule one step too far.

### 'AI' - Explain the Machine Code <a id="ai2-0x03"></a>

With x86-64 gcc `-O1` (the same structure with clang on ARM64), `f` contains four calls of `tracer::~tracer()`:

- The normal path: constructor of `a`, constructor of `b`, `work()`, destructor of `b`, destructor of `a`, `ret` - the
  reverse order, exactly where the `}` is.
- A cleanup path for an exception from `work()`: destructor of `b`, destructor of `a`, then `_Unwind_Resume`, which
  continues the unwinding in the caller.
- A cleanup path for an exception from the constructor of `b`: only the destructor of `a` - `b` was never born. gcc
  shares the second call with the path above and jumps into it.

The cleanup paths ("landing pads") are never reached by a jump from the normal code. When an exception is thrown, the
runtime looks up in a table (the `.gcc_except_table`, used by `__gxx_personality_v0`) which landing pad belongs to the
current return address, and continues there - frame by frame. That is stack unwinding, compiled. The normal path
contains no instruction for it: an exception costs time only when it is thrown ("zero-cost exceptions").

Typical weak spots of an LLM explanation, worth checking:
- "the extra destructor calls are for copies" or "for Debug builds" - wrong: there are no copies, and it is `-O1`.
- "after every call, the function checks whether an exception occurred" - wrong for this model: there is no check on the
  normal path, the table does the work.
- Mixing up which landing pad destroys what - the constructor case, with only one destructor, is the one most often
  missed.
- With a local gcc on Linux you may also see `__stack_chk_fail`: a stack protector, enabled by default in some
  distributions - nothing to do with exceptions.

A breakpoint in the destructor, with `work` throwing, shows `f` directly below the destructor, marked at its `}` - the
frames of `work` and of the unwinder are gone already: the runtime has jumped into the landing pad of `f`.

## Unit 0x04 <a id="check-0x04"></a>

**I can explain what a `string_view` is in memory, and when it is cheaper than a `const string&`.**

Two words: the address of the first character and the number of characters - 16 bytes on a 64-bit platform, whatever
the length of the text. It points into the characters of a `string`, a literal or any other buffer; creating one,
copying one and `substr` copy nothing. It is cheaper than `const string&` whenever the caller does not have a `string`
at hand: for a literal, a `const string&` parameter needs a temporary `string` first - a copy, and a heap allocation
for longer texts. Passed by value, a view travels in two registers.

**I know why a `string_view` can dangle, and why `data()` of a view is not a C string.**

A view owns nothing, and it does not extend the lifetime of anything: a view of a temporary `string` - e.g. the result
of a function returning `string` - points into released memory right after the `;`, and neither gcc nor clang warns.
The same holds for a view of a `string` that is changed or destroyed later. `data()` is only the address; the view
knows its length, the characters behind it do not. A view of a part of a text is not followed by `'\0'`, so printing
`data()` as a `const char*` runs on to the end of the whole text - or beyond.

**I can use `std::span` for functions that read or change elements of an array, a vector, or a part of them.**

`span<const int>` as a parameter accepts a `std::array`, a `std::vector` and a part of them (`first`, `last`,
`subspan`) - a pointer and a count, no copy. `span<int>` allows changing the elements; the `const` of the parameter
itself only means that the view will not look elsewhere. With the count in the type, `span<int, 3>`, it is a single
pointer. Like every view, it dangles if the elements move or die - e.g. after a `push_back` that reallocates.

**I can explain what an iterator into a `vector` is, and why `++it` on a `list` iterator does something else.**

A `vector` iterator is essentially the address of an element, wrapped in a class: `*it` reads there, `++it` adds
`sizeof(T)`, `&*it` is the address - and with `-O2` nothing but the address is left. A `list` stores every element in a
node of its own, with the addresses of the next and the previous node; its iterator is the address of a node, and
`++it` loads the next address from it. Both are one word; the type decides at compile time which `++` is called. That
is also why `it + n` exists only for random-access iterators.

**I know when iterators become invalid.**

When the element they point to moves or dies. For a `vector`: after a `push_back` or `insert` that exceeds the
capacity, all iterators, references and pointers into it are invalid (they hold the old address); after `insert` or
`erase` without reallocation, those behind the position. For a `list`, `set` or `map`, only the iterators to erased
elements - nodes never move. In a loop, `it = v.erase(it)` continues with a valid iterator. `end()` is never valid to
dereference.

**I can write an `operator<<` for my class, and I know why it returns the stream.**

A free function `ostream& operator<<(ostream& os, const T& x)` that writes the parts of `x` to `os` and returns `os`.
The stream by reference, because streams cannot be copied; the object by `const&`. `cout << a << b` is
`operator<<(operator<<(cout, a), b)` - without the returned stream, the chain breaks after the first `<<`. Because it
takes an `ostream&`, it works for `cout`, a file stream and an `ostringstream` alike.

**I know what `[[nodiscard]]` is for, and what it costs at runtime.**

It asks the compiler to warn when a result is thrown away - for functions whose result is the point: a computed value,
an error code, a handle. It is a warning, not an error (unless you build with `-Werror`), and it costs nothing at
runtime: the machine code and the symbols are the same with and without it. It is one of several attributes, e.g.
`[[deprecated]]` and `[[maybe_unused]]`.

**I can write function and class templates, and I know that the compiler generates code per type - and only for what is
used.**

A template is a recipe: for every combination of template arguments that is used, the compiler generates a separate
function or class - `largest<int>`, `largest<double>` - as if written by hand, with its own symbol and, for classes,
its own layout and `sizeof`. A template that is never used generates nothing, and the member functions of a class
template are generated only when called: `nm` shows `maybe<int>::twice`, but no `maybe<double>::twice`. Errors that
depend on the type appear only at the instantiation.

**I can specialize a template for a particular type, and I know why one would.**

A full specialization, `template <> class maybe<bool> { ... };`, replaces the recipe for exactly one type; a partial
specialization, `template <typename T> class maybe<T*> { ... };`, for a whole group of types (class templates only).
The specialization may have entirely different members and layout - e.g. one byte instead of two for `bool`, no flag
for pointers - or different behavior. The compiler picks the most specialized version that fits. For function
templates, an ordinary overload is usually the simpler tool.

**I can read the output of `nm`: defined, undefined, local and weak symbols, and a mangled name.**

Each line: an address, a letter, a name. `T` - defined here, callable from elsewhere; `U` - needed here, defined
elsewhere; `t` - defined here, but local (unnamed namespace, `static`); `W` - a weak definition, e.g. a template
instance or an `inline` function (macOS `nm` shows them as `T`). The names are mangled: `_Z4areadd` is `area(double,
double)` - `_Z`, the length of the name, the name, the parameter types. Members are nested, `_ZNK6circle4areaEv` is
`circle::area() const`. `nm -C` or `c++filt` demangle them.

**I can explain why templates and `inline` functions are defined in headers, and what the linker does with their
copies.**

The compiler can only generate `largest<double>` where it sees the whole template - so every translation unit that uses
it needs the body, i.e. the header. If the body is only in a `.cpp` file, the other files compile to `U`s, the `.cpp`
file generates nothing for types it does not use itself, and the linker reports `undefined reference`. So every object
file that uses the template, or an `inline` function, contains its own copy as a weak symbol `W`; the linker keeps one
and drops the others - they must be identical (ODR), which nobody checks. As Release, small ones are inlined, and no
copy is left at all. A member function defined in the class body is `inline`, too - it becomes a `W` as well. Ordinary
functions, in contrast, are defined once, in a `.cpp` file, and appear as `T` in exactly one object file. In the
extension of 'Fox Hollow', two different `inline int scale(int)` in `fraction.cpp` and `main.cpp`: as Debug, both files
call the same one - the one from the first object file the linker sees, so swapping the files in the `add_executable`
swaps the result; as Release, each file has its own inlined copy, and the output looks right. gcc 13 and clang 18 warn
in neither case, not even with `-flto -Wodr`.

**I can use `vector`, `list`, `set`, `map` and `unordered_map` - create, insert, find, erase - and I know how they
store their elements.**

A `vector` is created from a list of elements with braces, `vector<int>{5, 23}` (two elements), or with parentheses from
a count and a value, `vector<int>(5, 23)` (five times 23), or from a range of two iterators. `push_back` and
`emplace_back` (builds the element in place) at the end are cheap; `insert` and `erase` in the middle or at the front
move every element behind the position. `clear` keeps the capacity, `shrink_to_fit` asks to release it. For all
containers: `insert`/`push_back`/`push_front`, `find` (returns `end()` for "not found") or `contains`, `erase` by value
or by iterator, and a range-based `for` over all of them; for maps, the elements are `pair<const K, V>`, conveniently
taken apart with `const auto& [key, value]`. `map[key]` inserts a missing key with a default value - for lookups, use
`find`, `contains` or `at`. Prefer a member `find` to `std::find`: on a `set` of a million elements, `std::find` walks
node by node, the member function goes down the tree in about 20 steps. `list`, `set` and `map` keep every element in a
node of its own (doubly linked, or a balanced tree, sorted); the unordered containers put nodes into buckets chosen by a
hash - O(1) on average, no order. Only `vector`, `array` and, in blocks, `deque` store the elements next to each other.

### 'AI' - Two Opinions <a id="ai-0x04"></a>

| | `sizeof(string_view)` | `sizeof(string)` | `string_view` passed in |
|:--|:--|:--|:--|
| gcc (libstdc++), Linux x86-64 | 16 | 32 | two registers (`rdi`: size, `rsi`: address) |
| clang (libstdc++), Linux x86-64 | 16 | 32 | two registers |

Answer A:

- "a pointer and a length - 16 bytes, passed by value in two registers" - right (on x86-64 Linux and macOS, and on
  ARM64).
- "accepts literals, strings and parts of strings without a copy" - right.
- "a literal must first become a temporary `string`" for a `const string&` - right.
- "a `string_view` always ends with a `'\0'`, so you can hand `sv.data()` to C functions" - wrong: a view of a part of a
  text, `string_view{text}.substr(0, 4)`, prints the whole rest of the text via `data()`. A view of a buffer without
  any `'\0'` makes `printf` read on until it happens to find one. For a C function, build a `string` from the view and
  pass `c_str()`.

Answer B:

- "Both avoid copies when you pass a `string`, but only `string_view` avoids one for a literal" - right.
- "pass it by value, not as `const string_view&`" - right.
- "bound to a temporary, it keeps the temporary alive ... `const string_view name{make_name()};` is fine" - wrong: only
  a reference bound directly to a temporary extends its lifetime. A `string_view` is an object of its own, initialized
  with the address of the characters; the temporary `string` dies at the `;`, and `name` dangles. Printing its address
  and size works, printing its content is undefined behavior.

A is wrong about the terminating `'\0'`, B about the lifetime - both mistakes confuse a view with a `string`, and both
compile without a warning.

## Unit 0x05 <a id="check-0x05"></a>

**I can compute with pointers - `p + n`, `p[n]`, `q - p` - and say how many bytes each step is.**

`p + n` is the address `n` elements further: `n * sizeof(T)` bytes for a `T*` - 4 for an `int*`, 8 for a `double*`, 1
for a `char*`. `p[n]` is defined as `*(p + n)`, `++p` moves to the next element. `q - p`, for two pointers into the same
array, is the number of elements between them, a `ptrdiff_t` - the byte distance divided by `sizeof(T)`. On x86-64 the
scaling is part of the instruction: `mov eax, DWORD PTR [rdi+rsi*4]` is `p[i]` for an `int*`, ARM64 writes it
`ldr w0, [x0, x1, lsl 2]`.

**I know which addresses I may compute and which I may dereference, and what "one past the end" means.**

Inside an array, the address of every element, and one more: one past the last element. That one may be computed and
compared - it is the end of every pointer loop and what `end()` returns - but not dereferenced. Every other address
outside the array is undefined behavior as soon as it is computed, even without `*`; a single object counts as an array
of one element. The machine would not care, but the optimizer and the checking tools rely on the rule.

**I can explain array decay: when it happens, what is lost, and how C, `std::array` and `std::span` deal with it.**

In almost every expression, an array converts to a pointer to its first element - in `const int* p{a}`, in `a + 1` and
`a[i]`, and when it is passed to a function: a parameter `int a[4]` is adjusted to `int*`, and the 4 is ignored. What is
lost is the count: `sizeof` of the pointer is 8, and nothing checks the length. The exceptions: `sizeof(a)`, `&a`, and
binding the array to a reference - that is how `std::size(a)`, `std::begin(a)` and the range-based `for` know the count.
C passes the count as a second parameter, or marks the end in the data, as C strings do with `'\0'`. `std::array` is a
struct around the C array: it never decays, and it is copied and passed like any other value. `std::span` takes the
address and the count from the array type before the decay - a view that keeps the count.

**I can tell `a` from `&a` for an array, and predict `a + 1` and `&a + 1`.**

For `int a[4]`, both print the same address. `a` decays to an `int*`, the address of `a[0]`; `&a` is an `int (*)[4]`,
the address of the whole array. The type decides the step: `a + 1` is 4 bytes further, `&a[1]`; `&a + 1` is 16 bytes
further, one past the whole array. Accordingly, `sizeof(*a)` is 4 and `sizeof(*&a)` is 16.

**I can work with C strings, and I know why `strlen` walks, why `cout` prints a `char*` as text, and why `==` is the
wrong comparison.**

A C string is an array of `char` that ends with a `'\0'`; the literal `"Kind"` has 5 bytes. Its length is stored
nowhere, so `strlen` walks to the `'\0'` - on every call, in time proportional to the length, while
`std::string::size()` reads a member. `operator<<` has an overload for `const char*` that prints the characters up to
the `'\0'`; to print the address, convert it to `const void*`. `==` on two `const char*` compares addresses: two arrays
with the same text are different, and whether two equal literals share their characters is up to the compiler. `strcmp`
compares the characters and returns 0 for equal texts; as soon as one side is a `std::string`, `==` compares characters,
too.

**I can pass a `std::string` to a C function and back, and I know what `extern "C"` does to a symbol.**

`s.c_str()` gives a `const char*` to the characters, with a guaranteed `'\0'` - valid until `s` is changed or destroyed.
Back: `string{p}` copies up to the `'\0'`, `string{p, n}` exactly `n` characters. `extern "C"` gives a function C
linkage: its symbol is the plain name - `count_vowels` (on macOS `_count_vowels`) instead of a mangled `_Z...` name - so
C code can call it. The other way round, the C headers declare their functions in an `extern "C"` block, so C++ calls
them under their C names: `nm` shows `U strlen`. Without mangling there is no overloading; the calling convention stays
the same.

**I can use pointers as out-parameters, with `nullptr` for "not needed" - and I know when a returned struct is the
better interface.**

The caller passes addresses, e.g. `divmod(n, d, &q, &r)`, and the function writes through them - after checking for
`nullptr` where a result is optional. The `&` at the call shows what may change, and C interfaces use this pattern
everywhere. Returning a struct, taken apart with a structured binding, is usually the better interface: no variables to
prepare, nothing that can be null, and up to 16 bytes come back in registers. Out-parameters pay off when an existing
object should be reused, e.g. a large buffer.

**I can say where the arguments and the result of a function travel on x86-64 and on ARM64, and why small structs are
cheap to pass by value.**

The calling convention of the platform decides - all compilers on it follow the same rules. x86-64 Linux and macOS:
integers and addresses in `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`, floating point in `xmm0` ... `xmm7`, the two kinds
counted separately; more arguments on the stack. A struct of up to 16 bytes travels in up to two registers, a larger one
is copied onto the stack by the caller. The result comes back in `rax` (and `rdx`) or `xmm0`; a larger one is built at
an address the caller passes in `rdi`. ARM64: `x0` ... `x7` and `d0` ... `d7`; structs of up to 16 bytes in registers,
and structs of up to four `double`s even in `d` registers; larger ones via the address of a copy the caller makes; a
large result at the address in `x8`. Windows x64: `rcx`, `rdx`, `r8`, `r9` or `xmm0` ... `xmm3`, by position, and
everything over 8 bytes via an address. So a small, trivially copyable struct costs no more than its members - no
memory, no indirection - while `const&` would force it into memory. Large structs and types with a copy constructor of
their own: by `const&`.

Our results for 'Glen Ferry', with gcc 13 (x86-64) and gcc 11 (ARM64, Linux), `-O1`:

| Function | x86-64 Linux, macOS | ARM64 Linux |
|:--|:--|:--|
| `mix` | `edi`, `rsi`, `dl` - result in `rax` | `w0`, `x1`, `w2` - result in `x0` |
| `scale` | `xmm0`, `edi` - result in `xmm0` | `d0`, `w0` - result in `d0` |
| `length_by_address` | `rdi` | `x0` |
| `length_by_value` | 24 bytes on the stack, copied by the caller | `d0`, `d1`, `d2` |
| `swapped` | `rdi` - result in `rax`, one `rol` swaps the halves | `x0` - result in `x0`, one `ror` |
| `make_vec3` | `xmm0` - result at the address in `rdi` | `d0` - result in `d0`, `d1`, `d2` |
| `nine` | six in registers, `g`, `h`, `i` on the stack | eight in `w0` ... `w7`, `i` on the stack |

- `scale` finds its `int` in `edi`, although it is the second parameter: integers and floating point values have
  separate lists of registers. On Windows, the position counts - `factor` goes to `edx`.
- `vec3` by value: in memory on x86-64, in three registers on ARM64. So `const&` is the better choice for
  `length_by_value` on x86-64 and Windows, by value on ARM64 - "small" depends on the platform.
- `mix` widens `c` with `movsx`, a sign extension, on x86-64, and with `uxtb`, a zero extension, on ARM64 Linux: there,
  a plain `char` is unsigned. On Apple silicon it is signed.
- Windows x64 (MSVC, from the documented convention - not tested here): `length_by_value` and `make_vec3` get an address
  in `rcx`, `swapped` (8 bytes) travels in `rcx` and comes back in `rax`, `nine` has four arguments in registers and the
  rest on the stack.

**I can read the assembly of a small function as Debug and as Release: arguments, stack frame, calls, result.**

As Debug (`-O0`): a prologue sets up the stack frame (`push rbp`, `mov rbp, rsp`; on ARM64 `stp x29, x30, ...` or
`sub sp, ...`), the arguments are stored from their registers into the frame (`mov DWORD PTR [rbp-4], edi`) and loaded
again for every use, `call` (ARM64: `bl`) jumps to another function, the result is put into `eax` (`w0`), and `ret`
returns. As Release (`-O2`): no frame, values stay in registers, small functions are inlined - the `call` disappears -
and a whole function may shrink to two instructions. The labels are the mangled names that `nm` shows.

**I can name the kinds of wild pointers, and explain why a program with one may seem to work.**

Uninitialized (leftover bytes used as an address), null (points to nothing - usually a crash, the kind case), dangling
(the object is gone: a local after its `}`, the address of a local returned, an element after its `vector` has moved)
and out of bounds (before or behind an array). The machine checks none of it. The memory behind a dangling pointer is
usually still there, with the old bytes, until something else uses it - so a read often gives the right value; a write
past an array often hits padding. And the compiler assumes that none of it happens, so the Release build may behave
differently from the Debug build. Warnings (`-Wall -Wextra`) and AddressSanitizer find many of them.

**I know why the compiler may drop a null check that comes after the dereference.**

Dereferencing a null pointer is undefined behavior, so the compiler may assume that a pointer that has been dereferenced
is not null. A later `if (p == nullptr)` is then always false, and the test is removed - gcc and clang do it from `-O1`
on. The check must come before the dereference.

**I can read `const` in a pointer declaration from right to left, and choose the right one for a parameter.**

`const int* p` (or `int const* p`): a pointer to a `const int` - `*p` cannot be written through `p`, but `p` may point
elsewhere. `int* const p`: a `const` pointer to an `int` - it cannot point elsewhere, but `*p` may be written; a
reference behaves like it. `const int* const p`: both. An `int*` converts to a `const int*`, not back. For parameters:
`const T*` for what the function only reads, `T*` for what it writes - the `const` before the `*` is part of the
contract with the caller; a `const` after the `*` only concerns the function's own copy of the address.

**I can use `strcpy`, `strncpy`, `memcpy` and `memset`, and I know what each of them trusts me with.**

`strcpy(dest, src)` copies up to and including the `'\0'`, `strcat` appends at the `'\0'` of `dest` - both trust that
`dest` is big enough. `strncpy(dest, src, n)` copies at most `n` characters, but writes no `'\0'` if `src` is longer -
so copy `n - 1` and terminate yourself. `memcpy(dest, src, bytes)` trusts that the count is in bytes (`sizeof`), and
that the areas do not overlap (`memmove` if they do). `memset(dest, value, bytes)` sets every byte - only 0 and -1 give
the expected `int`s. None of them knows the size of a buffer; `std::string`, `std::copy` and `std::fill` do that work
for you, with types.

**I can use a pointer to a pointer, e.g. to move the caller's pointer or to read `argv`.**

`int** pp{&p}` holds the address of the pointer `p`: `*pp` is `p`, `**pp` the `int`. A function that moves the caller's
pointer gets its address - `skip_spaces(&text)` with a `const char**` parameter - or a reference, `const char*&`, which
is the same machine code. `argv` is an array of C strings, passed as `char**`: `argv[0]` is the program, `argv[1]` ...
`argv[argc - 1]` are the arguments, and `argv[argc]` is `nullptr`. An array of pointers is not a 2D array:
`int grid[2][3]` is one block, row after row, and decays to a pointer to its first row, `int (*)[3]` - not to an
`int**`.

### 'AI' - Two Opinions <a id="ai-0x05"></a>

| | `sizeof(a)` | `a`, `&a` | `a + 1`, `&a + 1` | C array parameter | `std::array<int, 4>` parameter |
|:--|:--|:--|:--|:--|:--|
| gcc and clang, Linux x86-64 | 16 | the same address | 4 and 16 bytes further | 8 | 16 |

Answer A:

- "`int a[4]` is four `int`s, 16 bytes, and no pointer is stored anywhere" - right.
- "in almost every expression, `a` converts to a pointer to its first element" - right.
- "`a` and `&a` print the same address" - right.
- "since they are the same pointer, `a + 1` and `&a + 1` are the same address, too" - wrong: the same address, but not
  the same pointer. `&a` is a pointer to an array of four `int`s, so `&a + 1` is 16 bytes further, `a + 1` only 4.

Answer B:

- "`a` becomes `&a[0]` as soon as you use it, so `a[i]` is just `*(a + i)`" - right, apart from the exceptions:
  `sizeof`, `&` and binding to a reference.
- "a parameter `int a[4]` is really an `int*` - `sizeof(a)` in the function is 8" - right, on a 64-bit platform, and the
  compiler warns about that `sizeof`.
- "`std::array` ... pass one by value, and it decays to a pointer, too" - wrong: a `std::array` is a struct with a C
  array inside. Passed by value, it is copied with all its elements, `sizeof` of the parameter is 16, and `size()` still
  works. It never decays - that is what it is for.

A is wrong about `&a + 1`, B about `std::array` - both mistakes take "the same address" for "the same thing".
