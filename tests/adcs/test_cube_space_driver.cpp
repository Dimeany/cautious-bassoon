#include "cube_space_types.h"
#include "cube_space_driver.h"
#include "struct_decode_operators.h"
#include "i2c.h"

#include <memory>
#include <iostream>

#include <gtest/gtest.h>

TEST(CubeSpaceDriver, i2c_driver_open) {
    uint8_t port_id = 10;
    const char* port = "/dev/i2c-1";
    bool ten_bit_addr = false;
    bool packet_error_checking = true;
    uint64_t retries =  5;
    uint64_t timeout = 100;

    I2C_Setting main_settings(
        port_id, 
        port, 
        ten_bit_addr, 
        packet_error_checking,
        retries,
        timeout);
    std::shared_ptr<I2C> i2c_driver_ptr = std::make_shared<I2C>(main_settings);
    int result = i2c_driver_ptr->open();
    EXPECT_EQ(result, 0);
}

int main(int argc, char *argv[]){
    // uint8_t port_id = 10;
    // const char* port = "/dev/i2c-1";
    // bool ten_bit_addr = false;
    // bool packet_error_checking = true;
    // uint64_t retries =  5;
    // uint64_t timeout = 100;

    // I2C_Setting main_settings(
    //     port_id, 
    //     port, 
    //     ten_bit_addr, 
    //     packet_error_checking,
    //     retries,
    //     timeout);
    // std::shared_ptr<I2C> i2c_driver_ptr = std::make_shared<I2C>(main_settings);
    // i2c_driver_ptr->open();
    // CubeSpaceDriver driver = CubeSpaceDriver(i2c_dritest_udp_packet_headerver_ptr);

    // adcs::ADCSStateTelemetryFormat state = driver.read<adcs::ADCSStateTelemetryFormat>();
    // std::cout << state << "\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}