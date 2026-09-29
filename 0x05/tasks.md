[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x05 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Some tasks look at the machine code. Use Compiler Explorer (godbolt.org), or compile with
  `g++ -std=c++23 -S -O1 -masm=intel ...` in a terminal - see the preparation.
- Some tasks do undefined behavior on purpose. That is fine in a task - it never is in a program you hand in.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Copper View'

- Write a function `sum(const int* first, const int* last)` that adds up all `int`s from `first` up to, but not
  including, `last` - with a pointer loop, without an index.
- Write a function `reverse(int* first, int* last)` that reverses the elements in place: one pointer from the front,
  one from the back, swapping until they meet.
- Test both with a C array, a `std::array` and a `std::vector`. Where do you get `first` and `last` from in each case?
- Print the addresses of the first and the last element, and `last - first`.

Extension:

- Write a function `find_first(const int* first, const int* last, int value)` that returns a pointer to the first
  element equal to `value`, or `last` if there is none - like `std::find`.
- Turn `sum` into a function template with a type parameter `It` for both parameters, and call it with pointers, with
  `vector` iterators and with `list` iterators. Can you do the same with `reverse`? What does your version need from
  `It`, and what does a `list` iterator not have?

<hr>

### 👉 Task 'Kengate'

Returning two results through pointers:

- The function `divmod` computes the quotient and the remainder of an integer division `n / d`, and "returns" them
  through two pointers - out-parameters, as in the session. Its return value says whether the division was done, i.e.
  whether `d` was not 0.
- The signature is `bool divmod(int n, int d, int* quotient, int* remainder)`.
- For `d == 0`, it returns `false` and sets both results to 0.
- Each pointer may be `nullptr` if the caller does not need that result.
- Write meaningful test code, with negative numbers, too: what are `-7 / 2` and `-7 % 2` in C++?

Extension:

- Write a second version, `divide(int n, int d)`, that returns a `struct division` with `quotient`, `remainder` and a
  `bool ok`. Compare both versions in Compiler Explorer with `-O1`: where does the result of `divide` go?
- Make the members of `division` `long long`. What changes in the machine code - and at how many bytes does it change?

<hr>

### 👉 Task 'McAllen Spring'

A first improvised list:

- Write a class `node` with a private `int` member for the payload and a private pointer to the next `node`.
- A constructor sets both; the pointer is optional, `nullptr` by default.
- Add the member functions `payload()` and `next()`.
- In `main`, create four local `node`s, each with the address of the one created before it as its next node. Call the
  last one `head`.
- Starting at `head`, walk through the list with a `const node*`, and print each payload - until the pointer is
  `nullptr`.

Extension:

- Predict `sizeof(node)`, then check it. Print `this` and `next()` of each node: in which order are the nodes in
  memory, and how far apart?
- Write a function `int length(const node* first)` - once with a loop, once recursively.
- Write a function `make_node(int payload, node* next)` that creates a local `node` and returns its address, and put
  its node in front of `head`. What does the compiler say, and what happens when you walk the list? How a node can
  outlive the function that creates it: see the next unit.

<hr>

### 👉 Task 'Thornbury'

C strings by hand - without `<cstring>`, only with pointers:

- `length(const char* text)` returns the number of characters before the `'\0'`.
- `count(const char* text, char c)` returns how often `c` occurs in `text`.
- `copy_into(char* dest, size_t size, const char* src)` copies `src` into a buffer of `size` bytes: never more than
  `size - 1` characters, and always with a `'\0'` at the end. It returns whether the whole text fitted.
- Test `copy_into` with a `char buffer[8]` and three texts: one that fits, one of exactly 7 characters, and one that is
  too long.

Extension:

- Compare your functions with `strlen` and `strncpy` from `<cstring>`. Which one may leave the buffer without a
  `'\0'`?
- Create a `std::string` of ten million characters and measure, as Release, how long `strlen(s.c_str())` takes, and
  how long `s.size()`. Why the difference?

<hr>

### 👉 Task 'Glen Ferry'

Where do the arguments go? For each function below, predict where its arguments and its result travel on x86-64 Linux
or macOS - which register, or memory - then check in Compiler Explorer, x86-64 gcc, `-O1`.

```cpp
struct vec3 {
    double x;
    double y;
    double z;
};

struct pair_of_ints {
    int first;
    int second;
};

long long mix(const int a, const long long b, const char c) {
    return a + b + c;
}

double scale(const double x, const int factor) {
    return x * factor;
}

double length_by_address(const vec3* v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

double length_by_value(const vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

pair_of_ints swapped(const pair_of_ints p) {
    return pair_of_ints{p.second, p.first};
}

vec3 make_vec3(const double x) {
    return vec3{x, 2 * x, 3 * x};
}

int nine(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return a + b + c + d + e + f + g + h + i;
}
```

- Which of your predictions were wrong, and why?
- Switch the compiler to "ARM64 gcc", then to "x64 msvc". What is the same, what is different?

Extension:

- Which of the functions would be better off with a `const&` parameter - on which platform?
- `mix` adds a `char`. Compare the instruction that widens `c` on x86-64 and on ARM64. What does it tell you about
  `char` on each platform?

Our results: [answers](../docs/answers.md#check-0x05), the point on registers.

<hr>

### 👉 Task 'Don't do this, or I'll tell your mum'

Change local variables without naming them - through pointers into the stack frame. This is undefined behavior, and
that is the point: find out what your machine does with it.

- In a function, define three `int`s `a`, `b` and `c` with values you recognize, e.g. `0x11111111`, `0x22222222` and
  `0x33333333`. Print their addresses. Who is next to whom, and in which direction?
- Take the address of `b`, and write through `&b + 1` and through `&b - 1`. Print all three variables: which ones
  changed?
- Build and run it as Debug and as Release, with gcc and clang if you have both. Compare with your neighbors.

Extension:

- Put an `int` array of three elements between two variables, and write one element past its end. What happens? If the
  program ends with `*** stack smashing detected ***`: who detected it, and how?
- Build it with AddressSanitizer (`-fsanitize=address`, see the follow-up of this unit), if your platform has it.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "Is the name of a C array a pointer - and what does `sizeof` tell you about it?", as an LLM
might write them:

> **Answer A:** No. `int a[4]` is four `int`s, 16 bytes, and no pointer is stored anywhere - `sizeof(a)` is 16. But in
> almost every expression, `a` converts to a pointer to its first element: array decay. That is why `a` and `&a` print
> the same address - and since they are the same pointer, `a + 1` and `&a + 1` are the same address, too.

> **Answer B:** In practice, yes. `a` becomes `&a[0]` as soon as you use it, so `a[i]` is just `*(a + i)`, and a
> parameter `int a[4]` is really an `int*` - `sizeof(a)` in the function is 8. Only where the array is defined is
> `sizeof(a)` 16. A `std::array` is a thin wrapper around a C array and behaves the same way: pass one by value, and it
> decays to a pointer, too.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them with a small program of your own: print `a`, `&a`, `a + 1` and `&a + 1`, and `sizeof` of a C array
  parameter and of a `std::array<int, 4>` parameter passed by value.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x05).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x05](../docs/answers.md#check-0x05).

- I can compute with pointers - `p + n`, `p[n]`, `q - p` - and say how many bytes each step is.
- I know which addresses I may compute and which I may dereference, and what "one past the end" means.
- I can explain array decay: when it happens, what is lost, and how C, `std::array` and `std::span` deal with it.
- I can tell `a` from `&a` for an array, and predict `a + 1` and `&a + 1`.
- I can work with C strings, and I know why `strlen` walks, why `cout` prints a `char*` as text, and why `==` is the
  wrong comparison.
- I can pass a `std::string` to a C function and back, and I know what `extern "C"` does to a symbol.
- I can use pointers as out-parameters, with `nullptr` for "not needed" - and I know when a returned struct is the
  better interface.
- I can say where the arguments and the result of a function travel on x86-64 and on ARM64, and why small structs are
  cheap to pass by value.
- I can read the assembly of a small function as Debug and as Release: arguments, stack frame, calls, result.
- I can name the kinds of wild pointers, and explain why a program with one may seem to work.
- I know why the compiler may drop a null check that comes after the dereference.
- I can read `const` in a pointer declaration from right to left, and choose the right one for a parameter.
- I can use `strcpy`, `strncpy`, `memcpy` and `memset`, and I know what each of them trusts me with.
- I can use a pointer to a pointer, e.g. to move the caller's pointer or to read `argv`.
