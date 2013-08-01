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
file(GLOB_RECURSE INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(APPEND SOURCE_FILES ${SEDNL_VERSION_FILE})

#define the dyn/static library name
set(SEDNL_INCLUDE_DIRECTORY_NAME "sednl-${SEDNL_VERSION_MAJOR}.${SEDNL_VERSION_MINOR}")
set(SEDNL_LIBRARY_NAME "sednl-${SEDNL_VERSION_MAJOR}.${SEDNL_VERSION_MINOR}")

if(BUILD_STATIC OR BUILD_DYNAMIC)
  install(FILES ${INCLUDE_FILES} DESTINATION "include/${SEDNL_INCLUDE_DIRECTORY_NAME}")
endif()

if(BUILD_STATIC)
  add_library(SEDNLStatic STATIC ${SOURCE_FILES})
  set_target_properties(SEDNLStatic PROPERTIES OUTPUT_NAME ${SEDNL_LIBRARY_NAME})

  install(TARGETS SEDNLStatic DESTINATION lib)
endif(BUILD_STATIC)

if(BUILD_DYNAMIC)
  add_library(SEDNLDyn SHARED ${SOURCE_FILES})
  set_target_properties(SEDNLDyn PROPERTIES OUTPUT_NAME ${SEDNL_LIBRARY_NAME})

  install(TARGETS SEDNLDyn DESTINATION lib)
endif(BUILD_DYNAMIC)
