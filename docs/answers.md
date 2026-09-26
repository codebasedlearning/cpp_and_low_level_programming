# Answers

## A-023                                    <a id="a-023"></a>
It is unknown and random. The variable just uses a specific memory location.
Reading the value of an uninitialized local variable, here of type `int`, is 
undefined behavior (UB) in C++ 
<br> - !![UB](glossary.md#undefined-behavior).

## A-029                                    <a id="a-029"></a>
`init_variable_on_stack` wrote 23 into its stack frame. When it returned, the frame was
released, but not cleared. The second call of `define_variable_without_init` gets a frame at
the same place, and `v` happens to use the same slot - so it "finds" the 23.
This is an observation about one build, not a guarantee: with `-O2` or another compiler the
output differs. Reading `v` is still undefined behavior.
<br> - !![Stack and heap](glossary.md#stack-and-heap)

## A-059                                    <a id="a-059"></a>
- `string hello{"Hello!"};` creates the object itself, here on the stack - no `new`, no reference.
- A C++ `string` is mutable (`+=`, `replace`, ...), a Java `String` is not.
- Assignment copies the text (value semantics); in Java it copies the reference.
- There is no garbage collector: the memory is released when `hello` goes out of scope.
- `size()` counts bytes, not characters - it makes a difference for text beyond ASCII.

## A-073                                    <a id="a-073"></a>
`cout` is of type `ostream`.

## A-101                                    <a id="a-101"></a>
`33 22 11 00` - the least significant byte comes first, at the lowest address. This is called
little-endian, and it is what x86 and (usually) ARM do. Big-endian systems, and most network
protocols, store the bytes in the order you write them: `00 11 22 33`. The value is the same,
only its order in memory differs.

## A-102                                    <a id="a-102"></a>
The compiler decides where each variable lives in the stack frame - the order in the source is
no promise. It places each variable at an address that suits its type (alignment): an `int` at a
multiple of 4, a `double` at a multiple of 8. Bytes that are left over stay unused (padding).
Order and gaps can change with another compiler or with `-O2`.
<br> - !![sizeof](glossary.md#sizeof)

## A-103                                    <a id="a-103"></a>
No, both are equally big. `sizeof(string)` belongs to the type and is fixed at compile time -
32 bytes with gcc, 24 with clang. The object holds only the bookkeeping: where the characters
are, the size, the capacity - and, for short texts, room for the characters themselves.
Long texts live outside the object, on the heap.
<br> - !![Small string optimization](glossary.md#sso)

## A-104                                    <a id="a-104"></a>
`s2` is a variable on the stack, so it stays where it is. Its text no longer fit into the
object, so the string requested memory on the heap, copied the characters there and now
refers to them. That is the cost: an allocation and a copy, invisible in the source code.
Appending in a loop can trigger it again and again - `reserve()` helps if you know the final size.

## A-105                                    <a id="a-105"></a>
2,692,537. Every call with `n > 1` makes two more calls, so the number of calls grows like the
Fibonacci numbers themselves - exponentially: `fib(n)` needs `2*fib(n+1) - 1` calls. Each of them
creates and removes a stack frame. A loop needs 30 steps and no extra frames.

## A-106                                    <a id="a-106"></a>
It depends on the build. With `-O0` every call gets its own stack frame; a million of them do not
fit into the usual 8 MB stack, and the program crashes with a segmentation fault (exit status 139).
With `-O2`, gcc turns the recursion into a loop: no crash at all, and it prints `0` - the product
overflowed long before, and after enough factors of 2 all its bits are 0.
Same source code, two completely different results - that is undefined behavior in practice.
<br> - !![UB](glossary.md#undefined-behavior)

## Answer                               <a id="answer-mem-dump1"></a>
lorim

## Answer Mem-Dump2
ipsum

