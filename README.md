[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Low-Level Programming with C++ – A Course for Experienced Programmers

## Overview

This course is designed for programmers who:

- Already have programming experience.
- Know another object-oriented language (Java, C#, Python, etc.).
- Want to learn C++ implementation techniques.

The course assumes familiarity with fundamental programming concepts like classes and loops. Instead, it focuses on
C++-specific features and language constructs, explaining their practical applications and use cases.

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

- [Unit 0x01](0x01/README.md)
- [Unit 0x02](0x02/README.md)
- [Unit 0x03](0x03/README.md)
- [Unit 0x04](0x04/README.md)

## Feedback

Feel free to send constructive feedback and suggestions to [me](mailto:info@codebasedlearning.dev).
