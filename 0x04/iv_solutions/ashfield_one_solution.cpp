// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Ashfield', see ../tasks.md.

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <cstdlib>

using std::cout, std::ostream, std::string, std::unordered_map, std::map;

struct book {
    string author;
    string title;
};

ostream& operator<<(ostream& os, const book& b) {
    return os << b.author << ": " << b.title;
}

using catalog_t = unordered_map<string, book>;
// Extension: `using catalog_t = map<string, book>;` - the same code compiles, and the output is sorted by ISBN.

int main() {
    catalog_t catalog{};
    catalog["44245381X"] = {"Walter Moers", "The 13 1/2 Lives of Captain Bluebear"};
    catalog["0-00-000001-1"] = {"A. Author", "A Very Long Title for a Book That Nobody Has Written Yet"};
    catalog["0-00-000002-2"] = {"B. Writer", "Short"};

    cout << "titles longer than 40 characters:\n";
    for (const auto& [isbn, b] : catalog) {
        if (b.title.size() > 40) {
            cout << "  " << isbn << " - " << b << '\n';
        }
    }

    // Extension: `[]` inserts a missing key, with a default-constructed book - two empty strings.
    cout << "size before: " << catalog.size() << '\n';
    const book& missing{catalog["0-00-000003-3"]};
    cout << "size after:  " << catalog.size() << " - one more, author '" << missing.author << "'\n";
    catalog.erase("0-00-000003-3");

    // The right way: `find` or `contains` only look.
    if (const auto it{catalog.find("0-00-000004-4")}; it == catalog.end()) {
        cout << "not found, size still " << catalog.size() << '\n';
    }

    return EXIT_SUCCESS;
}
