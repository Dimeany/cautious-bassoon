if("CDH" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)
    # Define the CDH library first
    add_library(cdh
        struct_decode_operator.cpp
    )

    # Include directories for the CDH library
    target_include_directories(cdh
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Link libraries for the CDH library
    target_link_libraries(cdh
        adcs
    )

    # Define the CDH service executable
    add_executable(cdh_service
        cdh_service.cpp
        cdh_temp_sensor.cpp
        bounds_loader.cpp
        ${UTIL_SOURCES}
    )

    # Include directories for the CDH service executable
    target_include_directories(cdh_service
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Link libraries for the CDH service executable
    target_link_libraries(cdh_service
        socket
        udp
        adcs
        eps
        det-messages
        cdh
        i2c
        project_utils
    )

    # Define the CDH service exact executable
    add_executable(exact_cdh_service
        exact_cdh_service.cpp
        cdh_temp_sensor.cpp
        bounds_loader.cpp
        ${UTIL_SOURCES}
    )

    # Include directories for the CDH service exact executable
    target_include_directories(exact_cdh_service
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Link libraries for the CDH service exact executable
    target_link_libraries(exact_cdh_service
        socket
        udp
        adcs
        eps
        det-messages
        cdh
        i2c
        project_utils
    )

    # Install the CDH service executables
    install(
        TARGETS cdh_service exact_cdh_service
        COMPONENT binaries
    )
endif()
