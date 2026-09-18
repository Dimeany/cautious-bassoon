#ifndef SOCKET_HEADER
#define SOCKET_HEADER

#include <arpa/inet.h>
#include <chrono>
#include <netinet/in.h>
#include <optional>
#include <span>
#include <sys/socket.h>
#include <unistd.h>

#include "packet.h"
#include "socket_address.h"
#include "socket_exception.h"

namespace udp
{

struct Socket {
    int fd;
    udp::SocketAddress bound_address;

    // Constructor and destructor respectively open and close socket file.
    Socket(const SocketAddress &socket_address);
    ~Socket();

    // Delete copy constructors.
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    // Send data.
    void send(std::span<char const> span, const SocketAddress &destination);

    // Receive data.  May also return the sender's address as an out parameter.
    std::optional<std::vector<char>> receive();
    std::optional<std::vector<char>> receive(SocketAddress &src_socket_address);

    // Set the socket timeout.
    void set_timeout(std::chrono::seconds);
};

} // namespace udp

/**
 * @brief Contains the address port pairs that are important for flat sat setup.
 * These will change and ultimately will be all local address.
 * To use these address pass them into the string_to_sockaddr_in function
 */
namespace subsystem_address
{
// filled on 2024-3-5 using ssh_guide.md
const udp::SocketAddress WriteOnly = udp::SocketAddress::create("0.0.0.0:0");
const udp::SocketAddress ComsService =
    udp::SocketAddress::create("0.0.0.0:50000"); // TODO update
const udp::SocketAddress ComsServiceReliable =
    udp::SocketAddress::create("0.0.0.0:56000"); // TODO update
const udp::SocketAddress ComsUdpCapture =
    udp::SocketAddress::create("192.168.0.97:50001");
const udp::SocketAddress CdhService =
    udp::SocketAddress::create("127.0.0.1:51000");
const udp::SocketAddress CdhUdpCapture =
    udp::SocketAddress::create("127.0.0.1:51001");
const udp::SocketAddress EpsService =
    udp::SocketAddress::create("192.168.2.84:52000");
const udp::SocketAddress EpsUdpCapture =
    udp::SocketAddress::create("192.168.2.84:52001");
const udp::SocketAddress AdcsService =
    udp::SocketAddress::create("192.168.2.95:53000");
const udp::SocketAddress AdcsUdpCapture =
    udp::SocketAddress::create("192.168.2.95:53001");
const udp::SocketAddress DetService =
    udp::SocketAddress::create("0.0.0.0:54000"); // TODO update
const udp::SocketAddress DetUdpCapture =
    udp::SocketAddress::create("0.0.0.0:54001"); // TODO update
const udp::SocketAddress GpsService =
    udp::SocketAddress::create("192.168.2.92:55000");
const udp::SocketAddress GpsUdpCapture =
    udp::SocketAddress::create("192.168.2.92:55001");
} // namespace subsystem_address

#endif
// Header guard
