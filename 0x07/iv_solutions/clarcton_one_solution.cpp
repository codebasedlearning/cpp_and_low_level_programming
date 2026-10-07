// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Clarcton', see ../tasks.md.

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <optional>
#include <cstdlib>

using std::cout, std::ostream, std::ifstream, std::ofstream, std::string, std::map, std::optional;
namespace fs = std::filesystem;

// Removes spaces and tabs at both ends.
string trim(const string& text) {
    const size_t first{text.find_first_not_of(" \t")};
    if (first == string::npos) {
        return "";
    }
    const size_t last{text.find_last_not_of(" \t")};
    return text.substr(first, last - first + 1);
}

class config {
public:
    void set(const string& key, const string& value) { settings_[key] = value; }

    optional<string> get(const string& key) const {
        const auto it{settings_.find(key)};
        if (it == settings_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    // A `map` is sorted by its keys - the loop visits them in order.
    friend ostream& operator<<(ostream& os, const config& c) {
        for (const auto& [key, value] : c.settings_) {
            os << key << " = " << value << '\n';
        }
        return os;
    }

private:
    map<string, string> settings_;
};

config read_config(const fs::path& path) {
    config result;
    ifstream in{path};
    string line;
    while (getline(in, line)) {
        const string content{trim(line)};
        if (content.empty() || content.front() == '#') {
            continue;
        }
        const size_t equals{content.find('=')};
        if (equals == string::npos) {
            continue;                       // not a setting - skip it
        }
        result.set(trim(content.substr(0, equals)), trim(content.substr(equals + 1)));
    }
    return result;
}

void print_bytes(const fs::path& path) {
    ifstream in{path, std::ios::binary};
    char c{};
    cout << path.filename() << ':';
    while (in.get(c)) {
        cout << ' ' << std::hex << static_cast<int>(static_cast<unsigned char>(c)) << std::dec;
    }
    cout << '\n';
}

int main() {
    const fs::path path{fs::temp_directory_path() / "server.cfg"};
    {
        ofstream out{path};
        if (!out) {
            cout << "cannot write " << path << '\n';
            return EXIT_FAILURE;
        }
        out << "# server settings\n" << "host = example.org\n" << "port = 8080\n" << "\n" << "user = John\n";
    }

    const config settings{read_config(path)};
    cout << settings;
    if (const optional<string> port{settings.get("port")}) {
        const int number{std::stoi(*port)};
        cout << "port as an int: " << number << '\n';

        // Extension: the same number, as text and as bytes.
        const fs::path text_path{fs::temp_directory_path() / "port.txt"};
        const fs::path binary_path{fs::temp_directory_path() / "port.bin"};
        {
            ofstream text{text_path};
            text << number;
            ofstream binary{binary_path, std::ios::binary};
            binary.write(reinterpret_cast<const char*>(&number), sizeof(number));
        }
        // port.txt: 38 30 38 30 - the characters '8', '0', '8', '0'.
        // port.bin: 90 1f 0 0 - 8080 is 0x1f90, stored with the lowest byte first: little-endian.
        print_bytes(text_path);
        print_bytes(binary_path);
        fs::remove(text_path);
        fs::remove(binary_path);
    }
    fs::remove(path);

    return EXIT_SUCCESS;
}
