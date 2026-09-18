#ifndef _EXACT_COMMS_SERVICE_H_
#define _EXACT_COMMS_SERVICE_H_

#include <getopt.h>
#include <iostream>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <arpa/inet.h>

#include "../udp/packet.h"
#include "../udp/socket.h"
#include "file_descriptor_manager.h"
#include "signal_manager.h"

#include "backends/ethernet.h"
#include "backends/filein.h"
#include "backends/fileout.h"
#include "backends/serial.h"
#include "backends/ccsds.h"

// Enumeration of supported communication backends
enum class COMMS_MODE {
    SERIAL,     // Serial port mode
};

// Structure holding all program configuration parsed from CLI
struct ProgramArguments {
    int verbose;                            // Verbosity level for logging
    bool force;                             // Enable force mode
    bool dump;                              // Enable raw data dumping
    useconds_t usec_wait;                   // Delay in microseconds between reads or packets
    udp::SocketAddress live_address;        // Socket address for live traffic
    udp::SocketAddress norm_address;        // Socket address for norm packet destination
    uint16_t norm_receiver;                 // Destination node ID for norm packets
    COMMS_MODE mode;                        // Backend mode selected
    int baud_raw;                           // User-specified baud rate
    speed_t baud_rate;                      // Baud rate for termios setup
    std::string file;                       // Path to input/output file depending on mode
};

// --- Command HRT Handling ---
// static constexpr uint16_t HRT_listen_port = 50002;
// static constexpr const char* CMD_service_addr = "127.0.0.1:56000";
// static constexpr uint8_t HRT_packet_type = 0x87;
// static constexpr uint8_t HRT_packet_length = 1288;

// // Socket we use to send command to cmd_service and receive the replies
// std::shared_ptr<udp::Socket> hrt_socket;
// udp::SocketAddress cmd_socket_addr;

// DICE target ID right now
uint8_t target_id_dice = 0x8C;

// // EXACT header defaults (tweak if your IDs differ)
// static constexpr uint16_t EXACT_DST = 0x0010;
// static constexpr uint16_t EXACT_SRC = 0xDAC0;
// static constexpr uint16_t EXACT_PORT = 0x7FFF;

// // Simple sequence counter for EXACT header
// static uint16_t g_seq_counter = 0;

/**
 * @brief Processes inbound DICE packets and routes them through the backend
 * 
 * @param socket The UDP socket used to transmit processed packets
 * @param backend The communication backend to read from
 */
void processDICERequest(std::shared_ptr<udp::Socket> socket, std::shared_ptr<comms::backend::Backend> backend);

/**
 * @brief Prints help and usage information to stdout
 */
void help();

/**
 * @brief Parses the command-line arguments and produces a ProgramArguments struct
 * 
 * @param argc Number of arguments provided
 * @param argv List of arguments
 * 
 * @return ProgramArguments structure with all parsed options
 */
ProgramArguments parseArgs(int argc, char *argv[]);

/**
 * @brief Converts a numeric baud rate to a system-specific speed_t constant
 * 
 * @param baud_rate Integer baud rate input
 * 
 * @return Matching speed_t value understood by termios
 */
speed_t parseBaudRate(int baud_rate);

/**
 * @brief Displays the parsed program arguments for user verification
 * 
 * @param args The ProgramArguments instance to print
 */
void printArgs(ProgramArguments args);

/**
 * @brief Calculates the change in time between EXACT system time and time received in DICE packets. Calculated by
 * converting DICE coarse and fine time from a packet into milliseconds, and subtracting it from the DICE system time.
 * 
 * 
 * @param packet_bytes pointer to the DICE packet 
 * @param verbosity the verbosity level for debug output
 * @return long long representing the time delta in milliseconds
 */
long long calculateTimeDelta(char* packet_bytes, int verbosity);

/**
 * @brief Forwards unknown packets to a specified port for future-proofing against new packet types. This allows the service to continue functioning and provides a way to capture and analyze new packet types without crashing or losing data.
 * 
 * @param packet_bytes The bytes of the unknown packet to forward
 * @param dice_forward_socket The socket to use for forwarding the packet
 * @return int 0 on success, -1 on failure
 */
int forward_unknown_packet(std::vector<char> packet_bytes, std::shared_ptr<udp::Socket> dice_forward_socket);

#endif
