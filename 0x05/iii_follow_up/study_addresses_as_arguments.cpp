// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A pointer parameter gets an address, and the function writes its result there: an out-parameter.
 * - A pointer may be `nullptr` - "I do not need this one". A reference cannot say that.
 * - Returning a struct is often the better interface - and it costs no more.
 * - The calling convention decides where arguments and results go: in registers while they are small, in memory - via
 *   an address - when they are large.
 * - A small struct passed by value is as cheap as its members; a large one is copied by the caller first.
 * - Small types by value, large ones by `const&`: the question of unit 0x02, answered by the machine.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `split_seconds` ---
     * Two results, one `return` - so the caller passes two addresses, and the function writes to them. `int*` without
     * `const` says: this one will be written; a pure input would be a `const int*`. `seconds` may be `nullptr` if the
     * caller does not need it - the function checks before it writes.
     * - !![#parameter-passing]
     */
    void split_seconds(const int total, int* minutes, int* seconds) {
        *minutes = total / 60;
        if (seconds != nullptr) {
            *seconds = total % 60;
        }
    }

    /* --- `use_out_parameters` ---
     * The `&` at the call shows what may change - that is what C programmers like about out-parameters. You will meet
     * them in every C interface.
     */
    void use_out_parameters() {
        print_function_header();

        int minutes{0};
        int seconds{0};
        split_seconds(562, &minutes, &seconds);
        cout << " 1| 562 s = " << minutes << " min " << seconds << " s\n";

        split_seconds(589, &minutes, nullptr);
        cout << " 2| 589 s = " << minutes << " min and some seconds\n";
    }

    /* --- `duration` and `to_duration` --- The same two results, returned as one value. */
    struct duration {
        int minutes;
        int seconds;
    };

    duration to_duration(const int total) {
        return duration{total / 60, total % 60};
    }

    /* --- `return_a_struct` ---
     * No variables to prepare, no addresses to pass, nothing that can be null - and a structured binding takes the
     * result apart. The C++ Core Guidelines prefer this (F.20: "prefer return values to output parameters").
     * And the machine? `duration` has 8 bytes, and the result comes back in one register - see below.
     * - !![#structured-bindings]
     */
    void return_a_struct() {
        print_function_header();

        const auto [minutes, seconds]{to_duration(337)};
        cout << " 1| 337 s = " << minutes << " min " << seconds << " s, sizeof(duration)=" << sizeof(duration) << '\n';
    }

    /* --- `point` and `samples` --- A small struct, 16 bytes, and a large one, 64 bytes. */
    struct point {
        double x;
        double y;
    };

    struct samples {
        double values[8];
    };

    /* --- `length_squared`, `spread` and `spread_by_ref` --- Small by value, large by value, large by `const&`. */
    double length_squared(const point p) {
        return p.x * p.x + p.y * p.y;
    }

    double spread(const samples s) {
        cout << " a|   spread:        &s=" << &s << '\n';
        return s.values[7] - s.values[0];
    }

    double spread_by_ref(const samples& s) {
        cout << " b|   spread_by_ref: &s=" << &s << '\n';
        return s.values[7] - s.values[0];
    }

    /* --- `pass_small_and_large` ---
     * By value, `spread` gets a copy: another address, 64 bytes copied. By `const&`, it gets the caller's object - the
     * same address, nothing copied. So far, unit 0x02. Where exactly the arguments travel, only the machine code says.
     */
    void pass_small_and_large() {
        print_function_header();

        const point p{3.0, 4.0};
        const samples s{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}};
        cout << " 1| sizeof(point)=" << sizeof(point) << ", sizeof(samples)=" << sizeof(samples) << '\n';
        cout << " 2| &s=" << &s << '\n';
        cout << " 3| length_squared=" << length_squared(p) << '\n';
        const double by_value{spread(s)};
        const double by_ref{spread_by_ref(s)};
        cout << " 4| spread=" << by_value << ", spread_by_ref=" << by_ref << '\n';
    }

    /* --- `make_samples` --- A large result: 64 bytes. */
    samples make_samples(const double start) {
        return samples{{start, start + 1, start + 2, start + 3, start + 4, start + 5, start + 6, start + 7}};
    }

    /* --- `return_small_and_large` --- Two results, 8 and 64 bytes. */
    void return_small_and_large() {
        print_function_header();

        const duration d{to_duration(562)};
        const samples s{make_samples(10.0)};
        cout << " 1| d.minutes=" << d.minutes << ", s.values[7]=" << s.values[7] << '\n';

        /* -- .Q&A -- !![`split_seconds` or `to_duration` - which one is faster?](#a-505) */
    }

}

/* --- The calling convention ---
 * Where arguments and results travel is fixed by the calling convention of the platform - the rules every compiler on
 * it follows, so that functions compiled by different compilers can call each other. Paste `split_seconds`,
 * `duration`, `to_duration`, the two structs and the four functions below them into Compiler Explorer - without the
 * `cout` lines in `spread` and `spread_by_ref` - and compile with `-O1`.
 * x86-64, Linux and macOS (System V):
 * - Integers and addresses in `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`; floating point in `xmm0` ... `xmm7`. The result
 *   in `rax` or `xmm0`.
 * - `split_seconds`: `total` in `edi`, the addresses in `rsi` and `rdx` - and `test rdx, rdx` is the check for
 *   `nullptr`. The results go to memory: `mov DWORD PTR [rsi], eax`.
 * - `to_duration`: no memory at all. Both `int`s come back in `rax`, the seconds shifted into the upper 32 bits.
 * - `length_squared`: `x` in `xmm0`, `y` in `xmm1` - a struct of up to 16 bytes travels in registers.
 * - `spread`: the caller copies the 64 bytes onto the stack before the `call`, and the function reads them from there,
 *   `[rsp+8]` and `[rsp+64]`. `spread_by_ref`: the address in `rdi`, and nothing is copied.
 * - `make_samples`: the caller passes the address of the result in `rdi` - the hidden argument that builds the object
 *   in the caller's place (see previous snippets).
 * ARM64 (checked with gcc on Linux):
 * - Integers and addresses in `x0` ... `x7`, floating point in `d0` ... `d7`; the result in `x0` or `d0`.
 * - `point` in `d0` and `d1`, `duration` comes back in `x0`.
 * - `spread`: the caller copies the 64 bytes into its own frame and passes the address of the copy in `x0`. Inside,
 *   `spread` and `spread_by_ref` are the same instructions - the copy is the caller's work.
 * - `make_samples`: the address of the result in `x8`.
 * Windows x64 (choose "x64 msvc"): only four registers, `rcx`, `rdx`, `r8`, `r9`, and a struct of more than 8 bytes is
 * always passed via the address of a copy - also `point`.
 * - !![#calling-convention]
 */

/* --- By value or by `const&`? ---
 * Now the rule from unit 0x02 has a reason. A small, trivially copyable type - an `int`, a `point`, a `string_view`, a
 * `span` - travels in registers when passed by value: no memory, no address, and nothing to load. Passed by `const&`,
 * the caller must put it into memory and pass the address, and the function reads through it. For a large type it is
 * the other way round: by value means a copy, by `const&` only an address.
 * The border depends on the platform - 16 bytes on x86-64 Linux and macOS, 16 bytes or up to four `double`s on ARM64,
 * 8 bytes on Windows - and on whether copying is trivial: a `string` is always passed via an address, whatever its
 * size - it has a copy constructor of its own, and that needs the copy in memory.
 */

/* --- `main` --- */
int main() {
    use_out_parameters();
    return_a_struct();
    pass_small_and_large();
    return_small_and_large();

    return EXIT_SUCCESS;
}
