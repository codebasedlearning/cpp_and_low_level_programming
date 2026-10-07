// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The whole program in one picture: machine code, constants, initialized and zero-initialized globals, the heap and
 *   the stack - each in its own region of the address space, and in its own part of the program file.
 * - A global with a value costs bytes in the file (`.data`); a global that is zero costs only a number (`.bss`).
 * - Code runs before `main` - the constructors of globals - and after it - their destructors.
 * - The tools: `size`, `objdump -h`, `nm` on Linux; on macOS `size -m` and `otool -l`.
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector, std::uintptr_t;


/* ---- Content ---- */

namespace {

    /* --- The globals ---
     * One of each kind. The names say where the compiler puts them - on Linux with gcc and clang, the sections of an
     * ELF file; macOS and Windows have the same idea under other names (`__TEXT`, `__DATA`; `.text`, `.rdata`).
     * - !![#memory-layout]
     */
    int initialized{42};                    // .data - the 42 is stored in the file
    int zeroed{};                           // .bss - only "4 bytes of zeros" is stored
    constexpr int table[]{1, 2, 3};         // .rodata - read-only, next to the string literals
    char big_data[100'000]{1};              // .data - 100000 bytes in the file, all but one of them 0
    char big_zero[100'000]{};               // .bss - 100000 bytes of memory, nothing in the file

    /* --- `early` ---
     * A global object with a constructor: it runs before `main`, and its destructor after `main` returns - the
     * startup code of the program calls them. The order among globals of one file is the order of definition; across
     * files it is not defined (see previous snippets on `static`).
     */
    struct early {
        early() { cout << " 0| before main: the constructor of a global\n"; }
        ~early() { cout << " 9| after main: its destructor\n"; }
    };

    early at_startup{};

    /* --- `show_the_regions` ---
     * Collects one address per region, sorts them, and prints them from low to high. On Linux the order is: code,
     * constants, `.data`, `.bss`, then - after a gap - the heap, and far above all of it the stack. Run it twice: with
     * address space layout randomization, the numbers change, the order does not.
     */
    void show_the_regions() {
        print_function_header();

        const int local{7};
        const int* heap{new int{8}};
        const char* literal{"So What"};
        vector<std::pair<uintptr_t, string>> regions{
            {reinterpret_cast<uintptr_t>(&show_the_regions), "code (.text) - this function"},
            {reinterpret_cast<uintptr_t>(literal), "string literal (.rodata)"},
            {reinterpret_cast<uintptr_t>(table), "constexpr table (.rodata)"},
            {reinterpret_cast<uintptr_t>(&initialized), "initialized global (.data)"},
            {reinterpret_cast<uintptr_t>(big_data), "big initialized array (.data)"},
            {reinterpret_cast<uintptr_t>(&zeroed), "zero global (.bss)"},
            {reinterpret_cast<uintptr_t>(big_zero), "big zero array (.bss)"},
            {reinterpret_cast<uintptr_t>(heap), "heap block"},
            {reinterpret_cast<uintptr_t>(&local), "local (stack)"},
        };
        std::ranges::sort(regions);
        for (const auto& [address, name] : regions) {
            cout << " 1| 0x" << std::hex << address << std::dec << "  " << name << '\n';
        }
        cout << " 2| " << initialized + zeroed + table[0] + big_data[0] + big_zero[0] + *heap + local << '\n';
        delete heap;
    }

}

/* --- Look at the file ---
 * Build, then in the build folder (Linux):
 * - `size tinker_memory_map` - the sizes of `text`, `data` and `bss`. `data` and `bss` are both over 100000, for the
 *   two big arrays.
 * - `ls -l tinker_memory_map` - the file is about 100 KB larger than without `big_data`, and not larger at all for
 *   `big_zero`: `.bss` is only a size, the system hands out zeroed pages when the program starts.
 * - `objdump -h tinker_memory_map` - the sections, with their addresses and offsets in the file.
 * - `nm -C tinker_memory_map | grep anonymous` - the symbols of the unnamed namespace, with the letters `d`, `b` and
 *   `r` for data, bss and read-only data (lowercase: local), and `t` for the functions.
 * On macOS, `size -m` and `otool -l` show the segments and sections of the Mach-O file.
 * - !![#symbols]
 */

/* --- `main` --- */
int main() {
    show_the_regions();

    return EXIT_SUCCESS;
}
