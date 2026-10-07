// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Below `operator new` and `malloc` is the operating system. It hands out memory in pages - usually 4 KiB - and
 *   only virtual addresses at first: a page gets real memory when it is touched for the first time.
 * - `malloc` asks the system for large pieces and cuts them into small blocks. With glibc, small blocks come from one
 *   region that grows (`brk`), a large block - 128 KiB or more - gets its own mapping (`mmap`), far away.
 * - 256 MiB with `new` cost almost nothing - until the program writes to them.
 * - POSIX only (Linux, macOS): `sysconf` and `getrusage` are not standard C++. The `brk` and `mmap` details are glibc;
 *   macOS has its own allocator, with the same idea.
 */

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cbl/printing.hpp>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>                         // for sysconf - POSIX
#include <sys/resource.h>                   // for getrusage - POSIX
#define CBL_POSIX 1
#else
#define CBL_POSIX 0
#endif

using std::cout, std::uintptr_t;


/* ---- Content ---- */

#if CBL_POSIX

namespace {

    /* --- `resident_kib` ---
     * The largest amount of real memory the program has used so far, in KiB - "resident set size". Linux reports it in
     * KiB, macOS in bytes. It only grows, which is all we need here.
     */
    long resident_kib() {
        rusage usage{};
        getrusage(RUSAGE_SELF, &usage);
#if defined(__APPLE__)
        return usage.ru_maxrss / 1024;
#else
        return usage.ru_maxrss;
#endif
    }

    /* --- `ask_for_the_page_size` ---
     * The unit in which the system maps memory: 4096 bytes on x86-64 and on most ARM64 Linux systems, 16 KiB on macOS
     * with Apple silicon.
     * - !![#virtual-memory]
     */
    void ask_for_the_page_size() {
        print_function_header();

        cout << " 1| page size: " << sysconf(_SC_PAGESIZE) << " bytes\n";
    }

    /* --- `compare_small_and_large_blocks` ---
     * Two small blocks lie next to each other, 64 bytes plus the allocator's own bytes apart (see previous snippets).
     * The block of 1 MiB is somewhere else entirely: with glibc it has a mapping of its own, near the shared
     * libraries, and `delete[]` gives it straight back to the system.
     */
    void compare_small_and_large_blocks() {
        print_function_header();

        const char* a{new char[64]};
        const char* b{new char[64]};
        const char* big{new char[1024 * 1024]};
        const int local{0};
        cout << " 1| small a:  0x" << std::hex << reinterpret_cast<uintptr_t>(a) << '\n';
        cout << " 2| small b:  0x" << reinterpret_cast<uintptr_t>(b) << std::dec << " - "
             << reinterpret_cast<uintptr_t>(b) - reinterpret_cast<uintptr_t>(a) << " bytes after a\n";
        cout << " 3| big:      0x" << std::hex << reinterpret_cast<uintptr_t>(big) << '\n';
        cout << " 4| local:    0x" << reinterpret_cast<uintptr_t>(&local) << std::dec << '\n';
        delete[] big;
        delete[] b;
        delete[] a;
    }

    /* --- `touch_the_pages` ---
     * `new char[n]` does not initialize the `char`s, so nothing is written: the system has handed out addresses, and
     * the resident memory does not change. Writing one byte per page makes the system find a real page for each one
     * - and now the 256 MiB are there. This is the overcommit of the previous snippets, seen from inside.
     */
    void touch_the_pages() {
        print_function_header();

        const long page{sysconf(_SC_PAGESIZE)};
        const long size{256L * 1024 * 1024};
        const long before{resident_kib()};
        char* block{new char[static_cast<std::size_t>(size)]};
        const long allocated{resident_kib()};
        for (long i{0}; i < size; i += page) {
            block[i] = 1;
        }
        const long touched{resident_kib()};
        cout << " 1| resident: " << before << " KiB before, " << allocated << " KiB after new, " << touched
             << " KiB after touching every page\n";
        cout << " 2| " << int{block[0]} << '\n';
        delete[] block;
    }

}

/* --- Watch the system calls ---
 * On Linux, `strace -e trace=brk,mmap,munmap ./tinker_pages_and_mmap` lists the calls into the kernel: a `brk` that
 * moves the end of the small-block region, an `mmap` of a little more than 1 MiB for `big`, and one of a little more
 * than 256 MiB - and an `munmap` for each at its `delete[]`. On macOS, `dtruss` does the same, but needs special
 * rights.
 */

/* --- `main` --- */
int main() {
    ask_for_the_page_size();
    compare_small_and_large_blocks();
    touch_the_pages();

    return EXIT_SUCCESS;
}

#else

/* --- `main` --- Without POSIX, the functions above are not available. */
int main() {
    print_function_header();
    cout << " 1| This snippet needs POSIX (Linux, macOS) - `sysconf` and `getrusage` do not exist here.\n";

    return EXIT_SUCCESS;
}

#endif
