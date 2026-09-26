[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# C++ Course for Experienced Programmers

## Overview

This crash course is designed for programmers who:

- Already have programming experience
- Know another object-oriented language (Java, C#, Python, etc.)
- Want to learn C++ implementation techniques

The course assumes familiarity with fundamental programming concepts like classes and loops. Instead, it focuses on
C++-specific features and language constructs, explaining their practical applications and use cases.

## Course Structure

The material is organized into units, each containing:

- **Snippets**: Core C++ concepts and practical topics
- **Study**: Additional examples and in-depth content for self-study

Note to enrolled students: The "More" sections are part of the required curriculum.

## Repository Structure

- `snippets/`: Code examples and demonstrations
- `solutions/`: Complete solution implementations
- `CMakeLists.txt`: Build information
- `README.md`: Information and tasks

## Feedback

Feel free to send constructive feedback and suggestions
to [me](mailto:info@codebasedlearning.dev).

## Content

### [Unit 0x00: Compile and Run](0x00/README.md)
| Snippet                | Content                       |
|:-----------------------|:------------------------------|
| a_helloworld           | Traditional First C++ program |
| a_helloworld_explained | First C++ program explained   |

### [Unit 0x01: Basics I](0x01/README.md)
| Snippet        | Content                                         |
|:---------------|:------------------------------------------------|
| a_uniform_init | Variable declaration and initialization methods |
| b_console_io   | Console I/O, `cin`, `cout`                      |
| c_primitives   | Primitive data types and operators              |
| d_strings      | String handling                                 |
| e_if_for_while | Control flow statements                         |
| f_functions    | Function declarations and definitions, scopes   |

### [Unit 0x02: Basics II](0x02/README.md)
| Snippet       | Content                   |
|:--------------|:--------------------------|
| a_consts      | `const`-related concepts  |
| b_structs     | Structs and their usage   |
| c_references  | References and aliases    |
| d_call_by_ref | Parameter passing         |
| e_exception   | Exception handling        |
| f_arrays      | Arrays                    |

### [Unit 0x03: Class I](0x03/README.md)
| Snippet           | Content                            |
|:------------------|:-----------------------------------|
| a_simple_class    | First classes and instances        |
| b_explicit_const  | Explicit and `const` qualifiers    |
| c_copy_assignment | op=                                |
| d_dtor_dtor_throw | ctor and dtor treatment            |
| e_getter_setter   | Discussions on getters and setters |
| f_raii            | RAII principle                     |

### [Unit 0x04: Class II](0x04/README.md)
| Snippet            | Content                |
|:-------------------|:-----------------------|
| a_nodiscard        | `nodiscard` attribute  |
| b_stream_insertion | op<<                   |
| c_string_view      | (String)Views          |
| d_iterators        | Iterators              |
| e_first_generics   | Generics               |
| f_containers       | Containers/Collections |

### [Unit 0x05: Pointers](0x05/README.md)
| Snippet               | Content             |
|:----------------------|:--------------------|
| a_asm_view            | Assembler view      |
| b_first_pointers      | First pointers      |
| c_more_pointers       | More pointers       |
| d_arrays_and_pointers | Arrays and pointers |
| e_swap                | Swap                |
| f_this                | this-pointer        |

### [Unit 0x06: Class III](0x06/README.md)
| Snippet               | Content        |
|:----------------------|:---------------|
| a_const               | More on const  |
| b_new_delete          | Dynamic memory |
| c_unique_ptr          | Smart pointers |
| d_smallbuffer_example | Example        |
| e_addressof           | op&            |

### [Unit 0x07: Class IV](0x07/README.md)
| Snippet       | Content              |
|:--------------|:---------------------|
| a_statics     | static               |
| b_friends     | friend               |
| c_operators_1 | operator overloading |
| d_operators_2 | operator overloading |
| e_operators_3 | operator overloading |

### [Unit 0x08: Inheritance](0x08/README.md)
| Snippet                | Content               |
|:-----------------------|:----------------------|
| a_single_inheritance   | Single inheritance    |
| b_modifier             | Modifier              |
| c_virtual              | Virtual               |
| d_abstract             | Abstract              |
| e_multiple_inheritance | Multiple inheritance  |

### [Unit 0x09: Misc](0x09/README.md)
| Snippet           | Content                  |
|:------------------|:-------------------------|
| a_casts           | Casting                  |
| b_enums           | Enums                    |
| c_specializations | Template specializations |
| d_more_smartptrs  | shared_ and weak_ptr     |
| e_in_out          | print and format         |

### [Unit 0x0a: Functions and Threads](0x0a/README.md)
| Snippet      | Content        |
|:-------------|:---------------|
| a_lambdas    | Lambdas        |
| b_functions  | Functions      |
| c_algorithms | Algorithms lib |
| d_threads    | Threads        |
| e_mutex      | Mutex          |

### [Unit 0x0b: Outlook and Structure](0x0b/README.md)
| Snippet                 | Content                  |
|:------------------------|:-------------------------|
| a_conditions_variables  | Conditions variables     |
| b_smart_ptr_v1          | Smart pointers version 1 |
| c_smart_ptr_v2          | Smart pointers version 2 |
| d_ui_lib / d_use_ui_lib | UI library and usage     |
