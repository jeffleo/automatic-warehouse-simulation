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

#==============  PenPals ===============================

add_process_executable(${PROJECT}_penpal penpal . src/penpal.cpp)
add_process_executable(${PROJECT}_parent parent . src/parent.cpp)
add_dependencies(${PROJECT}_parent ${PROJECT}_penpal)
install(TARGETS ${PROJECT}_parent ${PROJECT}_penpal
        DESTINATION bin/${MY_OUTPUT_DIR})
