cmake_minimum_required(VERSION 2.6.2)
project(AITD)
add_definitions(-DUNIX -DFORBIDDEN_SYMBOL_ALLOW_ALL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread -march=native")
find_package(SDL REQUIRED)

add_subdirectory(aitd)
add_subdirectory(src)

