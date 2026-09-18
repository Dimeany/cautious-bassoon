#ifndef CDH_TEMP_SENSOR_HEADER
#define CDH_TEMP_SENSOR_HEADER

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <cstring>


#define AVIONICS_ADDRESS 0x48 // I2C address for avionics 
#define FLIGHT_ADDRESS 0x49 // I2C address for flight computer
#define IGSE_ADDRESS 0x4a // I2C address for IGSE
// #define BURN_ADDRESS 0x4b // I2C address for burnwire board
#define TEMP_REG 0x00 // Temperature register
#define TEMP_BUS 11 // Temperature sensor bus


namespace cdh{

    class PCT2075TP {
    public:
        PCT2075TP(int bus, int address);
        ~PCT2075TP();
        float readTemperature();
        int i2cBus;
        int i2cAddress;
        int i2cFile;
        bool openI2CBus();
        void closeI2CBus();
        int readRegister(uint8_t reg, int &tempRaw);
        bool read_flag = false;
    };
}

#endif