cmake_minimum_required(VERSION 2.8)
project(archiver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_executable(archiver main.cpp)

add_library(ARCHIVER "archiver/archiver.cpp")
add_library(READER reader/reader.cpp)
add_library(WRITER writer/writer.cpp)

target_link_libraries(archiver ARCHIVER)
target_link_libraries(archiver READER)
target_link_libraries(archiver WRITER)
