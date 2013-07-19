# Set include directories (include/ and src/)
include_directories (${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/)

#Compile tests
file(GLOB_RECURSE TEST_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp SRC)

add_executable (ringbuffer "${PROJECT_SOURCE_DIR}/test/ringbuffer.cpp" ${TEST_SRC} )
# TODO : Use target_link_libraries(ringbuffer SEDNL_LIB)

#Run tests
add_test (NAME RingBuffer WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/" COMMAND "ringbuffer")
