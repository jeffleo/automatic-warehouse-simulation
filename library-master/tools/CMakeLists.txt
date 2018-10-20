cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

set(CMAKE_CXX_STANDARD 11)
include(../config/Macros.cmake)

# directories to search for header files
include_directories(../include)

#==============  SEM UNLINK ===============================
add_thread_executable(${PROJECT}_sem_unlink sem_unlink . src/sem_unlink.cpp)
install(TARGETS ${PROJECT}_sem_unlink DESTINATION bin/${MY_OUTPUT_DIR})

#==============  SHM UNLINK ===============================
add_process_executable(${PROJECT}_shm_unlink shm_unlink . src/shm_unlink.cpp)
install(TARGETS ${PROJECT}_shm_unlink DESTINATION bin/${MY_OUTPUT_DIR})