[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x0a - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task. For threads, add
  `find_package(Threads REQUIRED)` and `link_libraries(Threads::Threads)` before the `add_executable`s, as in this
  unit's `CMakeLists.txt`.
- Programs with threads give different results in different runs. Run everything several times, as Debug and as
  Release - and do not trust a run that looks right.
- Measure as Release, with `stopwatch` - copy `stopwatch.hpp` from `utils/cbl` into your project where a task needs it.
  Your numbers depend on the number of cores: print `std::thread::hardware_concurrency()`.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Westwheat'

Two threads, one `vector`:

- Start two threads. Each appends `n` numbers to one global `std::vector<int>` with `push_back`, without any protection.
- Start with a small `n`, say 3. Does it work? Then increase `n` - 1,000, 100,000 - until the program loses elements,
  crashes, or hangs. Run each `n` several times, as Debug and as Release.
- What happens in the machine? `push_back` reads the size and the capacity, maybe allocates a bigger block, moves the
  elements there and frees the old block, writes the new element, and increases the size. Describe one way in which the
  two threads lose an element, and one way in which they crash the program.
- Protect the `vector` with a `std::mutex` and a `std::lock_guard`. Does it work now - every time?

Extension:

- If your toolchain has ThreadSanitizer (see the follow-up), build the unprotected version with it. What does it
  report - and does it report the race even for `n = 3`?
- Measure the protected version: one thread that appends `2n` numbers against two threads that append `n` each. Which is
  faster, and why?

<hr>

### 👉 Task 'Coldwall'

A sum, in parallel. Fill a `std::vector<int>` with 50 million random digits (0 to 9), and add them up:

- Split the vector into `k` parts of equal size; `k` threads add up one part each, into a `std::vector<long long> sums`
  with one element per thread; `main` adds up `sums` after `join`.
- Measure it for `k` = 1, 2, 3, ... up to twice `std::thread::hardware_concurrency()`, as Release. From how many threads
  on does it not get faster any more - and why there?
- Write the thread's function in two ways: (A) it adds every digit directly to its `sums[i]`; (B) it adds up in a local
  variable, and writes `sums[i]` once, at the end. Measure both with `k = 2` and `k = 4` - as Debug and as Release.
  Explain what you see, with the session in mind - and with what the optimizer may do with `sums[i]`.

Extension:

- A third way: one `std::atomic<long long>` for all threads, and every digit added to it with `+=`. How slow is it?
- Where your library has parallel algorithms (see the follow-up): `std::reduce(std::execution::par, ...)`. And
  `std::execution::seq`?
- The old version of this task added up the numbers 1 to `n` directly, without a vector, in a loop per part. Try that
  with gcc and with clang, as Release. Why does clang need no time at all, whatever `n` is? Look at the loop in Compiler
  Explorer.

<hr>

### 👉 Task 'Granite Falls'

One counter, four ways. Two threads increment a counter 10 million times each:

- (a) a plain `int` - undefined behavior, but look at it anyway, as Debug and as Release;
- (b) a `std::atomic<int>`;
- (c) a plain `int`, protected by a `std::mutex`;
- (d) each thread counts in a local variable, and adds its count to a `std::atomic<int>` once, at the end.
- Predict the results, and the order of the times. Then measure, as Release.
- Paste the four loops into Compiler Explorer, `-O2`, x86-64 gcc and ARM64 gcc. Which instruction does the increment in
  each? Where is the `lock` - and what does ARM64 do instead?

Extension:

- (e) `fetch_add(1, std::memory_order_relaxed)` instead of `++`. Another instruction on x86-64? On ARM64? Faster?
- Run (b) to (d) with 4 and with 8 threads.

<hr>

### 👉 Task 'Otter Bay'

A producer and two consumers:

- A `std::queue<int>`, protected by a `std::mutex`, and a `std::condition_variable` that says "not empty".
- The producer puts the numbers 1 to 10,000 into the queue, then two times -1 - one for each consumer.
- Each consumer waits until the queue is not empty, takes a number, and adds it to its own sum - until it takes a -1.
- Print both sums and the total (it must be 50,005,000), and how the numbers were split between the consumers. Count for
  each consumer how often it found the queue empty.

Extension:

- A bounded queue: at most 10 numbers. The producer waits while the queue is full - a second condition variable, "not
  full". Count how often the producer waits, and the consumers.
- Replace `notify_one` by `notify_all`. Still correct? Count the wakeups.

<hr>

### 👉 Task 'Juniper Gate'

Where the bytes of a thread are. Start four threads; each one reports:

- the address of a local variable, of a `thread_local` variable, of a global variable, and of a block it allocates on
  the heap itself;
- `main` collects the addresses, prints them after `join` - and its own.
- How far apart are the stacks? Where are the `thread_local`s, compared to the stacks? Which address is the same in all
  threads? And the heap blocks: are they near `main`'s heap block, or somewhere else?

Extension:

- How deep can a recursion go? Write a recursive function with a local array of 1 KiB that prints its depth every 100
  levels (with `std::endl` - the program will crash, and must print first). Call it in `main`, then - in a second run -
  in a thread. Which one gets deeper, and by how much? Compare with a classmate on another platform.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "Is `++counter` safe with two threads, on x86-64?", as an LLM might write them:

> **Answer A:** No. `++counter` is a read-modify-write: the value is loaded, incremented and stored. Two threads can
> load the same value and both store it plus one, so an increment is lost. With optimizations, the compiler emits a
> single `add` instruction to memory on x86-64, and a single instruction is executed atomically by the processor - so as
> Release, it is safe in practice. To be sure everywhere, use `std::atomic<int>`.

> **Answer B:** No. Two threads that access the same variable, one of them writing, without synchronization, are a data
> race - undefined behavior in C++, whatever the processor does. `std::atomic<int>` makes `++` atomic; on x86-64, the
> compiler emits the instruction with a `lock` prefix. A lighter alternative is to declare `counter` as `volatile`: it
> forces every access to go to memory, so each thread always sees the current value.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them: the counter of the session with two threads, as Debug and as Release - and with `volatile int`. And the
  machine code in Compiler Explorer.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x0a).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x0a](../docs/answers.md#check-0x0a).

- I can say what a thread has of its own and what it shares with the others - and show it with addresses.
- I can explain `thread_local`, and how the machine finds a thread's variable.
- I can start threads with functions and lambdas, pass arguments and references, and join them.
- I can explain what a `std::thread` object holds, and where the callable and its arguments go.
- I know what it costs to start a thread, and when a thread pays off.
- I can explain why `++counter` loses updates with two threads - in the machine, on x86-64 and on ARM64.
- I can explain why a data race is undefined behavior, and what the optimizer did with a racy loop and with a plain
  flag.
- I know why `volatile` is no protection between threads.
- I can use `std::atomic`, and say what `++` on an atomic is in the machine.
- I can protect data with a mutex - `lock_guard`, `unique_lock`, `scoped_lock` - and say what locking costs, with and
  without competition.
- I can explain cache lines and false sharing, and avoid false sharing.
- I can explain a deadlock and two ways to avoid it.
- I can wait with a condition variable and a predicate - and say why the predicate is needed.
- I can get a result or an exception from another thread with `std::async` and `std::future`, or with a `std::promise`.
- I can explain what a memory order is, and why x86-64 and ARM64 need different instructions for it.
