# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

# header-only helper library, no sources to compile
add_library(cbl_utils INTERFACE)
target_include_directories(cbl_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
