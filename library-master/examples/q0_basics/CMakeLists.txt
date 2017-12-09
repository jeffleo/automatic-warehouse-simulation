cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})            # create new project with name PROJECT

set(CMAKE_CXX_STANDARD 11)      # use C++11 standard

# directories to search for header files
include_directories(../../include)

# customize output directory to be within a "bin/<project>" folder, relative to the BASE CMakeLists.txt file
#   (i.e. if this project is added to another top-level one, the bin folder will be relative to the top level)
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

#==============  Objects ===============================
# create executable with unique target label, add all sources
set(TARGET_NAME ${PROJECT}_objects)
set(TARGET_SOURCES src/objects/main.cpp src/objects/Animal.cpp src/objects/Cat.cpp src/objects/Dog.cpp)

# add target and customize output name (otherwise would be given target name by default)
add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "objects")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})

#==============  Namespaces ===============================
set(TARGET_NAME ${PROJECT}_namespaces)
set(TARGET_SOURCES src/namespaces/main.cpp src/namespaces/foo.cpp src/namespaces/bar.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "namespaces")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})

#==============  RAII ===============================
set(TARGET_NAME ${PROJECT}_raii)
set(TARGET_SOURCES src/RAII/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "raii")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})

#==============  Templates ===============================
set(TARGET_NAME ${PROJECT}_templates)
set(TARGET_SOURCES src/templates/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "templates")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})

#==============  Containers ===============================
set(TARGET_NAME ${PROJECT}_containers)
set(TARGET_SOURCES src/containers/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "containers")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})

#==============  Advanced ===============================
set(TARGET_NAME ${PROJECT}_advanced)
set(TARGET_SOURCES src/advanced/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "advanced")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})

#==============  Console ===============================
set(TARGET_NAME ${PROJECT}_console)
set(TARGET_SOURCES src/console/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME}  PROPERTIES OUTPUT_NAME "console")
install(TARGETS ${TARGET_NAME} DESTINATION bin/${MY_OUTPUT_DIR})
