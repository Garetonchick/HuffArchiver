cmake_minimum_required(VERSION 2.8)
project(archiver)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_library(BINARY_TRIE binary_trie.h)
set_target_properties(BINARY_TRIE PROPERTIES LINKER_LANGUAGE CXX)

# Setup testing
link_directories(/usr/local/lib)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_custom_target(tests COMMAND GTEST_COLOR=yes ${CMAKE_CTEST_COMMAND} --verbose)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable("binary_trie_tests" EXCLUDE_FROM_ALL ${TEST_FILES})
target_link_libraries("binary_trie_tests" gtest pthread BINARY_TRIE)
add_dependencies(tests "binary_trie_tests")
add_test("binary_trie_tests" "./binary_trie_tests")
