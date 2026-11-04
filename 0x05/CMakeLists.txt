# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

add_executable(a_recap_episode_0x05 i_preparation/a_recap_episode_0x05.cpp)
add_executable(b_pointers_and_arrays i_preparation/b_pointers_and_arrays.cpp)

add_executable(a_pointer_arithmetic ii_session/a_pointer_arithmetic.cpp)
# target_compile_options(a_pointer_arithmetic PRIVATE -O3)

add_executable(b_new_and_delete ii_session/b_new_and_delete.cpp)
add_executable(c_singly_linked_list ii_session/c_singly_linked_list.cpp)
add_executable(d_const_and_pointers ii_session/d_const_and_pointers.cpp)
add_executable(e_a_box_and_the_rule_of_three ii_session/e_a_box_and_the_rule_of_three.cpp)
add_executable(f_assembly ii_session/f_assembly.cpp)

add_executable(study_wild_pointers iii_follow_up/study_wild_pointers.cpp)
add_executable(study_addresses_as_arguments iii_follow_up/study_addresses_as_arguments.cpp)
add_executable(study_pointers_to_pointers iii_follow_up/study_pointers_to_pointers.cpp)
add_executable(tinker_arrays_decay iii_follow_up/tinker_arrays_decay.cpp)
add_executable(tinker_c_strings iii_follow_up/tinker_c_strings.cpp)
add_executable(tinker_address_sanitizer iii_follow_up/tinker_address_sanitizer.cpp)
add_executable(tinker_aliasing iii_follow_up/tinker_aliasing.cpp)
add_executable(tinker_row_or_column iii_follow_up/tinker_row_or_column.cpp)
add_executable(tinker_simd iii_follow_up/tinker_simd.cpp)
add_executable(tinker_a_list_with_rule_of_three iii_follow_up/tinker_a_list_and_the_rule_of_three.cpp)

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
