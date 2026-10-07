// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Rose Pond', see ../tasks.md.

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::string, std::unique_ptr, std::make_unique, std::unordered_map;

struct address {
    string name;
    string phone;
};

int main() {
    const heap_watch heap{};
    {
        unordered_map<int, address*> book;
        book[1] = new address{"Max", "112"};
        book[2] = new address{"Mini", "110"};
        book[3] = new address{"Jane", "911"};
        for (const auto& [id, entry] : book) {
            cout << id << ": " << entry->name << ", " << entry->phone << '\n';
        }
        for (const auto& [id, entry] : book) {
            delete entry;
        }
        book.clear();                       // the pointers in the map dangle now - remove them

        // Extension: 3 addresses, 3 nodes of the map (the key, the pointer and the link to the next node) and the array
        // of buckets - 7 allocations with gcc's library, 8 with clang's. The short strings need none (SSO).
        cout << "allocations in the block: " << heap.allocations() << '\n';
    }
    cout << "raw pointers: live=" << heap.live() << '\n';

    // Extension: the map owns the addresses. No `delete` loop, and `erase` or `clear` destroys the address together
    // with its node. The map cannot be copied any more - a `unique_ptr` cannot.
    {
        unordered_map<int, unique_ptr<address>> book;
        book.emplace(1, make_unique<address>(address{"Max", "112"}));
        book.emplace(2, make_unique<address>(address{"Mini", "110"}));
        book.emplace(3, make_unique<address>(address{"Jane", "911"}));
        book.erase(2);
        for (const auto& [id, entry] : book) {
            cout << id << ": " << entry->name << ", " << entry->phone << '\n';
        }
        // const unordered_map<int, unique_ptr<address>> copy{book};  // compiler error
    }
    cout << "unique_ptr: live=" << heap.live() << '\n';

    return EXIT_SUCCESS;
}
