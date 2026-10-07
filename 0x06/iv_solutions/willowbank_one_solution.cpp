// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Willow Bank', see ../tasks.md.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::shared_ptr, std::make_shared;
using std::weak_ptr;

// The first version: children and parent as `shared_ptr` - a cycle between every folder and its parent.
class shared_folder {
public:
    explicit shared_folder(const string& name) : name_{name} {}
    ~shared_folder() { cout << "  dtor " << name_ << '\n'; }

    string path() const { return parent_ != nullptr ? parent_->path() + "/" + name_ : name_; }

    void set_parent(const shared_ptr<shared_folder>& parent) { parent_ = parent; }
    void add_child(const shared_ptr<shared_folder>& child) { children_.push_back(child); }

private:
    string name_;
    vector<shared_ptr<shared_folder>> children_;
    shared_ptr<shared_folder> parent_;
};

shared_ptr<shared_folder> add(const shared_ptr<shared_folder>& parent, const string& name) {
    const shared_ptr<shared_folder> child{make_shared<shared_folder>(name)};
    child->set_parent(parent);
    parent->add_child(child);
    return child;
}

// The fix: the parent as `weak_ptr`. It does not keep the parent alive, and `lock()` gives a `shared_ptr` for the
// moment it is used.
class weak_folder {
public:
    explicit weak_folder(const string& name) : name_{name} {}
    ~weak_folder() { cout << "  dtor " << name_ << '\n'; }

    string path() const {
        const shared_ptr<weak_folder> parent{parent_.lock()};
        return parent != nullptr ? parent->path() + "/" + name_ : name_;
    }

    void set_parent(const shared_ptr<weak_folder>& parent) { parent_ = parent; }
    void add_child(const shared_ptr<weak_folder>& child) { children_.push_back(child); }

private:
    string name_;
    vector<shared_ptr<weak_folder>> children_;
    weak_ptr<weak_folder> parent_;
};

shared_ptr<weak_folder> add(const shared_ptr<weak_folder>& parent, const string& name) {
    const shared_ptr<weak_folder> child{make_shared<weak_folder>(name)};
    child->set_parent(parent);
    parent->add_child(child);
    return child;
}

// Extension: one owner per folder - its parent - and a raw pointer back. The raw pointer is safe: a child lives inside
// its parent's `vector`, so it dies before its parent does. Nothing is counted, nothing is atomic.
class unique_folder {
public:
    unique_folder(const string& name, const unique_folder* parent) : name_{name}, parent_{parent} {}
    ~unique_folder() { cout << "  dtor " << name_ << '\n'; }

    string path() const { return parent_ != nullptr ? parent_->path() + "/" + name_ : name_; }

    unique_folder& add(const string& name) {
        children_.push_back(make_unique<unique_folder>(name, this));
        return *children_.back();
    }

private:
    string name_;
    vector<unique_ptr<unique_folder>> children_;
    const unique_folder* parent_;
};

int main() {
    cout << "shared_ptr for the parent:\n";
    const heap_watch shared_heap{};
    {
        const shared_ptr<shared_folder> root{make_shared<shared_folder>("root")};
        const shared_ptr<shared_folder> docs{add(root, "docs")};
        add(root, "music");
        const shared_ptr<shared_folder> letters{add(docs, "letters")};
        cout << "  " << letters->path() << '\n';
    }
    cout << "  no dtor at all, live=" << shared_heap.live() << '\n';     // 4 folders, 2 vector blocks

    cout << "weak_ptr for the parent:\n";
    const heap_watch weak_heap{};
    {
        const shared_ptr<weak_folder> root{make_shared<weak_folder>("root")};
        const shared_ptr<weak_folder> docs{add(root, "docs")};
        add(root, "music");
        const shared_ptr<weak_folder> letters{add(docs, "letters")};
        cout << "  " << letters->path() << '\n';
    }
    cout << "  live=" << weak_heap.live() << '\n';

    cout << "unique_ptr for the children, a raw pointer for the parent:\n";
    const heap_watch unique_heap{};
    {
        unique_folder root{"root", nullptr};
        unique_folder& docs{root.add("docs")};
        root.add("music");
        const unique_folder& letters{docs.add("letters")};
        cout << "  allocations=" << unique_heap.allocations() << '\n';
        cout << "  " << letters.path() << '\n';
    }
    cout << "  live=" << unique_heap.live() << '\n';

    // Extension: `sizeof` with gcc's library on x86-64 Linux - shared_folder and weak_folder: 72 bytes (a `string`, a
    // `vector`, and 16 bytes for the parent); unique_folder: 64 (8 bytes for the parent). With clang's library, each is
    // 8 bytes less - its `string` has 24 bytes. The shared versions allocate one block per folder (object and control
    // block together, thanks to `make_shared`) plus the blocks of the `vector`s; the unique version one block per
    // folder but the root, which lives on the stack, plus the `vector`s: 3 + 3 = 6.
    cout << "sizeof: " << sizeof(shared_folder) << ", " << sizeof(weak_folder) << ", " << sizeof(unique_folder) << '\n';

    return EXIT_SUCCESS;
}
