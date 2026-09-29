// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `[[nodiscard]]` is one of several attributes: notes to the compiler in double brackets. It works on a type, too.
 * - Most of them only change warnings: `[[maybe_unused]]`, `[[deprecated]]`, `[[fallthrough]]`.
 * - `[[likely]]` hints at the more frequent branch - the compiler may arrange the machine code for it.
 * - `[[no_unique_address]]` changes the layout: an empty member may take no bytes at all.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout;


/* ---- Content ---- */

namespace {

    /* --- `old_area` ---
     * Still works, but every use gets a warning with the reason. Two attributes in one pair of brackets: without
     * `maybe_unused`, the compiler would warn that nobody calls it.
     */
    [[deprecated("use area_of instead"), maybe_unused]]
    int old_area(const int w, const int h) {
        return w * h;
    }

    int area_of(const int w, const int h) {
        return w * h;
    }

    /* --- `silence_and_warn` ---
     * `[[maybe_unused]]`: a variable only needed in some builds, e.g. for an `assert` - no "unused variable" warning.
     * Remove the `//` in front of `old_area` and read the warning.
     */
    void silence_and_warn() {
        print_function_header();

        [[maybe_unused]] const int expected{12};
        cout << " 1| area_of(3, 4)=" << area_of(3, 4) << '\n';
        // cout << old_area(3, 4);          // warning: 'old_area' is deprecated: use area_of instead
    }

    /* --- `describe` ---
     * Without `break`, a `case` falls through into the next one - usually a bug, so compilers warn
     * (`-Wimplicit-fallthrough`, part of `-Wextra`). `[[fallthrough]];` says: this one is on purpose.
     */
    void describe(const int level) {
        switch (level) {
            case 2:
                cout << " loud and";
                [[fallthrough]];
            case 1:
                cout << " clear\n";
                break;
            default:
                cout << " silent\n";
        }
    }

    /* --- `fall_through` --- */
    void fall_through() {
        print_function_header();

        cout << " 1| level 2:"; describe(2);
        cout << " 2| level 1:"; describe(1);
        cout << " 3| level 0:"; describe(0);
    }

    /* --- `count_errors` ---
     * `[[likely]]` and `[[unlikely]]` (C++20) tell the optimizer which branch to make fast - e.g. by putting the rare
     * one out of the way. Nothing changes in the result. Compare the `-O2` machine code with and without them in
     * Compiler Explorer - if you find a difference.
     */
    int count_errors(const int value) {
        if (value >= 0) [[likely]] {
            return 0;
        } else [[unlikely]] {
            return 1;
        }
    }

    /* --- `hint_the_branch` --- */
    void hint_the_branch() {
        print_function_header();

        cout << " 1| errors: " << count_errors(5) + count_errors(-1) << '\n';
    }

    /* --- `handle` ---
     * `[[nodiscard]]` on a type: every function that returns a `handle` by value gets the warning - without having to
     * mark each of them.
     */
    struct [[nodiscard]] handle {
        int id;
    };

    handle open_channel(const int n) {
        return handle{n};
    }

    /* --- `keep_the_result` --- Remove the `//` and read the warning. */
    void keep_the_result() {
        print_function_header();

        const handle h{open_channel(7)};
        cout << " 1| channel " << h.id << '\n';
        // open_channel(8);                 // warning: ignoring returned value of type 'handle'
    }

    /* --- `no_policy` --- An empty class - no members at all. Still, `sizeof` is 1: every object needs an address. */
    struct no_policy {};

    /* --- `plain_holder` and `compact_holder` --- An `int` and an empty member - with and without the attribute. */
    struct plain_holder {
        int value;
        no_policy policy;
    };

    struct compact_holder {
        int value;
        [[no_unique_address]] no_policy policy;
    };

    /* --- `save_the_empty_byte` ---
     * `plain_holder` needs 1 byte for the empty member - plus 3 bytes padding: 8. With `[[no_unique_address]]`, the
     * empty member may share its address with another one: 4. Libraries use it for empty helper types, e.g. allocators.
     * MSVC accepts the attribute and ignores it; there, it is `[[msvc::no_unique_address]]`.
     */
    void save_the_empty_byte() {
        print_function_header();

        cout << " 1| sizeof(no_policy)=" << sizeof(no_policy) << ", sizeof(plain_holder)=" << sizeof(plain_holder)
             << ", sizeof(compact_holder)=" << sizeof(compact_holder) << '\n';
    }

}

/* --- `main` --- */
int main() {
    silence_and_warn();
    keep_the_result();
    fall_through();
    hint_the_branch();
    save_the_empty_byte();

    return EXIT_SUCCESS;
}
