#include <sstream>

#include "socket.h"

namespace udp
{

Socket::Socket(const SocketAddress &socket_address)
    : bound_address{socket_address}
{
    // Create file descriptor
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::string error_str = "failed to return valid file descriptor from "
                                "socket() call: ";
        error_str += std::strerror(errno);
        throw SocketException(error_str);
    }

    // Bind to address
    int result = bind(
        fd,
        (sockaddr *)&bound_address.storage,
        sizeof(bound_address.storage)
    );
    if (result < 0) {
        // Construct nice error string
        auto errno_str = std::strerror(errno);

        char ip_addr_str[INET_ADDRSTRLEN] {};
        inet_ntop(
            AF_INET,
            &(bound_address.storage.sin_addr),
            ip_addr_str,
            sizeof(ip_addr_str)
        );

        std::stringstream err;
        err << "Failed to bind socket internet address.\n"
            << "Error: " << errno_str << "\n"
            << "address: " << ip_addr_str << "\n"
            << "port: " << bound_address.get_port();

        throw SocketException(err.str());
    }
}

Socket::~Socket() {
    close(fd);
}

void Socket::send(std::span<char const> data, const SocketAddress &destination) {
    sockaddr_in dest = destination.storage;
    int result = sendto(
        fd,
        data.data(),
        data.size(),
        MSG_CONFIRM,
        (struct sockaddr *)&dest,
        sizeof(dest)
    );
    if (result < 0) {
        throw SocketException(std::string("Failed to send data: ") + std::strerror(errno));
    }
}

std::optional<std::vector<char>> Socket::receive() {
    SocketAddress unused_address{};
    return receive(unused_address);
}

std::optional<std::vector<char>>
Socket::receive(SocketAddress &src_socket_address) {
    static constexpr size_t MAX_BYTES = std::numeric_limits<uint16_t>::max();
    auto buffer = std::vector<char>(MAX_BYTES);

    socklen_t recv_address_length = sizeof(src_socket_address.storage);
    int rc = recvfrom(
        fd,
        buffer.data(),
        buffer.size(),
        MSG_WAITALL,
        (sockaddr *)&src_socket_address.storage,
        &recv_address_length
    );
    if (rc < 0) {
        if (errno == EAGAIN) {
            // Read timed out.
            return {};
        }
        throw SocketException{
            "Error receiving: " + std::string{strerror(errno)}
        };
    }

    buffer.resize(rc);
    return buffer;
}

void Socket::set_timeout(std::chrono::seconds timeout_value) {
    struct timeval tv {
        .tv_sec = static_cast<long int>(timeout_value.count()),
        .tv_usec = 0,
    };

    int rc = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (rc < 0) {
        throw SocketException {
            "Error setting timeout: " + std::string{strerror(errno)}
        };
    }
}

} // namespace udp
