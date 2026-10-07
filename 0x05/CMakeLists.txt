# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

add_executable(a_recap_episode_0x05 i_preparation/a_recap_episode_0x05.cpp)
add_executable(b_pointers_and_arrays i_preparation/b_pointers_and_arrays.cpp)
add_executable(c_assembly i_preparation/c_assembly.cpp)

add_executable(a_pointer_arithmetic ii_session/a_pointer_arithmetic.cpp)
add_executable(b_arrays_decay ii_session/b_arrays_decay.cpp)
add_executable(c_addresses_as_arguments ii_session/c_addresses_as_arguments.cpp)
add_executable(d_wild_pointers ii_session/d_wild_pointers.cpp)

add_executable(study_c_strings iii_follow_up/study_c_strings.cpp)
add_executable(study_const_and_pointers iii_follow_up/study_const_and_pointers.cpp)
add_executable(study_pointers_to_pointers iii_follow_up/study_pointers_to_pointers.cpp)
add_executable(tinker_address_sanitizer iii_follow_up/tinker_address_sanitizer.cpp)
add_executable(tinker_aliasing iii_follow_up/tinker_aliasing.cpp)
add_executable(tinker_row_or_column iii_follow_up/tinker_row_or_column.cpp)
add_executable(tinker_simd iii_follow_up/tinker_simd.cpp)

# AddressSanitizer for one target, where the toolchain can link it (gcc and clang on Linux, Apple clang on macOS).
# Elsewhere - MinGW, MSVC - the check fails, the snippet is built without it and says so when it runs.
include(CheckLinkerFlag)
check_linker_flag(CXX -fsanitize=address CBL_HAVE_ASAN)
if (CBL_HAVE_ASAN)
    target_compile_options(tinker_address_sanitizer PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(tinker_address_sanitizer PRIVATE -fsanitize=address)
endif ()

add_executable(copperview_one_solution iv_solutions/copperview_one_solution.cpp)
add_executable(kengate_one_solution iv_solutions/kengate_one_solution.cpp)
add_executable(mcallenspring_one_solution iv_solutions/mcallenspring_one_solution.cpp)
add_executable(thornbury_one_solution iv_solutions/thornbury_one_solution.cpp)
add_executable(dontdothisorilltellyourmum_one_solution iv_solutions/dontdothisorilltellyourmum_one_solution.cpp)
