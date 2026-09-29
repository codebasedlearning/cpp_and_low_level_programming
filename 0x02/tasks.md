[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x02 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as in unit 0x01 - one file and one `add_executable` per task.
- From now on you may read input from the console. Where a task does not need input, `const` values in the code are
  fine.
- For time measurements, copy `utils/cbl/stopwatch.hpp` into your project, and measure as Release - see the preparation.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Baker River'

- Write a function `cut` that sets the decimal places of two `double` numbers, passed by reference, to 0. Use
  `std::floor` from `<cmath>`.
- Write a function `rotate` that rotates three strings passed by reference: the first gets the content of the second,
  the second that of the third, the third that of the first.
- Read the numbers and the strings from the console and print them before and after.

Extension:

- Print the addresses of your variables in `main` and of the parameters in `cut`. What do you expect, and what do you
  see?

<hr>

### 👉 Task 'Sanborn Cliff'

- Create a `std::array` of three `double`s, initialize the first two elements with 1.0 and 2.0, and store their sum in
  the third one. Print all elements.
- Copy the array into a second array with `=` and change the copy. Print both arrays.
- Print the addresses of the first element of both arrays. Are they two arrays or one?

Extension:

- Try the same with plain C arrays (`double a[3]`). Why does `=` not compile?

<hr>

### 👉 Task 'Elkford'

- Define a `constexpr` dimension `dim` with the value 3, and a `struct polynom` that holds the coefficients in a
  `std::array<double, dim>`. Initialize it e.g. with `polynom p{{1.0, 2.0, 3.0}}` - the inner braces belong to the
  array.
- Write a function `eval` that evaluates a polynomial at a position `x`. Pass the polynomial so that it is neither
  copied nor changed.
- Write meaningful test code.

Extension:

- Evaluate with Horner's method, `c0 + x*(c1 + x*c2)`. How many multiplications do you save?
- Look up how to implement `operator<<` for `polynom`, so that `cout << p` works.

<hr>

### 👉 Task 'Harshire'

- Learn how a stack works (last in, first out).
- Define a `struct stack` with a `std::array<int, 3>` for the data and a member `next` for the next free position.
- Write functions `push` and `pop` that add a value to the stack or take the top one off - only if there is space, or
  something to take.
- Write meaningful test code.

Extension:

- Throw a `std::runtime_error` when the stack is full or empty, and catch it in `main`.

<hr>

### 👉 Task 'Stretchford'

- Append 1000 elements to an empty `vector` with `push_back`. Every time the capacity changes, print the size and the
  new capacity.
- By which factor does the capacity grow? Compare with someone who uses another compiler.

Extension:

- When the capacity is full, all elements are copied. How many element copies does it take in total to fill the vector
  with 1000 elements? Compare with 1000.

<hr>

### 👉 Task 'Packwood'

- Define a `struct` with the members `bool active`, `double balance`, `int id`, `char grade` and `double limit`, in this
  order.
- Predict its `sizeof`, then check it. Where is the padding?
- Reorder the members to make the struct as small as possible.

Extension:

- How much memory does the reordering save for a `vector` with ten million elements?

<hr>

### 👉 Task 'Copley'

- Create a `vector` with one million strings, each at least 40 characters long - e.g. `string(40, 'x')`, 40 times
  `'x'`. Note the `()`: `string{40, 'x'}` would be a string of two characters.
- Measure how long it takes to sum up the sizes of all strings with `for (auto s : v)`, and with
  `for (const auto& s : v)`. Measure as Release.

Extension:

- Repeat it with strings of 10 characters. Why is the difference much smaller now? Hint: small string optimization.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "How big are `std::string` and `std::vector<int>`, and how many characters fit into a
`std::string` before it needs the heap?", as an LLM might write them:

> **Answer A:** `sizeof(std::string)` is 32 bytes on every 64-bit system: a pointer, the length, and a 16-byte buffer
> for short strings of up to 15 characters. `sizeof(std::vector<int>)` is 24 - three pointers.

> **Answer B:** It depends on the standard library. With libc++ (Apple clang) a `string` has 24 bytes and holds up to 22
> characters inline; with libstdc++ (gcc) it has 32 bytes and 15. A `vector<int>` has 24 bytes everywhere, and like a
> `string` it keeps a few elements inside the object before it uses the heap.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them with a small program of your own: `sizeof`, `std::string{}.capacity()`, and for a small `vector` compare
  `v.data()` with `&v`. Compare with someone who uses another compiler or operating system.
- If you have an LLM at hand, ask it the same question, and tell it your compiler and operating system. Is its answer
  closer to A or to B?

Our results: [answers](../docs/answers.md#ai-0x02).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x02](../docs/answers.md#check-0x02).

- I can explain what a reference is, and why it is not a copy.
- I can choose between passing by value, by reference and by `const&` - and justify it.
- I can say where the elements of a `std::array` and of a `std::vector` live.
- I can explain `size` and `capacity`, and why a reference into a `vector` can dangle.
- I can estimate the memory of a `struct`, including padding.
- I can measure the cost of a copy instead of guessing it.
- I know what `auto`, `auto&` and `const auto&` deduce, especially in a range-based `for`.
- I can throw, catch and rethrow standard exceptions, and I know why the order of the `catch` blocks matters.
- I can convert between text and numbers with `stoi`, `stod` and `to_string`, and handle what goes wrong.
