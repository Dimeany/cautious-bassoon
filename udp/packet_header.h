#ifndef PACKET_HEADER_HEADER
#define PACKET_HEADER_HEADER

#include <cstdint>
#include <ostream>
#include <type_traits>

namespace udp
{

#pragma pack(push, 1)
/**
 * @brief The packet header is a header that all packets passed by the sockets
 * will contain. When a member of the Packet class it will be in network byte
 * order. When a member of the WrappedPacket class it will be in host byte
 * order.
 * @details The length field constains the length of the packet header plus the
 * length of the data payload. The destination_port field contains the port of
 * destination. The source_port_and_packet_type is the port sent from and the
 * packet type. The integer division with 1000 is the source port and the
 * modulus with 1000 is the packet type. The PacketType enum class defines the
 * packet types possible.
 */
struct PacketHeader {
    uint16_t length;
    uint16_t destination_port;
    uint16_t source_port;
    uint8_t status;
    uint8_t sequence_number;
    uint32_t time;

    PacketHeader(
        uint16_t length_,
        uint16_t destination_port_,
        uint16_t source_port_,
        uint8_t sequence_number_,
        uint8_t status
    );
    PacketHeader() = default;
};
#pragma pack(pop)

static_assert(12 == sizeof(PacketHeader),
              "PacketHeader is the wrong size");
static_assert(std::is_trivially_copyable_v<PacketHeader>,
              "PacketHeader must be trivially copyable");

std::ostream &
operator<<(std::ostream &os, const udp::PacketHeader &packet_header);

} // namespace udp

#endif
