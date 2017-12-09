cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

set(CMAKE_CXX_STANDARD 11)
include(../../config/Macros.cmake)

# directories to search for header files
include_directories(../../include)

#==============  Process  ===============================

add_process_executable(${PROJECT}_process_child_unsafe child_unsafe process src/process/child_unsafe.cpp)
add_process_executable(${PROJECT}_process_child_safe child_safe process src/process/child_safe.cpp)
add_process_executable(${PROJECT}_process_parent parent process src/process/parent.cpp)
add_dependencies(${PROJECT}_process_parent ${PROJECT}_process_child_unsafe ${PROJECT}_process_child_safe)
install(TARGETS ${PROJECT}_process_parent ${PROJECT}_process_child_unsafe ${PROJECT}_process_child_safe
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Thread ===============================

add_thread_executable(${PROJECT}_thread_thread thread thread src/thread/main.cpp)
install(TARGETS ${PROJECT}_thread_thread DESTINATION bin/${MY_OUTPUT_DIR}/thread)