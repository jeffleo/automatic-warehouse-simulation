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

#==============  Event Process ===============================
add_process_executable(${PROJECT}_process_robot robot process src/process/robot.cpp)
add_process_executable(${PROJECT}_process_production_line production_line process src/process/production_line.cpp)
add_process_executable(${PROJECT}_process_factory factory process src/process/factory.cpp)

# factory depends on both robot and production_line
add_dependencies(${PROJECT}_process_factory ${PROJECT}_process_robot ${PROJECT}_process_production_line)
install(TARGETS ${PROJECT}_process_factory ${PROJECT}_process_robot ${PROJECT}_process_production_line
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Event Thread  ===============================
add_thread_executable(${PROJECT}_thread_factory factory thread src/thread/factory.cpp)
install(TARGETS ${PROJECT}_thread_factory
        DESTINATION bin/${MY_OUTPUT_DIR}/thread)