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
