#include "i2c_setting.h"

I2C_Setting::I2C_Setting(uint8_t port_id,
						const char* port,
						bool ten_bit_addr,
						bool packet_error_checking,
						uint64_t retries,
						uint64_t timeout) :
						port_id_(port_id), 
						port_(port) {
    //Defaults
	if (set_packet_error_checking(packet_error_checking)) {
		packet_error_checking_ = false;
	}

	if (set_ten_bit_addr(ten_bit_addr)) {
		ten_bit_addr_ = false;
	}

	if (set_retries(retries)) {
		retries_ = 0;
	}

	if (set_timeout(timeout)) {
		timeout_ = 0;
	}
}

I2C_Setting::I2C_Setting(I2C_Setting &setting)
	: ten_bit_addr_(setting.ten_bit_addr_),
	packet_error_checking_(setting.packet_error_checking_),
	retries_(setting.retries_),
	timeout_(setting.timeout_),
	port_id_(setting.port_id_), 
	port_(setting.port_) {}

I2C_Setting &I2C_Setting::operator=(const I2C_Setting &other)
{
	ten_bit_addr_ = other.ten_bit_addr_;
	packet_error_checking_ = other.packet_error_checking_;
	retries_ = other.retries_;
	timeout_ = other.timeout_;
	port_id_ = other.port_id_;
	port_ = other.port_;
	return *this;
}

I2C_Setting::~I2C_Setting() {}

//Getters

bool I2C_Setting::get_ten_bit_addr() {
	return ten_bit_addr_;
}

bool I2C_Setting::get_packet_error_checking() {
	return packet_error_checking_;
}

uint64_t I2C_Setting::get_retries() {
	return retries_;
}

uint64_t I2C_Setting::get_timeout() {
	return timeout_;
}

//Setters

int I2C_Setting::set_ten_bit_addr(bool ten_bit_addr) {
	ten_bit_addr_ = ten_bit_addr;
	return 0;
}

int I2C_Setting::set_packet_error_checking(bool packet_error_checking) {
	packet_error_checking_ = packet_error_checking;
	return 0;
}

int I2C_Setting::set_retries(uint64_t retries) {
	retries_ = retries;
	return 0;
}

int I2C_Setting::set_timeout(uint64_t timeout) {
	timeout_ = timeout;
	return 0;
}

uint8_t I2C_Setting::get_port_id() const {
  return port_id_;
}

const char* I2C_Setting::get_port() const {
  return port_.c_str();
}
