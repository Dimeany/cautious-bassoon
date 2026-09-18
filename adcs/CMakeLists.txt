if("ADCS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)

# ADCS SERVICE EXECUTABLE
add_executable(adcs_service
    adcs_service.cpp
    messages_json.cpp
)
# Include the include directory
target_include_directories(adcs_service
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# Link Libraryies
target_link_libraries(adcs_service
    # thread_safe_queue
    udp
    socket
    adcs
    i2c
    pthread
    project_utils
    CubeSpaceTablesJson
)
install(
    TARGETS adcs_service
    COMPONENT binaries
)

# GenerateCppCode Executable
add_executable(
    generateCppCode
    generateCppCode.cpp
)
target_link_libraries(
    generateCppCode
    CubeSpaceTablesJson
)

add_executable(
    sendCubeComputerCommand
    sendCubeComputerCommand.cpp
)
target_link_libraries(
    sendCubeComputerCommand
    CubeSpaceTablesJson
)


### ADCS Library ###
add_library(adcs
    struct_decode_operators.cpp
)

# Include the include directory
target_include_directories(adcs
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

endif()
