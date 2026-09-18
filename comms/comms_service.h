#ifndef _COMMS_SERVICE_H_
#define _COMMS_SERVICE_H_

#include <getopt.h>
#include <iostream>
#include <cstdint>
#include <ctime>

#include "packet.h"
#include "socket.h"
#include "file_descriptor_manager.h"
#include "signal_manager.h"

#include "backends/ethernet.h"
#include "backends/filein.h"
#include "backends/fileout.h"
#include "backends/ppp.h"
#include "backends/serial.h"

enum class COMMS_MODE {
    UNSET,
    ETHERNET,
    SERIAL,
    FILEIN,
    FILEOUT
};

struct ProgramArguments {
    int verbose;
    bool ppp;
    bool force;
    bool dump;
    useconds_t usec_wait;
    udp::SocketAddress live_address;
    udp::SocketAddress norm_address;
    uint16_t local_norm_receiver;
    uint16_t remote_norm_receiver;
    COMMS_MODE mode;
    speed_t baud_rate;
    std::string file;
    udp::SocketAddress ethernet_source;
    udp::SocketAddress ethernet_dest;
};

/**
 * @brief Processes incoming data from the backend implementation
 *
 * @param socket A std::shared_ptr<udp::Socket> that will send out the packet once it is processed
 *
 */
void processFromBackend(std::shared_ptr<udp::Socket> socket, udp::Packet packet);

/**
 * @brief Processes incoming data from either socket
 *
 * @param socket A std::shared_ptr<udp::Socket> that has a packet waiting within
 *
 * @param applyHeader a bool for if a header should be applyed to the incoming packet. This should only happen for norm packets.
 */
void processFromSocket(std::shared_ptr<udp::Socket> socket, bool applyHeader);

/**
 * @brief Checks if bytes are available in internal buffer
 *
 * @return true if bytes available, false otherwise
 */
bool bytesInBuffer();

/**
 * @brief Refills the internal buffer from the backend
 *
 * @returns true if bytes available, false otherwise
 */
bool refillBuffer();

/**
 * @brief Pulls a byte from the internal buffer
 *
 * @return char a single byte
 */
char getByteFromBuffer();


/**
 * @brief Prints out a packet header for debug information
 *
 * @param input boolean specifying if packet came from live_socket or norm_socket
 *
 * @param packet a udp::Packet or std::vector<char> of a serialized or deserialized packet
 * @param header a udp::PacketHeader
 */
void printHeader(bool input, udp::Packet& packet);
void printHeader(bool input, std::vector<char>& packet);
void printHeader(bool input, udp::PacketHeader& header);

/**
 * @brief Prints out program help
 */
void help();

/**
 * @brief Parses through the arguments provided to the program. Exits program if arguments invalid.
 *
 * @param argc int number of arguments
 * @param argv char*[] arguments
 *
 * @return ProgramArguments with valid state to run.
 */
ProgramArguments parseArgs(int argc, char *argv[]);

// Helper function to parse baud rate from arguments
speed_t parseBaudRate(int baud_rate);

/**
 * @brief Prints out the arguments given to stdout
 *
 * @param args ProgramArguments with program state
 */
void printArgs(ProgramArguments args);

#endif
