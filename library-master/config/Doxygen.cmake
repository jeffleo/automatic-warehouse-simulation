# Doxygen documentation build
# Add a "BUILD_DOC" option
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/config/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("-- Configuring Doxygen")

    # custom target "docs" for generating documentation
    if(BUILD_DOC)
        set(ADD_ALL ALL)
    endif(BUILD_DOC)
    add_custom_target( docs ${ADD_ALL}
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    set_target_properties(docs PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
else (DOXYGEN_FOUND)
    if (BUILD_DOC)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif(BUILD_DOC)
endif (DOXYGEN_FOUND)