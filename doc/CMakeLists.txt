cmake_minimum_required (VERSION 2.6)

find_package(Doxygen)

# set the input and output documentation paths
set(DOXYGEN_INPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/doc)

set(DOXYGEN_TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in)
set(DOXYGEN_CONFIGURED_FILE ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

if(DOXYGEN_FOUND)

  configure_file(${DOXYGEN_TEMPLATE_FILE}
    ${DOXYGEN_CONFIGURED_FILE}
    @ONLY
    )
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE}
    ${DOXYGEN_CONFIGURED_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

endif(DOXYGEN_FOUND)
