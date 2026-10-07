# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

add_executable(a_recap_episode_0x04 i_preparation/a_recap_episode_0x04.cpp)
add_executable(b_print_and_view i_preparation/b_print_and_view.cpp)
add_executable(c_symbols i_preparation/c_symbols.cpp)

add_executable(a_views_in_memory ii_session/a_views_in_memory.cpp)
add_executable(b_iterators_in_memory ii_session/b_iterators_in_memory.cpp)
add_executable(c_templates_per_type ii_session/c_templates_per_type.cpp)
add_executable(d_symbols_revisited ii_session/d_symbols_revisited.cpp)

add_executable(study_vector iii_follow_up/study_vector.cpp)
add_executable(study_containers iii_follow_up/study_containers.cpp)
add_executable(study_iterators iii_follow_up/study_iterators.cpp)
add_executable(study_class_templates iii_follow_up/study_class_templates.cpp)
add_executable(study_namespaces iii_follow_up/study_namespaces.cpp)
add_executable(study_concepts iii_follow_up/study_concepts.cpp)

# A static library and a program that uses it: the library from three .cpp files, the program from main.cpp alone.
add_library(weather STATIC
        iii_follow_up/study_static_library/stats.cpp
        iii_follow_up/study_static_library/report.cpp
        iii_follow_up/study_static_library/convert.cpp)
add_executable(study_static_library iii_follow_up/study_static_library/main.cpp)
target_link_libraries(study_static_library PRIVATE weather)

add_executable(tinker_attributes iii_follow_up/tinker_attributes.cpp)
add_executable(tinker_views iii_follow_up/tinker_views.cpp)
add_executable(tinker_literals iii_follow_up/tinker_literals.cpp)
add_executable(tinker_more_templates iii_follow_up/tinker_more_templates.cpp)
add_executable(tinker_type_traits iii_follow_up/tinker_type_traits.cpp)

add_executable(brickgate_one_solution iv_solutions/brickgate_one_solution.cpp)
add_executable(millbrook_one_solution iv_solutions/millbrook_one_solution.cpp)
add_executable(oakberg_one_solution iv_solutions/oakberg_one_solution.cpp)
add_executable(ashfield_one_solution iv_solutions/ashfield_one_solution.cpp)
add_executable(sparrowtown_one_solution iv_solutions/sparrowtown_one_solution.cpp)
add_executable(eastbourne_one_solution iv_solutions/eastbourne_one_solution.cpp)
add_executable(wintervale_one_solution iv_solutions/wintervale_one_solution.cpp)
