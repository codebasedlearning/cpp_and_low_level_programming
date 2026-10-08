[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# C++ and Low-Level Programming – A Course for Experienced Programmers

## Overview

This course is designed for programmers who:

- Already have programming experience.
- Know another object-oriented language (Java, C#, Python, etc.).
- Want to learn C++ - and understand what their programs do on the machine.

The course assumes familiarity with fundamental programming concepts like classes and loops. Instead, it focuses on
C++-specific features and language constructs, and follows every one of them one level down: what the compiler makes
of it, where the bytes go, when an object lives and dies. Two threads, one course.

## Why C++

Underneath any program are bytes, addresses, lifetimes, and owners. Most languages hide them, and for good reasons.
C++ hides the least while still offering the abstractions that make the gap visible: a `std::string` is a class, and
three words on the stack, and a block on the heap - all in one line of code, and all of it can be looked at.

That makes C++ a good place to learn both at once. Java, C#, and Python hide the machine behind a runtime: no `sizeof`,
no address, no moment at which an object dies. C shows the bytes but has little to contrast them with. Rust shows
ownership, but its borrow checker refuses exactly the programs whose failure is instructive. C++ lets you write them,
run them, and see what happens - in the debugger, in the memory view, in the symbol table.

So this is a C++ course, and a course on what programs are made of; each topic is taught with both in view. What has
a machine-level story is in the sessions, what is C++ for its own sake goes to the `study_` and `tinker_` files.

## Why low-level?

A course that only teaches a language ages quickly: syntax can be looked up, and today a tool often writes it for us. 
What lasts is understanding what a program actually does – memory, addresses, lifetime, ownership, cost. 
C++ is particularly suited to that, because it does not hide these things. So this is still a C++ course, but one that 
follows every language feature down to the machine: not only how to write it, but what happens – and why.

## Working with this repository

- Clone the repository and open its root folder as a project in CLion — not a unit folder. The root `CMakeLists.txt` 
  includes all units and the small helper library in `utils`.
- Every snippet is a program of its own: one file, one executable, runnable from CLion.
- `docs/` holds the glossary and the answers to the questions in the snippets.

## How a unit is organized

Every unit follows the same pattern — before, during and after the live session:

| Folder / file            | What it is                                                                   |
|:-------------------------|:-----------------------------------------------------------------------------|
| `README.md`              | what the unit is about                                                       |
| `i_preparation/`         | preparation before the session – read and run, nothing to hand in            |
| `ii_session/`            | the material of the live session/lecture                                     |
| `iii_follow_up/study_*`  | required study – not discussed in the session, but assumed in tasks and exam |
| `iii_follow_up/tinker_*` | optional study – for the curious, not exam-relevant                          |
| `iv_solutions/`          | one solution per task                                                        |
| `tasks.md`               | the tasks - solve them in a project of your own, outside this repository     |

From unit 0x02 on, the preparation starts with a recap of the unit before.

## The Codebook

Most comments in the snippets are written with the Codebook in mind, a plugin for CLion. It folds the explanations away
in the editor and shows them in a side panel instead — as an outline, as the explanation for the code at the cursor,
with glossary entries and answers opening in place.
Everything also works without it, but the snippets are much easier to read with it.

What it does, which CLion version it needs and how to install it:
[github.com/codebasedlearning/cbl_codebook](https://github.com/codebasedlearning/cbl_codebook).

## Units

- [Unit 0x01 - Up and Running](0x01/README.md): How does a text file become a program that solves something?
- [Unit 0x02 – To Copy or Not to Copy](0x02/README.md): Is it copied — and what does that cost?
- [Unit 0x03](0x03/README.md)
- [Unit 0x04](0x04/README.md)
- [Unit 0x05](0x05/README.md)
- [Unit 0x06](0x06/README.md)
- [Unit 0x07](0x07/README.md)
- [Unit 0x08](0x08/README.md)
- [Unit 0x09](0x09/README.md)
- [Unit 0x0a](0x0a/README.md)

## Feedback

Feel free to send constructive feedback and suggestions to [me](mailto:info@codebasedlearning.dev).
