cmake_minimum_required (VERSION 2.6)

#Create the version cpp file
set(SEDNL_VERSION_TEMPLATE ${PROJECT_SOURCE_DIR}/src/SEDNL/Version.cpp.in)
set(SEDNL_VERSION_FILE ${PROJECT_BINARY_DIR}/src/SEDNL/Version.cpp)
configure_file(${SEDNL_VERSION_TEMPLATE}
  ${SEDNL_VERSION_FILE}
  @ONLY
  )

# Set include directories (include/ and src/)
include_directories (${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/)

# Add files that should be compiled
file(GLOB_RECURSE SOURCE_FILES *.cpp)
list(APPEND SOURCE_FILES ${SEDNL_VERSION_FILE})


if(BUILD_STATIC)
  add_library(SEDNLStatic STATIC ${SOURCE_FILES})
  set_target_properties(SEDNLStatic PROPERTIES OUTPUT_NAME sednl)
endif(BUILD_STATIC)

if(BUILD_DYNAMIC)
  add_library(SEDNLDyn SHARED ${SOURCE_FILES})
  set_target_properties(SEDNLDyn PROPERTIES OUTPUT_NAME sednl)
endif(BUILD_DYNAMIC)
