cmake_minimum_required(VERSION 3.3)
project(opencl_char_pol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES main.cpp)
add_executable(opencl_char_pol ${SOURCE_FILES})

#LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)
#TARGET_LINK_LIBRARIES(opencl_char_pol libOpenCL.so)
#find_library(OpenCL REQUIRED)
#link_directories(/home/xotonic/AMDAPPSDK-3.0/lib/x86_64)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} libOpenCL.so")
#target_link_libraries(opencl_char_pol OpenCL)