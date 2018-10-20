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

#==============  Bakery ===============================
add_process_executable(${PROJECT}_baker baker . src/baker.cpp)
add_process_executable(${PROJECT}_cookie_monster cookie_monster . src/cookie_monster.cpp)
add_process_executable(${PROJECT}_bakery bakery . src/bakery.cpp)
add_dependencies(${PROJECT}_bakery ${PROJECT}_baker ${PROJECT}_cookie_monster)
install(TARGETS ${PROJECT}_bakery ${PROJECT}_baker ${PROJECT}_cookie_monster
        DESTINATION bin/${MY_OUTPUT_DIR})
