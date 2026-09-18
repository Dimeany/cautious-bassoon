find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)


# SENDWAIT EXECUTABLE
add_executable(sendwait
    sendwait.cpp
)
# UDP_ECHO EXECUTABLE
add_executable(udp_echo
    udp_echo.cpp
)
# APPLYHEADER EXECUTABLE
add_executable(applyheader
    applyheader.cpp
)

target_link_libraries(applyheader udp)
# STRIPHEADER EXECUTABLE
add_executable(stripheader
    stripheader.cpp
)
target_link_libraries(stripheader udp)

install(
    TARGETS sendwait udp_echo applyheader stripheader
    COMPONENT binaries
)

# UDP_CAPTURE EXECUTABLE
add_executable(udp_capture 
    udp_capture.cpp
)
# Include the include directory
target_include_directories(udp_capture
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
target_link_libraries(udp_capture
    PRIVATE
    PkgConfig::SYSTEMD
)
install(
    TARGETS udp_capture
    COMPONENT binaries
)
