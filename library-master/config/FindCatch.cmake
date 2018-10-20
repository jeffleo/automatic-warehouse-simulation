# Look for Catch unit test library
find_path(CATCH_INCLUDE_DIR catch/catch.hpp
        HINTS
        $ENV{CATCH_ROOT}/include
        ${CATCH_ROOT}/include
        )
mark_as_advanced(CATCH_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Catch DEFAULT_MSG CATCH_INCLUDE_DIR)

if(CATCH_FOUND)
    set(CATCH_INCLUDE_DIRS ${CATCH_INCLUDE_DIR})
endif()