#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include "../../udp/socket.h"
#include "backend.h"

namespace comms {
    namespace backend {

        class Ethernet : public Backend {
            int fd = -1;
          public:
            std::shared_ptr<udp::Socket> socket;
            udp::SocketAddress dest;

            std::vector<char> buffer;
            size_t buffer_pos;

            Ethernet(udp::SocketAddress ethernet_source, udp::SocketAddress ethernet_dest, bool force);

            int getFd() override;
            size_t write(std::span<char const> data) override;
            std::vector<char> read() override;
            bool bytesAvailable() override;
        };
    }
}

#endif