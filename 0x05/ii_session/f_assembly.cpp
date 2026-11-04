// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of this unit: the assembly the compiler makes of your code - with `g++ -S`, or in the browser with
 *   a [Compiler Explorer](www.godbolt.org).
 * - The arguments of a function arrive in registers, the result leaves in a register.
 * - Debug (`-O0`) keeps every variable in the stack frame; Release (`-O2`) keeps them in registers - and inlines small
 *   functions.
 * - Two instruction sets: x86-64 (Intel, AMD) and ARM64 (Apple silicon, most phones).
 * - `extern "C"`: a name without mangling - a function that C, and every language that can call C, can call.
 * - The name is one half of the agreement, the calling convention the other: x86-64 has two.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * As in the preparation of unit 0x04, the next functions are not in the unnamed namespace: this way they stay in the
 * assembly under their own names, even when they are inlined everywhere.
 */

/* --- `square` --- One parameter, one result. */
int square(const int n) {
    return n * n;
}

/* --- `sum_of_squares` --- Two parameters, two local variables, two calls. */
int sum_of_squares(const int a, const int b) {
    const int sa{square(a)};
    const int sb{square(b)};
    return sa + sb;
}

/* --- `increment` --- A pointer parameter: the function writes to the address it gets. */
void increment(int* p) {
    ++*p;
}

/* --- `square_c` ---
 * `square` once more, for other languages. `extern "C"` switches off the name mangling: the label is `square_c` (on
 * macOS `_square_c`), not `_Z8square_ci`. That is a name C knows - and with it every language that can call C:
 * Python, Rust, Swift, C#, Java ... The machine code is the same as for `square`, only the name differs.
 * - !![#extern-c]
 */
extern "C" int square_c(const int n) {
    return n * n;
}

namespace {

    /* --- `use_the_functions` --- The results are not the point, the machine code is. */
    void use_the_functions() {
        print_function_header();

        int n{41};
        increment(&n);
        cout << " 1| square(7)=" << square(7) << ", sum_of_squares(3, 4)=" << sum_of_squares(3, 4) << '\n';
        cout << " 2| n=" << n << '\n';
        cout << " 3| square_c(7)=" << square_c(7) << '\n';
    }

}

/* --- Getting the assembly ---
 * Two ways - the result is the same.
 * - Compiler Explorer, e.g. godbolt.org, in the browser: paste the functions `square`, `sum_of_squares`, `increment`
 *   and `square_c` - no `#include`, no `main` - into the left window, choose "x86-64 gcc" as the compiler, and write
 *   `-O0` into the compiler options. Each source line and its instructions get the same color.
 * - In a terminal in this folder: `g++ -std=c++23 -S -O0 -masm=intel -I ../../utils f_assembly.cpp` writes
 *   `f_assembly.s`. It is long - `cout` pulls in a lot. Search for `_Z6squarei` (on macOS `__Z6squarei`): the labels
 *   are the mangled names from unit 0x04.
 * Without `-masm=intel`, `g++` writes the AT&T syntax: `movl %edi, -4(%rbp)` - the source first, a `%` before each
 * register, the size as a suffix. This course uses the Intel syntax, the default of Compiler Explorer: the destination
 * comes first, as in an assignment. On Apple silicon, `-masm=intel` could be an error - you may get ARM64 anyway.
 */

/* --- Reading x86-64 ---
 * `square` as Debug (`-O0`), with gcc:
 * - `push rbp`, `mov rbp, rsp` - the function sets up its stack frame; `rbp` marks where it is.
 * - `mov DWORD PTR [rbp-4], edi` - the argument arrives in register `edi` and is stored in the frame. `[...]` is the
 *   memory at that address, `DWORD` means 4 bytes. That is `n` - the variable whose address you printed in earlier
 *   units. (Some gcc versions write `-4[rbp]`, the same address.)
 * - `mov eax, DWORD PTR [rbp-4]`, `imul eax, eax` - load `n` into `eax`, multiply.
 * - `pop rbp`, `ret` - remove the frame, return. The result is whatever is in `eax` now.
 * The first `int` arguments arrive in `edi`, `esi`, `edx`, ..., an `int` result leaves in `eax`. The `e` registers are
 * the lower 32 bits of the 64-bit registers `rdi`, `rsi`, `rdx`, `rax`, ...
 * `sum_of_squares` calls `square` twice: `mov edi, ...` puts the argument in place, `call _Z6squarei` jumps there and
 * pushes the return address onto the stack. `endbr64` at the start of each function (Ubuntu's gcc) marks it for a
 * security feature - ignore it.
 * Now as Release (`-O2`): no frame, no memory.
 * - `square` is an `imul`, a `mov` and `ret`.
 * - `sum_of_squares` has no `call` any more: `square` is inlined, two `imul`s, and `lea eax, [rdi+rsi]` adds the
 *   squares. `lea` computes an address - here it serves as an adder.
 * - `increment` is one instruction, `add DWORD PTR [rdi], 1` (clang: `inc`) - `*p` in the machine: the memory at the
 *   address in `rdi`.
 * - `square_c` is the same three instructions as `square`, under the label `square_c`.
 * - !![#calling-convention]
 */

