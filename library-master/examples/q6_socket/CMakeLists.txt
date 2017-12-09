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

#==============  Socket implementation ===============================

add_thread_executable(${PROJECT}_socket socket . src/socket.cpp)
if(WIN32)
    target_link_libraries(${PROJECT}_socket wsock32 ws2_32)
endif()

add_thread_executable(${PROJECT}_chat_client client chat src/chat/client.cpp)
add_thread_executable(${PROJECT}_chat_server server chat src/chat/server.cpp)
if(WIN32)
    target_link_libraries(${PROJECT}_chat_client wsock32 ws2_32)
    target_link_libraries(${PROJECT}_chat_server wsock32 ws2_32)
endif()