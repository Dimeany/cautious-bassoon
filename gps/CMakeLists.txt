if("GPS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)

# GPS SERVICE EXECUTABLE
add_executable(gps_service
    gps_service.cpp
    main.cpp
)
# Include the include directory
target_include_directories(gps_service
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# Link Libraryies
target_link_libraries(gps_service
    udp
    socket
    novatel
    uart
    project_utils
    "/usr/local/lib/libgpiodcxx.so"
)
install(
    TARGETS gps_service
    COMPONENT binaries
)
# GPS DRIVER TEST
add_executable(test_gps_driver
    gps_service.cpp
    test_gps_driver.cpp
)
# Include the include directory
target_include_directories(test_gps_driver
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# Link Libraryies
target_link_libraries(test_gps_driver
    udp
    socket
    novatel
    uart
    project_utils
    "/usr/local/lib/libgpiodcxx.so"
)

endif()
