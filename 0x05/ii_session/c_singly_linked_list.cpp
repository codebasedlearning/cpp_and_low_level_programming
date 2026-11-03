// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - In an array, the next element is at `p + 1` - its address follows from its position. In a list, every element
 *   carries the address of the next one.
 * - Inserting into an array moves every element behind the gap - and a pointer into the array suddenly sees another
 *   value. Inserting into a list changes two addresses, and no element moves.
 * - `p->next` is `(*p).next`: follow the address, then take the member.
 * - Walking a list is the pointer loop over an array, with `p = p->next` instead of `++p`, and `nullptr` as the end.
 * - The nodes can be anywhere in memory: a list is held together by addresses, not by neighborhood.
 * - The price: one address per element, and no `p + i` - the i-th node is i steps away.
 */

#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `node` ---
     * One element of a singly linked list: the payload, and the address of the next node - `nullptr` in the last one.
     * A `node` contains a `node*`, although `node` is not complete at that point: the compiler knows the size of an
     * address without knowing what is at it. A member `node next;` would be a node in a node in a node ... - a compiler
     * error.
     * - !![#linked-list]
     */
    struct node {
        int payload;
        node* next;                         // a `node` alone would not work, only a pointer to it
    };

    /* --- `print_payloads` ---
     * The walk: start at the first node, follow `next` until there is none. Compare it with the pointer loop over an
     * array (see previous snippets): `p = p->next` instead of `++p`, `nullptr` instead of the end. The list does not
     * know its length - to count the nodes, you walk.
     */
    void print_payloads(const node* first) {
        for (const node* p{first}; p != nullptr; p = p->next) {
            cout << ' ' << p->payload;
        }
        cout << '\n';
    }

    /* --- `far_away` ---
     * A node that is not a local variable: it lives with the globals, far away from the stack.
     * - !![#memory-layout]
     */
    node* on_heap = new node{60, nullptr};
    node far_away{50, on_heap};

    /* --- `link_nodes` ---
     * Three local nodes, a node on the heap and a global one, linked by hand - built from the back, because each node
     * needs the address of its successor.
     * Look at the addresses: the local nodes are close together in the frame, in an order the
     * compiler chose; `far_away` is somewhere else entirely. The list does not care - it only follows `next`.
     * `p->next` is `(*p).next`. The parentheses are needed, since `.` binds stronger than `*`: `*p.next` would be
     * `*(p.next)`, and an address has no members. `->` saves them - you know it from iterators (see previous snippets).
     */
    void link_nodes() {
        print_function_header();

        node n40{40, &far_away};
        node n20{20, &n40};
        node n10{10, &n20};
        const node* head{&n10};

        for (const node* p{head}; p != nullptr; p = p->next) {
            cout << " 1|   p=" << p << ", payload=" << p->payload << ", next=" << p->next << '\n';
        }
        cout << " 2| head->next->payload=" << head->next->payload
             << ", (*(*head).next).payload=" << (*(*head).next).payload << '\n';
        cout << " 3| payloads:";
        print_payloads(head);
    }

    /* --- `insert_after` and `remove_after` ---
     * Two addresses change, nothing else. The order of the two lines in `insert_after` matters: first the new node
     * takes over the rest of the list, then it is hooked in. The other way round, `fresh->next` would be `fresh` itself
     * - a list without an end, and a walk without one.
     * `remove_after` unhooks a node. The node itself is still there - it is just no longer in the list.
     */
    void insert_after(node* pos, node* fresh) {
        fresh->next = pos->next;
        pos->next = fresh;
    }

    void remove_after(node* pos) {
        if (pos->next != nullptr) {
            pos->next = pos->next->next;
        }
    }

    /* --- `insert_and_remove_nodes` ---
     * The same insert as in the array, now in a list: 30 between 20 and 40. `to_40` still points to 40, and every
     * node keeps its address - nothing moved. Then 20 leaves the list, again by changing one address.
     */
    void insert_and_remove_nodes() {
        print_function_header();

        node n50{50, nullptr};
        node n40{40, &n50};
        node n20{20, &n40};
        node n10{10, &n20};
        const node* to_40{&n40};
        cout << " 1| payloads:";
        print_payloads(&n10);

        node n30{30, nullptr};
        insert_after(&n20, &n30);
        cout << " 2| payloads:";
        print_payloads(&n10);
        cout << " 3| to_40=" << to_40 << ", to_40->payload=" << to_40->payload << " - same address, same value\n";

        remove_after(&n10);
        cout << " 4| payloads:";
        print_payloads(&n10);
        cout << " 5| n20.payload=" << n20.payload << ", n20.next->payload=" << n20.next->payload << '\n';
    }

}

/* --- The price of a list ---
 * - No `p + i`: the i-th node is i steps away; the i-th element of an array is one address computation.
 * - One address per element, next to the payload.
 * - Every step is a load that has to wait for the one before - it brings the address of the next. An array is read
 *   in one stream, and the processor sees the next addresses coming; the nodes of a list can be anywhere.
 * In practice, a `vector` beats a list surprisingly often, even with inserts.
 * The standard library has both: `std::forward_list` (singly linked) and `std::list` (doubly linked - a node also
 * knows its predecessor).
 */

/* --- `main` --- */
int main() {
    link_nodes();
    insert_and_remove_nodes();

    return EXIT_SUCCESS;
}
