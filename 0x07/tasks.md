[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x07 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Some tasks look at the machine code or at what the compiler makes of your code: Compiler Explorer (godbolt.org) and
  C++ Insights (cppinsights.io) - see the preparation.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Heart Land'

A vector in space, with operators:

- Write a class `vec3` with three private `double`s and a constructor that takes them.
- Member functions: `+=` and `-=` for another `vec3`, `*=` for a `double`; `[]` in a `const` and a non-`const` version
  for the indexes 0, 1 and 2; `==` with `= default`.
- Free functions: `+`, `-`, the unary `-`, `*` with the `double` on either side, and `<<`, which prints `(x, y, z)`.
- Test them: `a + b`, `a - b`, `2.0 * a`, `a * 2.0`, `-a`, `a[1] = 5.0`, `a == b`, `a != b`.
- Predict `sizeof(vec3)`, then check.

Extension:

- In Compiler Explorer, x86-64 gcc, `-O2`: compare `vec3 add(const vec3& a, const vec3& b) { return a + b; }` with a
  function that adds the three pairs of `double`s by hand into a plain `struct` of three `double`s. Where does the
  result go - in a register, or in memory (unit 0x05)? Is there any difference between the two?
- Make it generic: a class template `vec<T, N>` with a `std::array<T, N>` inside. Try `vec<int, 2>` and
  `vec<double, 4>`.

<hr>

### 👉 Task 'Deerwoods'

A value that may be missing, with operators:

- Write a class template `nullable<T>` with a value of type `T` and a `bool` that says whether there is one. The
  default constructor creates a null, a constructor from a `T` a value.
- Make this work for a `nullable<int> n`: `n = 1;` sets a value, `n.reset();` sets it to null, `n += 3;` adds (null
  stays null), `n1 + n2` adds two of them (null if one is null), `if (n)` and `!n` test for a value,
  `static_cast<int>(n)` returns the value (and throws for null), `cout << n` prints the value or `null`.
- Decide for each operator: member or free function? `explicit` or not?
- Test all of it, including `n + 1` and `1 + n`.

Extension:

- If `1 + n` does not compile, why not - and what changes if `+` is a hidden `friend`, defined inside the class?
- Predict `sizeof(nullable<int>)`, `sizeof(nullable<double>)` and `sizeof(nullable<char>)`, then compare with
  `std::optional<T>` of the same types.

<hr>

### 👉 Task 'Dover Town'

A traffic light as an enum:

- Define `enum class light : std::uint8_t { red, red_yellow, green, yellow };`.
- Write a prefix `++` for `light` - a free function `light& operator++(light& l)` - that switches to the next phase:
  after `yellow` comes `red` again.
- Write an `operator<<` that prints the name of the phase, and a function `int seconds_of(light l)` with a `switch`:
  red 30, red-yellow 3, green 25, yellow 4.
- Run one full cycle from `red`: print every phase with its seconds, and the total.
- A crossing: `struct crossing { light north_south; light east_west; std::uint16_t remaining; };`. Predict
  `sizeof(crossing)`, then check. And without the `: std::uint8_t`?

Extension:

- Add a postfix `++`. What does `l++` return - and what is `l` afterwards?
- What do your `operator<<` and `seconds_of` do with `static_cast<light>(9)`? Make both robust - without a `default:`.
- Look at `seconds_of` in Compiler Explorer with `-O2`. A table?

<hr>

### 👉 Task 'Tin Harbor'

Predict what each line prints - on paper, before you run it.

```cpp
cout << static_cast<int>(7.99) << '\n';                                     //  1
cout << static_cast<int>(-7.99) << '\n';                                    //  2
cout << static_cast<unsigned>(-1) << '\n';                                  //  3
cout << static_cast<long long>(-1) << '\n';                                 //  4
cout << static_cast<int>(static_cast<std::uint8_t>(321)) << '\n';           //  5
cout << static_cast<int>(3'000'000'000LL) << '\n';                          //  6
cout << static_cast<long long>(static_cast<float>(16'777'217)) << '\n';     //  7
cout << 7 / 2 * 2.0 << '\n';                                                //  8
cout << 7 / 2.0 * 2 << '\n';                                                //  9
cout << (-1 < 1u) << '\n';                                                  // 10
cout << std::hex << std::bit_cast<std::uint32_t>(-0.0f) << std::dec << '\n';  // 11
```

- Run it and compare. Where were you wrong, and why?
- Build with `-Wall -Wextra -Wconversion`. Which lines get a warning?
- The compiler computes all of these lines at compile time. To see the instructions, write a function with a parameter
  for the conversions of lines 1, 3, 4, 6 and 11 - `int f(double d) { return static_cast<int>(d); }` and so on - and
  look at them in Compiler Explorer, `-O2`, x86-64 gcc and ARM64 gcc. Which ones are an instruction, which ones are
  nothing?

Extension:

- Write line 3 as a C-style cast, `(unsigned)-1`, and add `unsigned* p{(unsigned*)&x};` for an `int x`. Which named
  casts does C++ Insights show?

<hr>

### 👉 Task 'Ember Falls'

Tickets, numbered by the class:

- Write a class `ticket` with a private `int number_`. Two private static data members, declared in the class and
  defined outside it: `next_`, the number for the next ticket, starting at 1, and `live_`, the number of tickets that
  exist right now.
- The constructor takes the next number and counts `live_` up, the destructor counts it down. A ticket cannot be
  copied.
- `number()`, and two static member functions: `live()`, and `issued()` - how many numbers were given out so far.
- Test it: tickets in nested blocks, and one on the heap with a `std::unique_ptr`. Print `live()` and `issued()` after
  every step.
- Predict `sizeof(ticket)`, then check.

Extension:

- Split the class into `ticket.hpp`, `ticket.cpp` and `main.cpp` (unit 0x03). Where do the definitions of `next_` and
  `live_` go? Move them into the header, and include it into a second `.cpp` file: what does the linker say? Then make
  them `inline static`.
- `nm -C` on `ticket.o`: which letter do `next_` and `live_` get - and why not the same one?
- Add a function `const ticket& first_ticket()` that returns a reference to a `static` local `ticket`. When is that
  ticket created, and which number does it get?

<hr>

### 👉 Task 'Clarcton'

A configuration file:

- Write a file `server.cfg` into the temp directory (see the follow-up of this unit) with these lines:

```
# server settings
host = example.org
port = 8080

user = John
```

- Read it back line by line. Skip empty lines and comments, split every other line at the `=`, remove the spaces around
  the key and the value, and keep the pairs in a `std::map<std::string, std::string>`.
- Put the map into a class `config` with a function `std::optional<std::string> get(const std::string& key) const` and
  an `operator<<` that prints all settings, sorted by key.
- Read `port` as an `int`.

Extension:

- Write the port into two more files: once as text, once as the bytes of the `int` (`std::ios::binary`). Both files
  have 4 bytes. Read both back byte by byte and print the bytes in hex: what is in each file - and why?

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "Does a cast cost anything at run time?", as an LLM might write them:

> **Answer A:** No. A cast only tells the compiler how to look at a value - the bits stay where they are.
> `static_cast<int>(x)` for a `double`, `static_cast<long long>(n)` for an `int`, `reinterpret_cast` for a pointer:
> none of them generates an instruction. That is why the named casts are a zero-cost abstraction - all the work
> happens at compile time.

> **Answer B:** It depends on the cast. A conversion between number types can cost an instruction: a `double` must be
> brought into the format of an `int`. `reinterpret_cast` and `const_cast` cost nothing - they change only the type the
> compiler sees - and neither does `static_cast<unsigned>(n)` for an `int`, the bits stay the same. The cheapest way to
> get the bits of a `float` is `*reinterpret_cast<std::uint32_t*>(&f)`: one load, no conversion.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them in Compiler Explorer, `-O2`, x86-64 gcc and ARM64 gcc, with one function per conversion: `double` to
  `int`, `int` to `long long`, `int` to `unsigned`, a `reinterpret_cast` of a pointer, and `std::bit_cast` from a
  `float` to a `std::uint32_t`.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x07).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x07](../docs/answers.md#check-0x07).

- I can write `a + b`, `a += b` and `cout << a` as calls by name, and say which of them are member functions.
- I can decide whether an operator is a member or a free function, and write `+` with `+=`.
- I can say what is left of an operator call as Debug and as Release, and show it in Compiler Explorer.
- I know that precedence, associativity and the number of operands are fixed, and why an overloaded `&&` does not
  short-circuit.
- I can use `= default` for `==` and `<=>`, and say what the compiler makes of `!=` and `<`.
- I can write `[]` in two versions, `()`, prefix and postfix `++`, `<<` and `>>` for a class, and I know what `friend`
  grants.
- I can use C++ Insights to see the calls and conversions the compiler adds.
- I can name the named casts and `std::bit_cast`, say what each one is for, and which of them may cost an instruction.
- I can predict integer and floating-point conversions: sign extension, the same bits for `unsigned`, the lower bits
  for a narrower type, truncation toward zero - and the trap in `-1 < 1u`.
- I know why writing through a `const_cast` to a `const` object is undefined behavior, why reading a `float` through a
  `uint32_t*` is, too, and why a C-style cast is dangerous.
- I can say where a static data member lives, why it does not change `sizeof`, where it is defined, and how many there
  are for a class template.
- I can explain what a function-local `static` costs: constant or dynamic initialization, the guard, the destructor
  after `main`.
- I can say what an enum is in memory, choose its underlying type, and explain why every value of the underlying type
  is valid - and what that means for a `switch`.
- I can write operators for an `enum class`, e.g. to combine flags.
- I can read and write a text file, take lines apart with a string stream, and say how a binary file differs.
- I can set, clear, toggle and test bits with masks and shifts, take a field out of a packed number, and I know the
  traps of integer promotion and of shifts.
- I can explain two's complement, byte order and the three parts of a `float` - and why `0.1 + 0.2 != 0.3` - and say
  what a `std::variant` stores besides its value.
- I can format numbers and texts with `std::format` - width, precision, base - make my own type formattable, and say
  when the result needs the heap.
