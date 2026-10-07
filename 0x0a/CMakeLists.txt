# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

# Threads: on some systems (older glibc, MinGW) a program with threads must be linked with the thread library. CMake
# knows how - and `link_libraries` applies to all targets below.
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

add_executable(a_recap_episode_0x0a i_preparation/a_recap_episode_0x0a.cpp)
add_executable(b_threads i_preparation/b_threads.cpp)
add_executable(c_threads_in_the_debugger i_preparation/c_threads_in_the_debugger.cpp)

add_executable(a_threads_in_memory ii_session/a_threads_in_memory.cpp)
add_executable(b_races_in_the_machine ii_session/b_races_in_the_machine.cpp)
add_executable(c_atomics_and_locks ii_session/c_atomics_and_locks.cpp)
add_executable(d_cache_lines ii_session/d_cache_lines.cpp)

add_executable(study_threads_and_locks iii_follow_up/study_threads_and_locks.cpp)
add_executable(study_condition_variables iii_follow_up/study_condition_variables.cpp)
add_executable(study_async_and_futures iii_follow_up/study_async_and_futures.cpp)
add_executable(tinker_thread_sanitizer iii_follow_up/tinker_thread_sanitizer.cpp)
add_executable(tinker_memory_order iii_follow_up/tinker_memory_order.cpp)
add_executable(tinker_parallel_algorithms iii_follow_up/tinker_parallel_algorithms.cpp)

# ThreadSanitizer for one target, where the toolchain can link it (gcc and clang on Linux, Apple clang on macOS) - as
# AddressSanitizer in unit 0x05. Elsewhere - MinGW, MSVC - the check fails, the snippet is built without it and says so
# when it runs.
include(CheckLinkerFlag)
check_linker_flag(CXX -fsanitize=thread CBL_HAVE_TSAN)
if (CBL_HAVE_TSAN)
    target_compile_options(tinker_thread_sanitizer PRIVATE -fsanitize=thread)
    target_link_options(tinker_thread_sanitizer PRIVATE -fsanitize=thread)
endif ()

add_executable(westwheat_one_solution iv_solutions/westwheat_one_solution.cpp)
add_executable(coldwall_one_solution iv_solutions/coldwall_one_solution.cpp)
add_executable(granitefalls_one_solution iv_solutions/granitefalls_one_solution.cpp)
add_executable(otterbay_one_solution iv_solutions/otterbay_one_solution.cpp)
add_executable(junipergate_one_solution iv_solutions/junipergate_one_solution.cpp)
