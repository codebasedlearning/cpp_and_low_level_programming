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

# Questionnaires

Answers to the questionnaires at the end of the tasks, and our results for the AI warm-ups. Try first, then read. The
outputs were checked with gcc on Linux; where your platform may differ, it says so.

## Unit 0x01 <a id="questionnaire-0x01"></a>

### `assert`

**Activate the failing `assert`. What does the program print, and what is its exit status?**

It prints program name, file, line, function and the condition, e.g. `...: Assertion 'is_even(3)' failed.`, and aborts.
On Linux and macOS the shell reports 134 = 128 + 6, the signal number of `SIGABRT`. Windows shows a dialog or a message,
with exit code 3.

**Build it with `-DNDEBUG`, or as 'Release', and run it again.**

Every `assert` is gone: the failing one does nothing, the program ends normally with 0, and `disable_asserts_in_release`
reports that `NDEBUG` is defined. CLion's 'Release' configuration passes `-DNDEBUG`.

**Why is `assert(++count < 10);` a bad idea?**

The increment is part of the `assert` and disappears with it in a release build - the program then behaves differently
in Debug and Release. An `assert` may only check, never do work the program needs.

### Control flow

**In `skip_and_leave_loops`: what changes if you swap the two `if`s?**

Then `i >= 10` is checked before the output: for `i == 10` the loop is left before printing. The output is 5 ... 9
instead of 5 ... 10.

**In `branch_with_switch`: remove the `break` after `case 3` and set `n` to 3.**

Both `case 3` and `case 4` are printed - execution falls through into the next `case` until the next `break`. With
`-Wextra` gcc warns: "this statement may fall through".

**What does `if (int n2 = n * n > 500)` do instead?**

`>` binds tighter than `=`, so it means `int n2 = (n * n > 500)`: `n2` is the `bool` result converted to `int`, i.e.
`1`. The condition is still right, but the output says "n^2=1". The init-statement form `if (int n2{n * n}; n2 > 500)`
avoids exactly this.

**Why is `switch` on a `string` not allowed?**

`switch` works on integral and enumeration values, and the `case` labels must be constants - the compiler turns it into
comparisons or a jump table. A `string` is a class type, its comparison is a function call. Use `if`/`else`, or map
strings to numbers first.

### Strings

**Line 2 in `search_strings` prints a huge number. Why that one?**

`string::npos` is the largest value of the unsigned `size_type`, i.e. `-1` converted to unsigned: 18446744073709551615 =
2^64 - 1 on a 64-bit system. No valid position can have that value, so it serves as "not found".

**What does `s[42]` print instead of `s.at(42)`? Is that an answer?**

Whatever lies in memory there - a strange character, nothing, or a crash. It is undefined behavior, so no output is an
answer. `s.at(42)` throws `std::out_of_range`; uncaught, the program ends with a message like
`basic_string::at: __n (which is 42) >= this->size() (which is 7)` and exit status 134.

### `goto` (optional)

**How different are `loop_with_for` and `loop_with_goto` in the assembly?**

Hardly: both are a compare, a conditional jump and a jump back (`cmp`/`jle`/`jmp` on x86, `cmp`/`ble`/`b` on ARM), with
almost the same number of instructions. The small differences come from `i++ < 10` versus `++i` and `i <= 10`, not from
`goto`.

**Rewrite `loop_with_goto` with `while`. Which version is easier to read?**

`int i{5}; while (i <= 10) { cout << ...; ++i; }` - the condition is at the top, where a reader looks for it, and the
body is a block.

**A third way out of nested loops is a `bool` flag.**

`bool found{false};` and `for (int i{1}; i <= 9 && !found; ++i)` for both loops, `found = true;` instead of the `goto`.
It works, but the flag has to be checked in every loop condition. Most people prefer the function with `return`: the
search gets a name, and the loops end at once.

**Java has `break outer;`. Why might that be less of a loss than it seems?**

Nested loops that need a labeled break are usually a search - and a search is best a function of its own, left with
`return`. For the rare remaining cases, `goto` to a label right after the loops does the same.

**What problem does `goto cleanup;` solve in C?**

Releasing resources on every error path: all paths jump to one place at the end that frees memory and closes files in
reverse order. C++ does this with destructors (RAII), which run automatically on every way out of a scope - including
exceptions.

### Integer types (optional)

**`long double` has 16 bytes on x86-64 Linux, but only 8 on Windows and Apple Silicon. What does that mean for a file?**

A binary file that stores one is not portable: written on Linux, 16 bytes in x87 extended format (10 bytes used); read
on Windows or a Mac, 8 bytes of an IEEE `double` are expected - the result is garbage. Binary formats need types of
fixed size and format, e.g. `double`, or text.

**Why is "a byte" and "a small number" the same type in C++?**

History: in C, `char` is the smallest addressable unit - used for characters and for raw bytes alike - and
`int8_t`/`uint8_t` are just other names for `signed char`/`unsigned char`. Hence streams print them as characters. C++17
added `std::byte` for bytes that are neither characters nor numbers.

**What happens with `uint8_t u{255}; ++u;`? And with `int8_t i{127}; ++i;`?**

`u` becomes 0, `i` becomes -128. Neither is an overflow: the arithmetic happens in `int` (256 and 128), and only the
conversion back to the small type wraps around. For unsigned types that is always defined (modulo 256), for signed ones
since C++20.

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

## Unit 0x02 <a id="questionnaire-0x02"></a>

### `auto`

