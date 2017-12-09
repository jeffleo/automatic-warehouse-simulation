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

#==============  Process Monitor ===============================
add_process_executable(${PROJECT}_process_child child process src/process/child.cpp)
add_process_executable(${PROJECT}_process_mom mom process src/process/mom.cpp)
add_dependencies(${PROJECT}_process_mom ${PROJECT}_process_child)
install(TARGETS ${PROJECT}_process_mom ${PROJECT}_process_child
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Thread Monitor ===============================
add_thread_executable(${PROJECT}_thread_spender spender thread src/thread/spender.cpp)
install(TARGETS ${PROJECT}_thread_spender
        DESTINATION bin/${MY_OUTPUT_DIR}/process)
