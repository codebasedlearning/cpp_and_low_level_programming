[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x04 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Some tasks look into object files with `nm`. Build them in CLion and find the object files in `cmake-build-debug`, or
  compile them yourself in a terminal with `g++ -std=c++23 -c ...` - see the preparation.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Brickgate'

- Write a function `split` that takes a text as a `string_view` and returns its words - separated by spaces - as a
  `vector<string_view>`. Not a single character is copied.
- Test it with a `string` and print each word, its size and its address. Print the address of the text, too: show that
  every word lies inside the text.

Extension:

- What is `sizeof` of one element of the result? How many bytes does the vector need for the words of a text with 1000
  words, compared with a `vector<string>`?
- Call `split(string{"just a temporary"})` and keep the result in a variable. What do the views point to after the
  `;`? Do not print them - explain why not. Would a parameter of type `const string&` have prevented it?

<hr>

### 👉 Task 'Millbrook'

- Write three functions `smallest`, `largest` and `sum` for `int`s, each taking a `span<const int>`.
- Test them with an `array`, a `vector`, and a part of the vector, e.g. `subspan`.
- Write a function `clamp_all(span<int> values, int low, int high)` that limits every element to the interval
  `[low, high]`. Apply it only to the second half of a vector, and print the whole vector.

Extension:

- Predict `sizeof(span<const int>)` and `sizeof(span<const int, 5>)`, then check them.
- What happens if `smallest` gets an empty span? Make it throw a `std::invalid_argument` instead.

<hr>

### 👉 Task 'Oakberg'

- Define an `array<int, 5>` and a `vector<int>`, both with the values `2, 3, 5, 7, 11`, and do the following with both.
- Print all elements, once with a range-based `for`, once with an explicit iterator loop.
- Double every element in a range-based `for`.
- Search for the element 6 with `std::find`, and print it and the next two elements - if they exist.

Extension:

- Do the same with a `list<int>`. Which line does not compile any more, and how do you fix it?
- Print `&*it` for all elements of the `vector` and of the `list`. Explain the distances.
- Write the steps once, as a function template that works for all three containers.

<hr>

### 👉 Task 'Ashfield'

A catalog maps ISBN numbers to books.

- Write a `struct book` with an author and a title, and an `operator<<` that prints both.
- Define a type `catalog_t` as an `unordered_map` from a `string` (the ISBN) to a `book`, and create a `catalog`.
- Fill it with three or more real books, e.g. `"44245381X"` →
  `{"Walter Moers", "The 13 1/2 Lives of Captain Bluebear"}`.
- Print all books whose title is longer than 40 characters.

Extension:

- Look up an ISBN that is not in the catalog with `catalog[isbn]`, and print `catalog.size()` before and after. What
  happened? Look it up the right way, with `find` or `contains`.
- Replace `unordered_map` by `map`. What changes in the output, and why?

<hr>

### 👉 Task 'Sparrow Town'

- Turn the class `fraction` from 'Ravencastle' (unit 0x03) into a class template `fraction<T>`, for integer types `T`.
- Add an `operator<<` that prints `num/denom`.
- Test it with `int`, `short` and `long long`.

Extension:

- Predict `sizeof(fraction<short>)`, `sizeof(fraction<int>)` and `sizeof(fraction<long long>)`, then check them.
- Compile the file into an object file, and list its symbols with `nm -C`. Which member functions of `fraction<short>`
  are there, and which of `fraction<long long>`? Call `value()` only for one of the types - where does it appear?
- Try `fraction<double>`. Does it compile - and if not, which line does the error point to?

<hr>

### 👉 Task 'Fox Hollow'

Take your 'Ravencastle' project, split into `fraction.hpp`, `fraction.cpp` and `main.cpp` as in its extension.

- Compile each `.cpp` file into an object file with `-c`, and list the symbols with `nm -C`. For each member function
  of `fraction`: in which object file is it `T`, where `U` - and where does it not appear at all?
- Add a function template `twice` to the header, `template <typename T> T twice(const T& x) { return x + x; }`, and
  call it with an `int` in both `.cpp` files. Which letter does it get, in how many object files - and how many
  `twice<int>` are in the executable?
- Compile everything again with `-O2`. What is gone, and why?

Extension:

- Move the body of `twice` into `fraction.cpp`, keep only the declaration in the header. Read the linker error: why does
  `fraction.o` not contain `twice<int>`, although the body is there?
- Move the body of a member function from `fraction.cpp` into the class body. Which letter does it get now?
- Break the promise of `inline`: define `inline int scale(const int n) { return n * 10; }` directly in `fraction.cpp`,
  and the same function with `n * 1000` in `main.cpp`. Call it in both files and print the result. Predict, then run as
  Debug and as Release - and swap the two files in the `add_executable`. Does the compiler or the linker complain?

Our results: [answers](../docs/answers.md#check-0x04), the point on templates in headers.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "Should a function that only reads a text take a `std::string_view` or a
`const std::string&` - and what does each of them cost?", as an LLM might write them:

> **Answer A:** Take `string_view`. It is a pointer and a length - 16 bytes, passed by value in two registers - and it
> accepts literals, strings and parts of strings without a copy. A `const string&` is only one address, but a literal
> must first become a temporary `string`, which means an allocation for a longer text. A bonus: a `string_view` always
> ends with a `'\0'`, so you can hand `sv.data()` directly to C functions like `printf("%s", ...)`.

> **Answer B:** Both avoid copies when you pass a `string`, but only `string_view` avoids one for a literal. It is
> small, so pass it by value, not as `const string_view&`. And it is just as safe as `const string&`: bound to a
> temporary, it keeps the temporary alive, so `const string_view name{make_name()};` is fine, even if `make_name`
> returns a `string` by value.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them with a small program of your own: print `sv.data()` of a view of a part of a string, and print the address
  and size of a view of a temporary (not its content).
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x04).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files and folders there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x04](../docs/answers.md#check-0x04).

- I can explain what a `string_view` is in memory, and when it is cheaper than a `const string&`.
- I know why a `string_view` can dangle, and why `data()` of a view is not a C string.
- I can use `std::span` for functions that read or change elements of an array, a vector, or a part of them.
- I can explain what an iterator into a `vector` is, and why `++it` on a `list` iterator does something else.
- I know when iterators become invalid.
- I can write an `operator<<` for my class, and I know why it returns the stream.
- I know what `[[nodiscard]]` is for, and what it costs at runtime.
- I can write function and class templates, and I know that the compiler generates code per type - and only for what is
  used.
- I can specialize a template for a particular type, and I know why one would.
- I can read the output of `nm`: defined, undefined, local and weak symbols, and a mangled name.
- I can explain why templates and `inline` functions are defined in headers, and what the linker does with their copies.
- I can use `vector`, `list`, `set`, `map` and `unordered_map` - create, insert, find, erase - and I know how they
  store their elements.
