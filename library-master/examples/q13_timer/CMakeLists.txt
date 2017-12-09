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

#==============  Simple Timer ===============================
add_thread_executable(${PROJECT}_simple_timer timer simple src/simple/timer.cpp)
install(TARGETS ${PROJECT}_simple_timer DESTINATION bin/${MY_OUTPUT_DIR}/simple)

# assets
set(TIMER_ASSET_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/simple/timer.dat")
set(TIMER_ASSET_DST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simple/timer.dat")
configure_file(${TIMER_ASSET_SRC} ${TIMER_ASSET_DST} COPYONLY)
install(FILES src/simple/timer.dat DESTINATION bin/${MY_OUTPUT_DIR}/simple)

#==============  Process Faucet ===============================
add_process_executable(${PROJECT}_process_faucet faucet process src/process/faucet.cpp)
add_process_executable(${PROJECT}_process_house house process src/process/house.cpp)
add_dependencies(${PROJECT}_process_house ${PROJECT}_process_faucet)
install(TARGETS ${PROJECT}_process_house ${PROJECT}_process_faucet
        DESTINATION bin/${MY_OUTPUT_DIR}/process)

#==============  Thread Faucet  ==============================
add_thread_executable(${PROJECT}_thread_house house thread src/thread/house.cpp)
install(TARGETS ${PROJECT}_thread_house
        DESTINATION bin/${MY_OUTPUT_DIR}/thread)