// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A class that creates its nodes with `new` owns them: its destructor has to delete them.
 * - The generated copy copies member by member - the pointer, not what it points to. Two lists, one chain of nodes,
 *   and two destructors that delete the same nodes.
 * - The Rule of Three (see previous snippets): who writes a destructor, writes the copy constructor and the copy
 *   assignment as well - here, they copy the nodes. Or forbids the copies with `= delete`.
 * - The Rule of Five adds the moves; the Rule of Zero lets the members own.
 */

#include <iostream>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::endl;


/* ---- Content ---- */

namespace {

    /* --- `node` --- As in the list before (see previous snippets) - now always on the heap. */
    struct node {
        int payload;
        node* next;
    };

    /* --- `print_payloads` --- The walk, as before. */
    void print_payloads(const node* first) {
        for (const node* p{first}; p != nullptr; p = p->next) {
            cout << ' ' << p->payload;
        }
        cout << '\n';
    }

    /* --- `naive_list` ---
     * `push_front` creates a node with `new` and hooks it in front; the destructor walks the list and deletes every
     * node. In that walk, `next` is read before the `delete` - afterwards, there is no node to read it from.
     * No copy constructor, no copy assignment: the compiler generates them, and they copy `head_` - an address.
     * Generating them next to a destructor of your own has been deprecated since C++11 for exactly this reason - but
     * they are still generated, and without `-Wdeprecated` (clang) or `-Wdeprecated-copy-dtor` (gcc), nobody warns.
     */
    class naive_list {
    public:
        ~naive_list() {
            while (head_ != nullptr) {
                node* next{head_->next};
                delete head_;
                head_ = next;
            }
        }

        void push_front(const int payload) { head_ = new node{payload, head_}; }
        const node* head() const { return head_; }

    private:
        node* head_{nullptr};
    };

    /* --- `try_to_copy_a_naive_list` ---
     * `b` gets a copy of `a.head_`, and with it all of `a`'s nodes - two owners, one chain. At the `}`, `b` deletes the
     * nodes, then `a` deletes them again: a double `delete`, undefined behavior. With glibc, the program aborts with
     * `free(): double free detected` - if you are lucky.
     * Remove the `//`s, run it as Debug and as Release - and put the `//`s back. `endl` on purpose: when the program
     * is killed, what is still in the buffer is lost.
     * - !![#dangling-pointer]
     */
    void try_to_copy_a_naive_list() {
        print_function_header();

        naive_list a{};
        a.push_front(30);
        a.push_front(20);
        a.push_front(10);
        cout << " 1| a.head()=" << a.head() << endl;

        // const naive_list b{a};           // copies the address, not the nodes
        // cout << " 2| b.head()=" << b.head() << " - the same nodes" << endl;
    }                                       // with `b`: the nodes are deleted twice here

    /* --- `int_list` ---
     * The same list with the Rule of Three. The copy constructor creates a node of its own for every node of `other`,
     * and hooks it behind the last one, so the order stays. The copy assignment first gives back its own nodes, then
     * copies - but not for `a = a`: that would delete the nodes it is about to copy.
     * Declaring the copy constructor removes the generated default constructor - `= default` brings it back (see
     * previous snippets).
     * - !![#rule-of-zero]
     */
    class int_list {
    public:
        int_list() = default;

        int_list(const int_list& other) { copy_nodes_of(other); }

        int_list& operator=(const int_list& other) {
            if (this != &other) {
                delete_nodes();
                copy_nodes_of(other);
            }
            return *this;
        }

        ~int_list() {
            const int count{delete_nodes()};
            cout << " a|   ~int_list: " << count << " node(s) deleted\n";
        }

        void push_front(const int payload) { head_ = new node{payload, head_}; }
        const node* head() const { return head_; }

    private:
        int delete_nodes() {
            int count{0};
            while (head_ != nullptr) {
                node* next{head_->next};
                delete head_;
                head_ = next;
                ++count;
            }
            return count;
        }

        void copy_nodes_of(const int_list& other) {  // `head_` is `nullptr` here
            node* last{nullptr};
            for (const node* p{other.head_}; p != nullptr; p = p->next) {
                node* fresh{new node{p->payload, nullptr}};
                if (last == nullptr) {
                    head_ = fresh;
                } else {
                    last->next = fresh;
                }
                last = fresh;
            }
        }

        node* head_{nullptr};
    };

    /* --- `copy_a_list` ---
     * Every list owns nodes of its own now: other addresses, and a change to `a` does not reach `b` or `c`. At the `}`,
     * every destructor deletes its own nodes - each node exactly once, in the reverse order of construction: `c`, `b`,
     * `a`. The price is visible, too: a copy costs one `new` per node.
     */
    void copy_a_list() {
        print_function_header();

        int_list a{};
        a.push_front(30);
        a.push_front(20);
        a.push_front(10);
        const int_list b{a};                // copy constructor

        int_list c{};
        c.push_front(99);
        c = a;                              // copy assignment: 99 goes, 10 20 30 come

        a.push_front(5);
        cout << " 1| a.head()=" << a.head() << ", a:";
        print_payloads(a.head());
        cout << " 2| b.head()=" << b.head() << ", b:";
        print_payloads(b.head());
        cout << " 3| c.head()=" << c.head() << ", c:";
        print_payloads(c.head());
    }

}

/* --- Three, five, zero ---
 * - Three: destructor, copy constructor, copy assignment - who writes one, writes all three. Or forbids the copies:
 *   `int_list(const int_list&) = delete;` - then `b{a}` is a compiler error instead of a crash.
 * - Five: a copy costs one `new` per node. When the source is about to die anyway - a temporary, a list returned from
 *   a function - it would be enough to take over its `head_` and leave a `nullptr` behind. That is a move, and a move
 *   constructor and a move assignment make five: see future snippets.
 * - Zero: let the members own. A `unique_ptr` (see previous snippets) deletes by itself, and it cannot be copied - so
 *   the generated copy of a class with a `unique_ptr` member is not generated at all, and a copy does not compile.
 * The mantra of this unit, once more: an address is just a number - and copying a number copies no node.
 */

/* --- `main` --- */
int main() {
    try_to_copy_a_naive_list();
    copy_a_list();

    return EXIT_SUCCESS;
}
