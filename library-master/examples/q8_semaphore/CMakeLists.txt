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
include_directories(../../include src)

#==============  Process Child ===============================

add_process_executable(${PROJECT}_process_car car process src/process/car.cpp)
target_include_directories(${PROJECT}_process_car PRIVATE src/process)  # local headers

#==============  Process Parent ===============================
add_process_executable(${PROJECT}_process_gas_station gas_station process src/process/gas_station.cpp)
target_include_directories(${PROJECT}_process_gas_station PRIVATE src/process)  # local headers

# gas_station depends on car
add_dependencies(${PROJECT}_process_gas_station ${PROJECT}_process_car )
install(TARGETS ${PROJECT}_process_gas_station ${PROJECT}_process_car
        DESTINATION bin/${MY_OUTPUT_DIR}/process)
#==============  Thread ===============================
add_thread_executable(${PROJECT}_thread_gas_station gas_station thread src/thread/gas_station.cpp)
install(TARGETS ${PROJECT}_thread_gas_station
        DESTINATION bin/${MY_OUTPUT_DIR}/thread)
