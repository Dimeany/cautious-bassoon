# Vendor

### Keeping vendor libraries in their own folders to make includes more explicit

###### Novatel ######

if("GPS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)
    add_subdirectory(novatel SYSTEM)
endif()

###### Cube Space ######

if("ADCS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)
    add_library(
            CubeSpaceTablesJson
            cubeSpace/command_sender.cpp
            cubeSpace/tables_json.cpp
    )
    target_include_directories(
        CubeSpaceTablesJson
        SYSTEM PUBLIC
             cubeSpace
    )
    set_target_properties(
        CubeSpaceTablesJson
        PROPERTIES
            COMPILE_WARNING_AS_ERROR OFF
    )
    target_compile_options(
        CubeSpaceTablesJson
        PRIVATE
            "-w"
    )
endif()


if("EPS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)
    add_subdirectory(SpaceInventor SYSTEM)
endif()

add_custom_target(
    norm ALL
    COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/norm/makefiles/Makefile.linux
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/norm/makefiles
)

install(
    PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/norm/makefiles/norm
    TYPE BIN
)
