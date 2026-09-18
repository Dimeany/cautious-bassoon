#include <gtest/gtest.h>
#include <memory>
#include <DetectorService.hh>
#include <IoContainer.hh>

class TestableDetectorService : public DetectorService {
public:
    using DetectorService::DetectorService;

    std::vector<std::byte> expose_generate_health() {
        return generate_health();
    }
};

class ThrowingHealthService : public DetectorService {
public:
    using DetectorService::DetectorService;

    std::vector<std::byte> generate_health() override {
        throw std::runtime_error{"simulated health failure"};
    }

    using DetectorService::has_health_timer;
};

TEST(detservice, PpsDetect) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    auto ser = std::make_unique<DetectorService>(socket_fd);
    bool had_pps = ser->await_pps_edge();
    EXPECT_TRUE(had_pps) << "PPS could not be detected";
    close(socket_fd);
}

TEST(detservice, HealthGenerationWithoutControllers) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    auto ser = std::make_unique<TestableDetectorService>(socket_fd);

    auto health = ser->expose_generate_health();

    EXPECT_EQ(health.size(), sizeof(DetectorMessages::HealthPacket));
    close(socket_fd);
}

TEST(detservice, StartPeriodicHealthKeepsTimerAfterHealthFailure) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    auto ser = std::make_unique<ThrowingHealthService>(socket_fd);

    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(0);
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    ser->push_message(DetectorMessages::StartPeriodicHealth{
        .seconds_between = 1,
        .fwd = {dest},
    });
    ser->evt_loop_step();

    EXPECT_TRUE(ser->has_health_timer());
    close(socket_fd);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
