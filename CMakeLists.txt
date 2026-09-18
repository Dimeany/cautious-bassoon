cmake_minimum_required(VERSION 3.7...3.27)

# the 3.7...3.27 range syntax is only suported past version 3.12
# so if version is less than 3.12 the ...3.27 is ignored and VERSION is 3.7
# this if block makes sure we have the most up to date polices for the version
if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(smallsat_lib VERSION 1.0
                     DESCRIPTION "Library that is common to all subsystems of flight software"
                     LANGUAGES C CXX)

# If the user passes -DSTRIP_BINARIES=ON for this configure,
# read the value, then remove it from the cache so it isn't persisted.
# Use a local variable (`_strip_binaries`) for the stripping decision.
set(_strip_bin_default OFF)
if(DEFINED STRIP_BINARIES)
    set(_strip_binaries "${STRIP_BINARIES}")
    unset(STRIP_BINARIES CACHE)
else()
    set(_strip_binaries "${_strip_bin_default}")
endif()

if(_strip_binaries)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
endif()

set(COMMON_LIBRARY_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR})
# Export for other projects to use
set(COMMON_LIBRARY_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SMALLSAT_SUBSYSTEMS is a list of subsystems to build.  If empty,
# build all subsystems.
set(SMALLSAT_SUBSYSTEMS "" CACHE STRING "List of subsystems to build.  Default: empty string, which builds all subsystems.")

# SMALLSAT_SUBSYSTEMS_LIST is a non-cached variable.  For the case of
# building all subsystems, caching the empty string and setting a new
# variable with the full list allows us to implement the
# least-surprising behavior of continuing to build all subsystems if
# we add another one, rather than building only the set of subsystems
# that had existed before.
if("${SMALLSAT_SUBSYSTEMS}" STREQUAL "")
    set(SMALLSAT_SUBSYSTEMS_LIST "ADCS;CDH;CMD;COMMS;DET;EPS;GPS")
else()
    set(SMALLSAT_SUBSYSTEMS_LIST "${SMALLSAT_SUBSYSTEMS}")
endif()
message("Building subsystems: ${SMALLSAT_SUBSYSTEMS_LIST}")

# Print STRIP_BINARIES status for this configure 
if(_strip_binaries)
    message("STRIP_BINARIES=ON for this configure: All linked executables and shared libs will be stripped at link time")
else()
    message("STRIP_BINARIES=OFF for this configure: Binaries will keep symbols (no -s added)")
endif()

# Custom target to install only the "binaries" component
add_custom_target(install-binaries COMMAND cmake --install . --component binaries)

set(UTIL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/util/status_util.cpp
)

###############################################################################
# toplevel function definitions

# Description: Build a standard executable
# Note: Must double quote lists to pass them in.
function(BUILD_SIMPLE_EXECUTABLE
NAME # string
INCLUDE_PATHS # list: set of file paths to include
LINKS # list: set of libraries to link against
SRC_FILES # list: set of source files to build
)
    add_executable(${NAME} "${SRC_FILES}")
    target_include_directories(${NAME} PRIVATE "${INCLUDE_PATHS}")
    target_link_libraries(${NAME} ${LINKS})
endfunction()

# Description: Build a basic library.
# Note: Must double quote lists to pass them in.
function(BUILD_STANDALONE_LIBRARY
NAME # string
INCLUDE_PATHS # list: set of file paths to include
SRC_FILES # list: set of source files to build
)
    add_library(${NAME} "${SRC_FILES}")
    target_include_directories(${NAME} PRIVATE "${INCLUDE_PATHS}")
endfunction()

# Description: Build a basic test.
# Note: Must double quote lists to pass them in.
function(BUILD_SIMPLE_TEST
NAME # string
INCLUDE_PATHS # list: set of file paths to include
LINKS # list: set of libraries to link against
SRC_FILES # list: set of source files to build
)
    find_package(GTest REQUIRED)
    list(APPEND INCLUDE_PATHS "${GTEST_INCLUDE_DIRS}")
    BUILD_SIMPLE_EXECUTABLE(${NAME} "${INCLUDE_PATHS}" "${LINKS}" "${SRC_FILES}")
    gtest_discover_tests(${NAME})
endfunction()
# end toplevel function definitions
###############################################################################

add_compile_options(-Wall -Wextra -Wpedantic)

# Way to envoke from cmd line: GET_DEPS_GH=1 cmake ..
if("$ENV{GET_DEPS_GH}" EQUAL 1)
    message("Fetching dependancies from github...")
    message("Fetching googletest")
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG f8d7d77 # release-1.14
    )
    FetchContent_MakeAvailable(googletest)
else()
    message("Not trying to fetch dependancies from github.")
endif()

enable_testing()
find_package(GTest)

###############################################################################
# Generate version header at build time
###############################################################################

# Create generated directory for version header
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

# Get git version information
execute_process(
    COMMAND git describe --tags --long --dirty=+
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    RESULT_VARIABLE GIT_RESULT
)

# If git command failed, use a default dev version
if(NOT GIT_RESULT EQUAL 0 OR GIT_VERSION STREQUAL "")
    set(GIT_VERSION "dev")
endif()

# Get current date
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

# Create the version string
set(NEBULA_VERSION "Nebula-${GIT_VERSION}-${BUILD_DATE}")
message(STATUS "Building Nebula version: ${NEBULA_VERSION}")

# Generate version.h header
file(WRITE "${GENERATED_DIR}/version.h"
    "#ifndef NEBULA_VERSION_H\n"
    "#define NEBULA_VERSION_H\n"
    "\n"
    "#define NEBULA_VERSION \"${NEBULA_VERSION}\"\n"
    "\n"
    "#endif // NEBULA_VERSION_H\n"
)

# Add generated directory to global include paths
include_directories("${GENERATED_DIR}")

# Also set CMAKE_BINARY_DIR/generated in global include to ensure all targets can find it
set_property(GLOBAL PROPERTY CMAKE_GENERATED_INCLUDE "${GENERATED_DIR}")

###############################################################################

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(install)

add_custom_target(
    package
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/package.sh
)

# Install all script files from Nebula/scripts to /usr/local/bin
install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts/
        DESTINATION /usr/local/bin
        FILES_MATCHING
        PATTERN "*"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Install health_bounds.json config file to /usr/local/bin/config
install(FILES ${CMAKE_SOURCE_DIR}/src/cdh/config/health_bounds.json
        DESTINATION /usr/local/bin/config
        COMPONENT configuration)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
add_compile_options(-Wno-unused-parameter)
