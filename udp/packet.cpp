#include "packet.h"
#include <cstring>

std::string udp::Packet::get_payload_string() const {
    return std::string(payload.data(), payload.size());
}

udp::Packet udp::Packet::Packet::create_from_buffer(
    std::span<char const> buffer,
    uint16_t destination_port,
    uint16_t source_port,
    uint8_t sequence_number,
    uint8_t status
) {
    Packet result;

    // Set header
    result.header = PacketHeader(
        sizeof(result.header) + buffer.size(),
        destination_port,
        source_port,
        sequence_number,
        status
    );

    // Set payload
    result.payload.resize(buffer.size());
    memcpy(result.payload.data(), buffer.data(), buffer.size());

    return result;
}

std::vector<char> udp::Packet::serialize() const {
    PacketHeader net_byte_order_header {};
    net_byte_order_header.length = htons(header.length);
    net_byte_order_header.destination_port = htons(header.destination_port);
    net_byte_order_header.source_port = htons(header.source_port);
    net_byte_order_header.sequence_number = header.sequence_number;
    net_byte_order_header.status = header.status;
    net_byte_order_header.time = htonl(header.time);

    std::vector<char> result(sizeof(header) + payload.size());
    memcpy(result.data(), &net_byte_order_header, sizeof(header));
    memcpy(result.data() + sizeof(header), payload.data(), payload.size());
    return result;
}

udp::Packet udp::Packet::deserialize(std::span<char const> data) {
    Packet result;

    // Copy header
    PacketHeader net_byte_order_header {};
    memcpy(&net_byte_order_header, data.data(), sizeof(result.header));
    result.header.length = ntohs(net_byte_order_header.length);
    result.header.destination_port = ntohs(net_byte_order_header.destination_port);
    result.header.source_port = ntohs(net_byte_order_header.source_port);
    result.header.sequence_number = net_byte_order_header.sequence_number;
    result.header.status = net_byte_order_header.status;
    result.header.time = ntohl(net_byte_order_header.time);

    // Copy payload
    result.payload.resize(data.size() - sizeof(result.header));
    memcpy(
        result.payload.data(),
        data.data() + sizeof(result.header),
        data.size() - sizeof(result.header)
    );

    return result;
}

std::ostream &udp::operator<<(std::ostream &os, const udp::Packet &packet)
{
    os << "0x";

    char hex_map[]{
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f'
    };

    constexpr unsigned char first_nibble{0xF0};
    constexpr unsigned char second_nibble{0x0F};

    for (char b : packet.serialize()) {
        os << hex_map[static_cast<int>((b & first_nibble) >> 4)];
        os << hex_map[static_cast<int>(b & second_nibble)];
    }

    return os;
}

bool udp::isValidPacket(const std::vector<char> &packet)
{
    return packet.size() >= sizeof(PacketHeader);
}
