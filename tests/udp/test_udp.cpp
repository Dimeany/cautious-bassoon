#include "udp.h"
#include <gtest/gtest.h>

TEST(UDP, packet_create_from_string)
{
    std::string payload_string = "Hello world!";
    udp::Packet p = udp::Packet::create_from_buffer(
        payload_string,
        50000,
        54000,
        25,
        0
    );
    EXPECT_EQ(sizeof(udp::PacketHeader) + payload_string.size(), p.header.length);
    EXPECT_EQ(50000, p.header.destination_port);
    EXPECT_EQ(54000, p.header.source_port);
    EXPECT_EQ(25, p.header.sequence_number);

    // Expect time of constructed packet to be within 1 second of now
    auto now = time(nullptr);
    EXPECT_LE(now - 1, p.header.time);
    EXPECT_GE(now, p.header.time);

    EXPECT_EQ(payload_string, p.get_payload_string());
}

#pragma pack(push, 1)
struct MyStruct {
    int n;
    char ch;
    double d;
};
#pragma pack(pop)

TEST(UDP, packet_create_from_struct)
{
    MyStruct my_struct {
        5,
        'X',
        0.3
    };
    udp::Packet p = udp::Packet::create_from_struct(
        my_struct,
        50000,
        54000,
        25,
        0
    );
    EXPECT_EQ(sizeof(udp::PacketHeader) + sizeof(my_struct), p.header.length);
    EXPECT_EQ(50000, p.header.destination_port);
    EXPECT_EQ(54000, p.header.source_port);
    EXPECT_EQ(25, p.header.sequence_number);

    // Expect time of constructed packet to be within 1 second of now
    auto now = time(nullptr);
    EXPECT_LE(now - 1, p.header.time);
    EXPECT_GE(now, p.header.time);

    auto payload_struct = p.get_payload_struct<MyStruct>();
    EXPECT_EQ(my_struct.n, payload_struct.n);
    EXPECT_EQ(my_struct.ch, payload_struct.ch);
    EXPECT_EQ(my_struct.d, payload_struct.d);
}

TEST(UDP, packet_serialize_deserialize) {
    std::string payload_string = "Hello world!";
    udp::Packet orig_packet = udp::Packet::create_from_buffer(
        payload_string,
        50000,
        54000,
        25,
        0
    );

    auto buf = orig_packet.serialize();
    auto packet = udp::Packet::deserialize(buf);

    EXPECT_EQ(sizeof(udp::PacketHeader) + payload_string.size(), packet.header.length);
    EXPECT_EQ(50000, packet.header.destination_port);
    EXPECT_EQ(54000, packet.header.source_port);
    EXPECT_EQ(25, packet.header.sequence_number);

    // Expect time of constructed packet to be within 1 second of now
    auto now = time(nullptr);
    EXPECT_LE(now - 1, packet.header.time);
    EXPECT_GE(now, packet.header.time);

    EXPECT_EQ(payload_string, packet.get_payload_string());
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
