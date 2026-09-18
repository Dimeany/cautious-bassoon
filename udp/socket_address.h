#ifndef SOCKET_ADDRESS_HEADER
#define SOCKET_ADDRESS_HEADER

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <ostream>

namespace udp
{

struct SocketAddress
{
    sockaddr_in storage;

    static SocketAddress create(std::string const& ip, uint16_t port);
    static SocketAddress create(std::string const& ip_and_port);

    // Accessors that convert from struct data to language-friendly types.
    uint16_t get_port() const;
    std::string get_address() const;
};

} // namespace udp

std::ostream& operator<<(std::ostream& os, udp::SocketAddress const& sa);

#endif
