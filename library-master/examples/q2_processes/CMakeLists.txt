cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

set(CMAKE_CXX_STANDARD 11)
include(../../config/Macros.cmake)          # add_process_executable and add_thread_executable

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

# directories to search for header files
include_directories(../../include)

#==============  Children  ======================================
add_process_executable(${PROJECT}_child1 child1 . src/child1.cpp)

add_process_executable(${PROJECT}_child2 child2 . src/child2.cpp)

add_process_executable(${PROJECT}_child3 child3 . src/child3.cpp)

add_process_executable(${PROJECT}_child4 child4 . src/child4.cpp)

#==============  Parent  ======================================
add_process_executable(${PROJECT}_parent parent . src/parent.cpp)

# Parent executable depends on all children (since it tries to spawn them)
add_dependencies(${PROJECT}_parent ${PROJECT}_child1 ${PROJECT}_child2
        ${PROJECT}_child3 ${PROJECT}_child4)

# install all targets
install(TARGETS ${PROJECT}_child1 ${PROJECT}_child2 ${PROJECT}_child3
        ${PROJECT}_child4 ${PROJECT}_parent
        DESTINATION bin/${MY_OUTPUT_DIR})
