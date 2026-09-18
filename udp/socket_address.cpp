#include "socket_address.h"
#include "socket_exception.h"

#include <cstring>

using namespace udp;

// Construct a socket address from an IP and a port
SocketAddress SocketAddress::create(const std::string &ip, uint16_t port) {
    auto storage = sockaddr_in {};

    storage.sin_family = AF_INET;
    storage.sin_port = htons(port);

    int inet_pton_result = inet_pton(AF_INET, ip.c_str(), &storage.sin_addr);
    if (inet_pton_result == 0) {
        throw SocketException("Invalid IP in Socket construction");
    } else if (inet_pton_result == -1) {
        throw SocketException(
            std::string("inet_pton() error: ") + std::strerror(errno)
        );
    }

    return SocketAddress {
        storage,
    };
}

// Construct a socket address from an "ip:port" string.
SocketAddress SocketAddress::create(const std::string &ip_and_port) {
    size_t colon_pos = ip_and_port.find_first_of(":");
    if (colon_pos == std::string::npos) {
        throw SocketException("String must be of the form \"ip:port\"");
    }

    auto ip = ip_and_port.substr(0, colon_pos);
    auto port = std::stoi(ip_and_port.substr(colon_pos + 1));
    return SocketAddress::create(ip, port);
}

uint16_t SocketAddress::get_port() const {
    return ntohs(storage.sin_port);
}

std::string SocketAddress::get_address() const {
    char addrstr[INET_ADDRSTRLEN] {};
    inet_ntop(AF_INET, &storage.sin_addr, addrstr, INET_ADDRSTRLEN);
    return std::string(addrstr);
}

std::ostream& operator<<(std::ostream& os, udp::SocketAddress const& sa) {
    os << sa.get_address() << ":" << sa.get_port();
    return os;
}
