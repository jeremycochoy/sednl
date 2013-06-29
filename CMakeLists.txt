cmake_minimum_required (VERSION 2.6)

# Project name
project(SEDNL)

# C++ 2011 flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -ggdb -W -Wall")

#C 99 flag
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -ggdb -W -Wall")

# Set version numbers
set(SEDNL_VERSION_MAJOR 0)
set(SEDNL_VERSION_MINOR 1)
set(SEDNL_VERSION_MICRO 0)

#Set include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Ad subdirectories

add_subdirectory(src/SEDNL)
add_subdirectory(examples)
add_subdirectory(doc)
