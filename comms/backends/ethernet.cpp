#include "ethernet.h"

comms::backend::Ethernet::Ethernet(udp::SocketAddress ethernet_source, udp::SocketAddress ethernet_dest, bool force) : dest(ethernet_dest) {
    socket = std::make_shared<udp::Socket>(ethernet_source);
    fd = socket->fd;

    if (!force && system("which ping > /dev/null 2>&1") == 0) {
        std::string command = "ping -c1 -s1 -I " + ethernet_source.get_address() + " -- " + ethernet_dest.get_address() + " > /dev/null 2>&1";
        if (system(command.c_str())) {
            std::cerr << "Failed to reach destination IP address from the source. Try changing the source or destination addresses, or pass -f to ignore this message.\n";
            exit(1);
        }
    }
}

int comms::backend::Ethernet::getFd() {
    return fd;
}

size_t comms::backend::Ethernet::write(std::span<char const> data) {
    socket->send(data, dest);
    return data.size();
}

std::vector<char> comms::backend::Ethernet::read() {
    if (buffer_pos < buffer.size()) {
        std::vector<char> output {buffer.data() + buffer_pos, buffer.data() + buffer.size()};
        buffer_pos = buffer.size();
        return output;
    }

    auto opt_packet = socket->receive();
    if (!opt_packet)
        return {};
    std::vector<char>& packet = *opt_packet;
    return packet;
}

bool comms::backend::Ethernet::bytesAvailable() {
    return buffer_pos < buffer.size();
}
