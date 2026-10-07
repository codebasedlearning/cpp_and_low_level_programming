# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

add_executable(a_recap_episode_0x03 i_preparation/a_recap_episode_0x03.cpp)
add_executable(b_first_class i_preparation/b_first_class.cpp)
add_executable(c_call_stack i_preparation/c_call_stack.cpp)

add_executable(a_objects_in_memory ii_session/a_objects_in_memory.cpp)
add_executable(b_born ii_session/b_born.cpp)
add_executable(c_copied ii_session/c_copied.cpp)
add_executable(d_buried ii_session/d_buried.cpp)
# Several files, one program: all .cpp files of the folder; the header is found through the #include.
add_executable(e_header_and_source
        ii_session/e_header_and_source/main.cpp
        ii_session/e_header_and_source/temperature.cpp)

add_executable(study_getters_and_setters iii_follow_up/study_getters_and_setters.cpp)
add_executable(study_special_members iii_follow_up/study_special_members.cpp)
add_executable(tinker_properties iii_follow_up/tinker_properties.cpp)
add_executable(tinker_throwing_destructor iii_follow_up/tinker_throwing_destructor.cpp)
add_executable(tinker_stack_overflow iii_follow_up/tinker_stack_overflow.cpp)

add_executable(ravencastle_one_solution iv_solutions/ravencastle_one_solution.cpp)
add_executable(stoneridge_one_solution iv_solutions/stoneridge_one_solution.cpp)
add_executable(luckyrock_one_solution iv_solutions/luckyrock_one_solution.cpp)
add_executable(meadowriver_one_solution iv_solutions/meadowriver_one_solution.cpp)
add_executable(coralcreek_one_solution iv_solutions/coralcreek_one_solution.cpp)
add_executable(ashfordmill_one_solution iv_solutions/ashfordmill_one_solution.cpp)
