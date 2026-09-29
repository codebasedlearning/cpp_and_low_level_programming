# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

cmake_minimum_required(VERSION 3.20)        # Enforce at least CMake 3.20 for good C++23 support
project(cpp_machine_level_programming LANGUAGES CXX)       # Use the LANGUAGES keyword to declare C++ explicitly

set(CMAKE_CXX_STANDARD 23)                  # Requests C++23 mode for targets
set(CMAKE_CXX_STANDARD_REQUIRED ON)         # Fails configuration if the compiler can’t provide the requested standard
set(CMAKE_CXX_EXTENSIONS OFF)               # Improves portability

add_subdirectory(utils)                     # Defines the header-only INTERFACE target 'cbl_utils'
link_libraries(cbl_utils)                   # Directory property, inherited by all subdirectories below,
                                            # so every target gets utils/ on its include path.
                                            # Must appear *before* the add_subdirectory() calls.

add_subdirectory(0x01)
add_subdirectory(0x02)
add_subdirectory(0x03)
add_subdirectory(0x04)
add_subdirectory(0x05)
