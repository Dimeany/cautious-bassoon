#include "packet_header.h"
#include <cstdint>

std::ostream &
operator<<(std::ostream &os, const udp::PacketHeader &packet_header)
{
    os << "length: " << packet_header.length << "\n";
    os << "dest: " << packet_header.destination_port << "\n";
    os << "source: " << packet_header.source_port << "\n";
    os << "time: " << packet_header.time << "\n";
    os << "sequence number: " << packet_header.sequence_number << "\n";
    return os;
}

udp::PacketHeader::PacketHeader(
    uint16_t length_,
    uint16_t destination_port_,
    uint16_t source_port_,
    uint8_t sequence_number_,
    uint8_t status_
)
    : length(length_)
    , destination_port(destination_port_)
    , source_port(source_port_)
    , status(status_)
    , sequence_number(sequence_number_)
{
    this->time = ::time(nullptr);
}
