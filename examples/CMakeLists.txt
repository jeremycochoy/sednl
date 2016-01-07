# Set include directories (include/ and src/) and link directories
include_directories (${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/)
link_directories(${PROJECT_BINARY_DIR}/src/SEDNL/)

#We need threads
find_package(Threads)

#Build client
add_executable (helloworld_client "${PROJECT_SOURCE_DIR}/examples/helloworld/helloworld_client.cpp")
target_link_libraries(helloworld_client ${SEDNL_LIBRARY_NAME})
target_link_libraries(helloworld_client ${CMAKE_THREAD_LIBS_INIT})

#Build server
add_executable (helloworld_server "${PROJECT_SOURCE_DIR}/examples/helloworld/helloworld_server.cpp")
target_link_libraries(helloworld_server ${SEDNL_LIBRARY_NAME})
target_link_libraries(helloworld_server ${CMAKE_THREAD_LIBS_INIT})

#Build serializer example
add_executable (serializer "${PROJECT_SOURCE_DIR}/examples/serializer/serializer.cpp")
target_link_libraries(serializer ${SEDNL_LIBRARY_NAME})
target_link_libraries(serializer ${CMAKE_THREAD_LIBS_INIT})
