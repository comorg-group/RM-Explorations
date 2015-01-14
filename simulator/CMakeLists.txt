cmake_minimum_required(VERSION 2.8.4)
project(simulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cc)
set(SOURCE_FILES BaseCache.cc)
add_executable(simulator ${SOURCE_FILES})