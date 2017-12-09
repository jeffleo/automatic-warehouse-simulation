cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

set(CMAKE_CXX_STANDARD 14)        # We need C++14 for shared_mutex!!
include(../../config/Macros.cmake)

# directories to search for header files
include_directories(../../include)

#==============  Process Reader/Writer ===============================
add_process_executable(${PROJECT}_process_read read_priority process src/process/read_priority.cpp)
add_process_executable(${PROJECT}_process_write write_priority process src/process/write_priority.cpp)
add_process_executable(${PROJECT}_process_fair fair_priority process src/process/fair_priority.cpp)
install(TARGETS ${PROJECT}_process_read ${PROJECT}_process_write ${PROJECT}_process_fair
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Thread Reader/Writer ===============================
add_thread_executable(${PROJECT}_thread_read read_priority thread src/thread/read_priority.cpp)
add_thread_executable(${PROJECT}_thread_write write_priority thread src/thread/write_priority.cpp)
add_thread_executable(${PROJECT}_thread_fair fair_priority thread src/thread/fair_priority.cpp)
install(TARGETS ${PROJECT}_thread_read ${PROJECT}_thread_write ${PROJECT}_thread_fair
        DESTINATION bin/${MY_OUTPUT_DIR}/thread)

