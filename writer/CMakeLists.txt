cmake_minimum_required(VERSION 2.8)
project(archiver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_library(WRITER writer.cpp)
add_library(READER ../reader/reader.cpp)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mock)

# Setup testing
link_directories(/usr/local/lib)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_custom_target(tests COMMAND GTEST_COLOR=yes ${CMAKE_CTEST_COMMAND} --verbose)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable("writer_tests" EXCLUDE_FROM_ALL ${TEST_FILES})
target_link_libraries("writer_tests" gtest pthread WRITER)
target_link_libraries("writer_tests" gtest pthread READER)
add_dependencies(tests "writer_tests")
add_test("writer_tests" "./writer_tests")
