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

#==============  Shared Memory Process =====================================

add_process_executable(${PROJECT}_process_child child process src/process/child.cpp)
add_process_executable(${PROJECT}_process_parent parent process src/process/parent.cpp)
add_dependencies(${PROJECT}_process_parent ${PROJECT}_process_child)    # parent depends on child
install(TARGETS ${PROJECT}_process_parent ${PROJECT}_process_child
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Shared Memory Thread  ======================================

add_thread_executable(${PROJECT}_thread_shmem shmem thread src/thread/thread.cpp)
install(TARGETS ${PROJECT}_thread_shmem
        DESTINATION bin/${MY_OUTPUT_DIR}/thread)
