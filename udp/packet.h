#ifndef PACKET_HEADER
#define PACKET_HEADER

#include <arpa/inet.h>
#include <cstdint>
#include <stdint.h>

#include <cstddef>
#include <cstring>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "packet_header.h"
#include "socket_exception.h"

namespace ppp
{
class Packet;
}

namespace udp
{

// Forward declaration of socket class to make friend
class Socket;

/**
 * @brief Constructs a packet to be sent over udp. Packet header will NOT be in
 * network byte order.
 */
struct Packet {
    PacketHeader header;
    std::vector<char> payload;

    // Return the payload as a string.
    std::string get_payload_string() const;

    // Return the payload as a struct.
    template<typename T>
    T get_payload_struct() const {
        static_assert(std::is_trivially_copyable_v<T>);

        T result;
        if (sizeof(result) != payload.size()) {
            std::stringstream error_message;
            error_message << "Packet: tried to get struct of size " << sizeof(result)
                          << " but  payload was of size " << payload.size();
            throw SocketException(error_message.str());
        }
        memcpy(&result, payload.data(), payload.size());
        return result;
    }

    // Construct a packet from a payload buffer
    static Packet create_from_buffer(
        std::span<char const> buffer,
        uint16_t destination_port,
        uint16_t source_port,
        uint8_t sequence_number,
        uint8_t status
    );

    // Construct a packet from a struct
    template<typename T>
    static Packet create_from_struct(
        const T &data_payload,
        uint16_t destination_port,
        uint16_t source_port,
        uint8_t sequence_number,
        uint8_t status
    ) {
        static_assert(std::is_trivially_copyable_v<T>);
        return create_from_buffer(
            std::span<char const>(reinterpret_cast<char const*>(&data_payload), sizeof(data_payload)),
            destination_port,
            source_port,
            sequence_number,
            status
        );
    }

    // Serialize packet.  Member function, since it serializes an
    // existing packet object.
    std::vector<char> serialize() const;

    // Deserialize packet.  Static function, since it deserializes
    // from an existing byte array and gives you a new packet.
    static Packet deserialize(std::span<char const> data);

    /**
     * @brief Print out all bytes as a hex string.
     *
     * @param os
     * @param header
     * @return std::ostream&
     */
    friend std::ostream &operator<<(std::ostream &os, const Packet &packet);
};

std::ostream &operator<<(std::ostream &os, const Packet &packet);

/**
 * @brief check to see if a packet is at least the size of a header
 *
 * @param packet
 * @return true
 * @return false
 */
bool isValidPacket(const std::vector<char> &packet);

} // namespace udp

#endif
