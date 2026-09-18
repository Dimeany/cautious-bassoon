#include "cdh_temp_sensor.h"
#include <string>


cdh::PCT2075TP::PCT2075TP(int bus, int address) : i2cBus(bus), i2cAddress(address) {
    openI2CBus();
}

cdh::PCT2075TP::~PCT2075TP() {
    closeI2CBus();
}

bool cdh::PCT2075TP::openI2CBus() {
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", i2cBus);
    i2cFile = open(filename, O_RDWR);
    if (i2cFile < 0) {
        std::cerr << "Failed to open the i2c bus" << std::endl;
        return false;
    }
    if (ioctl(i2cFile, I2C_SLAVE, i2cAddress) < 0) {
        std::cerr << "Failed to acquire bus access and/or talk to slave" << std::endl;
        return false;
    }
    return true;
}

void cdh::PCT2075TP::closeI2CBus() {
    if (i2cFile >= 0) {
        close(i2cFile);
    }
}

int cdh::PCT2075TP::readRegister(uint8_t reg, int &tempRaw) {
    uint8_t buf[2];

    // Set the register pointer
    if (ioctl(i2cFile, I2C_SLAVE, i2cAddress) < 0) {
        perror("Failed to set I2C address");
        return -1;
    }

    if (write(i2cFile, &reg, 1) != 1) {
        perror(("Failed to write register (" + std::to_string(reg) + ")").c_str());
        return -1;
    }

    // Read the data
    if (read(i2cFile, buf, 2) != 2) {
        perror("Failed to read data");
        return -1;
    }

    // Combine the data from the two bytes
    tempRaw = (buf[0] << 8) | buf[1];
    return 0;
}

float cdh::PCT2075TP::readTemperature() {
    read_flag = false;
    int tempRaw = 0;
    if (readRegister(TEMP_REG,tempRaw)){
        float temperature = 0;
        read_flag = true;
        return temperature;
    }

    // Extract 11-bit temperature data from the 16-bit raw data
    int tempData = (tempRaw >> 5) & 0x7FF; // Disregard 5 LSB of the LSByte

    // Check the sign bit
    if (tempRaw & 0x8000) { // If the MSByte’s 11th bit is 1, it's a negative temperature
        tempData = -(0x800 - tempData); // Two's complement conversion for negative value
    }

    // Convert to Celsius
    float temperature = tempData * 0.125;
    return temperature;
}
