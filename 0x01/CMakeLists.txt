# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

add_executable(a_helloworld i_preparation/a_helloworld.cpp)
add_executable(b_helloworld_revisited i_preparation/b_helloworld_revisited.cpp)
add_executable(c_helloworld_codebook i_preparation/c_helloworld_codebook.cpp)
add_executable(d_uniform_initialization i_preparation/d_uniform_initialization.cpp)

add_executable(a_uniform_initialization_revisited ii_session/a_uniform_initialization_revisited.cpp)
add_executable(b_control_flow ii_session/b_control_flow.cpp)
add_executable(c_functions ii_session/c_functions.cpp)

add_executable(study_control_flow iii_follow_up/study_control_flow.cpp)
add_executable(study_string iii_follow_up/study_string.cpp)
add_executable(study_assert iii_follow_up/study_assert.cpp)
add_executable(study_console_io iii_follow_up/study_console_io.cpp)
add_executable(tinker_int8 iii_follow_up/tinker_int8.cpp)
add_executable(tinker_goto iii_follow_up/tinker_goto.cpp)
add_executable(tinker_undefined_sanitizer iii_follow_up/tinker_undefined_sanitizer.cpp)

# UndefinedBehaviorSanitizer for one target, where the toolchain can link it (gcc and clang on Linux, Apple clang on
# macOS). Elsewhere - MinGW, MSVC - the check fails, the snippet is built without it and says so when it runs.
include(CheckLinkerFlag)
check_linker_flag(CXX -fsanitize=undefined CBL_HAVE_UBSAN)
if (CBL_HAVE_UBSAN)
    target_compile_options(tinker_undefined_sanitizer PRIVATE -fsanitize=undefined)
    target_link_options(tinker_undefined_sanitizer PRIVATE -fsanitize=undefined)
    target_compile_definitions(tinker_undefined_sanitizer PRIVATE CBL_UBSAN=1)
endif ()

add_executable(westrosecliff_one_solution iv_solutions/westrosecliff_one_solution.cpp)
add_executable(echovalley_one_solution iv_solutions/echovalley_one_solution.cpp)
add_executable(littleharbor_one_solution ../0x02/iv_solutions/littleharbor_one_solution.cpp)
add_executable(bluecanyon_one_solution iv_solutions/bluecanyon_one_solution.cpp)
add_executable(beverlyhollow_one_solution iv_solutions/beverlyhollow_one_solution.cpp)
add_executable(pinepoint_one_solution iv_solutions/pinepoint_one_solution.cpp)
