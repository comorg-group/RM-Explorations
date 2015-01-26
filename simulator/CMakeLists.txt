cmake_minimum_required(VERSION 2.8.4)
project(simulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Ofast")

set(SOURCE_FILES main.cc BaselineCache.cc BaseCache.cc global.hh BaselineCache.hh BaseCache.hh CacheStrip.cc CacheStrip.hh EagerCache.cc EagerCache.hh MassiveCache.hh MassiveCache.cc SetCrossGroupCache.hh SetCrossGroupCache.cc)
add_executable(simulator ${SOURCE_FILES})
