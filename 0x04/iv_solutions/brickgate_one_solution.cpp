// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Brickgate', see ../tasks.md.

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>

using std::cout, std::string, std::string_view, std::vector;

// The words of `text`, separated by one or more spaces - each word a view into `text`, nothing is copied.
vector<string_view> split(const string_view text) {
    vector<string_view> words{};
    size_t start{0};
    while (start < text.size()) {
        const size_t begin{text.find_first_not_of(' ', start)};
        if (begin == string_view::npos) {
            break;                          // only spaces left
        }
        size_t end{text.find(' ', begin)};
        if (end == string_view::npos) {
            end = text.size();
        }
        words.push_back(text.substr(begin, end - begin));
        start = end;
    }
    return words;
}

int main() {
    const string text{"So  What   Freddie Freeloader  Blue in Green"};
    const void* text_at{text.data()};
    cout << "text at " << text_at << ", size " << text.size() << '\n';

    const vector<string_view> words{split(text)};
    for (const string_view w : words) {
        const void* at{w.data()};
        cout << "'" << w << "' size " << w.size() << " at " << at << '\n';
    }
    cout << words.size() << " words (expected 7), all addresses between the text's first and last character\n";

    // Extension: two words per element - 16 bytes on a 64-bit platform, for any word length. 1000 words: 16000 bytes,
    // plus the text once. A `vector<string>` needs `sizeof(string)` (24 or 32) per word, plus a heap block for every
    // word longer than the SSO limit.
    cout << "sizeof(string_view)=" << sizeof(string_view) << ", sizeof(string)=" << sizeof(string) << '\n';

    // Extension: the temporary `string` dies at the `;` - the views point into released memory. Printing them would be
    // undefined behavior. A `const string&` parameter would not help: the temporary would live during the call, but
    // the returned views still outlive it.
    const vector<string_view> dangling{split(string{"just a temporary"})};
    cout << "dangling views: " << dangling.size() << " (the vector is fine, the characters are gone)\n";

    return EXIT_SUCCESS;
}
