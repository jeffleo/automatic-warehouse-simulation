#==========================================================
#  CMAKE Setup
#==========================================================

cmake_minimum_required(VERSION 3.1.3)
project(library)

# customize binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # separate binaries from build files

# set default install prefix to source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "default install path" FORCE )
endif()

# blank output directory, will be recursively set as we include files
set(MY_OUTPUT_DIR "")

# Custom CMake Modules: FindMinGW, FindCatch
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/")
# include Doxygen documentation support and custom macros
include(config/Doxygen.cmake)
include(config/Macros.cmake)
warnall()  # enable all warnings

add_subdirectory("examples")
add_subdirectory("tools")

#==========================================================
#  Testing
#==========================================================

# Enable top-level testing
enable_testing()

# Catch tester
find_package(Catch)

add_subdirectory("test")

#==========================================================
#  Testing
#==========================================================

# install header library if not current install directory
if (NOT ${CMAKE_INSTALL_PREFIX} EQUAL ${CMAKE_SOURCE_DIR} )
    install(DIRECTORY include/cpen333 DESTINATION include/)
endif()