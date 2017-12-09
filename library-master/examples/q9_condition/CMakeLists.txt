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

#==============  Condition Process ===============================
add_process_executable(${PROJECT}_process_car car process src/process/car.cpp)
add_process_executable(${PROJECT}_process_pedestrian pedestrian process src/process/pedestrian.cpp)
add_process_executable(${PROJECT}_process_intersection intersection process src/process/intersection.cpp)
add_dependencies(${PROJECT}_process_intersection ${PROJECT}_process_car ${PROJECT}_process_pedestrian)
install(TARGETS ${PROJECT}_process_intersection ${PROJECT}_process_car ${PROJECT}_process_pedestrian
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Condition Thread  ==============================
add_thread_executable(${PROJECT}_thread_intersection intersection thread src/thread/intersection.cpp)
install(TARGETS ${PROJECT}_thread_intersection
        DESTINATION bin/${MY_OUTPUT_DIR}/thread)
