cmake_minimum_required(VERSION 2.8)
project(archiver)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

file(COPY mock DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mock/texts/compressed)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mock/images/compressed)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mock/video/compressed)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mock/texts/decompressed)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mock/images/decompressed)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/mock/video/decompressed)

add_library(ARCHIVER ../archiver/archiver.cpp)
add_library(READER ../reader/file_reader.cpp)
add_library(WRITER ../writer/file_writer.cpp)
add_library(TIMER ../utility/timer/timer.cpp)
add_library(LOGGER ../utility/logger/logger.cpp)

add_executable(BENCHMARKS benchmarks.cpp)

target_link_libraries(BENCHMARKS ARCHIVER READER WRITER TIMER LOGGER)