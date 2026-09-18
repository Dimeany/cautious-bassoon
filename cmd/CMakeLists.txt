if("CMD" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)

add_executable(cmd_service
    cmd_service.cpp
    ../util/environment_util.cpp
)
target_include_directories(cmd_service PRIVATE ${SYSTEMD_INCLUDE_DIRS})
target_link_libraries(cmd_service
    udp
    socket
    ${SYSTEMD_LIBRARIES}
)
install(TARGETS cmd_service)



endif()
