cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

set(CMAKE_CXX_STANDARD 11)
include(../../config/Macros.cmake)          # add_process_executable and add_thread_executable

# directories to search for header files
include_directories(../../include)

#==============  Single Pipe ===============================

add_process_executable(${PROJECT}_single_child child single src/single/child.cpp)
add_process_executable(${PROJECT}_single_parent parent single src/single/parent.cpp)
add_dependencies(${PROJECT}_single_parent ${PROJECT}_single_child)    # parent depends on child
install(TARGETS ${PROJECT}_single_parent ${PROJECT}_single_child
        DESTINATION bin/${MY_OUTPUT_DIR}/single)

#===============  Multiple Pipes ============================

add_process_executable(${PROJECT}_multiple_child child multiple src/multiple/child.cpp)
add_process_executable(${PROJECT}_multiple_parent_poll parent_poll multiple src/multiple/parent_poll.cpp)
add_process_executable(${PROJECT}_multiple_parent_threads parent_threads multiple src/multiple/parent_threads.cpp)

add_dependencies(${PROJECT}_multiple_parent_poll ${PROJECT}_multiple_child)    # parent depends on child
add_dependencies(${PROJECT}_multiple_parent_threads ${PROJECT}_multiple_child)    # parent depends on child

install(TARGETS ${PROJECT}_multiple_parent_poll ${PROJECT}_multiple_parent_threads ${PROJECT}_multiple_child
        DESTINATION bin/${MY_OUTPUT_DIR}/multiple)

#===============  Pipe Server ============================

add_process_executable(${PROJECT}_server_server server server src/server/server.cpp)
add_process_executable(${PROJECT}_server_client client server src/server/client.cpp)

install(TARGETS ${PROJECT}_server_server ${PROJECT}_server_client
        DESTINATION bin/${MY_OUTPUT_DIR}/server)