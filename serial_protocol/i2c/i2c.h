#ifndef I2C_HEADER
#define I2C_HEADER

#include <mutex>
#include <stdio.h>				// Defines various functions for input/output
#include <stdlib.h>				// Defines various functions. Standard library.
#include <fcntl.h>				// Defines file control variables.
#include <cstdint>				// uint#_t
#include <linux/i2c-dev.h>	 	// Defines i2c controls.
#include <sys/ioctl.h>		   	// Defines the ioctl() function. Reduces various steps into a single function.
#include <unistd.h>		       	// Provides access to POSIX system API.
#include <termios.h>			// tcflush for flushing buffers
#include <errno.h>				// Errno
#include <cstdint>

#include "i2c_setting.h"

enum I2C_Tags { TEN_BIT_ADDR, PACKET_ERROR_CHECKING, RETRIES, TIMEOUT };

class I2C {
public:
	/**
	 * Creates a new I2C driver from a I2C_Setting.
	 * 
	 * @param setting an I2C_Setting
	 */
	I2C(I2C_Setting setting);
	~I2C();

	/**
	 * Opens the port and applies settings to the port.
	 * 
	 * @return 0 on success
	 */
	int open();

	/**
	 * Closes the port if it is open.
	 * 
	 * @return 0 on success
	 */
	int close();

	/**
	 * Flushes the defined buffer.
	 * 
	 * @param buffer 0b01 for input buffer, 0b10 for output buffer, 0b11 for both
	 * @return 0 on success
	 */
	int flush(uint8_t buffer);

	/**
	 * Attempts to read from the device into the buffer.
	 * 
	 * @param buffer_size the number of bytes to be read
	 * @param buffer the buffer to be read into
	 * @param device_address the device to read from
	 * @return 0 on success
	 */
	int read(uint16_t buffer_size, char* buffer, uint8_t device_address);
	
	/**
	 * Attempts to write to the device from the buffer.
	 * 
	 * @param buffer_size the number of bytes to be write
	 * @param buffer the buffer to be written from
	 * @param device_address the device to write to
	 * @return 0 on success
	 */
	int write(uint16_t buffer_size, const char* buffer, uint8_t device_address);

	/**
	 * Returns the value associated with the tag.
	 * 
	 * @param tag the tag for the variable
	 */
	int get(uint8_t tag);

	/**
	 * Returns the value associated with the tag.
	 * 
	 * @param tag the tag for the variable
	 * @param value the value to set teh variable to
	 */
	int set(uint8_t tag, uint64_t value);

	/**
	 * Loads the settings from an I2C_Setting
	 * 
	 * @param setting the I2C_Setting to load from
	 */
	int load_config(I2C_Setting &setting);

private:
	I2C_Setting setting_;
	int port_file_;
	std::mutex mutex;
};

#endif
