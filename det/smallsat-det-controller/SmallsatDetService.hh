#pragma once
#include <DetectorService.hh>
#include <packet.h>

class SmallsatDetectorService : public DetectorService {
public:
    SmallsatDetectorService(int socket_fd);
    ~SmallsatDetectorService();
private:
    virtual std::vector<std::byte> generate_health() override final;
};

SmallsatDetectorService::SmallsatDetectorService(int socket_fd) :
    DetectorService(socket_fd)
{ }

SmallsatDetectorService::~SmallsatDetectorService() { }

std::vector<std::byte> SmallsatDetectorService::generate_health() {
    static uint16_t sequence_number{0};
    // First, get the "base" health packet
    auto base = DetectorService::generate_health();

    // Next, build up the Smallsat header for the packet
    sockaddr_in sin;
    socklen_t len;
    getsockname(
        this->socket_fd,
        (sockaddr*)&sin,
        &len
    );

    uint16_t from = ntohs(sin.sin_port);
    uint16_t to = atoi(getenv("DET_HEALTH_PORT"));
    auto seq_num = sequence_number++;

    auto packet = udp::Packet::create_from_buffer(
        std::span{reinterpret_cast<char*>(base.data()), base.size()},
        to,
        from,
        seq_num,
        0
    );
    auto char_ret = packet.serialize();
    std::vector<std::byte> ret;
    ret.resize(char_ret.size());
    std::memcpy(ret.data(), char_ret.data(), ret.size());
    return ret;
}
