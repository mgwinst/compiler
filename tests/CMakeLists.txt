cmake_minimum_required(VERSION 4.0)

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(WARNING "GoogleTest not found -> skipping tests. Install GoogleTest.")
    return()
endif()

# ctest compiler_tests
add_executable(compiler_tests
    lexer/lexer_test.cpp
    parser/parser_test.cpp
)

target_link_libraries(compiler_tests PRIVATE
    compiler_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(compiler_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
