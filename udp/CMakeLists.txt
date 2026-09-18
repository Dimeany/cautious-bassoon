# UDP LIBRARY
add_library(udp
    packet.cpp
    packet_header.cpp
)
target_include_directories(udp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# SOCKET LIBRARY
add_library(socket
    socket.cpp
    socket_address.cpp
)
target_link_libraries(socket
    udp
)
target_include_directories(socket
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


add_subdirectory(util)
