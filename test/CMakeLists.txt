project(POG_test)

add_subdirectory(tools)

find_program(TEST_SHELL
        NAMES bash sh
        PATHS env PATH)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setenv.sh.model ${CMAKE_CURRENT_SOURCE_DIR}/setenv.sh @ONLY)

macro(add_pog_test id)
    add_test(NAME ${id}
        COMMAND ${TEST_SHELL} ${CMAKE_CURRENT_SOURCE_DIR}/dotest.sh ${CMAKE_CURRENT_SOURCE_DIR} ${id}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(${id} PROPERTIES FAIL_REGULAR_EXPRESSION "Test failed")
    set_tests_properties(${id} PROPERTIES PASS_REGULAR_EXPRESSION "Test passed")
    set_tests_properties(${id} PROPERTIES TIMEOUT 1)
endmacro(add_pog_test)

# add_pog_test(empty_1)
