# Set include directories (include/ and src/) and link directories
include_directories (${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/)
link_directories(${PROJECT_BINARY_DIR}/src/SEDNL/)

#We need threads
find_package(Threads)

#
#Compile tests
#

add_executable (ringbuffer "${PROJECT_SOURCE_DIR}/test/ringbuffer.cpp")
target_link_libraries(ringbuffer sednl)

add_executable (eventcollision "${PROJECT_SOURCE_DIR}/test/eventcollision.cpp")
target_link_libraries(eventcollision sednl)
target_link_libraries(eventcollision ${CMAKE_THREAD_LIBS_INIT})

add_executable (packet "${PROJECT_SOURCE_DIR}/test/packet.cpp")
target_link_libraries(packet sednl)
target_link_libraries(packet ${CMAKE_THREAD_LIBS_INIT})

#Run tests
add_test (NAME RingBuffer
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/"
  COMMAND "ringbuffer")
add_test (NAME EventCollision
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/"
  COMMAND "eventcollision")
add_test (NAME Packet
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/"
  COMMAND "packet")
add_test (NAME PacketValidity
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/"
  COMMAND "packet")
set_tests_properties (PacketValidity
  PROPERTIES PASS_REGULAR_EXPRESSION "Warning: Invalid packet for event \"a\". Dropped.")
