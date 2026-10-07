[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x06 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Several tasks count allocations. Copy `utils/cbl/heap_watch.hpp` into your project and include it in exactly one
  `.cpp` file of each program - see the preparation. Count as Debug.
- Some tasks do undefined behavior on purpose. That is fine in a task - it never is in a program you hand in.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'South Birds Gale'

C strings from the heap:

- Write a function `char* reverse(const char* text)`: it allocates a block with `new char[]` - with room for the
  `'\0'` - writes the characters of `text` into it in reverse order, and returns its address. The caller owns the block.
- Test it with `"stressed"` and a few more texts, the empty one included, and delete every result.
- Check with a `heap_watch` that your test leaves no live block. Then forget one `delete[]` on purpose: what does
  `heap_watch` say?
- Write a second version, `make_reverse`, that returns a `std::unique_ptr<char[]>`. What changes for the caller?

Extension:

- Could `make_reverse` simply be `return unique_ptr<char[]>{reverse(text)};`? And what if you wrote `unique_ptr<char>`
  instead?
- If your platform has AddressSanitizer, build with it and forget the `delete[]` again: what does LeakSanitizer
  report? On macOS: `leaks --atExit -- ./your_program` - see the follow-up of this unit.

<hr>

### 👉 Task 'Rose Pond'

Addresses from the heap in a map:

- Write a struct `address` with two `std::string`s, `name` and `phone`.
- Create three addresses with `new`, and store the pointers in an `std::unordered_map<int, address*>` under the keys 1,
  2 and 3.
- Print all entries, then delete all addresses.
- Put everything into a block and watch it with a `heap_watch` created before the block. After the block, `live()`
  must be 0.

Extension:

- Predict how many allocations the block makes - only three? Then count them. Where do the others come from?
- Change the map to `std::unordered_map<int, std::unique_ptr<address>>`. Which lines can go? What happens now when you
  `erase` an entry, or call `clear()`? Can you still copy the map?

<hr>

### 👉 Task 'McAllen Spring Smart'

The list of 'McAllen Spring' (unit 0x05), now on the heap - where a node can outlive the function that creates it:

- Write a class `node` with a private `int` payload and a private `std::unique_ptr<node> next_`: each node owns the
  next one. A constructor takes both, the `unique_ptr` by value; `payload()` and `next()` - the latter returns a
  `const node*`.
- Write a function `push_front(std::unique_ptr<node>& head, int payload)` that puts a new node in front of `head`.
- Build a list with four nodes, walk through it with a `const node*` and print the payloads.
- Print a line in the destructor of `node`. In which order are the nodes destroyed when `head` dies - and who calls
  whose destructor?

Extension:

- Build a list with a million nodes - without the line in the destructor - and let `head` die. What happens, as Debug
  and as Release? Why?
- Fix it: write a function `clear(std::unique_ptr<node>& head)` that deletes the list with a loop, one node after the
  other. Hint: take the next node away from the head before the head dies.

<hr>

### 👉 Task 'Harrow Gate'

A class `text` that owns its characters - a tiny `std::string`:

- Two private members: `char* chars_`, a C string on the heap, and `size_t size_`.
- A constructor from a `const char*` copies the characters, with the `'\0'`, into a new block.
- `size()` and `c_str()`.
- The Rule of Five: destructor, copy constructor, copy assignment, move constructor, move assignment. The moves are
  `noexcept`.
- Before you run it: predict how many allocations each of these lines makes, then check with `heap_watch`.

```cpp
text a{"Blue in Green"};
text b{a};
text c{std::move(a)};
b = c;
c = text{"So What"};
vector<text> v;
v.push_back(b);
v.push_back(std::move(c));
```

Extension:

- Remove the `noexcept` from the move constructor, `push_back` five `text`s into a `vector` without `reserve`, and
  count again. Why more?
- Replace `char*` by `std::unique_ptr<char[]>`. Which of the five special members can you drop - and which do you
  still have to write, and why?

<hr>

### 👉 Task 'Lantern Hill'

Predict, line by line, what this `main` prints - on paper, before you run it. `tracer` prints `ctor x` in its
constructor and `dtor x` in its destructor, and has a `name()`.

```cpp
unique_ptr<tracer> make(const string& name) {
    return make_unique<tracer>(name);
}

void take(const unique_ptr<tracer> t) {
    cout << "take " << t->name() << '\n';
}

int main() {
    unique_ptr<tracer> a{make("a")};
    unique_ptr<tracer> b{std::move(a)};
    cout << "a is " << (a ? "full" : "empty") << '\n';
    take(std::move(b));
    cout << "after take\n";
    shared_ptr<tracer> s{make_shared<tracer>("s")};
    {
        const shared_ptr<tracer> t{s};
        cout << "count " << s.use_count() << '\n';
        s.reset();
        cout << "count " << t.use_count() << '\n';
    }
    cout << "end of main\n";
}
```

- Run it and compare. Where were you wrong, and why?
- Predict how many allocations the whole `main` makes, then check with a `heap_watch`.

Extension:

- Replace `make_shared<tracer>("s")` by `shared_ptr<tracer>{new tracer{"s"}}`. What changes in the output, and what in
  the count?
- Does `shared_ptr<tracer> d{make("d")};` compile? How many allocations does it make?

<hr>

### 👉 Task 'Willow Bank'

A tree of folders, where every folder knows its parent:

- Write a class `folder` with a name, a `std::vector<std::shared_ptr<folder>>` for the children and a
  `std::shared_ptr<folder>` for the parent.
- Write a function `add(const std::shared_ptr<folder>& parent, const std::string& name)` that creates a child, sets its
  parent, adds it to the parent's children, and returns it.
- Build a small tree - `root`, two children `docs` and `music`, and `letters` in `docs` - and print the path of
  `letters` by walking up the parents: `root/docs/letters`.
- Print a line in the destructor. What is destroyed at the end? What does `heap_watch` say?
- Fix it with a `std::weak_ptr` for the parent.

Extension:

- A third version: the children as `std::vector<std::unique_ptr<folder>>`, the parent as a raw `folder*`. Why is a
  raw pointer fine here - who guarantees that the parent outlives its children?
- Compare `sizeof(folder)` and the number of allocations of the three versions.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "Is `std::unique_ptr` free - or does it cost something compared to a raw pointer?", as an
LLM might write them:

> **Answer A:** It is free - the textbook zero-cost abstraction. `sizeof(unique_ptr<int>)` is 8, the size of a raw
> pointer, and with optimization the compiler generates the same `new` and the same `delete`; the destructor is
> inlined. It can even be passed by value at no cost: it is 8 bytes, so it travels in a register, just like an `int*`.

> **Answer B:** Almost. In memory it is free: 8 bytes, no reference count - unlike `shared_ptr`, which has two
> pointers and counts atomically. Its code is what you would write by hand, plus the cleanup for exceptions, which you
> should have written anyway. The one cost is at function borders: a class with a destructor is passed via an address,
> not in a register. So pass it as `const unique_ptr<T>&` - then nothing is copied, and the function can use the object
> at the cost of a raw pointer.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them: `sizeof`, and in Compiler Explorer, x86-64 gcc, `-O2`, four functions that return `*p` - for a
  `const int*`, a `unique_ptr<int>` by value, a `const unique_ptr<int>&` and a `const int&`.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x06).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x06](../docs/answers.md#check-0x06).

- I can explain the two steps of `new` and of `delete`, and why `delete[]` must match `new[]`.
- I can say what a heap block costs, in memory and in time, compared to a local variable.
- I can count allocations with `heap_watch`, and I know why it counts as Debug.
- I can name the ways a raw owner goes wrong - a leak, a leak on the exception path, a double `delete`, a use after
  `delete` - and what glibc and the optimizer make of them.
- I know why the generated copy of a class with an owning raw pointer is wrong, and how the Rule of Three fixes it.
- I can use `unique_ptr`: `make_unique`, `get`, `reset`, `release`, `std::move`, and `unique_ptr<T[]>`.
- I can say what a `unique_ptr` costs: its size, its machine code, and how it is passed to a function.
- I can choose how to hand an object to a function: `T&`, `T*`, `unique_ptr<T>` or `unique_ptr<T>&` - and say what
  each one promises.
- I can explain move semantics: what a move constructor does, what `&&` binds to, what `std::move` is, and what is left
  of a moved-from object.
- I can write the Rule of Five for a class that owns a block, and I know why its moves should be `noexcept`.
- I know when the compiler generates the moves - and when it silently copies instead.
- I can use `shared_ptr` and `weak_ptr`, and I know what the control block holds, why `make_shared` is one allocation,
  and why the count is atomic.
- I can recognize a `shared_ptr` cycle and break it with a `weak_ptr`.
- I can allocate an array whose size is known only at run time, and build a 2D array as one block or as one block per
  row.
