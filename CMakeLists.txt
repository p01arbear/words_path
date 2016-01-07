project(words_path)
cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11 -Wall)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

#------------------------------ tests ------------------------------------------
macro(add_test_func fname)

      list(APPEND   tests_src_list  ${fname}.cpp)

      add_test( NAME                    ${PROJECT_NAME}:${fname}
                WORKING_DIRECTORY       ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND                 test_driver ${fname} ${CMAKE_CURRENT_SOURCE_DIR}
        )
endmacro()

add_test_func(test_words_path_program)

enable_testing()
create_test_sourcelist(tests_list test_driver.cpp ${tests_src_list} )
add_executable(test_driver      ${tests_list})
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure )
add_dependencies (check  test_driver)
