// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Every call takes a frame on the stack: the return address, saved registers, the locals. How big one is can be
 *   measured - the addresses of a local, one call deeper, differ by the size of the frame.
 * - The stack has a fixed size, set when the program starts (on Linux usually 8 MiB, `ulimit -s`). Frame size times
 *   depth must fit into it.
 * - Beyond the end is a guard page. A recursion that runs into it ends with a segmentation fault - no exception, no
 *   message from C++.
 * - Asking for the limit is not standard C++: `getrlimit` is POSIX (Linux, macOS), Windows has other functions.
 */

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cbl/printing.hpp>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>                   // for getrlimit - POSIX, not standard C++
#define CBL_POSIX 1
#else
#define CBL_POSIX 0
#endif

using std::cout, std::endl, std::uintptr_t;


/* ---- Content ---- */

namespace {

    /* --- Run the overflow? ---
     * The last topic lets a recursion run until the stack is full - the program crashes on purpose. Set it to `true`,
     * build, and run.
     */
    constexpr bool overflow_the_stack{false};

    /* --- `record_frames` ---
     * Each call has a local array of 256 bytes and writes its address into `addresses`, one entry per level. The
     * array is used after the recursive call, too, so the compiler must keep it - and cannot turn the recursion into a
     * loop.
     */
    int record_frames(const int level, uintptr_t* addresses) {
        char buffer[256]{};
        buffer[level] = static_cast<char>(level);
        addresses[level] = reinterpret_cast<uintptr_t>(buffer);
        const int deeper{level < 3 ? record_frames(level + 1, addresses) : 0};
        return deeper + buffer[level];
    }

    /* --- `measure_a_frame` ---
     * The stack grows downward on x86-64 and ARM64: each level has a lower address than the one before. The difference
     * is the frame: the 256 bytes of the array, plus the return address, saved registers and alignment - as Debug a
     * little more than as Release.
     * - !![#stack-and-heap]
     */
    void measure_a_frame() {
        print_function_header();

        uintptr_t addresses[4]{};
        const int sum{record_frames(0, addresses)};
        for (int level{1}; level < 4; ++level) {
            cout << " 1| level " << level << ": 0x" << std::hex << addresses[level] << std::dec << ", "
                 << addresses[level - 1] - addresses[level] << " bytes below the level before\n";
        }
        cout << " 2| sum=" << sum << '\n';
    }

    /* --- `ask_for_the_limit` ---
     * `getrlimit(RLIMIT_STACK, ...)` returns the limit for the stack of the main thread - what `ulimit -s` shows, in
     * bytes. Divided by the size of a frame, it says roughly how deep a recursion can go.
     */
    void ask_for_the_limit() {
        print_function_header();

#if CBL_POSIX
        rlimit limit{};
        if (getrlimit(RLIMIT_STACK, &limit) == 0) {
            if (limit.rlim_cur == RLIM_INFINITY) {
                cout << " 1| no limit for the stack\n";
            } else {
                cout << " 1| stack limit: " << limit.rlim_cur / 1024 << " KiB - about " << limit.rlim_cur / 300
                     << " frames of 300 bytes\n";
            }
        }
#else
        cout << " 1| `getrlimit` is POSIX - not available here (on Windows, the default is 1 MiB)\n";
#endif
    }

    /* --- `go_deeper` ---
     * A recursion with 1 KiB of locals per frame, which reports every 1000 levels. `endl` on purpose: the crash comes
     * without warning, and unflushed output would be lost. The condition `level < 100'000'000` is never reached - it is
     * there so that the compiler does not see an endless recursion (gcc would warn).
     */
    int go_deeper(const int level) {
        char buffer[1024]{};
        buffer[level % 1024] = 1;
        if (level % 1000 == 0) {
            cout << " 1| level " << level << endl;
        }
        const int deeper{level < 100'000'000 ? go_deeper(level + 1) : 0};
        return deeper + buffer[level % 1024];
    }

    /* --- `overflow_on_purpose` ---
     * With 8 MiB and a bit more than 1 KiB per frame, the last line printed is a little below level 8000. Then the
     * program touches the guard page: on Linux, a segmentation fault, exit status 139 - no exception to catch, no
     * destructor runs. As Release, the frames are smaller, so it gets further.
     */
    void overflow_on_purpose() {
        print_function_header();

        if (!overflow_the_stack) {
            cout << " 1| switched off - set `overflow_the_stack` to `true` to crash on purpose\n";
            return;
        }
        cout << " 2| never returns: " << go_deeper(0) << endl;
    }

}

/* --- Threads, and how to change it ---
 * Every thread has its own stack, and the other threads usually get less than the main thread (see future snippets).
 * The limit of the main thread can be raised in the shell - `ulimit -s 65536` for 64 MiB, then start the program from
 * there - or at link time on some platforms. Usually the better answer is: less on the stack. A large array belongs in
 * a `vector`, a deep recursion may be a loop.
 */

/* --- `main` --- */
int main() {
    measure_a_frame();
    ask_for_the_limit();
    overflow_on_purpose();

    return EXIT_SUCCESS;
}
