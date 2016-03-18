cmake_minimum_required(VERSION 3.3)
project(openmp_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(openmp_test ${SOURCE_FILES})