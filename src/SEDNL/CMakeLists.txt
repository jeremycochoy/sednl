cmake_minimum_required (VERSION 2.6)

# Set include directories (include/ and src/)
include_directories (${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/)

# Add files that should be compiled
file(GLOB_RECURSE SOURCE_FILES *.cpp)

if(BUILD_STATIC)
  add_library(SEDNLStatic STATIC ${SOURCE_FILES})
  set_target_properties(SEDNLStatic PROPERTIES OUTPUT_NAME sednl)
endif(BUILD_STATIC)

if(BUILD_DYNAMIC)
  add_library(SEDNLDyn SHARED ${SOURCE_FILES})
  set_target_properties(SEDNLDyn PROPERTIES OUTPUT_NAME sednl)
endif(BUILD_DYNAMIC)