/* -- .Q&A -- !![As Release, nobody calls `square` any more. Why is it still in the assembly?](#a-502) */

/* --- Reading ARM64 ---
 * In Compiler Explorer, choose "ARM64 gcc" or "armv8-a clang" - or look at the `.s` file on Apple silicon.
 * - The first arguments arrive in `w0`, `w1`, ... (32 bits) or `x0`, `x1`, ... (64 bits, e.g. an address), the result
 *   leaves in `w0` or `x0`. At `-O2`, `square` is `mul w0, w0, w0` and `ret`.
 * - `bl` calls a function and puts the return address into register `x30`, not onto the stack.
 * - ARM64 computes only in registers: `increment` needs three instructions - `ldr` loads `*p`, `add` adds, `str`
 *   stores. x86-64 can add to memory directly.
 * The instructions differ, the idea is the same: arguments in registers, the result in a register, the stack for
 * whatever does not fit or must have an address.
 */

/* --- Calling `square_c` from Python ---
 * Build this file as a shared library - machine code that other programs load at run time - and call it with
 * `ctypes`, Python's way to call C. In a terminal in this folder:
 *   g++ -std=c++23 -shared -fPIC -I ../../utils f_assembly.cpp -o libsquare.so
 *   python3 -c 'import ctypes; print(ctypes.CDLL("./libsquare.so").square_c(7))'
 * It prints 49. Ask for `square` instead, and Python says `undefined symbol: square` - the library only knows
 * `_Z6squarei`. `nm -D libsquare.so` lists both names. On macOS, the file is `libsquare.dylib` and `nm -gU` lists the
 * names; on Windows, it is a `.dll`.
 */

/* --- One name, two calling conventions ---
 * `extern "C"` fixes the name, not the registers. Where the argument travels is the calling convention, and x86-64
 * has two: System V (Linux, macOS) passes the first `int` in `edi`, Microsoft x64 (Windows) in `ecx` (see future
 * snippets). gcc and clang can generate both - paste this into Compiler Explorer, "x86-64 gcc", `-O2`:
 *   extern "C" int square_sysv(int n) { return n * n; }
 *   extern "C" __attribute__((ms_abi)) int square_ms(int n) { return n * n; }
 * Two versions of `square`, three instructions each - `imul edi, edi` in the first, `imul ecx, ecx` in the second.
 * Caller and function must agree on both, the name and the convention - otherwise the function reads its argument
 * from a register nobody wrote to. 32-bit x86 had a whole zoo of conventions (`__cdecl`, `__stdcall`, `__fastcall`),
 * and old Windows headers are full of them.
 * - !![#calling-convention]
 */

/* --- Who cleans up the stack? ---
 * A convention also says who removes the arguments from the stack after the call. On 32-bit x86, where the arguments
 * were pushed onto the stack, the conventions differed in exactly that. With `__cdecl`, the caller cleans up: only
 * the caller knows how many arguments it pushed - that is what makes `printf` with its variable number of arguments
 * possible. With `__stdcall` (the Win32 API), the function cleans up as it returns: the code exists once, not after
 * every call - but a variable number of arguments is impossible. If caller and function disagree, the stack pointer
 * is off after every call - until a function returns to a wrong address.
 * On x86-64 and ARM64, the caller cleans up, in every convention - and there is rarely anything to clean: the first
 * arguments travel in registers.
 */

/* --- `main` --- */
int main() {
    use_the_functions();

    return EXIT_SUCCESS;
}