**In `deduce_references`, replace `auto&` by `auto` in the first loop.**

The loop multiplies copies, the vector stays `{1, 2, 3}` - the second loop prints 1, 2, 3 instead of 10, 20, 30.

**What type does `auto x{v.size()};` have?**

`std::vector<int>::size_type`, i.e. `std::size_t` - an unsigned 64-bit type on 64-bit systems (`unsigned long` on Linux
and macOS, `unsigned long long` on Windows). Comparing it with an `int` gives the signed/unsigned warning.

**Why is `for (const auto& x : v)` a good default, and when is plain `auto` just as good?**

It never copies and never changes the element, whatever its type - a `string`, a `struct`, a `vector`. For small,
cheap-to-copy types like `int`, `double` or a pointer, `auto` is just as good and possibly a little faster: the value
lands in a register, with no address to follow.

### Exceptions

**Swap the first two `catch` blocks in `catch_in_order`. Does it change anything?**

No: `load` throws a `runtime_error`, and `out_of_range` is not related to it (it derives from `logic_error`). The order
only matters when one type is a base of the other - then the first matching `catch` wins, so the more specific one must
come first.

**Remove the outer `try` in `rethrow`. What happens, and what is the exit status?**

The rethrown exception is not caught anywhere, so `std::terminate` is called:
`terminate called after throwing an instance of 'std::runtime_error' what(): file not found`, then abort - exit status
134 on Linux and macOS.

**In C++ you can `throw 42;`. Why is that still a bad idea?**

An `int` carries no message and no meaning, `catch (const std::exception& e)` does not catch it, and there is no
`what()`. Callers would need `catch (int)` - and would have to know about it. Throw types derived from `std::exception`,
e.g. `std::runtime_error`.

### String conversion

**Why does `stoi("12abc")` not throw? How could you find out that there was more text?**

`stoi` converts as many characters as it can and throws only if there is no number at the start at all. The second
parameter tells how many characters were used: `std::size_t used{}; const int n{stoi(s, &used)};` gives `used == 2` - if
`used != s.size()`, there was more text.

**`to_string(1.5)` gives "1.500000". Where do the zeros come from?**

`to_string` for floating-point numbers is specified like `printf("%f")`, which always prints six decimal places.
`std::format("{}", 1.5)` gives the shortest exact form, "1.5". (C++26 changes `to_string` to that behavior.)

### Reading with a check (optional)

**What happens if you enter a number that is too large for an `int`?**

The read fails: `fail()` is true, and since C++11 `n` is set to the largest (or smallest) `int`, 2147483647. So check
the stream, not the value.

**Why is the error state sticky? Compare with `errno` in C.**

Once the stream has failed, every further read fails at once, until `clear()` - so you can read several values and check
once at the end, and no error gets lost. The bad input is still in the buffer, hence `ignore`. `errno` is a global
number that failing C functions set and nobody resets - it has to be checked right after the call, and it blocks
nothing.

### `const` and `constexpr` (optional)

**Look at the assembly (`g++ -S`): is there a call to `times4` for `b`? And for `m`?**

For `b` never: a `constexpr` variable must be computed by the compiler, `12` is in the code. For `m` there is a call at
`-O0` (`call` on x86, `bl` on ARM); at `-O2` it is inlined and the call is gone.

**Why can `std::array<int, n>` not take a plain `const int n` that comes from `cin`?**

The size is part of the type, and types must be known at compile time. `const` only means "does not change after
initialization" - a value read at runtime is still unknown to the compiler. `constexpr` is the promise "known at compile
time".

### `format` and `println` (optional)

**`printf("%d\n", 3.14);` compiles in C. What does it print, and why is that dangerous?**

Garbage, a different number on every run: `%d` makes `printf` read an `int`, but a `double` was passed - on x86-64 and
ARM even in a different register. It is undefined behavior; `printf` cannot check its arguments, only compilers warn
(`-Wformat`). `std::format` checks the format string against the arguments at compile time.

**Why do we still use `cout` in this course?**

It works with every compiler the students use - `std::format` and `std::println` need a recent standard library, which
is why the snippet asks for the feature-test macros. And streams are where `operator<<` for your own types comes in
later.

### Trailing return types (optional)

**Some style guides use `auto f() -> type` for every function. What speaks for it?**

The function names line up in one column, the style is the same as for lambdas and for return types that depend on the
parameters, and in a member function defined outside the class the return type can use the class's names without
qualification.

**What is `decltype(2 * 1.5f)`?**

`float`: the `int` is converted to `float` before the multiplication (usual arithmetic conversions).

### `vector` vs. `list` (optional)

**How many `int`s of the `vector` come with one 64-byte cache line - and how many of the `list`?**

16 of the `vector` - 64 / 4, and the next ones are loaded ahead. Of the `list` usually one: each node is a separate
allocation somewhere on the heap, and the next element is only known after the current node was loaded.

**How much memory does one `list` element need, compared to one `vector` element?**

A `vector` element needs 4 bytes (plus spare capacity). A `list` node holds two pointers and the `int` - 24 bytes with
padding - plus the allocator's bookkeeping: with gcc on Linux, consecutive nodes lie 32 bytes apart. So about 8 times as
much.

**When would you still choose a `list`?**

When elements must never move - pointers and references to them stay valid on insert and erase -, when you insert and
erase in the middle a lot with an iterator in hand, or to splice elements between lists without copying. In practice
rare: a `vector` usually wins, even for inserts in the middle of moderate sizes.

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
