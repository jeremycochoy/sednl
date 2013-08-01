cmake_minimum_required (VERSION 2.6)

#Create options
option(BUILD_STATIC "Tell if the static library should be compiled" ON)
option(BUILD_DYNAMIC "Tell if the dynamic library should be compiled" ON)
option(BUILD_DOC "Tell if the dynamic library should be compiled" ON)
option(BUILD_TEST "Compile the test applications and allow to run them with 'make test'" ON)
option(BUILD_EXAMPLES "Tell if we should also compile the examples" ON)

# Project name
project(SEDNL)

# C++ 2011 flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -ggdb -W -Wall -fvisibility=hidden")

#C 99 flag
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -ggdb -W -Wall -fvisibility=hidden")

# Set version numbers
set(SEDNL_VERSION_MAJOR 1)
set(SEDNL_VERSION_MINOR 0)
set(SEDNL_VERSION_MICRO 0)

#Set include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Add subdirectories

add_subdirectory(src/SEDNL)
if(BUILD_DOC)
  add_subdirectory(doc)
endif(BUILD_DOC)
if(BUILD_TEST)
enable_testing()
add_subdirectory(test)
endif(BUILD_TEST)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

