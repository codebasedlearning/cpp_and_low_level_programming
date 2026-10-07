# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

add_executable(a_recap_episode_0x06 i_preparation/a_recap_episode_0x06.cpp)
add_executable(b_new_and_delete i_preparation/b_new_and_delete.cpp)
add_executable(c_heap_watch i_preparation/c_heap_watch.cpp)

add_executable(a_heap_in_memory ii_session/a_heap_in_memory.cpp)
add_executable(b_raw_owners ii_session/b_raw_owners.cpp)
add_executable(c_unique_ptr_in_memory ii_session/c_unique_ptr_in_memory.cpp)
add_executable(d_moved ii_session/d_moved.cpp)
add_executable(e_shared_ownership ii_session/e_shared_ownership.cpp)

add_executable(study_unique_ptr iii_follow_up/study_unique_ptr.cpp)
add_executable(study_move iii_follow_up/study_move.cpp)
add_executable(study_dynamic_arrays iii_follow_up/study_dynamic_arrays.cpp)
add_executable(tinker_heap_sanitizers iii_follow_up/tinker_heap_sanitizers.cpp)
add_executable(tinker_new_by_hand iii_follow_up/tinker_new_by_hand.cpp)
add_executable(tinker_small_buffer iii_follow_up/tinker_small_buffer.cpp)
add_executable(tinker_transfer_on_copy iii_follow_up/tinker_transfer_on_copy.cpp)
add_executable(tinker_pages_and_mmap iii_follow_up/tinker_pages_and_mmap.cpp)
add_executable(tinker_emplace_and_forward iii_follow_up/tinker_emplace_and_forward.cpp)

# AddressSanitizer for one target, where the toolchain can link it - as in unit 0x05, whose check is reused here.
include(CheckLinkerFlag)
check_linker_flag(CXX -fsanitize=address CBL_HAVE_ASAN)
if (CBL_HAVE_ASAN)
    target_compile_options(tinker_heap_sanitizers PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(tinker_heap_sanitizers PRIVATE -fsanitize=address)
endif ()

add_executable(southbirdsgale_one_solution iv_solutions/southbirdsgale_one_solution.cpp)
add_executable(rosepond_one_solution iv_solutions/rosepond_one_solution.cpp)
add_executable(mcallenspringsmart_one_solution iv_solutions/mcallenspringsmart_one_solution.cpp)
add_executable(harrowgate_one_solution iv_solutions/harrowgate_one_solution.cpp)
add_executable(lanternhill_one_solution iv_solutions/lanternhill_one_solution.cpp)
add_executable(willowbank_one_solution iv_solutions/willowbank_one_solution.cpp)
