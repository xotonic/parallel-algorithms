cmake_minimum_required(VERSION 3.3)
project(opencl_char_pol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES main.cpp)
add_executable(opencl_char_pol ${SOURCE_FILES})
