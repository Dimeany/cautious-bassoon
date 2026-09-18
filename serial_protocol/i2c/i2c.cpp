#include "i2c.h"

I2C::I2C(I2C_Setting setting) : 
	setting_(setting), 
	port_file_(-1) {}

I2C::~I2C() {
	close();
}

int I2C::open() {
	// Check if port is open
	if (port_file_ > 0) {
		return load_config(setting_);
	}

	// Open the port file
	port_file_ = ::open(setting_.get_port(), O_RDWR);
	if (port_file_ < 0){
		return -errno;
	}

	// Uses load config to adjust port after it's open
	return load_config(setting_);
}

int I2C::close() {
	// Close the port file
	if (::close(port_file_) < 0) {
		return -errno;
	}

	// Reset fd
	port_file_ = 0;

	return 0;
}


int I2C::read(uint16_t buffer_size, char* buffer, uint8_t device_address) {
	std::lock_guard guard(mutex);

	// Check if port is open
	if (port_file_ <= 0) {
		return -EBADF;
	}
	
	// Set device
	if (ioctl(port_file_, I2C_SLAVE, device_address) < 0) {
		return -errno;
	}

	// Read
	int result = ::read(port_file_, buffer, buffer_size);

	//Couldn't read correct number of bytes
	if (result != buffer_size) {
		return -errno;
	}

	return 0;
}

int I2C::write(uint16_t buffer_size, const char* buffer, uint8_t device_address) {
	std::lock_guard guard(mutex);

	// Check if port is open
	if (port_file_ <= 0) {
		return -EBADF;
	}
	
	// Set device
	if (ioctl(port_file_, I2C_SLAVE, device_address) < 0) {
		return -errno;
	}

	// Write
	int result = ::write(port_file_, buffer, buffer_size);

	// Couldn't write correct number of bytes
	if (result != buffer_size) {
		return -errno;
	}

	return 0;
}

int I2C::flush(uint8_t buffer) {
	// Check if port is open
	if (port_file_ <= 0) {
		return -EBADF;
	}

	// Only checks last two bits
	// Uses tcflush, not sure if it works with I2C
	if(!(buffer ^ 0b11)) {
		if (tcflush(port_file_, TCIOFLUSH) != 0) {
			return -errno;
		}
	} else if(!(buffer ^ 0b10)) {
		if (tcflush(port_file_, TCIFLUSH) != 0) {
			return -errno;
		}
	} else if(!(buffer ^ 0b01)) {
		if (tcflush(port_file_, TCOFLUSH) != 0) {
			return -errno;
		}
	}

	return 0;
}

int I2C::get(uint8_t tag) {
	switch (tag) {
		case I2C_Tags::TEN_BIT_ADDR:
			return setting_.get_ten_bit_addr();
		case I2C_Tags::PACKET_ERROR_CHECKING:
			return setting_.get_packet_error_checking();
		case I2C_Tags::RETRIES:
			return setting_.get_retries();
		case I2C_Tags::TIMEOUT:
			return setting_.get_timeout();
	}

	return -EINVAL;
}

int I2C::set(uint8_t tag, uint64_t value) {
	switch (tag) {
		case I2C_Tags::TEN_BIT_ADDR:
			setting_.set_ten_bit_addr(value);

			if (port_file_ <= 0) {
				ioctl(port_file_, I2C_TENBIT, setting_.get_ten_bit_addr());
			}

			return 0;
		case I2C_Tags::PACKET_ERROR_CHECKING:
			setting_.set_packet_error_checking(value);

			if (port_file_ <= 0) {
				ioctl(port_file_, I2C_PEC, setting_.get_packet_error_checking());
			}

			return 0;
		case I2C_Tags::RETRIES:
			setting_.set_retries(value);
			
			if (port_file_ <= 0) {
				ioctl(port_file_, I2C_RETRIES, setting_.get_retries());
			}

			return 0;
		case I2C_Tags::TIMEOUT:
			setting_.set_timeout(value);
			
			if (port_file_ <= 0) {
				ioctl(port_file_, I2C_TIMEOUT, setting_.get_timeout());
			}

			return 0;
	}

	return -EINVAL;
}

int I2C::load_config(I2C_Setting &setting) {
	I2C_Setting i2c_setting = *static_cast<I2C_Setting*>(&setting);
	setting_ = i2c_setting;

	set(I2C_Tags::TEN_BIT_ADDR, setting_.get_ten_bit_addr());
	set(I2C_Tags::PACKET_ERROR_CHECKING, setting_.get_packet_error_checking());
	set(I2C_Tags::RETRIES, setting_.get_retries());
	set(I2C_Tags::TIMEOUT, setting_.get_timeout());

	return 0;
}
