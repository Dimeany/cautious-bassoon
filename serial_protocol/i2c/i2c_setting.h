#ifndef I2C_SETTING_HEADER
#define I2C_SETTING_HEADER

#include <cstring>
#include <string>
#include <cstdint>

class I2C_Setting {
public:
    /**
     * Creates an I2C_Setting object.
     * 
     * @param port_id the port id number
     * @param port the path to the port (E.g. "/dev/i2c-1")
     * @param ten_bit_addr if true then addresses will be 10 bits, otherwise 7 bits
     * @param packet_error_checking enables SMBus Packet Error Checking if true, otherwise disables it
     * @param retries the number of retries linux will try when device is unresponsive
     * @param timeout the time to wait for response in units of 10ms (0 for no change)
     */
     I2C_Setting(uint8_t port_id,
			 	 const char* port,
				 bool ten_bit_addr,
				 bool packet_error_checking,
				 uint64_t retries,
				 uint64_t timeout);
    
    /**
     * Constructs a new I2C_Setting as a copy of setting.
     * 
     * @param setting the setting to copy from
     */
	I2C_Setting(I2C_Setting &setting);

	/**
	 * Makes this object a copy of other.
	 *
	 * @param other the setting to copy from
	 */
     I2C_Setting &operator=(const I2C_Setting &other);

	~I2C_Setting();

     //Getters

    /**
     * Gets the current state of address length.
     * 
     * @return true if address length is 10 bits, false if 7 bits
     */
	bool get_ten_bit_addr();

    /**
     * Gets whether Packet Error Checking is enabled.
     *
     * @return true if Packet Error Checking is enabled, false if it's disabled
     */
	bool get_packet_error_checking();

    /**
     * Gets the retries parameter.
     * 
     * @return the current number of retries that will be attempted
     */
	uint64_t get_retries();

    /**
     * Gets the timeout parameter.
     * 
     * @return the time before timeout in 10ms increments
     */
	uint64_t get_timeout();

    //Setters

    /**
     * Sets the address length.
     * 
     * @param ten_bit_addr true if ten bits in address, false (default) if 7 bits
     * @return 0 on success
     */
	int set_ten_bit_addr(bool ten_bit_addr);

    /**
     * Sets whether SMBus PEC (packet error checking) generation and verification will be enabled. Only applies to
     * SMBus transactions and the adapters that support the feature.
     *
     * @param packet_error_checking true if SMBus PEC is wanted, false (default) if not
     * @return 0 on success
     */
	int set_packet_error_checking(bool packet_error_checking);

    /**
     * Sets the retries parameter.
     * 
     * @param retries the number of retries that will be attempted
     * @return 0 on success
     */
	int set_retries(uint64_t retries);

    /**
     * Sets the timeout parameter.
     * 
     * @param timeout the time before timeout in 10ms increments
     * @return 0 on success
     */
	int set_timeout(uint64_t timeout);

     uint8_t get_port_id() const;
     const char* get_port() const;

private:
	bool ten_bit_addr_;
	bool packet_error_checking_;
	uint64_t retries_;
	uint64_t timeout_;
     uint8_t port_id_;
     std::string port_;
};

#endif
