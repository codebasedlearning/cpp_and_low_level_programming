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


## A-601                                    <a id="a-601"></a>
`u` does - or rather its destructor. `u` is a local object, so it dies at the `}` of `own_with_a_unique_ptr`, after
` 2| end of function`; its destructor calls `delete` for the address it holds, and `delete` runs the destructor of the
`tracer`: the ` b|` line comes last. No line of the function says `delete` - the compiler inserted the call of the
`unique_ptr` destructor at the `}`, as for every local object.
<br> - !![unique_ptr](glossary.md#unique-ptr)

## A-602                                    <a id="a-602"></a>
Two pointers. A `std::list` is doubly linked: each node holds the address of the next node and of the previous one, 8
bytes each on a 64-bit platform, then the 4 bytes of the `int` - and 4 bytes of padding, so that the next node's
pointers are aligned. 24 bytes for 4 bytes of data, the same with gcc's and clang's libraries. And each node is a block
of its own, so the allocator adds its share: with glibc, 32 bytes per node. A `vector` of 1000 `int`s takes one block
of 4000 bytes.
<br> - !![Containers](glossary.md#containers)

## A-603                                    <a id="a-603"></a>
Because the stack needs no bookkeeping. The locals of a function get their place when the frame is set up - one
subtraction from the stack pointer for all of them - and lose it when the frame is removed. A heap block is a call of
`operator new`, and the allocator has to find a free block of the right size, note its size, take it out of its lists,
and be safe against other threads; `delete` puts it back. Then the memory: with glibc, a 4-byte `int` takes a 32-byte
block, so a million of them are 32 MB instead of 4 MB, and every access goes through a pointer that must be loaded
first. The stack, on the other hand, is small and used all the time - it is almost always in the cache. Our runs: a
million blocks 40 to 80 ms, one block 9 to 18 ms on x86-64 Linux; 19 to 33 ms and 2 to 6 ms on ARM64 Linux, in a
virtual machine on Apple silicon.
<br> - !![Stack and heap](glossary.md#stack-and-heap)

## A-604                                    <a id="a-604"></a>
Because of what the members are. The generated copy constructor copies member by member. `tracer` has a `string`, and a
`string` copies itself deeply: a new block with the same characters - so the generated copy of `tracer` is right (Rule
of Zero). `int_buffer` has an `int*`, and copying a pointer copies the address, not the block: two owners. The hint is
the destructor: a class that needs a destructor of its own manages a resource by hand, and then it needs its own copy
operations, too - the Rule of Three.
<br> - !![Rule of Zero / Three / Five](glossary.md#rule-of-zero)

## A-605                                    <a id="a-605"></a>
The same as for every `unique_ptr`: its destructor runs. It finds `nullptr`, and does nothing - `delete` of a null
pointer is allowed and does nothing, and the destructor tests for it anyway (`test` or `cbz` in the machine code). The
`tracer` that `u` owned before is not affected: it belongs to `v`, later to `w`, and is deleted exactly once, by its
last owner. That is the whole point of the null a move leaves behind - the source must be in a state its destructor can
handle.
<br> - !![std::move](glossary.md#std-move)

## A-606                                    <a id="a-606"></a>
Because `std::move` moves nothing. It is a cast: it turns the expression `a` into an rvalue, and that selects the `&&`
overload of `describe`. Whether anything is moved is up to the function that receives it - and `describe(int_buffer&&)`
only reads `b.size()`. A move happens only where a move constructor or a move assignment - or a function that does the
same - takes the object's resources. Still, after `std::move(a)`, treat `a` as moved-from: the caller cannot know what
the function did, only what it was allowed to do.
<br> - !![Value categories](glossary.md#value-categories)

## A-607                                    <a id="a-607"></a>
Only the control block. `shared_ptr{new tracer{...}}` has two blocks: the `tracer` and the control block. When the last
owner goes, the destructor of the `tracer` runs and its block is released at once; the control block stays, because
the `weak_ptr` still needs it - for `expired()`, `lock()` and its own count - until the last `weak_ptr` is gone. With
`heap_watch`: `live=2` before, 1 after `s.reset()`, 0 after `w.reset()` - the same numbers as with `make_shared`, but
now the object's bytes are gone at once. With `make_shared`, they are part of the control block's allocation, and a
large object observed by a long-lived `weak_ptr` keeps all its memory.
<br> - !![shared_ptr and weak_ptr](glossary.md#shared-ptr)

## A-608                                    <a id="a-608"></a>
The data are 48 bytes in each case. With glibc (x86-64 and ARM64 Linux), a block takes its size plus 8, rounded up to a
multiple of 16, and at least 32 bytes:

- one block, `unique_ptr<int[]>`: 48 bytes of `int`s, a 64-byte block, 1 allocation;
- one block per row: three blocks of 16 bytes (32 each) and the array of three pointers, 24 bytes (32): 128 bytes, 4
  allocations - and two loads per access;
- `grid`: the same 64-byte block as the first one, 1 allocation; the `vector` itself (24 bytes) and `columns_` are in
  the object, here on the stack.

`malloc_usable_size` (glibc) or the addresses in the log show the sizes. On macOS and Windows the allocators round
differently.
<br> - !![Stack and heap](glossary.md#stack-and-heap)

## A-609                                    <a id="a-609"></a>
The move constructor is generated only if the class declares none of: copy constructor, copy assignment, move
assignment, destructor; the move assignment only if it declares none of: copy constructor, copy assignment, move
constructor, destructor. `= default` counts as declared. So a destructor alone - for a log line, as in
`logged_buffer` - or a hand-written copy switches off the moves, silently: a `std::move` then selects the copy, which
is still generated (deprecated, but there). The other way round, declaring a move operation deletes the generated
copies. Hence the Rule of Five: if you declare one, declare all five - with `= default` where the compiler's version is
right.
<br> - !![= default and = delete](glossary.md#default-delete)

## A-701                                    <a id="a-701"></a>
`+=` changes its left operand, and the result of the expression is that operand itself - as for an `int`, where
`(a += b) += c` adds `b` and `c` to `a`. Returning `*this` by reference gives the caller the object, not a copy: nothing
is copied, and a chain changes the right object. `+` changes nothing; its result is a new amount that did not exist
before - a local object in `operator+`. A reference to it would dangle after the `return`, so it is returned by value -
and the compiler builds it right in the caller's place.
<br> - !![Operator overloading](glossary.md#operator-overloading)

## A-702                                    <a id="a-702"></a>
No, 2. The parentheses decide the order: `sum / count` is computed first, with two `int`s - an integer division, 2 -
and only then converted to a `double`, 2.0. The cast must be applied to an operand before the division:
`static_cast<double>(sum) / count` converts `sum`, then `count` is converted, too, and the division is one of two
`double`s. The same trap is in `const double average{sum / count};`.
<br> - !![Casts](glossary.md#casts)

## A-703                                    <a id="a-703"></a>
The compiler declares every special member that a class does not declare itself, but it writes - defines - only those
the program uses. `money sum{a};` copies: the copy constructor. `return sum;` needs a move constructor - even if the
compiler builds `sum` directly in the caller's place and never calls it, it must exist (C++ Insights marks `sum` as
`NRVO variable`). Nothing assigns a `money`, so there is no copy assignment; `copy_a_point` has `q = p;`, so `point`
gets one. In the machine, all of them are trivial: a copy of the bytes.
<br> - !![= default and = delete](glossary.md#default-delete)

## A-704                                    <a id="a-704"></a>
Because what counts for the calling convention is how an object is copied and destroyed, not which functions it has.
`money` has neither a copy constructor nor a destructor of its own: it is trivially copyable, and the compiler may copy
its 8 bytes like a `long long` - in a register, `rdi` or `x0`. A constructor that takes a `long long` and member
functions change nothing: they are functions, not data. A `unique_ptr` is 8 bytes, too, but has a destructor of its
own - and therefore travels via an address in memory.
<br> - !![Calling convention](glossary.md#calling-convention)

## A-705                                    <a id="a-705"></a>
No. With an overloaded `&&`, `a && b` is the call `operator&&(a, b)`, and a function is called only after all its
arguments have been evaluated - the function runs too late to prevent it. To delay the evaluation, the right operand
would have to be passed as something that computes its value only when asked - a function, or a lambda - and then it
would no longer look like `a && b`. Hence: do not overload `&&`, `||` and `,`.
<br> - !![Short-circuit evaluation](glossary.md#short-circuit)

## A-706                                    <a id="a-706"></a>
Our runs of `double_to_int(1e10)`:

- x86-64, gcc 13 and clang 18 as Debug: -2147483648. `cvttsd2si` returns `0x80000000` for every value that does not
  fit - the "integer indefinite" value, the same for a NaN.
- x86-64, gcc as Release: 2147483647. gcc computed the call at compile time and chose the largest `int`.
- x86-64, clang as Release: a different number in every build, e.g. 545028128. clang does not compute the result at
  all - it cannot be used in a correct program - and prints what is in the register.
- ARM64, gcc 11 as Debug and as Release: 2147483647. `fcvtzs` saturates: too large gives the largest `int`, too small
  the smallest, a NaN gives 0.

Four answers to one line - that is undefined behavior. Check the range before the conversion.
<br> - !![UB](glossary.md#undefined-behavior)

## A-707                                    <a id="a-707"></a>
Because the compiler did not read `limit` for the first one. A `const int` initialized with a constant is a constant
expression: wherever its value is used, the compiler writes 42 into the instruction - even as Debug. `*&limit` asks for
the object in memory, and there the write through the `const_cast` has put 43. In a correct program, both are the same:
the compiler may assume that a `const` object never changes, and the program broke that promise. For `global_limit`, the
memory itself is read-only, and the processor stops the write.
<br> - !![const](glossary.md#const)

## A-708                                    <a id="a-708"></a>
Call the function once, before the loop, and keep the value - `const int s{seed()};` - or a reference, if it is an
object. Then the loop works with a local, probably in a register. The test itself is cheap - one byte loaded, and a
branch that is almost always predicted correctly - but the compiler cannot remove it: any call may be the first one.
If the value can be computed at compile time, make it `constexpr`, or `constinit` for a variable that may change later:
then there is no initialization at run time, and no guard.
<br> - !![Three meanings of static](glossary.md#static-storage)

## A-709                                    <a id="a-709"></a>
Because it switches off the warning that helps most. With a `case` for every value and no `default:`, `-Wswitch` (in
`-Wall`) reports every `switch` that misses a value - add a value to the enum, and the compiler lists all the places
that must be updated. A `default:` takes the new value silently, and usually does the wrong thing with it. Values
without a name, like `static_cast<suit>(7)`, belong after the `switch`, as in `points_of`: the `switch` returns for
every named value, the line after it handles the rest.
<br> - !![enum and enum class](glossary.md#enum-class)

## A-710                                    <a id="a-710"></a>
Before two numbers of different types are compared, they are brought to one type - the usual arithmetic conversions.
`int` and `unsigned int` have the same size, and then the rules choose `unsigned int`. -1 becomes the `unsigned` with
the same bits, `0xffffffff`, which is 4294967295 - not less than 1. Nothing is converted in the machine: the processor
compares the same bits, only with the instruction for unsigned numbers (`setb` instead of `setl` on x86-64). gcc warns
with `-Wall`, clang with `-Wextra` (`-Wsign-compare`); `std::cmp_less` compares the values.
<br> - !![Integer conversions](glossary.md#integer-conversions)

## A-711                                    <a id="a-711"></a>
Because `it++` must return the old value: it makes a copy, counts, and returns the copy - which the loop throws away.
For an `int`, or a `vector` iterator, which is an address, the optimizer removes the copy, and both give the same
instructions. For a class with more state - an iterator of a checked library in a Debug build, say - the copy may stay,
at least as Debug. `++it` says what you mean, and never costs more.
<br> - !![Operator overloading](glossary.md#operator-overloading)

## A-712                                    <a id="a-712"></a>
The declaration in the class only says that the variable exists; its bytes are placed where it is defined. Without a
definition, every use is a symbol nobody provides: `undefined reference to thermometer::count_`. A definition in a
header ends up in every `.cpp` file that includes it - every object file has its own `thermometer::count_`, and the
linker reports a `multiple definition`. So: exactly one. `inline` (C++17) changes the rule, as for `inline` functions:
every file may contain the definition, the object files mark it (`u` with gcc), and the linker keeps one.
<br> - !![ODR](glossary.md#odr)

## A-713                                    <a id="a-713"></a>
When the numbers are small: 0 to 9 take two bytes as text - a digit and the `'\n'` - and always four as `int`s; the
numbers 0 to 999 take 3890 bytes as text, 4000 as binary. And text is often better anyway, because it is portable and
readable: every program on every machine reads "1000000" as the same number - and so does a person with an editor. The
binary file depends on the size of an `int` and on the byte order of the machine that wrote it, and one missing byte
shifts everything after it. Binary is for large amounts of data, where size and speed count - images, audio,
databases - and then with a documented format.
<br> - !![Streams](glossary.md#streams)

## A-801                                    <a id="a-801"></a>
A circle begins with its `shape` part, and that part is constructed first - by a constructor of `shape`, before the
members of `circle` and before the body of the constructor of `circle`. The member initializer list is the only place to
choose that constructor and to pass its arguments; in the body, it is too late - the shape exists already. Without
`shape{name}`, the compiler calls the default constructor of `shape` - and there is none, a shape needs a name: a
compiler error. Java writes `super(name)` as the first statement of the constructor, for the same reason.
<br> - !![Member initializer list](glossary.md#member-initializer-list)

## A-802                                    <a id="a-802"></a>
`offsetof` is guaranteed only for standard-layout classes - roughly: no virtual functions, and all data members declared
in one class of the hierarchy. `ring` has members in three classes, so `offsetof(ring, radius)` is only
conditionally-supported, and gcc and clang warn (`-Winvalid-offsetof`). They compute the right value anyway - but with a
virtual base, there is no fixed offset at all (see the session). The difference of two addresses always works, for the
object at hand.
<br> - !![sizeof](glossary.md#sizeof)

## A-803                                    <a id="a-803"></a>
No. The compiler looks up the name `describe` first - in `ring`, then in `circle`, where it finds one - and stops
there: `shape::describe(int)` is never considered, although it would fit. Then it chooses among what it found,
`circle::describe()`, which takes no argument: a compiler error. `r.shape::describe(1)` works, and
`using shape::describe;` in `circle` makes both overloads visible there (see the follow-up). The rule is the same for
virtual functions.
<br> - !![Function overloading](glossary.md#overloading)

## A-804                                    <a id="a-804"></a>
Still 16 bytes. The object holds one pointer to the table, whatever the table contains. Ten virtual functions make the
table longer - and the table exists once per class, not once per object. A derived class that adds virtual functions
appends them to its own table and shares the one vptr of its base part. More vptrs come only with more bases that have
virtual functions (see the session).
<br> - !![vtable](glossary.md#vtable)

## A-805                                    <a id="a-805"></a>
The slots are in the order in which the virtual functions are declared - and the first one declared in `shape` is the
destructor. With gcc and clang, a virtual destructor takes two slots: slot 0 destroys the object - the complete
destructor, for objects that are not on the heap, a local or a member - and slot 1 destroys it and gives its memory
back - the deleting destructor, which `delete` calls (see the session). Declare `area` first, and it moves to slot 0. A
derived class keeps the order of its base and puts its override into the same slot - that is what makes `[rax+16]` work
for every shape.
<br> - !![vtable](glossary.md#vtable)

## A-806                                    <a id="a-806"></a>
Inlining copies the body of a function into the caller - and only then can the optimizer work on both together: keep
values in registers across the call, compute what is known at compile time, drop what is not needed, turn a loop into
vector instructions. A virtual call hides the body until run time. So the compiler must assume that the function may
read and write anything it can reach, and it must follow the rules of a real call - the arguments in their registers,
the registers the callee may change saved. For a large function, that hardly matters; for a function of two
instructions, like `area`, the call is most of the work. In `measure_the_jump`, `direct` and `one type` are about the
same all the same: that loop waits for the memory, not for the calls - and a sum of `double`s is not vectorized
without `-ffast-math`, which would allow another order of the additions.
<br> - !![Devirtualization](glossary.md#devirtualization)

## A-807                                    <a id="a-807"></a>
Because the processor does not wait for the loads. It starts instructions long before the ones in front of them are
finished, and at an indirect jump, it must guess where to continue before the address has been loaded - from the history
of that jump. In `sorted`, the jump goes to `circle::area` half a million times in a row, then to `square::area`: the
guess is right almost always. In `mixed`, the target is random: the guess is wrong about half of the time, and every
wrong guess throws away the work started on the wrong path - more than a dozen cycles each. The loads and the calls are
the same; the wrong guesses make the difference. An `if` with a random condition, or a `switch` over random values, has
the same problem - a branch the processor cannot predict is expensive, whatever wrote it.
<br> - !![Branch prediction](glossary.md#branch-prediction)

## A-808                                    <a id="a-808"></a>
A circle whose own fields are not initialized yet. Java creates the object first, with all fields set to 0 or `null`,
then runs the constructors from the base down - and a virtual call in the base constructor runs the override of the
derived class, which reads `radius` as 0.0 and a `String` field as `null`. In C++, the members of `circle` are raw bytes
until its constructor initializes them - reading them would be undefined behavior. So C++ makes the object a `shape`
while the constructor of `shape` runs: the vptr points to the table of `shape`, and the override is out of reach. Both
languages have a rule; the better rule for both: no virtual calls in constructors.
<br> - !![virtual](glossary.md#virtual)

## A-809                                    <a id="a-809"></a>
Because its control block remembers how to destroy the object. `make_shared<circle>` creates the control block together
with the circle, for the type `circle` - its function that destroys the object calls `~circle`, whatever the type of the
`shared_ptr` that lets go of it last. The `shared_ptr<shape>` itself only keeps the address of the shape part, for `->`
and `*`. `unique_ptr<shape>` has no control block: its deleter is `default_delete<shape>`, which calls `delete` on a
`shape*` - and that needs the virtual destructor. Do not rely on the `shared_ptr`: sooner or later, somebody deletes
through the base in another way.
<br> - !![shared_ptr](glossary.md#shared-ptr)

## A-810                                    <a id="a-810"></a>
A mangled name, as in `nm` (unit 0x04) - this time of a type, not of a function. `N` ... `E` encloses a nested name,
`12_GLOBAL__N_1` is the unnamed namespace - 12 characters - and `6circle` the class.
`c++filt -t N12_GLOBAL__N_16circleE` prints `(anonymous namespace)::circle`. What `name()` returns is up to the
implementation; gcc and clang return the mangled name, which is in the object file anyway (`typeinfo name for circle`).
Good for a debug message - never compare or store it.
<br> - !![Symbols and name mangling](glossary.md#symbols)

## A-811                                    <a id="a-811"></a>
A null pointer means "no object", and it must stay null when it is converted: `nullptr` plus 8 would be the address 8,
which is neither null nor an object. So the conversion is: null stays null, anything else gets the 8. A reference always
refers to an object - there is no null reference in a correct program - so the compiler adds the 8 without asking. For
the first base, there is nothing to add, and nothing to test.
<br> - !![Multiple inheritance](glossary.md#multiple-inheritance)

## A-812                                    <a id="a-812"></a>
Because it is a property of the kind of object, not of the object: every `copier_v` has its `device` 28 bytes behind its
`scanner_v` part, every plain `scanner_v` 12 bytes behind its start. So the offset belongs to the class of the complete
object, like the addresses of the virtual functions - and the vptr, which the object has anyway, leads to it. The vptr
of the `scanner_v` part of a copier points into a table of `copier_v`, which holds 28. Storing the offset - or a pointer
to the virtual base - in every object would work, too, and some compilers did it; the table saves the bytes.
<br> - !![Multiple inheritance](glossary.md#multiple-inheritance)

## A-813                                    <a id="a-813"></a>
Rarely. `protected` data is public for everybody who derives from the class - and anybody can derive. The base loses
control over its members: it cannot check a rule, change the representation, or find all the places that use them. The
usual advice: data `private`, and `protected` member functions for what derived classes need - like `frame` in the
window example of the follow-up. In the machine, `protected` changes nothing.
<br> - !![Access modifiers](glossary.md#access-modifiers)

## A-814                                    <a id="a-814"></a>
Because objects are often owned through their interface: a `unique_ptr<drawable>`, or a `vector` of them, deletes
through a `drawable*`. Without a virtual destructor, that `delete` would run only `~drawable` - undefined behavior, and
the destructor of the real class never runs (see the session). If an interface is never used for deleting, a protected,
non-virtual destructor is the alternative: then `delete` through it does not compile (see the next follow-up). The
virtual destructor costs nothing extra - an interface has a vptr anyway.
<br> - !![Virtual destructor](glossary.md#virtual-destructor)

## A-815                                    <a id="a-815"></a>
A virtual call is two loads and a jump: the compiler knows the slot at compile time. `dynamic_cast` is a call into the
runtime library, `__dynamic_cast`, which must find the answer at run time: it reads the type information of the object
through the vptr, and walks the classes described there, comparing type information on the way - a search, longer for
deep or multiple hierarchies. Cheap enough now and then; in a loop over a million objects, or in a chain of five
`dynamic_cast`s, it adds up.
<br> - !![RTTI](glossary.md#rtti)

## A-901                                    <a id="a-901"></a>
Because the lambda got a copy of `limit` when it was created - 10. `limit = 20` changes the variable, not the copy
inside the lambda. `find_if` calls the lambda with 5, then with 12 - 12 is above 10, found. With `[&limit]`, the lambda
would read the variable itself when it is called - 20 - and find 21.
<br> - !![Lambda capture](glossary.md#capture)

## A-902                                    <a id="a-902"></a>
Because `x - y` can overflow. With `x` = 2,000,000,000 and `y` = -2,000,000,000, the difference does not fit into an
`int`: signed overflow is undefined behavior, and in practice the result wraps around to a negative number - and the
comparison says that `x` is smaller. `(x > y) - (x < y)` is -1, 0 or 1, without any arithmetic that could overflow. For
the numbers of the snippet, below a million, `x - y` would work - but a comparison should not depend on the range of the
data.
<br> - !![Integer conversions](glossary.md#integer-conversions)

## A-903                                    <a id="a-903"></a>
In the frame of `sort` - as a copy. `sort` takes the lambda object by value, and libstdc++ passes it on, by value again,
to its helper functions: a copy of the closure object in every frame on the way down. For a lambda without captures,
that is one byte that nobody reads; for `[&count]`, it is 8 bytes, the address - every copy refers to the same `count`.
As Release, after inlining, there is usually no object left at all: the captures are in registers. But a lambda that
captured a `vector` by copy would copy the whole vector with every copy of itself - capture big things by reference, or
wrap the lambda in `std::ref`.
<br> - !![Lambda expression](glossary.md#lambda)

## A-904                                    <a id="a-904"></a>
It forces the conversion to a function pointer. The class of the lambda has no `operator+`, but it has a conversion to a
function pointer, and the built-in unary `+` works on pointers. So the compiler converts, applies `+`, which changes
nothing - and `f` is an `int (*)(int)`, not a lambda any more. Useful when a function pointer is needed and `auto` would
keep the lambda's own type. It works only for a lambda without captures: the others have no conversion.
<br> - !![Function pointer and `std::function`](glossary.md#std-function)

## A-905                                    <a id="a-905"></a>
Because the compiler cannot know it. For the compiler, `pick(generator)` is some number computed at run time; it proves
nothing about it - not that it is below 1000, and so not which function `chosen` holds. It must compile the call through
the pointer. That a value is the same in every run does not help; only what the compiler can prove while it compiles
does. (gcc does not even inline the known `triple` here - it keeps `sum_with_pointer` as a call of its own.)
<br> - !![Zero overhead](glossary.md#zero-overhead)

## A-906                                    <a id="a-906"></a>
Our runs, x86-64 Linux, `times3(7)`: gcc 13 prints 229355 as Debug, and 0 as Release - with the warning "'factor' is
used uninitialized". clang 18 prints 49 as Debug - 7 times 7: the place where `factor` was now holds the argument `x` of
the call - and some large number as Release, with the warning "address of stack memory associated with parameter
'factor' returned". Four builds, four results, none of them 21. The address in the lambda points into a frame that is
gone; whatever the next call puts there, the lambda reads.
<br> - !![Dangling pointer / reference](glossary.md#dangling-pointer)

## A-907                                    <a id="a-907"></a>
`this` - the pointer, not `clicks_`. In the body, `clicks_` is `this->clicks_`, read through the pointer when the lambda
runs: it sees the current value, and it dangles when the object is gone or has moved - exactly as `[this]`. Because that
surprises people, C++20 deprecated the implicit capture of `this` by `[=]`, and compilers warn. Write `[this]` if you
mean the object, `[*this]` for a copy of the object, `[clicks = clicks_]` for a copy of the value.
<br> - !![Lambda capture](glossary.md#capture)

## A-1001                                   <a id="a-1001"></a>
Because the thread may still be writing it. Reading `result` while another thread writes it is a data race - undefined
behavior, see the session. And even when the thread happens to be done, nothing guarantees that `main` sees what it
wrote. `join` does both: it waits until the thread has ended, and everything the thread did before is visible after
`join` returns.
<br> - !![`std::thread`](glossary.md#thread)

## A-1002                                   <a id="a-1002"></a>
A stack that overflows would run into the next one. A deep recursion in one thread would silently overwrite the local
variables, return addresses and saved registers of another thread - which would crash later, somewhere else, far from
the cause. The guard is a region that the program must not touch: the first access to it ends the program at once, in
the thread that overflowed - a segmentation fault, exit status 139 (task 'Juniper Gate'). It costs address space, but no
memory.
<br> - !![Stack and heap](glossary.md#stack-and-heap)

## A-1003                                   <a id="a-1003"></a>
2. Thread A loads 0 and is interrupted. Thread B increments 999,999 times - the counter is 999,999. A stores 1 - its
   first increment. B loads 1 for its last increment, and is interrupted. A does its remaining 999,999 increments - the
   counter is 1,000,000. B stores 2. You will not see it in a run - but nothing forbids it, and it shows what a data
   race allows: any number from 2 to 2,000,000.
<br> - !![Race condition](glossary.md#race-condition)

## A-1004                                   <a id="a-1004"></a>
No. Each of the two operations is atomic, the pair is not: a load, then a store, and another thread can come in between.
Both threads read 0, both store 1, and both believe they were the one who did it. "If it is 0, make it 1" in one step is
`compare_exchange_strong(expected, 1)` - on x86-64 `lock cmpxchg`, which compares and writes in one step and reports
whether it wrote. It is the instruction a mutex uses to take the lock.
<br> - !![`std::atomic`](glossary.md#atomic)

## A-1005                                   <a id="a-1005"></a>
No. `alignas(64)` on the struct aligns the struct: it starts at a multiple of 64, and its size becomes a multiple of 64.
But the two counters are still at offsets 0 and 8 - in the same line. Every counter that one thread writes needs a line
of its own: `alignas(64)` on each member, or padding between them. Aligning the struct helps against sharing a line with
the objects around it - the neighbors in an array of such structs, say.
<br> - !![Cache line and false sharing](glossary.md#false-sharing)

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

**I know the difference between `const` and `constexpr`, can check a claim with `static_assert`, and know where a value
computed by the compiler ends up.**

A `const` variable cannot change after its initialization, but the value may come at runtime - from input, or from any
function. A `constexpr` variable must be computable by the compiler; that is why it can be used where the compiler
needs a number, e.g. as the size of a `std::array`. A `constexpr` function may run at compile time, if its arguments
are constants, and at runtime otherwise; a `consteval` function must run at compile time.
`static_assert(condition, message)` checks a claim while compiling - the build fails if it is wrong, and nothing is
left of it in the program. A table computed by the compiler is written into the program file itself, as read-only
data next to the string literals: no code runs to build it, and its address is far from the stack.

**I know where the characters of a `string` live.**

The `string` object has a fixed size (32 bytes with gcc, 24 with Apple clang) and sits where the variable is, e.g. on
the stack. A short text fits into the object itself (small string optimization, up to 15 or 22 characters), a longer
one lives on the heap. `c_str()` shows where. When a string grows beyond its capacity, the characters move: an
allocation and a copy you do not see in the code.

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

**I can constrain a template with a standard concept, write a simple concept of my own, and read the error when a type
does not satisfy it.**

`template <std::integral T>`, or a `requires std::integral<T>` after the template head or after the parameters, or
`std::integral auto` as the parameter type - four spellings of the same constraint. A concept of my own is a named
compile-time condition, usually with a `requires` expression that lists what must compile:
`template <typename T> concept printable = requires(std::ostream& os, const T& x) { os << x; };`. If a type does not
satisfy it, the compiler reports "no matching function" at the call and names the unsatisfied concept and the
expression that would be invalid - instead of an error deep inside the body of the template. Concepts also select
between overloads. They cost nothing at runtime: the check happens while compiling, and the generated function is the
same as without it.

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

**I can put my code into a namespace, and I know why `using namespace` does not belong in a header.**

`namespace geo { ... }` around declarations and definitions; outside, the names are `geo::point` and `geo::distance`,
or brought in one by one with `using geo::distance;`. The namespace becomes part of the name, and of the symbol in the
object file - it costs nothing at runtime. A `using namespace` makes every name of the namespace visible, including
those you do not know and those a future standard adds; two such directives with one common name make every
unqualified use ambiguous. In a header, it is forced on every file that includes the header, and nobody there can take
it back. So: full names in headers, using-declarations in `.cpp` files, a using-directive at most in a small scope.

**I can build a static library with CMake, share a variable between files with `extern` or `inline`, and say which
object files the linker takes from an archive.**

`add_library(weather STATIC stats.cpp report.cpp)` compiles the files and packs the object files into one archive,
`libweather.a`; `target_link_libraries(program PRIVATE weather)` links a program with it. By hand: `g++ -c` for each
file, `ar rcs libweather.a *.o`, and the library at the end of the link command - the GNU linker reads from left to
right and takes a member only if it defines a symbol that is undefined at that point. So an object file that nobody
needs is not linked at all, unlike a `.cpp` file listed in `add_executable`. A variable for several files is declared
in the header with `extern` - a declaration, `U` in the object files that use it - and defined in exactly one `.cpp`
file (`B` or `D` there). Or it is defined `inline` in the header: every object file that uses it has a copy (`u` with
gcc, `V` with clang), and the linker keeps one.

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

## Unit 0x06 <a id="check-0x06"></a>

**I can explain the two steps of `new` and of `delete`, and why `delete[]` must match `new[]`.**

`new T{...}` first calls `operator new(sizeof(T))`, which gets raw bytes from the allocator (with glibc: from `malloc`),
then runs the constructor at that address. `delete p` runs the destructor, then gives the bytes back with
`operator delete`. `new T[n]` for a type with a destructor asks for `n * sizeof(T)` plus a few bytes and stores the
count `n` in front of the elements - the array cookie; the address you get is behind it. `delete[]` reads the count,
runs `n` destructors and gives back the block from its real start. `delete` on such an array runs one destructor and
hands the wrong address to the allocator - glibc aborts with `munmap_chunk(): invalid pointer`. For `int` there is no
cookie, and the mismatch seems to work: undefined behavior all the same. gcc (`-Wall`) and clang warn.

**I can say what a heap block costs, in memory and in time, compared to a local variable.**

Memory: the allocator rounds every block up and keeps its size in front of it - with glibc a block takes its size plus 8
bytes, rounded up to 16, at least 32: an `int` on the heap takes 32 bytes; a `list` node for an `int`, 24 bytes of
data, too. Plus the pointer that holds the address. Time: a call of `operator new` and one of `operator delete`, each
with bookkeeping; a million single blocks took four to eight times as long as one block of a million `int`s. A local
costs nothing of this: its place in the frame is reserved together with all others, by one subtraction from the stack
pointer. So: locals first, one block rather than many, `reserve` where the size is known.

**I can count allocations with `heap_watch`, and I know why it counts as Debug.**

`heap_watch watch{};` counts the calls of `operator new` and `operator delete` from its creation or the last `reset()`:
`allocations()`, `releases()`, `bytes()` and `live()`, the difference. `const heap_log log{};` prints every allocation
and release while it lives. It works because a program may replace the global `operator new` and `operator delete` -
the header does that, so it must be included in exactly one `.cpp` file of a program, and not together with
AddressSanitizer. As Release, the compiler may remove a `new` and its `delete` if nobody needs the memory - clang does -
so the count may be lower than the code suggests.

**I can name the ways a raw owner goes wrong - a leak, a leak on the exception path, a double `delete`, a use after
`delete` - and what glibc and the optimizer make of them.**

A leak: the last address of a block is lost - overwritten, or gone with a local pointer; nothing crashes, the memory is
gone until the program ends. On the exception path: stack unwinding destroys the local pointer, not the object - a
`delete` after the `throw` never runs. A double `delete`: two pointers to one block, or two owners after a shallow
copy; glibc detects this one case and aborts (`free(): double free detected in tcache 2`, exit status 134). A use after
`delete`: the block belongs to the allocator again - glibc writes its list links into the first bytes, so a read gives
garbage, a write corrupts the allocator. All of them are undefined behavior: as Release, gcc and clang removed a
`new`/`delete`/`delete` sequence they could see through, and the program ended normally.

**I know why the generated copy of a class with an owning raw pointer is wrong, and how the Rule of Three fixes it.**

The generated copy constructor copies member by member, and for a pointer that is the address: two objects, one block,
both believe they own it, and both destructors delete it - a double `delete` at the `}`. The generated copy assignment
also loses the target's old block - a leak. The Rule of Three: a class with a destructor of its own also needs a copy
constructor that allocates its own block and copies the elements, and a copy assignment that does the same and then
releases its old block - in this order, so that a failing `new` leaves the object unchanged and `a = a` works. Or
`= delete` the copies.

**I can use `unique_ptr`: `make_unique`, `get`, `reset`, `release`, `std::move`, and `unique_ptr<T[]>`.**

`make_unique<T>(args...)` creates the object and its owner in one step. `*p` and `p->` use the object, `get()` returns
the address without giving up anything, a `unique_ptr` tests as `false` when empty. `reset()` deletes the object and
empties the pointer; assigning a new `unique_ptr` deletes the old object first. `release()` empties the pointer
without deleting and returns the address - the `delete` is yours again. A `unique_ptr` cannot be copied, only moved:
`q = std::move(p)` hands the ownership over, `p` is `nullptr` afterwards. `unique_ptr<T[]>` owns an array, calls
`delete[]` and offers `p[i]`; `make_unique<T[]>(n)` value-initializes the elements, `make_unique_for_overwrite` does
not.

**I can say what a `unique_ptr` costs: its size, its machine code, and how it is passed to a function.**

Its size is that of a raw pointer, 8 bytes - with the default deleter, which takes no space. Its machine code, as
Release, is the raw code: `operator new`, the use, `operator delete` - plus a cleanup part on the exception path, the
`delete` a raw version forgets. A move copies 8 bytes and writes a zero. The cost is at function borders: a class with a
destructor of its own is passed via an address, never in a register - so a `unique_ptr` by value arrives as the
address of an object in the caller's frame (one more load), and after the call the caller tests it and deletes, if it
still owns something. That matters only for sinks.

**I can choose how to hand an object to a function: `T&`, `T*`, `unique_ptr<T>` or `unique_ptr<T>&` - and say what each
one promises.**

`const T&` (or `T&` to change it): "I use the object, it must exist" - the function does not care who owns it. `const
T*` / `T*`: the same, but `nullptr` is allowed. `unique_ptr<T>` by value: "I take it" - a sink; the caller writes
`std::move(p)`. `unique_ptr<T>&`: "I may change your owner" - reset it, move from it, or give it another object.
Returning `unique_ptr<T>`: "you own what I made" - a factory. `const unique_ptr<T>&` says no more than `const T*`, and
forces the caller to hold the object in a `unique_ptr`: rarely the right choice.

**I can explain move semantics: what a move constructor does, what `&&` binds to, what `std::move` is, and what is left
of a moved-from object.**

A move constructor `T(T&& other)` takes over the resources of `other` - for an owner of a block: it copies the address
and the size, and sets `other` to empty, so that its destructor releases nothing. No allocation, no elements copied:
moving a `vector` of ten million `int`s copies three words. `T&&` binds to rvalues - temporaries, and objects cast with
`std::move` - so an overload with `&&` knows that it may steal. `std::move` itself is a cast to `T&&`, no machine code;
it only allows the move. A moved-from object is still an object and is destroyed as usual; for the library it is
"valid but unspecified" - destroy it or assign to it. A `unique_ptr` is guaranteed empty, a `vector` after its move
constructor, too; a `string` is empty in practice.

**I can write the Rule of Five for a class that owns a block, and I know why its moves should be `noexcept`.**

Destructor (`delete[]`), copy constructor (a new block, the elements copied), copy assignment (new block first, then
release the old one), move constructor (take the address and the size, empty the source), move assignment (release the
own block, take the other's, empty the source - and test for `this != &other`). The moves allocate nothing and cannot
fail, so they are `noexcept`. That matters for containers: a `vector` that grows moves its elements only if their move
constructor is `noexcept` - otherwise it copies them, because a move that fails halfway would leave the old block half
empty, and `push_back` promises to leave the `vector` unchanged if something throws. Better still: members that own
themselves (`vector`, `unique_ptr`), and no special member at all - the Rule of Zero.

**I know when the compiler generates the moves - and when it silently copies instead.**

The move operations are generated only if the class declares none of the copy operations, no destructor and no other
move operation. A destructor declared just for a log line is enough to switch them off - then `std::move(a)` selects
the still generated copy constructor, and every "move" is a copy; `heap_watch` sees the allocation. `= default` for all
five brings the moves back. Also silent: `std::move` of a `const` object is a copy, because the move constructor needs a
non-`const` source. And `return std::move(local)` turns an elided return into a move (`-Wpessimizing-move`).

**I can use `shared_ptr` and `weak_ptr`, and I know what the control block holds, why `make_shared` is one allocation,
and why the count is atomic.**

A `shared_ptr` is two pointers, 16 bytes: to the object and to the control block. The control block holds the number of
owners, the number of `weak_ptr`s, and what is needed to delete the object (the address of a table of functions, and the
deleter). Copying counts up, destroying counts down, the last owner deletes the object; `use_count()` shows the count.
`make_shared` allocates the object inside its control block: one allocation (48 bytes for a 32-byte `tracer` with gcc's
library); `shared_ptr{new T}` gets an existing object and needs a second block for the control block. The count is
changed atomically - `lock add` on x86-64, `ldadd` or a helper function on ARM64 - because owners in different threads
may change it at the same time; gcc's library skips the `lock` while the program has only one thread. A `weak_ptr`
refers to the control block without owning: `lock()` gives a `shared_ptr`, or an empty one if the object is gone;
`expired()` asks. The control block lives until the last `weak_ptr` is gone - with `make_shared`, together with the
object's memory.

**I can recognize a `shared_ptr` cycle and break it with a `weak_ptr`.**

Two objects that own each other through `shared_ptr` members - partners, a child and its parent - keep their counts at
1 after the last outside owner is gone: no destructor runs, and nobody has the addresses any more - a leak that
`heap_watch` shows as `live` blocks. The fix: one direction owns, the other only refers - the back link becomes a
`weak_ptr`, `lock()`ed when it is used. Where the owner always outlives the referring object - a parent and its
children, owned through `unique_ptr` - a raw pointer back is enough.

**I can allocate an array whose size is known only at run time, and build a 2D array as one block or as one block per
row.**

`new int[n]` - `n` may be any value computed at run time - leaves the `int`s uninitialized, `new int[n]{}` sets them to
0; it is released with `delete[]`, or owned by `unique_ptr<int[]>` from `make_unique<int[]>(n)`. A 2D array as one
block: `rows * columns` elements, element `[r][c]` at `r * columns + c` - one allocation, the rows one after the other,
as the compiler does for `int grid[3][4]`. As one block per row: an array of `rows` pointers (`int**`) and a block for
each row - `rows + 1` allocations, `rows + 1` `delete[]`s (rows first), two loads per access, and the rows anywhere in
memory. In C++, a `vector<int>` of `rows * columns` with an index function does the first, and keeps the size.

### 'AI' - Two Opinions <a id="ai-0x06"></a>

| | `sizeof` | `const int*` | `unique_ptr<int>` by value | `const unique_ptr<int>&` | `const int&` |
|:--|:--|:--|:--|:--|:--|
| gcc 13, x86-64 Linux, `-O2` | 8 | `mov eax, [rdi]` | `mov rax, [rdi]`, `mov eax, [rax]` | the same two loads | `mov eax, [rdi]` |
| gcc 11, ARM64 Linux, `-O2` | 8 | `ldr w0, [x0]` | `ldr x0, [x0]`, `ldr w0, [x0]` | the same two loads | `ldr w0, [x0]` |

Answer A:

- "`sizeof(unique_ptr<int>)` is 8, the size of a raw pointer" - right, with the default deleter.
- "with optimization the compiler generates the same `new` and the same `delete`; the destructor is inlined" - right;
  there is also a cleanup part for the exception path, which the raw code lacks.
- "passed by value at no cost: it is 8 bytes, so it travels in a register, just like an `int*`" - wrong: a class with a
  destructor of its own is passed via the address of an object in the caller's frame - whatever its size. The function
  needs one more load, and the caller tests and deletes after the call.

Answer B:

- "In memory it is free: 8 bytes, no reference count - unlike `shared_ptr`, which has two pointers and counts
  atomically" - right.
- "Its code is what you would write by hand, plus the cleanup for exceptions" - right.
- "a class with a destructor is passed via an address, not in a register" - right.
- "So pass it as `const unique_ptr<T>&` - then nothing is copied, and the function can use the object at the cost of a
  raw pointer" - wrong: `const unique_ptr<int>&` is the same machine code as by value - the address of the `unique_ptr`,
  and two loads - and the wrong interface: it ties the function to one kind of owner. To use an object, pass `const T&`
  or `const T*`: one register, one load, any owner.

A is wrong about the register, B about the reference - both take "8 bytes" for "as cheap as a raw pointer", and forget
that the address travels in a register only when it is passed as a raw address.

## Unit 0x07 <a id="check-0x07"></a>

**I can write `a + b`, `a += b` and `cout << a` as calls by name, and say which of them are member functions.**

`a + b` for a class with a free `operator+` is `operator+(a, b)`; `a += b` is the member function call
`a.operator+=(b)`, with `a` as `this`; `cout << a` for your own type is `operator<<(cout, a)`, a free function, because
the left operand is the stream. The library's `<<` for numbers are members of the stream - `cout.operator<<(42)` - the
one for a `char` or a text is a free function in `std`. C++ Insights shows every one of them.

**I can decide whether an operator is a member or a free function, and write `+` with `+=`.**

Members: the operators that change or belong to their left operand - `=`, `+=` and the other compound assignments, `[]`,
`()`, `->`, unary `*`, `++` and `--`; `=`, `[]`, `()` and `->` must be members. Free: the symmetric binary operators -
`+`, `-`, `*`, `==`, `<` - so that both operands are treated alike and `3 * price` works as well as `price * 3`; and
`<<` and `>>`, whose left operand is a stream. `+=` does the work and returns `*this` by reference; `+` takes its left
operand by value, applies `+=` and returns it.

**I can say what is left of an operator call as Debug and as Release, and show it in Compiler Explorer.**

As Debug, a call: `add_money` calls `operator+(money const&, money const&)`, which calls `money::operator+=`, with the
addresses of the operands in `rdi` and `rsi`. As Release, the calls are inlined, and `a + b` for two `money`s is
`lea rax, [rdi+rsi]` - the same instruction as for two `long long`s (ARM64: `add x0, x0, x1`). A `money` is trivially
copyable and travels in a register. An operator costs what a function costs, and a small function costs nothing.

**I know that precedence, associativity and the number of operands are fixed, and why an overloaded `&&` does not
short-circuit.**

An overloaded operator keeps the grammar of the built-in one: `cout << a == b` is `(cout << a) == b`, `a = b = c` goes
from right to left, and a binary operator stays binary. Since C++17, the operands of `<<`, `>>`, `=`, `&&`, `||` and
`,` are evaluated in a fixed order, for overloaded operators, too. But an overloaded `&&` is a function call, and all
arguments of a function are evaluated before it runs - the right operand is evaluated even if the left one is `false`.
So `&&`, `||` and `,` should not be overloaded.

**I can use `= default` for `==` and `<=>`, and say what the compiler makes of `!=` and `<`.**

`bool operator==(const T&) const = default;` compares member by member; `auto operator<=>(const T&) const = default;`
compares member by member in the order of declaration and returns a `std::strong_ordering` (for `double` members a
`std::partial_ordering`, because of NaN). The compiler rewrites the other four: `a != b` becomes `!(a == b)`, `a < b`
becomes `(a <=> b) < 0`, and so on. There is no `operator!=` or `operator<` in the object file.

**I can write `[]` in two versions, `()`, prefix and postfix `++`, `<<` and `>>` for a class, and I know what `friend`
grants.**

`T operator[](size_t) const` reads, `T& operator[](size_t)` returns a reference to write through; the compiler picks
the `const` one for a `const` object. `R operator()(params) const` makes an object callable. Prefix `T& operator++()`
counts and returns `*this`; postfix `T operator++(int)` copies, counts, and returns the copy - the `int` is only a
marker. `ostream& operator<<(ostream&, const T&)` and `istream& operator>>(istream&, T&)` return the stream. `friend`
gives one function or one class access to the private members - granted by the class, not taken; defined in the class
body, a friend function is still a free function, found only through its arguments. In the machine, access control
leaves no trace.

**I can use C++ Insights to see the calls and conversions the compiler adds.**

Paste the code - without the course's own headers - into cppinsights.io and run it. It shows operators as calls
(`operator+(a, b)`, `sum.operator+=(b)`, nested `operator<<`), implicit conversions as `static_cast`s, a range-based
`for` as the loop with iterators, the special members the compiler wrote, the rewritten comparisons, and for a C-style
cast the named cast it stands for. It is built on clang; the rules it shows are those of the standard.

**I can name the named casts and `std::bit_cast`, say what each one is for, and which of them may cost an instruction.**

`static_cast` converts values - numbers, enums, `void*` back to its type; between number types it may be an instruction
(`movsx`, `cvttsd2si`), or nothing (`int` to `unsigned`). `reinterpret_cast` changes the type of a pointer, to look at
bytes; it is never an instruction. `const_cast` adds or removes `const`, never an instruction. `std::bit_cast` keeps the
bits of a value in another type of the same size; it may cost a copy between registers (`movd`). `dynamic_cast` checks a
type at run time - the next unit. A conversion operator or a converting constructor is a call.

**I can predict integer and floating-point conversions: sign extension, the same bits for `unsigned`, the lower bits
for a narrower type, truncation toward zero - and the trap in `-1 < 1u`.**

To a wider signed type, the sign bit is copied: -5 stays -5. `int` to `unsigned` keeps the bits: -1 is 4294967295. To
a narrower type, the lower bits stay: 300 as a `uint8_t` is 44, 3'000'000'000 as an `int` is -1294967296 (defined since
C++20). `double` to `int` cuts off the fraction toward zero: 2.99 is 2, -2.99 is -2; out of range it is undefined
behavior. A `float` has 24 bits for the digits: 16'777'217 becomes 16'777'216. In `-1 < 1u`, the `int` becomes an
`unsigned` first, and the result is `false`; `std::cmp_less` compares the values.

**I know why writing through a `const_cast` to a `const` object is undefined behavior, why reading a `float` through a
`uint32_t*` is, too, and why a C-style cast is dangerous.**

A `const` object may be assumed never to change: the compiler puts its value into the instructions, and a global one
lives in read-only memory - `limit` printed 42 and `*&limit` 43, the write to `global_limit` ended with exit status
139. Reading an object through a pointer to an unrelated type breaks the strict aliasing rule: the compiler may assume
that a `float*` and a `uint32_t*` never point to the same object, and reorder accordingly; `std::bit_cast` or `memcpy`
are the legal ways, and bytes may always be read through `unsigned char*`. A C-style cast tries `const_cast`,
`static_cast` and `reinterpret_cast` until one compiles - it may remove a `const` or reinterpret a pointer without
anyone seeing it.

**I can say where a static data member lives, why it does not change `sizeof`, where it is defined, and how many there
are for a class template.**

In static storage, next to the global variables, from the start of the program to its end - one variable for the class,
not one per object, so it is not in the object and `sizeof` does not count it. Declared in the class, it is defined in
exactly one `.cpp` file (`int ticket::count{0};`), or in the class with `inline static` (C++17); `static constexpr`
members are `inline` anyway. A class template has one per instantiation: `registry<int>::count` and
`registry<double>::count` are two variables. A static member function has no `this`.

**I can explain what a function-local `static` costs: constant or dynamic initialization, the guard, the destructor
after `main`.**

With a constant initializer, a static local is initialized before `main`, like a global: access is a plain load and
store. With an initializer that must run - a call - it is initialized at the first call, thread-safe since C++11: a
guard variable is tested at every call (`movzx` and `test` on x86-64, a load-acquire `ldar` on ARM64), and the first
call goes through `__cxa_guard_acquire` and `__cxa_guard_release`. An object with a destructor is registered with
`__cxa_atexit` and destroyed after `main` returns.

**I can say what an enum is in memory, choose its underlying type, and explain why every value of the underlying type
is valid - and what that means for a `switch`.**

An enum is a number of its underlying type: `int` for an `enum class` without `:`, a type the compiler chooses for a
plain `enum` (`unsigned int` with gcc and clang when no value is negative), or the one after the `:` -
`enum class suit : std::uint8_t` takes one byte, and a struct with it shrinks. For an enum with a fixed underlying
type, every value of that type is a valid value, named or not: `static_cast<suit>(7)` is fine, and so is `suit s{7};`.
A `switch` must be ready for such values: gcc and clang turn a `switch` that maps every name to a value into a table,
with a range check in front for exactly these values; the code after the `switch` handles them.

**I can write operators for an `enum class`, e.g. to combine flags.**

An `enum class` has no arithmetic and no bit operators of its own, and an enum has no members - so its operators are
free functions: `permission operator|(permission a, permission b)` converts both to the underlying type with
`std::to_underlying`, combines the bits, and converts back with `static_cast`. The flags have values that are single
bits - 1, 2, 4 - and a combination is a value without a name. As Release, `a | b` is one `or` instruction.

**I can read and write a text file, take lines apart with a string stream, and say how a binary file differs.**

`ofstream out{path};` creates the file, `out << ...` writes, and the destructor closes it; `ifstream in{path};` opens
it, and `getline(in, line)` reads line by line. Always check the stream: `if (!in)`. An `istringstream` on a line reads
the fields with `>>`, or with `getline(stream, field, ';')`. A text file holds characters - 8080 is `'8' '0' '8' '0'`;
a binary file, written with `std::ios::binary` and `write`, holds the bytes of the values - 8080 as an `int` is
`90 1f 00 00` on a little-endian machine. Binary is compact and fast to read, text is portable and readable.

**I can format numbers and texts with `std::format` - width, precision, base - make my own type formattable, and say
when the result needs the heap.**

`std::format("{:>8} {:.3f} {:#x}", n, x, m)`: after the `:`, fill and alignment, width, sign, precision and base. The
format string is checked while compiling - a wrong number of arguments or a specification that does not fit the type
is a compiler error, because the constructor of `std::format_string` is `consteval`. An own type becomes formattable
through a specialization of `std::formatter`, often derived from the one for a member's type so that it can reuse its
parsing. The result is a `std::string`: a short one fits into the object itself (the small string), a longer one is a
heap block. `std::format_to_n` writes into a buffer of your own, with a limit, and needs no allocation - but it does
not write a `'\0'`.

**I can set, clear, toggle and test bits with masks and shifts, take a field out of a packed number, and I know the
traps of integer promotion and of shifts.**

A mask is a number with the bits of interest set, often `1u << n`. `x |= mask` sets them, `x &= ~mask` clears them,
`x ^= mask` toggles them, `(x & mask) != 0` tests them. A field is shifted to the bottom and masked:
`(colour >> 16) & 0xff`; packing is the way back, shifts and `|`. Each of these operators is one instruction. The
traps: every operand smaller than `int` - `uint8_t`, `char`, `short` - is promoted to `int` before the operation, so
`~a` for a `uint8_t` is a negative `int` and needs a cast back; a shift by the width of the type or more is undefined;
and `>>` of a negative number copies the sign bit. The compiler does these tricks itself: `x * 8` is a shift,
`x % 8` for an `unsigned` an `and`.

**I can explain two's complement, byte order and the three parts of a `float` - and why `0.1 + 0.2 != 0.3` - and say
what a `std::variant` stores besides its value.**

Two's complement: the highest bit means negative, -1 is all bits set, and `-x` is `~x + 1` - the same adder works for
signed and unsigned numbers. Byte order: x86-64 and ARM64 store the lowest byte first (little-endian), `0x12345678` as
`78 56 34 12`; networks and some file formats use big-endian, `std::endian` and `std::byteswap` handle it. A `float` is
1 sign bit, 8 exponent bits (biased by 127) and 23 mantissa bits after an implicit leading 1. 0.1 and 0.2 have no
finite binary form and are rounded, and their rounded sum is one bit away from the `double` closest to 0.3 - so
floating-point numbers are compared with a tolerance. A `union` stores one of its members and does not know which; a
`std::variant` stores the value and an index of the alternative, padded to the alignment of the largest one - 16
bytes for `variant<int, double>` with libstdc++.

### 'AI' - Two Opinions <a id="ai-0x07"></a>

| | `double` to `int` | `int` to `long long` | `int` to `unsigned` | `reinterpret_cast` of a pointer | `bit_cast<uint32_t>(float)` |
|:--|:--|:--|:--|:--|:--|
| gcc 13, x86-64 Linux, `-O2` | `cvttsd2si eax, xmm0` | `movsx rax, edi` | `mov eax, edi` | `mov rax, rdi` | `movd eax, xmm0` |
| gcc 11, ARM64 Linux, `-O2` | `fcvtzs w0, d0` | `sxtw x0, w0` | nothing, only `ret` | nothing, only `ret` | `fmov w0, s0` |

On x86-64, `mov eax, edi` and `mov rax, rdi` only move the argument into the result register - a function that
returns its parameter unchanged has the same instruction.

Answer A:

- "A cast only tells the compiler how to look at a value - the bits stay where they are" - wrong for conversions
  between number types: they compute a new value with new bits.
- "`static_cast<int>(x)` for a `double` ... none of them generates an instruction" - wrong: `cvttsd2si` or `fcvtzs`.
- "`static_cast<long long>(n)` for an `int`" generates nothing - wrong: the sign extension `movsx` or `sxtw`.
- "`reinterpret_cast` for a pointer" generates nothing - right.
- "all the work happens at compile time" - wrong, see above; right is that the casts that change only the type of a
  pointer have no run time.

Answer B:

- "A conversion between number types can cost an instruction" - right, and "can" is the right word: `int` to `unsigned`
  or `long long` to `int` cost nothing.
- "`reinterpret_cast` and `const_cast` cost nothing" - right. "and neither does `static_cast<unsigned>(n)`" - right.
- "The cheapest way to get the bits of a `float` is `*reinterpret_cast<std::uint32_t*>(&f)`" - wrong: it is undefined
  behavior - the strict aliasing rule - and gcc warns (`-Wstrict-aliasing`). It even compiles to the same `movd` as
  `std::bit_cast`, which is legal and costs nothing more.

A takes "cast" for "reinterpretation", and B knows better, but falls for the old trick - both confuse "it compiles to
little" with "it is right".

## Unit 0x08 <a id="check-0x08"></a>

**I can draw the layout of a derived class, and say why converting a pointer to its base costs nothing - with one
base.**

A derived object starts with its base part - the bytes of the base, as if it were a member at offset 0 - followed by the
members of the derived class, with padding as for a struct. A `ring : circle : shape` has `id` at 0, `radius` at 8,
`inner` at 16. Nothing else is in the object: no type, no pointer to the base. Since the base part is at offset 0, a
`shape*` to the ring has the address of the ring: the conversion - an upcast - is no instruction (`mov rax, rdi`, the
pointer goes back as it came).

**I know that a non-virtual function is chosen by the static type, at compile time, and that a function of the same
name in a derived class hides all functions of that name in the base.**

For a non-virtual member function, the compiler looks at the type of the expression - `const shape*` - and calls that
class's function: `call shape::describe() const`, the address in the instruction, and as Release often inlined. The
object behind the pointer does not matter. A `describe` in `circle` does not replace the one in `shape`, it hides it -
and every other `describe` of `shape`, whatever its parameters: name lookup stops at the first class that has the
name. `r.shape::describe()` calls the hidden one, `using shape::describe;` brings the overloads back.

**I can explain why an empty base class takes no space, and an empty member does.**

Every object takes at least one byte, so that two objects have two addresses. An empty member gets its byte - and the
next member its alignment: `struct { empty e; int count; }` is 8 bytes. An empty base class may share the address of the
derived object, because they are objects of different types: `struct : empty { int count; }` is 4 bytes - the empty base
optimization. The standard library stores empty helpers this way, or with `[[no_unique_address]]`: the deleter of a
`unique_ptr<int>` costs nothing, and `sizeof(unique_ptr<int>)` is 8.

**I can say what `virtual` adds to an object and to a class - and why the number of virtual functions does not change
`sizeof`.**

To every object: one pointer, the vptr, 8 bytes at offset 0 - `sizeof(shape)` with one `int` is 16, the `int` at offset
8 and 4 bytes of padding. To the class: one table, the vtable, in static storage - the offset to the top, a pointer to
the type information, then the addresses of the virtual functions, two for a virtual destructor. All objects of a class
point to the same table. More virtual functions make the table longer, not the object; a derived class shares the vptr
of its base part.

**I can describe a virtual call in the machine and find it in Compiler Explorer.**

Load the vptr from the object, load the address of the function from the table, jump. x86-64, `-O2`:
`mov rax, QWORD PTR [rdi]` and `jmp [QWORD PTR [rax+16]]` - slot 2, after the two destructors; ARM64: `ldr x1, [x0]`,
`ldr x1, [x1, 16]`, `br`. As Debug, a `call` through a register (`call rdx`). A non-virtual call names the function in
the instruction. So the target is known only at run time, and the call cannot be inlined.

**I can say when a virtual function is called directly - and what gcc's speculative devirtualization does.**

When the compiler knows the dynamic type: an object, not a pointer or a reference - a local, a parameter by value; a
class marked `final`, so that a reference to it cannot refer to anything derived; a call with the class name,
`c.circle::area()`, which is never virtual; and in a constructor or destructor, where the object is of the class being
constructed. Then the call is direct and can be inlined. Without `final`, gcc guesses: it loads the slot, compares it
with the address of the function it expects - `circle::area` - and runs the inlined code if they are equal, otherwise it
jumps. clang makes the plain virtual call.

**I can explain when a virtual call is cheap, when it is expensive, and what else it costs besides the jump.**

The two loads are cheap. The jump is cheap as long as the processor predicts its target - when it goes to the same
function many times in a row. With mixed types at random, the guess is often wrong, and each wrong guess throws away the
work already started: with the same objects and the same calls, our `mixed` sum took one and a half times as long as
`sorted` on an x86-64 machine, and five times as long on an Apple silicon Mac (gcc, in a Linux VM).
Besides: no inlining, so no optimization across the call; and objects used through a base pointer usually live on the
heap, one block each, reached through a pointer. In a hot loop, sorting by type, `final`, or a `vector` of one type by
value can help.

**I can explain who writes the vptr and when, what a virtual call in a constructor reaches, and what "pure virtual
method called" means.**

Every constructor writes the vptr of its own class, right after the constructors of its bases and before its members
are initialized: `call shape::shape(int)`, then `lea rdx, vtable for circle[rip+16]` and a `mov` to offset 0. So while
the constructor of `shape` runs, the object is a `shape`, and a virtual call reaches `shape`'s version - the circle part
does not exist yet. The destructors write the vptrs back, in reverse. Java calls the override instead, which sees
uninitialized fields. A call of a pure virtual function in that phase finds `__cxa_pure_virtual` in the table: it prints
"pure virtual method called" and aborts - exit status 134. gcc and clang warn only for a direct call in the constructor.

**I know why a base class needs a virtual destructor - or a protected one - what `delete` calls through the table, and
what happens without it.**

`delete p` for a `shape* p` must destroy the whole object and free all of its bytes. With a virtual destructor, `delete`
is a virtual call to slot 1, the deleting destructor of the real class: it runs `~circle`, which runs `~shape`, and
frees the block. Without it, `delete` calls `~shape` only - the derived destructor never runs, its members are not
destroyed (a `string` leaks its block), and the standard calls it undefined behavior. gcc and clang warn only if the
class has other virtual functions - and gcc not when the `delete` is inside a `unique_ptr`. So: a public virtual
destructor for a base that is deleted through, or a protected non-virtual one, then `delete` through the base does not
compile.

**I can explain slicing, and why the sliced copy behaves like a base object.**

Copying a derived object into a base object - `const shape copy{c};`, or passing it by value to a `shape` parameter -
copies only the base part; the members of the derived class are cut off. The vptr is not copied at all: the copy
constructor of `shape`, like every constructor of `shape`, writes the vptr of `shape`. So the copy is a complete shape -
`copy.name()` is "shape", `typeid(copy)` is `typeid(shape)`. Pass by reference, or copy with a virtual `clone`.

**I can use `typeid` and `dynamic_cast`, say where they find the type and what they cost, and when a virtual function is
the better choice.**

`typeid(ref)` for a reference to a polymorphic class reads the type information through the vptr - the entry in front
of the slots: two loads (`mov rax, [rdi]`, `mov rax, [rax-8]`). For a class without virtual functions, it is the static
type, known at compile time. `dynamic_cast<circle*>(p)` calls `__dynamic_cast` in the runtime library, with the type
information of both classes; it walks the hierarchy and returns the address, or `nullptr` if the object is something
else - for a reference, it throws `std::bad_cast`. Both need a polymorphic class. `static_cast` does not check, and a
wrong one is undefined behavior. A chain of `dynamic_cast`s is a `switch` over types: a virtual function does it without
asking, and a new class brings its own.

**I can explain the layout of a class with two bases, why a pointer to the second base has another address, and what a
thunk does.**

`sprite : printable, movable` contains a `printable` part at offset 0 and a `movable` part at offset 8 - each with its
own vptr - then its members: 24 bytes, two vptrs. A `movable*` to the sprite is its address plus 8: the conversion adds
the offset, and tests for null first (`lea rax, 8[rdi]`, `test rdi, rdi`, `cmove`), because a null pointer must stay
null; a reference needs no test. A virtual call through the `movable*` passes the address of the `movable` part as
`this` - so the slot holds a thunk, `non-virtual thunk to sprite::move_by(int, int)`, which subtracts 8 and jumps to
the real function (as Release, with the body copied in). `static_cast` back subtracts 8 without a check.

**I can explain the diamond, what `virtual` inheritance changes, and what it costs.**

`copier : scanner, printer`, both derived from `device`: the copier contains two `device` parts, two serial numbers,
and `c.serial` is ambiguous. With `scanner_v : virtual device` and `printer_v : virtual device`, there is one `device`,
constructed by the most derived class - `copier_v` - before all others; the `device{...}` of `scanner_v` and
`printer_v` are skipped. Where the shared part is depends on the complete object - 12 bytes behind a plain scanner, 28
behind the scanner part of a copier - so the offset is in the vtable: every class with a virtual base has a vptr, even
without virtual functions (`sizeof(scanner_v)` is 16, not 8), and every access through a `scanner_v&` is three loads -
the vptr, the offset from the table, the member.

**I can explain "undefined reference to `vtable for circle`" - and in which object file a vtable ends up.**

Every constructor of `circle` refers to `vtable for circle`, because it writes the vptr. The compiler emits the table in
one object file only: the one that defines the key function - the first virtual function of the class that is neither
pure nor defined in the class body. Our results for 'Wolf Creek', gcc 13 and clang 18 on Linux:

- Without the definition of `circle::area`, the key function, no file emits the table. The linker reports
  `undefined reference to 'vtable for circle'`, in the constructor - although what is missing is `area`.
- Without `circle::perimeter`, the table is emitted in `shapes.o`, and its slot for `perimeter` refers to a function
  nobody defines: `undefined reference to 'circle::perimeter() const'`, found in `.data.rel.ro` - in the table.
- `nm -C`: `vtable for circle` is in `shapes.o` (gcc `V`, clang `D`), not in `main.o` - there, gcc lists it as `U`,
  used by the inlined destructor.
- With all functions defined in the class body, there is no key function: every object file that constructs a circle
  has a copy of the table, `V`, and the linker keeps one - as for an `inline` function.

**I can use `override`, `final` and `= 0`, write an interface, and copy polymorphic objects with `clone`.**

`virtual` in the base, `override` in the derived classes - the compiler checks that the function overrides one.
`final` on a function forbids further overrides, on a class further derivation. `= 0` makes a function pure virtual and
the class abstract; a pure virtual function may still have a body, called by its full name, and a pure virtual
destructor must have one. An interface is a class with only pure virtual functions, a virtual (or protected) destructor,
and no data; a class may implement several. A virtual `clone` returns a `unique_ptr<base>` to a copy of the real
object - with a raw pointer as the return type, an override may even return a pointer to its own class (a covariant
return type).

**I can derive my own exception class from `std::runtime_error`, order the `catch` blocks, and say why an exception is
caught by reference.**

`class stack_full : public std::runtime_error` passes its message to the base constructor, which stores it and returns
it from the virtual `what()`; the class adds members a handler may need. The `catch` blocks are tried from top to
bottom, and a handler for a base also catches all derived classes - so the most derived one comes first. Caught by
value, the thrown object is copied into an object of the handler's type: it is sliced, gets the base's vptr, and
`what()` returns the base's text instead of the message. Caught by `const&`, the handler refers to the thrown object
itself, and virtual calls reach the overrides - no copy, no slicing. gcc warns about a polymorphic type caught by
value (`-Wcatch-value`), clang does not.

**I know the traps: a missing `const` without `override`, hidden overloads, default arguments in an override.**

Without `override`, `double read()` in a derived class of a `sensor` with `double read() const` is a new function, not
an override: a `const sensor&` still calls `sensor::read` - gcc 13 and clang warn with `-Woverloaded-virtual`, in
`-Wall`; with `override`, it is an error. A function in a derived class hides every function of the same name in the
base, all overloads - `using base::f;` brings them back. Default arguments are filled in by the compiler from the static
type, while the function comes from the dynamic type: `g.greet()` through a `const greeter&` calls the override with the
default of the base. So an override does not declare other defaults.

**I can explain `public`, `protected` and `private` inheritance, and when a class should derive - and when it should
have a member instead.**

`public` inheritance keeps the access of the base's members for the users of the derived class, and makes the derived
class usable as a base - "is a". `protected` and `private` inheritance turn the base's public members into protected or
private ones and hide the base from the users: "is implemented in terms of", for which a member is usually clearer.
Derive only if an object of the derived class can be used wherever the base is expected, with everything the base
promises - Liskov's rule. A square whose width cannot change on its own breaks the promise of a rectangle with a
`set_width`; then "has a" - a member, or no relation at all - is the better design.

### 'AI' - Two Opinions <a id="ai-0x08"></a>

| | `sizeof` of a class with one `int` and one virtual function | a virtual call in the constructor of the base |
|:--|:--|:--|
| gcc 13 and clang 18, x86-64 Linux | 16 | reaches the version of the base |
| gcc 11, ARM64 Linux | 16 | reaches the version of the base |

Answer A:

- "every object of the class larger by one pointer - 8 bytes on a 64-bit system - however many virtual functions" -
  right.
- "the table of function addresses exists once per class" - right.
- "loads the pointer, loads the address of the function from the table, and jumps there" - right.
- "the processor predicts that jump well, so the real price is that the compiler cannot inline the call" - right for
  loops over one type; with mixed types, the wrong guesses cost more (see the session). Right enough.
- "A class with an `int` and one virtual function is therefore 12 bytes" - wrong: 16. The object is aligned to 8,
  because of the pointer, and 4 bytes of padding follow the `int` - as for a struct with a pointer and an `int`.

Answer B:

- "two loads and an indirect jump" - right.
- "When the compiler knows the exact type - a local object, or a class marked `final` - it calls the function directly
  and may inline it" - right.
- "The destructor of a base class should be `virtual`, so that `delete` through a base pointer destroys the whole
  object" - right.
- "since the vptr is set before any constructor runs, a virtual call in the constructor of the base already reaches the
  override of the derived class - as in Java" - wrong: each constructor sets the vptr of its own class, the base first.
  While the constructor of the base runs, the object is a base, and the call reaches the version of the base.

A forgets the padding, B takes Java for C++ - both make a mistake that sounds like the rest of their answer, which is
right.

## Unit 0x09 <a id="check-0x09"></a>

**I can say where the code of a function is, what a function pointer holds, and what a call through it looks like in the
machine.**

The machine code of a function is loaded into memory with the program - into the text segment, readable and executable,
not writable - next to the global data, far from the stack and the heap. A function pointer holds the address of its
first instruction: 8 bytes. A direct call has the address in the instruction (`call square(double)`, `bl` on ARM64); a
call through a pointer loads it into a register and calls that (`call rdx`, `blr x0`) - an indirect call, like a virtual
call without the table.

**I can write the type of a function pointer - as a variable, a parameter and a return type, with and without `using` -
and use a table of them.**

`double (*f)(double)` - the parentheses make `f` a pointer; `using unary = double (*)(double);` gives the type a name.
As a parameter, `double apply(unary f, double x)`; as a return type, `unary pick(char op)` - without the alias
`double (*pick(char op))(double)`, read from the name outwards. `std::array<unary, 3> table{square, cube, half}` is a
table of addresses, and `table[i](x)` calls by a number - what a `switch` table and a vtable do.

**I can explain how `qsort` gets its comparison, and what that costs.**

As a function pointer: `int (*)(const void*, const void*)` - the addresses of two elements, as `const void*`, since
`qsort` knows only their size. `qsort` is compiled into the C library and cannot see the comparison: every comparison is
a real, indirect call, with two conversions back to the element type - thousands of calls for a thousand elements. That
is why `qsort` is slower than `std::sort` with a lambda (task 'Silver Lake': 135 against 75 ms for a million `int`s).

**I can write the class behind a lambda by hand, and read what C++ Insights shows for it.**

A struct with one data member per capture, and a `const` member function `operator()` with the lambda's parameters and
body: `[low, high](int v) { return low <= v && v <= high; }` is
`struct in_range { int low; int high; bool operator()(int v) const {...} };`, and the lambda expression creates an
object of it, `in_range{low, high}`. C++ Insights shows exactly that, with a made-up name like `__lambda_33_26`, a
constructor for the captures, `mutable` as a missing `const`, a template `operator()` for `auto` parameters, and for a
lambda without captures a conversion to a function pointer.

**I can predict `sizeof` of a lambda from its captures.**

The captures, laid out like the members of a struct: `[]` 1 byte (every object takes one), `[n]` 4, `[n, d]` 16 (an
`int`, 4 bytes of padding, a `double`), `[&n]` 8 - an address -, `[&n, &d, &k]` 24 with gcc and clang, one address per
variable (the standard leaves that open), `[s]` `sizeof(string)` - 32 with libstdc++, 24 with libc++.

**I can explain where a captured copy lives, and what a captured reference holds.**

A captured copy is a data member of the lambda object: inside the lambda, `&n` is the address of the member - the
address of the lambda object itself, if it is the first member. It lives where the lambda object lives: in the frame, in
a `std::function`'s buffer, on the heap. A captured reference is an address stored in the lambda: its 8 bytes are the
address of the original variable, and every call reads the variable through it.

**I can explain why a `mutable` lambda keeps its state, and what a copy of it does.**

Its state is a data member of the object; `mutable` removes the `const` from `operator()`, so the call may change the
member, and the next call sees the change - the object lives on between the calls. A copy of the lambda copies its
members: from then on there are two independent states - two counters that count on their own.

**I know that every lambda has its own type - and why that makes `std::sort` with a lambda fast.**

Every lambda expression creates a new class, even two with the same text; its name is made up by the compiler (`UliE_`,
`$_0`) and cannot be written - hence `auto`. `std::sort` is a template, instantiated for the type of the comparison:
with a lambda, that type says exactly which `operator()` is called, so the compiler inlines it into the sort. With a
function pointer, the type `bool (*)(int, int)` does not say which function - every comparison is an indirect call.

**I can explain which lambdas convert to a function pointer, and why the others cannot.**

Only lambdas without captures: their `operator()` needs nothing from the object, so a static function can do the same,
and the conversion returns its address - `qsort` takes such a lambda. A lambda with captures needs its data, and a
function pointer is only the address of code: there is no place for the data. C APIs work around it with an extra
`void*` argument; C++ passes the object.

**I can compare a template parameter, a function pointer and a `std::function` as a parameter: machine code, size,
allocations, speed.**

A template parameter: one instantiation per lambda type, the lambda's body inlined - no call at all, and the optimizer
sees the whole loop; the parameter is the lambda object itself. A function pointer: 8 bytes, an indirect call per
element, unless the compiler inlines the function that takes it and knows the value (clang did, gcc did not). A
`std::function`: 32 bytes (libstdc++), maybe an allocation, a test for empty and an indirect call through the invoker
per element, the argument passed through memory. In the session, the lambda took 6 ms, the others 13 to 27 ms on
x86-64 - about one and a half times as long on an Apple silicon Mac.

**I can explain what a `std::function` stores, when it allocates, and what happens when it is empty.**

libstdc++: 16 bytes of storage and two function pointers - one to call the callable, one to copy and destroy it. A
callable of up to 16 bytes that can be copied byte by byte - a function pointer, a lambda with two `double`s - is stored
inside; anything else goes into a block on the heap, and every copy of the `std::function` copies it again. libc++ has
48 bytes with room for 24. An empty `std::function` converts to `false`, and calling it throws
`std::bad_function_call` - the test for that is part of every call.

**I can explain why a lambda that captured a reference or `this` can dangle - and three ways to prevent it.**

A lambda can live longer than the scope it was made in - returned, stored in a `std::function`, in a list of callbacks.
A captured reference is the address of a variable, `[this]` the address of an object; when the variable dies, or the
object moves - a `vector` that grows moves its elements -, the lambda points to memory that belongs to something else.
Prevent it: capture by copy (or move) what the lambda needs later; keep the objects where they are - `reserve`,
`unique_ptr` elements, a class that cannot be copied or moved; or give the lambda a way that does not need the address
at all.

**I can move a `unique_ptr` into a lambda, and say what that does to the lambda.**

With an init-capture: `[p = std::move(p)] { ... }` creates a member `p` and moves the pointer into it - the lambda owns
the object now, and deletes it when it dies; the old `p` is empty. The lambda cannot be copied any more - its member
cannot. So `std::function`, which must copy, refuses it; C++23 has `std::move_only_function` (libstdc++ from gcc 12).

**I can use the everyday algorithms with lambdas, and I know the traps of `accumulate` and `remove_if`.**

`find_if`, `count_if`, `all_of`/`any_of`/`none_of`, `transform`, `accumulate`, `sort` and `stable_sort` with a lambda or
with a projection in `std::ranges`, `min_element`/`max_element`, `iota`, `generate`. `accumulate` sums in the type of
its start value: `accumulate(v.begin(), v.end(), 0)` over `double`s cuts every value to an `int` - start with `0.0`.
`remove_if` removes nothing: it moves the elements to keep to the front and returns where the rest begins; `erase` cuts
it off - or `std::erase_if` in one step.

**I can write generic lambdas, init-captures, an immediately invoked lambda and a recursive lambda.**

Generic: `[](const auto& a, const auto& b) { return a < b; }`, or with a name for the type,
`[]<typename T>(const vector<T>& v) {...}`. Init-capture: `[total = 0]`, `[&hits = counter]`, `[v = std::move(v)]`.
Immediately invoked: `const string status{[&] { ...; return "..."; }()};` - a few lines of logic for a `const`.
Recursive: a lambda cannot call itself by name - capture a `std::function` by reference, pass the lambda to itself
(`fib(fib, 10)`), or, in C++23, `this auto self`.

**I know what `[=]` captures in a member function, and the difference between `[this]` and `[*this]`.**

`[=]` in a member function captures `this` - the pointer - and the members are read through it, not copied; C++20
deprecated that. `[this]` is the pointer, 8 bytes: the lambda sees and changes the object, and dangles with it.
`[*this]` is a copy of the whole object inside the lambda (C++17) - `const`, unless the lambda is `mutable`; changes do
not reach the original.

**I can write a list of callbacks with `std::function` - and say why a widget that registers `[this]` must not be copied
or moved.**

`std::vector<std::function<void(const button&)>>`, `add` appends a callback, raising the event calls them all in order -
the button does not care what they are. A widget that registers a callback with `[this]` has its own address in that
callback; a copy or a move would leave the callback pointing to the old object. So the widget base class deletes copy
and move: a widget lives where it was created, and is passed by reference.

### 'AI' - Two Opinions <a id="ai-0x09"></a>

| | `sizeof` of a lambda without captures | `std::function` with a lambda of three `double`s | with a lambda that captured a long `string` |
|:--|:--|:--|:--|
| gcc 13, libstdc++, x86-64 Linux | 1 | 1 allocation | 2 allocations |
| clang 18, libc++, x86-64 Linux | 1 | 0 allocations | 2 allocations |
| gcc 11, libstdc++, ARM64 Linux | 1 | 1 allocation | 2 allocations |

Answer A:

- "an object of a class that the compiler generates ... the captured variables become its data members, and its body
  becomes the `operator()`" - right.
- "every lambda has its own type, an algorithm like `std::sort` is instantiated for it, and the call can be inlined" -
  right.
- "that is why a lambda is usually faster than a function pointer" - right (task 'Silver Lake').
- "A lambda without captures takes no memory at all - `sizeof` is 0" - wrong: 1. Every object takes at least one byte,
  so that two objects have two addresses - the empty class of unit 0x08. As a member or an argument, it may cost nothing
  after inlining, but `sizeof` is 1.
- "it can be converted to a plain function pointer" - right.

Answer B:

- "A lambda that captures by reference stores the addresses of the variables, so it must not outlive them" - right.
- "`std::function` can hold any lambda without an allocation, because it stores the lambda inside its own object" -
  wrong: only a small one - up to 16 bytes that can be copied byte by byte with libstdc++, 24 with libc++. A lambda with
  a `string`, a `vector` or a few `double`s goes to the heap, and every copy of the `std::function` allocates again.
- "A call through a `std::function` is an indirect call, which the compiler usually cannot inline" - right.
- "in a hot loop, pass a lambda as a template parameter rather than as a `std::function`" - right.

A forgets the byte of an empty object, B takes the small buffer for the rule - both mistakes sound like the rest of
their answers, which is right.

## Unit 0x0a <a id="check-0x0a"></a>

**I can say what a thread has of its own and what it shares with the others - and show it with addresses.**

Its own: a stack - 8 MiB on Linux, 512 KiB for a thread on macOS - with its local variables and frames, its registers,
and its `thread_local` variables. Shared: the global variables, the heap, the code. The address of a local variable is
different in every thread, the stacks a little more than 8 MiB apart on Linux (a guard between them); the address of a
global and of a block on the heap is the same for all threads.

**I can explain `thread_local`, and how the machine finds a thread's variable.**

A `thread_local` variable exists once per thread, created when the thread starts (or at its first use, if it has a
constructor), destroyed when it ends. The machine reaches it through a register that holds the address of the current
thread's block - `fs` on x86-64 (`add DWORD PTR fs:per_thread@tpoff, 1`), `tpidr_el0` on ARM64 - set by the operating
system at every switch; the linker fills in the offset. glibc puts the block next to the thread's stack.

**I can start threads with functions and lambdas, pass arguments and references, and join them.**

`std::thread t{f, args...}` starts `f(args...)`; the arguments are copied into the thread, so a reference parameter
needs `std::ref(x)` - or a lambda that captures `[&x]`. `t.join()` waits; every thread must be joined (or detached)
before its `std::thread` is destroyed, otherwise the program ends with `std::terminate`. Several threads go into a
`std::vector<std::thread>` with `emplace_back`, and are joined in a loop.

**I can explain what a `std::thread` object holds, and where the callable and its arguments go.**

A handle of the operating system's thread - 8 bytes, a number on Linux, a pointer on macOS - nothing else. The callable
and the copies of its arguments are copied into a block on the heap, which the new thread gets and frees when it is
done: one allocation with libstdc++, three with libc++. The stack of the thread comes from the operating system
directly.

**I know what it costs to start a thread, and when a thread pays off.**

Tens of microseconds - 40 to 60 µs per thread started and joined on our Linux machines (both virtual) - where a function
call takes a nanosecond or less. A thread pays off for work of milliseconds, not microseconds; many small tasks go to a
few long-lived threads - a thread pool.

**I can explain why `++counter` loses updates with two threads - in the machine, on x86-64 and on ARM64.**

`++counter` is a load, an add and a store. As Debug, x86-64 makes three instructions of it, as Release one -
`add DWORD PTR counter[rip], 1` - which is still a load, an add and a store inside the core; ARM64 always needs three,
`ldr`, `add`, `str`. Two cores can both load 41, both add, both store 42: two increments, one counted. Without a `lock`
prefix (x86-64) or an atomic instruction (`ldadd`, ARM64), nothing stops the other core in between. Two threads, a
million each: 1.0 to 1.4 million as Debug.

**I can explain why a data race is undefined behavior, and what the optimizer did with a racy loop and with a plain
flag.**

The standard makes a data race - two threads, the same location, at least one writes, no synchronization - undefined, so
that the compiler can optimize each thread as if it were alone, and the processor can reorder its loads and stores. As
Release, gcc and clang turned the loop of a million `++counter` into one `add` of a million - two million, "right", by
luck. And `while (!ready) {}` with a plain `bool` became `ret`: without a race, `ready` cannot change during the loop,
and a loop without side effects must end - so the loop is gone, and the waiting thread read `data` as 0.

**I know why `volatile` is no protection between threads.**

`volatile` forces the compiler to load and store the variable every time the program says so - meant for device
registers. It makes no operation atomic - a `volatile int` counted up by two threads lost half of the increments as
Release in our runs (about 1.0 million of 2) - and it orders nothing: the other core may see a `volatile` flag before
the data written in front of it (ARM64). It is still a data race, and undefined behavior; ThreadSanitizer reports it.

**I can use `std::atomic`, and say what `++` on an atomic is in the machine.**

`std::atomic<int> counter{0}; ++counter;` from any number of threads - always right. It is an `int`, 4 bytes, lock-free.
`++` is `lock add` on x86-64 (clang: `lock inc`); on ARM64 `ldaddal`, or with gcc a call of `__aarch64_ldadd4_acq_rel`,
which picks `ldaddal` or a `ldaxr`/`stlxr` loop, depending on the processor. Each operation is atomic, a sequence is not
- for "if it is 0, make it 1", `compare_exchange_strong`.

**I can protect data with a mutex - `lock_guard`, `unique_lock`, `scoped_lock` - and say what locking costs, with and
without competition.**

`const std::lock_guard lock{m};` locks until the `}`; `unique_lock` can unlock and lock again (for condition variables);
`scoped_lock{a, b}` takes several mutexes without deadlock. A class that protects itself has a `mutable` mutex and locks
it in every member function. A free mutex costs a `lock cmpxchg` to lock and another atomic instruction to unlock, plus
two calls - a few dozen nanoseconds. A taken one sends the waiting thread to sleep with a system call (`futex` on
Linux), and the unlock must wake it: with two threads on one mutex, our counter took 55 to 240 ms instead of 9 to 55.

**I can explain cache lines and false sharing, and avoid false sharing.**

Caches hold memory in lines of 64 bytes. A core that writes to a line needs it exclusively; another core that writes to
it must take it over. Two counters in the same line, written by two threads, make the line travel back and forth with
every increment: four times slower than two counters 64 bytes apart in the session, slower even than one thread doing
both. Avoid it: `alignas(64)` on each member that another thread writes, or
`std::hardware_destructive_interference_size`, or - best - let each thread work in a local and write the result once.

**I can explain a deadlock and two ways to avoid it.**

Two threads, two mutexes: thread 1 holds `a` and waits for `b`, thread 2 holds `b` and waits for `a` - both wait
forever. Avoid it by locking in one fixed order everywhere (by address, by number), by taking both at once with
`std::scoped_lock{a, b}`, or by never holding more than one lock at a time.

**I can wait with a condition variable and a predicate - and say why the predicate is needed.**

`std::unique_lock lock{m}; cv.wait(lock, [&] { return ready; });` - checks the predicate, and if it is `false`, unlocks
the mutex and sleeps; `notify_one` or `notify_all` wakes it, and it locks and checks again. The other side sets `ready`
under the same mutex, then notifies. Without the predicate, a notification that came before the `wait` is lost - a
condition variable remembers nothing - and a spurious wakeup would pass for a real one. A spinning thread checked its
flag 40 to 85 million times in 100 ms; the waiting one checked its condition twice.

**I can get a result or an exception from another thread with `std::async` and `std::future`, or with a
`std::promise`.**

`auto result = std::async(std::launch::async, f, args...);` starts `f` in a thread; `result.get()` waits and returns the
value - or throws the exception `f` threw. A `std::promise<int>` is the other end: a thread calls `set_value` (or
`set_exception`), another waits at `promise.get_future().get()`. The value goes through a shared state on the heap.
`launch::deferred` runs `f` in the thread that calls `get()`. The destructor of a future from `std::async` waits - a
future that is thrown away makes the call synchronous.

**I can explain what a memory order is, and why x86-64 and ARM64 need different instructions for it.**

Processors reorder loads and stores as long as their own thread cannot tell; other threads can. A memory order says how
much order an atomic operation needs: `relaxed` none but itself, `release`/`acquire` that what was written before the
release is visible after the acquire, `seq_cst` (the default) one order of all atomic operations. x86-64 reorders only a
store with a later load: release and acquire are plain `mov`s, a `seq_cst` store is `xchg`. ARM64 reorders much more:
acquire and release need `ldar` and `stlr`. The store-buffer test showed both threads reading 0 with `release`/`acquire`
on x86-64 - never with `seq_cst`.

### 'AI' - Two Opinions <a id="ai-0x0a"></a>

| | two threads, a million `++counter` each, Debug | Release | with `volatile int`, Release |
|:--|:--|:--|:--|
| gcc 13, x86-64 Linux | 1.0 to 1.4 million | 2,000,000 | about 1.0 million |
| gcc 11, ARM64 Linux | about 1.0 million | 2,000,000 | about 1.0 million |

Answer A:

- "`++counter` is a read-modify-write ... Two threads can load the same value and both store it plus one, so an
  increment is lost" - right.
- "With optimizations, the compiler emits a single `add` instruction to memory on x86-64" - right, as Release gcc and
  clang do.
- "a single instruction is executed atomically by the processor - so as Release, it is safe in practice" - wrong: an
  `add` to memory is a load, an add and a store inside the core, and another core can come in between; only `lock add`
  is atomic. That the Release build counted to two million is the compiler's doing - it made one `add` of a million out
  of the loop - and luck; it is still a data race, and undefined behavior.
- "To be sure everywhere, use `std::atomic<int>`" - right.

Answer B:

- "a data race - undefined behavior in C++, whatever the processor does" - right.
- "`std::atomic<int>` makes `++` atomic; on x86-64, the compiler emits the instruction with a `lock` prefix" - right.
- "A lighter alternative is to declare `counter` as `volatile`: it forces every access to go to memory, so each thread
  always sees the current value" - wrong: every access goes to memory, but the load, the add and the store are still
  three steps - with `volatile`, the Release build lost half of the increments. `volatile` is for device registers, not
  for threads. (And `++` on a `volatile` is deprecated since C++20.)

A trusts the single instruction, B trusts `volatile` - the two classic ways to believe that a race is harmless.
