// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Union Beach', see ../tasks.md.

#include <iostream>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::ostream, std::unique_ptr, std::make_unique, std::unordered_map, std::size_t;

// The interface for all square matrices. How the numbers are stored is the business of the derived classes.
class matrix {
public:
    explicit matrix(const size_t dim) : dim_{dim} {}
    virtual ~matrix() = default;

    size_t dim() const { return dim_; }

    virtual int get(size_t row, size_t col) const = 0;
    virtual void set(size_t row, size_t col, int value) = 0;
    virtual void make_zero() = 0;

    // Not virtual: the same for all matrices, built on the virtual functions.
    void make_identity() {
        make_zero();
        for (size_t i{0}; i < dim_; ++i) {
            set(i, i, 1);
        }
    }

private:
    size_t dim_;
};

// All coefficients in one block. `final`: see the extension.
class full_matrix final : public matrix {
public:
    explicit full_matrix(const size_t dim) : matrix{dim}, data_{make_unique<int[]>(dim * dim)} {}

    int get(const size_t row, const size_t col) const override { return data_[row * dim() + col]; }
    void set(const size_t row, const size_t col, const int value) override { data_[row * dim() + col] = value; }
    void make_zero() override { std::fill(data_.get(), data_.get() + dim() * dim(), 0); }

private:
    unique_ptr<int[]> data_;
};

// Only the coefficients that are not 0.
class sparse_matrix final : public matrix {
public:
    explicit sparse_matrix(const size_t dim) : matrix{dim} {}

    int get(const size_t row, const size_t col) const override {
        const auto it{data_.find(row * dim() + col)};
        return it == data_.end() ? 0 : it->second;
    }

    void set(const size_t row, const size_t col, const int value) override {
        if (value == 0) {
            data_.erase(row * dim() + col);
        } else {
            data_[row * dim() + col] = value;
        }
    }

    void make_zero() override { data_.clear(); }

private:
    unordered_map<size_t, int> data_;
};

ostream& operator<<(ostream& os, const matrix& m) {
    for (size_t row{0}; row < m.dim(); ++row) {
        os << (row == 0 ? "[" : " ");
        for (size_t col{0}; col < m.dim(); ++col) {
            os << ' ' << m.get(row, col);
        }
        os << (row + 1 == m.dim() ? " ]\n" : "\n");
    }
    return os;
}

// Extension: two matrices of any kind - through the interface - and the result as a full matrix.
full_matrix operator+(const matrix& a, const matrix& b) {
    if (a.dim() != b.dim()) {
        throw std::invalid_argument{"matrix dimensions differ"};
    }
    full_matrix sum{a.dim()};
    for (size_t row{0}; row < a.dim(); ++row) {
        for (size_t col{0}; col < a.dim(); ++col) {
            sum.set(row, col, a.get(row, col) + b.get(row, col));
        }
    }
    return sum;
}

// Extension: `full_matrix` is `final`, so a `const full_matrix&` refers to a `full_matrix` - `get` is called directly,
// and inlined. In Compiler Explorer, `-O2`: a loop that loads from `data_` - no vptr is read. Without `final`, clang
// calls through the table, gcc checks the slot first (speculative devirtualization).
int trace(const full_matrix& m) {
    int sum{0};
    for (size_t i{0}; i < m.dim(); ++i) {
        sum += m.get(i, i);
    }
    return sum;
}

int main() {
    full_matrix f{3};
    f.make_identity();
    f.set(0, 2, 5);
    cout << "f:\n" << f;

    sparse_matrix s{3};
    s.make_identity();
    s.set(1, 0, 7);
    cout << "s:\n" << s;
    cout << "f + s:\n" << f + s << "trace(f)=" << trace(f) << '\n';

    // gcc and clang, 64-bit: `matrix` 16 - the vptr and `dim_` -, `full_matrix` 24 - plus the `unique_ptr` -,
    // `sparse_matrix` 16 plus the `unordered_map`: 72 with libstdc++, where the map is 56 bytes - other libraries have
    // other sizes for it.
    cout << "sizeof: matrix " << sizeof(matrix) << ", full_matrix " << sizeof(full_matrix) << ", sparse_matrix "
         << sizeof(sparse_matrix) << '\n';

    // As Debug. The full matrix allocates its 100 x 100 `int`s once, in the constructor - 1 allocation, 40'000 bytes;
    // `make_identity` itself allocates nothing. The sparse matrix allocates a node per coefficient that is not 0, and
    // the buckets of the map whenever it grows - with libstdc++, 104 allocations, 4'224 bytes.
    heap_watch heap{};
    full_matrix big_full{100};
    big_full.make_identity();
    cout << "full 100 x 100:   " << heap.allocations() << " allocations, " << heap.bytes() << " bytes\n";
    heap.reset();
    sparse_matrix big_sparse{100};
    big_sparse.make_identity();
    cout << "sparse 100 x 100: " << heap.allocations() << " allocations, " << heap.bytes() << " bytes\n";

    // Extension: printing a 100 x 100 matrix through `const matrix&` makes 10'000 virtual calls of `get` - one per
    // coefficient, whatever the kind.

    return EXIT_SUCCESS;
}
