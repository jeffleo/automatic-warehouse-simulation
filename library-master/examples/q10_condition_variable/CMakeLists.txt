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

#==============  Condition Variable Process ===============================
add_process_executable(${PROJECT}_process_dishwasher dishwasher process src/process/dishwasher.cpp)
add_process_executable(${PROJECT}_process_customer customer process src/process/customer.cpp)
add_process_executable(${PROJECT}_process_restaurant restaurant process src/process/restaurant.cpp)
add_dependencies(${PROJECT}_process_restaurant ${PROJECT}_process_dishwasher ${PROJECT}_process_customer)
install(TARGETS ${PROJECT}_process_restaurant ${PROJECT}_process_dishwasher ${PROJECT}_process_customer
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Condition Variable Thread  ==============================
add_thread_executable(${PROJECT}_thread_restaurant restaurant thread src/thread/restaurant.cpp)
install(TARGETS ${PROJECT}_thread_restaurant DESTINATION bin/${MY_OUTPUT_DIR}/thread)
