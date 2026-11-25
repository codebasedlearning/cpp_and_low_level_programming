[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x08 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Some tasks look at the layout of a class, the machine code or the object files: the layout dump of the preparation,
  Compiler Explorer (godbolt.org) and `nm`.
- Measure as Release, count allocations as Debug - copy `stopwatch.hpp` and `heap_watch.hpp` from `utils/cbl` into
  your project where a task needs them.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Kestrel Point'

Shapes behind a base class:

- Write an abstract class `shape` with the pure virtual functions `area()`, `perimeter()` and `name()`, and a virtual
  destructor. Add a non-virtual function `describe()` that returns a text with the name, the area and the perimeter -
  built on the three virtual functions.
- Derive `circle` (a radius), `rectangle` (width and height) and `triangle` (three sides; the area by Heron's formula).
- Fill a `std::vector<std::unique_ptr<shape>>` with a few shapes of each kind. Print `describe()` for all of them, the
  total area, and the shape with the largest perimeter.
- Predict `sizeof(shape)`, `sizeof(circle)`, `sizeof(rectangle)` and `sizeof(triangle)`, then check.

Extension:

- Remove `virtual` from the destructor of `shape` - What is wrong here? 
- Make `triangle` `final`. Can you still derive from `rectangle`? Should you be able to derive a `square` from it?

<hr>

### 👉 Task 'Cedar Gap'

Predict what this program prints - every line, on paper, before you run it.

```cpp
struct base {
    base() { cout << "base() " << who() << '\n'; }
    virtual ~base() { cout << "~base() " << who() << '\n'; }
    void f() const { cout << "base::f\n"; }
    virtual void g() const { cout << "base::g\n"; }
    virtual const char* who() const { return "base"; }
};

struct derived : base {
    derived() { cout << "derived() " << who() << '\n'; }
    ~derived() override { cout << "~derived() " << who() << '\n'; }
    void f() const { cout << "derived::f\n"; }
    void g() const override { cout << "derived::g\n"; }
    const char* who() const override { return "derived"; }
};

void by_reference(const base& b) {
    b.f();
    b.g();
}

void by_value(const base b) {
    b.g();
}

int main() {
    cout << "1\n";
    const derived d;
    cout << "2\n";
    const base& r{d};
    r.f();
    r.g();
    cout << "3\n";
    by_reference(d);
    cout << "4\n";
    by_value(d);
    cout << "5\n";
    const base* p{&d};
    p->base::g();
    cout << "6\n";
    const base copy{d};
    copy.g();
    cout << "7\n";
}
```

- Run it and compare. Where were you wrong, and why?

Extension:

- Predict `sizeof(base)` and `sizeof(derived)`, then check. Add an `int` to `derived`: which one changes, and by how
  much?

<hr>

### 👉 Task 'Union Beach'

An abstract matrix, with two ways to store it:

- Write an abstract class `matrix` for a square matrix of `int`s. The constructor takes the dimension and keeps it.
  Pure virtual functions: `get(row, col)`, `set(row, col, value)` and `make_zero()`.
- A non-virtual function `make_identity()`: it calls `make_zero()`, then sets the diagonal to 1 - without knowing how
  the numbers are stored.
- Derive `full_matrix`: it keeps all coefficients in one block, a `std::unique_ptr<int[]>`.
- Derive `sparse_matrix`: it keeps only the coefficients that are not 0, in a `std::unordered_map`, with the key
  `row * dim + col`. A coefficient that is not in the map is 0; setting a coefficient to 0 removes it.
- Write an `operator<<` for a `const matrix&`, and test both classes.
- Predict `sizeof(matrix)`, `sizeof(full_matrix)` and `sizeof(sparse_matrix)`, then check. Count the allocations of
  `make_identity()` for a 100 x 100 matrix of each kind with `heap_watch`, as Debug.

Extension:

- Write `operator+` for two matrices of any kind, and return a `full_matrix`.

<hr>

### 👉 Task 'Peters Mines'

Multiple inheritance, with a shared base:

- Define a class `vehicle_base` with the number of seats, and a class `vehicle` with a maximum speed.
- Derive `car` and `boat`, each from both `vehicle_base` and `vehicle`. `vehicle_base` is inherited virtually.
- Derive `amphibian` from `car` and `boat`. An amphibian has one number of seats, but two maximum speeds - on the road
  and on the water. Pass everything to the constructors: `amphibian a{4, 120, 15};`.
- Print the seats and both speeds.
- Predict `sizeof` of all five classes, then check. Print the addresses of all parts of an amphibian - its `car` part,
  its `boat` part, both `vehicle` parts and the `vehicle_base` - and draw its layout. Who constructs the
  `vehicle_base`?

Extension:

- Remove `virtual`. What does the compiler say about the seats of an amphibian? How many are there now, and what is
  `sizeof(amphibian)`?
- Add a member to `car` - the number of wheels - and one to `boat` - its draught (depth of the vessel below the 
  waterline) - and pass them to the constructors.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "What does `virtual` cost?", as an LLM might write them:

> **Answer A:** A virtual function makes every object of the class larger by one pointer - 8 bytes on a 64-bit
> system - however many virtual functions the class has; the table of function addresses exists once per class. A call
> loads the pointer, loads the address of the function from the table, and jumps there. In most loops, the processor
> predicts that jump well, so the real price is that the compiler cannot inline the call. A class with an `int` and one
> virtual function is therefore 12 bytes.

> **Answer B:** A virtual call is two loads and an indirect jump. When the compiler knows the exact type - a local
> object, or a class marked `final` - it calls the function directly and may inline it. The destructor of a base class
> should be `virtual`, so that `delete` through a base pointer destroys the whole object. And since the vptr is set
> before any constructor runs, a virtual call in the constructor of the base already reaches the override of the
> derived class - as in Java.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them: `sizeof` and a virtual call in a constructor.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x08).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x08](../docs/answers.md#check-0x08).

- I know that a non-virtual function is chosen by the static type, at compile time, and that a function of the same
  name in a derived class hides all functions of that name in the base.
- I can say what `virtual` adds to an object and to a class - and why the number of virtual functions does not change
  `sizeof`.
- I can explain who writes the vptr and when.
- I know why a base class needs a virtual destructor - or a protected one - what `delete` calls through the table, and
  what happens without it.
- I can explain slicing, and why the sliced copy behaves like a base object.
- I can use `typeid` and `dynamic_cast`, say where they find the type and what they cost, and when a virtual function is
  the better choice.
- I can explain the diamond, what `virtual` inheritance changes, and what it costs.
- I can use `override`, `final` and `= 0`, write an interface, and copy polymorphic objects with `clone`.
- I can derive my own exception class from `std::runtime_error`, order the `catch` blocks, and say why an exception is
  caught by reference.
- I know the traps: a missing `const` without `override`, hidden overloads, default arguments in an override.
- I can explain `public`, `protected` and `private` inheritance, and when a class should derive - and when it should
  have a member instead.
