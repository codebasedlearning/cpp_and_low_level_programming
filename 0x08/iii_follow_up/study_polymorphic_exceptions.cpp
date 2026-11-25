// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Exceptions are polymorphic objects, too: an own exception class derives from `std::runtime_error`, and a `catch`
 *   takes it by reference - by value, it slices.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::to_string, std::vector, std::unique_ptr, std::make_unique;
using std::exception, std::runtime_error;


/* ---- Content ---- */

namespace {

    /* --- `stack_full` ---
     * An exception class of its own: it derives from `runtime_error`, passes the message to the base - which keeps it
     * and returns it from the virtual `what()` - and adds what a handler may want to know, here the capacity.
     * `runtime_error` for conditions found only while running, `logic_error` for bugs the caller could have avoided.
     * - !![#std-exception]
     */
    class stack_full : public runtime_error {
    public:
        explicit stack_full(const int capacity)
            : runtime_error{"stack full, capacity " + to_string(capacity)}, capacity_{capacity} {}

        int capacity() const { return capacity_; }

    private:
        int capacity_;
    };

    /* --- `parse_error` ---
     * Directly from `exception`, with an override of `what()`: it must be `const` and `noexcept`, and the text it
     * returns must live as long as the exception - hence the `string` member.
     */
    class parse_error : public exception {
    public:
        explicit parse_error(const string& message) : message_{message} {}

        const char* what() const noexcept override { return message_.c_str(); }

    private:
        string message_;
    };

    /* --- `catch_own_exceptions` ---
     * `catch` blocks are tried from top to bottom, and a handler for a base class also catches every derived class -
     * so the most derived one comes first. `catch (const runtime_error& e)` would catch `stack_full`, too, but it could
     * not ask for `capacity()`.
     * How does `catch` know the type? `throw` puts the object into memory of the runtime (`__cxa_allocate_exception` in
     * `nm`, gcc and clang), together with its type information - the same `typeinfo` that `dynamic_cast` uses.
     */
    void catch_own_exceptions() {
        print_function_header();

        try {
            throw stack_full{3};
        } catch (const stack_full& e) {
            cout << " 1| " << e.what() << ", capacity() is " << e.capacity() << '\n';
        } catch (const runtime_error& e) {
            cout << " 1| never printed: " << e.what() << '\n';
        }
    }

    /* --- `try_to_catch_by_value` ---
     * `catch (exception e)` copies the thrown `parse_error` into an `exception` - slicing: the copy gets the vptr of
     * `exception`, so `what()` calls `exception::what()`, and the message is gone. gcc warns (`-Wcatch-value`, part of
     * `-Wall`) - switched off here, on purpose; clang does not warn. By reference, `what()` reaches the override.
     */
    void try_to_catch_by_value() {
        print_function_header();

#pragma GCC diagnostic push
#if !defined(__clang__)                     // clang has no such warning - and warns about the unknown name
#pragma GCC diagnostic ignored "-Wcatch-value"
#endif
        try {
            throw parse_error{"line 3: '=' expected"};
        } catch (exception e) {
            cout << " 1| by value:     " << e.what() << '\n';
        }
#pragma GCC diagnostic pop

        try {
            throw parse_error{"line 3: '=' expected"};
        } catch (const exception& e) {
            cout << " 2| by reference: " << e.what() << '\n';
        }
    }

}

/* --- `main` --- */
int main() {
    catch_own_exceptions();
    try_to_catch_by_value();

    return EXIT_SUCCESS;
}
