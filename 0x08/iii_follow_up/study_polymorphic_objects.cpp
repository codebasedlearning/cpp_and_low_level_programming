// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Objects of different classes in one container: a `vector<unique_ptr<window>>` - one owner per object, one type for
 *   all elements. The base class has a virtual destructor.
 * - A copy through the base would slice. To copy a polymorphic object, ask it to copy itself: a virtual `clone` that
 *   returns a `unique_ptr<window>`.
 * - The rule for the destructor of a base class: public and virtual - or protected and not virtual, if nobody may
 *   delete through the base.
 * - `dynamic_cast` asks "are you a checkbox?". Now and then, that is fine. A chain of them is a `switch` over types -
 *   a virtual function belongs there.
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

    /* --- `window` ---
     * The base of all windows on a screen - the old example of a user interface library. `draw` and `clone` are pure
     * virtual; `show`, `hide` and `id` are the same for all windows, and not virtual. Copying a `window` directly is
     * allowed only for derived classes: the copy constructor is `protected`, so `clone` can use it, and nobody can
     * slice a button into a `window` by accident.
     * - !![#virtual-destructor]
     */
    class window {
    public:
        explicit window(const int id) : id_{id} {}
        virtual ~window() = default;
        window& operator=(const window&) = delete;

        virtual string draw() const = 0;
        virtual unique_ptr<window> clone() const = 0;

        void show() { visible_ = true; }
        void hide() { visible_ = false; }
        int id() const { return id_; }

    protected:
        window(const window&) = default;

        string frame(const string& content) const {
            return visible_ ? "#" + to_string(id_) + " " + content : "#" + to_string(id_) + " (hidden)";
        }

    private:
        int id_;
        bool visible_{true};
    };

    /* --- `button`, `checkbox` and `label` ---
     * Three windows. Each `clone` makes a copy of its own class - with its own copy constructor - and returns it as a
     * `unique_ptr<window>`.
     */
    class button : public window {
    public:
        button(const int id, const string& caption) : window{id}, caption_{caption} {}

        string draw() const override { return frame("[ " + caption_ + " ]"); }
        unique_ptr<window> clone() const override { return make_unique<button>(*this); }

    private:
        string caption_;
    };

    class checkbox : public window {
    public:
        checkbox(const int id, const string& caption) : window{id}, caption_{caption} {}

        string draw() const override { return frame((checked_ ? "[x] " : "[ ] ") + caption_); }
        unique_ptr<window> clone() const override { return make_unique<checkbox>(*this); }

        void toggle() { checked_ = !checked_; }

    private:
        string caption_;
        bool checked_{false};
    };

    class label : public window {
    public:
        label(const int id, const string& text) : window{id}, text_{text} {}

        string draw() const override { return frame(text_); }
        unique_ptr<window> clone() const override { return make_unique<label>(*this); }

    private:
        string text_;
    };

    /* --- `make_dialog` --- A dialog: a vector of windows, each owned by its `unique_ptr`. */
    vector<unique_ptr<window>> make_dialog() {
        vector<unique_ptr<window>> dialog;
        dialog.push_back(make_unique<label>(1, "Save changes?"));
        dialog.push_back(make_unique<checkbox>(2, "don't ask again"));
        dialog.push_back(make_unique<button>(3, "Yes"));
        dialog.push_back(make_unique<button>(4, "No"));
        return dialog;
    }

    /* --- `draw_all` --- Every window draws itself - one virtual call each. */
    void draw_all(const vector<unique_ptr<window>>& windows) {
        for (const unique_ptr<window>& w : windows) {
            cout << " a|   " << w->draw() << '\n';
        }
    }

    /* --- `own_windows` ---
     * The dialog owns its windows, and at its `}`, the `vector` destroys the `unique_ptr`s, which delete the windows -
     * through a `window*`, so `~window` must be virtual (see the session). `hide` is not virtual: it is the same for
     * every window.
     * - !![#unique-ptr]
     */
    void own_windows() {
        print_function_header();

        const vector<unique_ptr<window>> dialog{make_dialog()};
        dialog[1]->hide();
        draw_all(dialog);
    }

    /* --- `copy_a_dialog` ---
     * A `vector<unique_ptr<window>>` cannot be copied: a `unique_ptr` has one owner. And what would a copy of a
     * `window&` be - `window` does not know if it is a button. `clone` knows: each class copies itself, and the
     * virtual call picks the right class. The copy is independent: hiding a window in it changes nothing in the
     * original.
     */
    void copy_a_dialog() {
        print_function_header();

        const vector<unique_ptr<window>> original{make_dialog()};
        vector<unique_ptr<window>> copy;
        for (const unique_ptr<window>& w : original) {
            copy.push_back(w->clone());
        }
        copy[0]->hide();
        cout << " 1| original\n";
        draw_all(original);
        cout << " 2| copy\n";
        draw_all(copy);
        // const vector<unique_ptr<window>> wrong{original};  // compiler error: `unique_ptr` cannot be copied
        // const window sliced{*original[2]};  // compiler error: the copy constructor of `window` is protected
    }

    /* --- `listener` and `panel` ---
     * `listener` is a base for something else than deletion: a panel is a listener, but nobody should own a panel
     * through a `listener*`. So the destructor of `listener` is protected, and not virtual - no vptr needed for it.
     * `delete` through a `listener*` does not compile - the mistake of the session cannot happen.
     */
    class listener {
    public:
        virtual void notify(const string& event) = 0;

    protected:
        ~listener() = default;
    };

    class panel final : public listener {
    public:
        void notify(const string& event) override { last_ = event; }
        const string& last() const { return last_; }

    private:
        string last_;
    };

    /* --- `protect_the_destructor` --- Used through the base - but not deleted through it. */
    void protect_the_destructor() {
        print_function_header();

        const unique_ptr<panel> p{make_unique<panel>()};
        listener& l{*p};
        l.notify("closed");
        cout << " 1| p->last()=" << p->last() << '\n';
        // const unique_ptr<listener> owner{make_unique<panel>()};  // compiler error: `~listener` is protected
    }

    /* --- `toggle_all_checkboxes` ---
     * `dynamic_cast<checkbox*>` returns the address of the checkbox if `w` points to one, `nullptr` otherwise (see the
     * session). Here, it finds the one window that can be toggled.
     * Fine, now and then. But a function that asks "is it a checkbox? is it a button? is it a label?" is a `switch`
     * over the types - and has to be changed for every new class. A virtual function does the same without asking, and
     * a new class brings its own override.
     * - !![#rtti]
     */
    void toggle_all_checkboxes() {
        print_function_header();

        const vector<unique_ptr<window>> dialog{make_dialog()};
        for (const unique_ptr<window>& w : dialog) {
            if (checkbox* c{dynamic_cast<checkbox*>(w.get())}) {
                c->toggle();
                cout << " 1| toggled #" << c->id() << '\n';
            }
        }
        draw_all(dialog);

        /* -- .Q&A -- !![Why is `dynamic_cast` slower than a virtual call?](#a-815) */
    }

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
    own_windows();
    copy_a_dialog();
    protect_the_destructor();
    toggle_all_checkboxes();
    catch_own_exceptions();
    try_to_catch_by_value();

    return EXIT_SUCCESS;
}
