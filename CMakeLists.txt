cmake_minimum_required(VERSION 2.8)
project(archiver)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_subdirectory(archiver/)

add_executable(MAIN main.cpp)

target_link_libraries(MAIN ARCHIVER)
target_link_libraries(MAIN READER)
target_link_libraries(MAIN WRITER)