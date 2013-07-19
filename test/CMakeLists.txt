# Set include directories (include/ and src/) and link directories
include_directories (${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/)
link_directories(${PROJECT_BINARY_DIR}/src/SEDNL/)

#Compile tests
file(GLOB_RECURSE TEST_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp SRC)

add_executable (ringbuffer "${PROJECT_SOURCE_DIR}/test/ringbuffer.cpp" ${TEST_SRC} )
target_link_libraries(ringbuffer sednl)

#Run tests
add_test (NAME RingBuffer WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/" COMMAND "ringbuffer")
