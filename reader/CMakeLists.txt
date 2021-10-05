cmake_minimum_required(VERSION 2.8)
project(archiver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_library(READER reader.cpp)

file(COPY tests/mock DESTINATION ${CMAKE_BINARY_DIR}/)

# Setup testing
link_directories(/usr/local/lib)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_custom_target(tests COMMAND GTEST_COLOR=yes ${CMAKE_CTEST_COMMAND} --verbose)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable("reader_tests" EXCLUDE_FROM_ALL ${TEST_FILES})
target_link_libraries("reader_tests" gtest pthread READER)
add_dependencies(tests "reader_tests")
add_test("reader_tests" "./reader_tests")
