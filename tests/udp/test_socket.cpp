#include "socket.h"
#include "udp.h"
#include <gtest/gtest.h>

using namespace std::chrono_literals;

TEST(Socket, timeout) {
    auto s = udp::Socket(udp::SocketAddress::create("127.0.0.1", 61000));

    s.set_timeout(1s);

    // Make sure socket returns empty optional on timeout
    auto opt_data = s.receive();
    EXPECT_FALSE(opt_data);
}

TEST(Socket, send_and_receive_data) {
    const udp::SocketAddress local1 = udp::SocketAddress::create("127.0.0.1:51000");
    const udp::SocketAddress local2 = udp::SocketAddress::create("127.0.0.1:52000");
    udp::Socket sock1 = udp::Socket(local1);
    udp::Socket sock2 = udp::Socket(local2);

    // Send and receive
    std::string send_data = "hello i am data to be sent and received";
    sock1.send(send_data, local2);

    udp::SocketAddress sender {};
    auto opt_recv_data = sock2.receive(sender);
    ASSERT_TRUE(opt_recv_data);
    EXPECT_TRUE(sender.get_address() == local1.get_address());
    EXPECT_TRUE(sender.get_port() == local1.get_port());

    std::string recv_data(opt_recv_data->begin(), opt_recv_data->end());
    EXPECT_EQ(recv_data, send_data);

    // Return to sender and receive
    std::string send_response = "Hello, I'm the response!";
    sock2.send(send_response, sender);
    auto opt_recv_response = sock1.receive(sender);
    EXPECT_TRUE(opt_recv_response);

    std::string recv_response(opt_recv_response->begin(), opt_recv_response->end());
    EXPECT_EQ(recv_response, send_response);
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
