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

#==============  Barbershop ===============================
add_process_executable(${PROJECT}_barber barber . src/barber.cpp)
add_process_executable(${PROJECT}_customer customer . src/customer.cpp)
add_process_executable(${PROJECT}_barber_shop barber_shop . src/barber_shop.cpp)
add_process_executable(${PROJECT}_sleeping_barbers sleeping_barbers . src/sleeping_barbers.cpp)
add_dependencies(${PROJECT}_sleeping_barbers ${PROJECT}_barber ${PROJECT}_customer ${PROJECT}_barber_shop)
install(TARGETS ${PROJECT}_sleeping_barbers ${PROJECT}_barber ${PROJECT}_customer ${PROJECT}_barber_shop
        DESTINATION bin/${MY_OUTPUT_DIR$})
