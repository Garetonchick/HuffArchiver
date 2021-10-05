cmake_minimum_required(VERSION 2.8)
project(archiver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# Setup testing
link_directories(/usr/local/lib)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_custom_target(tests COMMAND GTEST_COLOR=yes ${CMAKE_CTEST_COMMAND} --verbose)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable("priority_queue_tests" EXCLUDE_FROM_ALL ${TEST_FILES})
target_link_libraries("priority_queue_tests" gtest pthread)
add_dependencies(tests "priority_queue_tests")
add_test("priority_queue_tests" "./priority_queue_tests")
