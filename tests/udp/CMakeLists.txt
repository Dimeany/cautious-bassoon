find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_udp
    test_udp.cpp
)
target_link_libraries(
    test_udp
    udp
    gtest
    pthread
)

add_executable(test_socket
    test_socket.cpp
)
target_link_libraries(
    test_socket
    udp
    socket
    gtest
    pthread
)

gtest_discover_tests(test_udp)
gtest_discover_tests(test_socket)
