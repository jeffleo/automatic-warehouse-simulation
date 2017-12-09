cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})            # create new project with name PROJECT

set(CMAKE_CXX_STANDARD 11)     # use C++11 standard
include(../../config/Macros.cmake)    # add_process_executable and add_thread_executable

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

# directories to search for header files
include_directories(../../include)

#==============  Basics ===============================
set(TARGET_NAME ${PROJECT}_basics)
set(TARGET_SOURCES src/basics/main.cpp)

# add target and customize output name (otherwise would be given target name by default)
add_thread_executable(${TARGET_NAME} basics . ${TARGET_SOURCES})
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})


#==============  Object Oriented ===============================
set(TARGET_NAME ${PROJECT}_objectoriented)
set(TARGET_SOURCES src/objectoriented/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
target_link_libraries(${TARGET_NAME} Threads::Threads)      # link with threads library
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "objectoriented")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})