#include "exact_comms_service.h"
#include "../cdh/cdh_hs_packet.h"
#include "socket.h"
#include <csignal>
#include <map>
#include <set>

// Default command-line parameters
const ProgramArguments DEFAULT_ARGS {
    .verbose = 0,
    .force = false,
    .dump = false,
    .usec_wait = 0,
    .live_address = udp::SocketAddress::create("0.0.0.0:50000"),
    .norm_address = udp::SocketAddress::create("0.0.0.0:50007"), // Where NORM packets interact with this service
    .norm_receiver = 50006, // Where outgoing NORM packets will be addressed to
    .mode = COMMS_MODE::SERIAL,
    .baud_raw = 921600,
    .baud_rate = B921600,
    .file = "/dev/serial0",
};

ProgramArguments args {};
bool running = true;
std::shared_ptr<comms::backend::Backend> backend;

// Cache the last non-zero LRT tail (bytes after the COMMS region) so we
// can persist subsystem data between CDH updates and avoid sending empty tails.
static std::vector<uint8_t> last_lrt_tail;

// Cache last-seen EXACT-framed blocks keyed by destination port so each
// subsystem's health packet can be persisted independently.
static std::map<uint16_t, std::vector<uint8_t>> last_lrt_subsystems;

// Hold EXACT-DICE time delta
long long TIME_DELTA;

// Hold HRT status
bool hrt_stop = false;

// Hold time delta counter (number of HNS and LRT packets received, reset (with modulo) after 200 to avoid overflow)
uint8_t TIME_DELTA_COUNTER = 0;

// Signal handler to gracefully shut down on Ctrl+C (SIGINT or SIGTERM)
void signalHandler(int signum) {
    std::cout << "[INFO] Signal " << signum << " received\n";
    (void)signum;
    running = false;
    
    auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend);
    if ( framer ) {
        framer->printByteCounters();
    } else {
        std::cout << "[ERROR] No framer available to print byte counters.\n";
    }
    std::cout << "[DEBUG] Current time delta: " << TIME_DELTA << " ms\n";
}

// Handle packets from DICE
void handleDicePackets(int verbosity, std::vector<uint8_t> response, size_t total_length, std::shared_ptr<comms::backend::Backend> backend) {
    // Zero-fill to payload size (excluding sync, type/id, and CRC)
    response.resize(total_length - 2, 0x00);

    // Compute and append CRC
    uint16_t crc = comms::backend::ccsds::compute_crc(std::vector<uint8_t>(response.begin() +4, response.end()));
    response.push_back((crc >> 8) & 0xFF);
    response.push_back(crc & 0xFF);

    if (verbosity >= 3) {
        std::cout << "[DEBUG] Computed CRC=0x" << std::hex << std::setw(4) << std::setfill('0') << crc << std::dec << "\n";
        std::cout << "[DEBUG] Outgoing packet size with CRC: " << response.size() << " bytes\n";

        // Print first 8 and last 8 bytes for quick inspection
        size_t show_head = std::min<size_t>(8, response.size());
        std::cout << "[DEBUG] Packet head: ";
        for (size_t i = 0; i < show_head; ++i)
            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(response[i]) << " ";
        std::cout << std::dec << std::setfill(' ') << "\n";

        size_t show_tail = std::min<size_t>(8, response.size());
        std::cout << "[DEBUG] Packet tail: ";
        for (size_t i = 0; i < show_tail; ++i) {
            size_t idx = response.size() - show_tail + i;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(response[idx]) << " ";
        }
        std::cout << std::dec << std::setfill(' ') << "\n";

        // Verify CRC by recomputing over packet (excluding sync) and comparing to appended bytes
        try {
            std::vector<uint8_t> packet_for_crc(response.begin() + 4, response.end() - 2);
            uint16_t verify = comms::backend::ccsds::compute_crc(packet_for_crc);
            uint16_t appended = (static_cast<uint16_t>(response[response.size()-2]) << 8) | static_cast<uint16_t>(response[response.size()-1]);
            if (verify != appended) {
                std::cout << "[ERROR] CRC verification mismatch: recomputed=0x" << std::hex << verify
                          << " appended=0x" << appended << std::dec << "\n";
            } else {
                std::cout << "[DEBUG] CRC verification OK\n";
            }
        } catch (...) {
            std::cout << "[WARN] CRC verification threw an exception\n";
        }
    }

    // Convert to char and write
    std::vector<char> out_packet(response.begin(), response.end());
    backend->write(out_packet);

    if (verbosity >= 2) 
        std::cout << "[SEND] Sent response packet of size: " << out_packet.size() << std::endl;
}

long long calculateTimeDelta(char* packet_bytes, int verbosity) {

    uint32_t coarseTime;
    std::memcpy(&coarseTime, packet_bytes + 4, 4);

    uint16_t fineTime;
    std::memcpy(&fineTime, packet_bytes + 8, 2);

    coarseTime = ntohl(coarseTime);
    fineTime = ntohs(fineTime);

    double fineMs = (static_cast<double>(fineTime) / 65536.0) * 1000.0;

    long long diceMs = (static_cast<long long>(coarseTime) * 1000) + static_cast<long long>(fineMs);

    diceMs += (static_cast<long long>(315964800) * 1000); // 315964800 from Unix to GPS leap seconds, convert to ms

    if (verbosity >= 2)
    std::cout << "[DEBUG] Extracted DICE time: coarse=" << coarseTime << " fine=" << fineTime 
              << " -> " << diceMs << " ms\n";

    // Get the current time point from the system clock
    auto now = std::chrono::system_clock::now();

    // Convert the time point to duration since the epoch
    auto duration = now.time_since_epoch();

    // Cast the duration to milliseconds
    auto exactMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    if (verbosity >= 2)
    std::cout << "[DEBUG] Current system time: " << exactMs << " ms\n";

    long long timeDelta = exactMs - diceMs;

    return timeDelta;
}

int forward_unknown_packet(std::vector<char> packet_bytes, std::shared_ptr<udp::Socket> dice_forward_socket) {
    try {
        std::vector<char> forward_bytes(packet_bytes.begin(), packet_bytes.end());
        udp::Packet forward_pkt = udp::Packet::create_from_buffer(forward_bytes, 50010, 50000, 0, 0);
        std::vector<char> forward_serialized = forward_pkt.serialize();
        dice_forward_socket->send(forward_serialized, udp::SocketAddress::create("127.0.0.1:50010"));
        return 0;
    } catch (udp::SocketException const&) {
        return -1;
    }
}

int main(int argc, char *argv[]) {
    // Parse CLI arguments into config
    args = parseArgs(argc, argv);

    // Open the live socket for receiving DICE requests and checks it
    auto live_socket = std::make_shared<udp::Socket>(args.live_address);
    if (!live_socket || live_socket->fd < 0) {
        std::cerr << "[ERROR] Failed to create or bind live socket.\n";
        return 1;
    }

    // CDH socket on 50001 (COMMS sends and receives CDH packets to and from this socket)
    auto cdh_socket = std::make_shared<udp::Socket>(udp::SocketAddress::create("127.0.0.1:50001"));
    if (!cdh_socket || cdh_socket->fd < 0) {
        std::cerr << "[ERROR] Failed to create or bind CDH socket (50001).\n";
        return 1;
    }

    // CMD socket on 50002 (sends and receives CMD packets to and from this socket)
    auto cmd_socket = std::make_shared<udp::Socket>(udp::SocketAddress::create("127.0.0.1:50002"));
    if (!cmd_socket || cmd_socket->fd < 0) {
        std::cerr << "[ERROR] Failed to create or bind CMD socket (50002).\n";
        return 1;
    }

    // NORM socket (receives outgoing NORM and sends incoming NORM packets)
    auto norm_socket = std::make_shared<udp::Socket>(args.norm_address);
    if (!norm_socket || norm_socket->fd < 0) {
        std::cerr << "[ERROR] Failed to create or bind NORM socket.\n";
        return 1;
    }

    // DICE forward socket on 50005 (any data received is sent straight to backend)
    auto dice_forward_socket = std::make_shared<udp::Socket>(udp::SocketAddress::create("127.0.0.1:50005"));
    if (!dice_forward_socket || dice_forward_socket->fd < 0) {
        std::cerr << "[ERROR] Failed to create or bind DICE forward socket (50005).\n";
        return 1;
    }

    // CMD service socket on 56000 (for sending NORM STOP/GO commands to cmd_service)
    auto cmd_service_addr = udp::SocketAddress::create("127.0.0.1:56000");

    // Initialize backend
    if (args.mode == COMMS_MODE::SERIAL)
        backend = std::make_shared<comms::backend::Serial>(args.file, args.baud_rate);

    // Check backend validity
    if (!backend) {
        std::cerr << "[ERROR] Backend failed to initialize. Exiting.\n";
        return 1;
    }

    // Always wrap backend with CCSDS framing
    backend = std::make_shared<comms::backend::ccsds>(std::move(backend), args.verbose, target_id_dice);

    // Set up descriptor manager to watch sockets and backend fd
    FileDescriptorManager fd_manager {live_socket->fd};
    if (backend->getFd() >= 0)
        fd_manager.addFileDescriptor(backend->getFd());
    else if (args.verbose >= 1) 
        std::cerr << "[WARN] Backend file descriptor is invalid.\n";

    // Watch the CDH socket for HNS and LRT data
    fd_manager.addFileDescriptor(cdh_socket->fd);

    // Watch the CMD socket for replies
    fd_manager.addFileDescriptor(cmd_socket->fd);

    // Watch the NORM socket for outgoing NORM packets
    fd_manager.addFileDescriptor(norm_socket->fd);

    // Watch the DICE forward port for any data
    fd_manager.addFileDescriptor(dice_forward_socket->fd);

    // Register graceful signal shutdown handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Print args if verbosity is enabled
    if (args.verbose >= 1)
        printArgs(args);

    // Initialize
    std::vector<uint8_t> cdh_hns_data = {0x1A, 0xCF, 0xFC, 0x1D, 0xA0, 0x8C};
    std::vector<uint8_t> cdh_lrt_data = {0x1A, 0xCF, 0xFC, 0x1D, 0x81, 0x8C};

    // Main event loop
    while(running) {
        bool activity = false;

        if (fd_manager.wait() == 0)
        continue;

        // Handle backend data for H&S, LRT, and CMD ACK packets (packets coming from DICE)
        if (fd_manager.areBytesWaiting(backend->getFd())) {
            activity = true;
            
            if (args.verbose >= 2) {
                std::cout << "[DEBUG] Backend has data waiting on fd " << backend->getFd() << "\n"; // (for testing)
            }

            while (backend->bytesAvailable()) {
                
                if (args.verbose >= 3) {
                std::cout << "[DEBUG] bytesAvailable() is true\n"; // (for testing)
                }

                std::vector<char> packet_bytes = backend->read();
                if (packet_bytes.empty()) {
                    // std::cout << "[DEBUG] backend-> read() returned empty packet\n"; // (for testing) 
                    break;
                } 
                
                // Debug output for received frames
                if (args.verbose >= 2)
                    std::cout << "[RECV] Received packet of size: " << packet_bytes.size() << std::endl;

                // Assign packet type to the 11th byte and the target ID to the 12th byte
                uint8_t packet_type = static_cast<uint8_t>(packet_bytes[10]);

                // -----------------------------
                // Check for HNS or LRT request
                // -----------------------------
                if (packet_bytes.size() == 14) {
                    // Determine the packet length based on packet type
                    std::vector<uint8_t> response;
                    size_t total_length = 0;
                    
                    switch (packet_type) {
                        case 0xa0: {
                            // HNS
                            TIME_DELTA_COUNTER += 1;
                            response = cdh_hns_data;
                            total_length = 58;

                            if (TIME_DELTA_COUNTER % 10 <= 2) {
                                // Get EXACT-DICE time delta
                                TIME_DELTA = calculateTimeDelta(packet_bytes.data(), args.verbose);
                            }

                            auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend);
                            if (framer) {
                                framer->incHNSRequests();
                                // Ensure response is sized to the full payload (CRC appended later)
                                response.resize(total_length - 2, 0x00);
                                size_t insert_pos = 6 + sizeof(cdh::CDHHS); // after CCSDS header + CDH H&S struct

                                if (insert_pos + 8 <= response.size()) {
                                    uint16_t pkts_in = framer->getIncomingPackets() % 65536;
                                    uint16_t crc_fail = framer->getCrcFail() % 65536;
                                    uint32_t total_pkts = framer->getTotalPackets();

                                    // write big-endian pkts_in
                                    for (int i = 1; i >= 0; --i)
                                        response[insert_pos + (1 - i)] = static_cast<uint8_t>((pkts_in >> (8 * i)) & 0xFF);

                                    // write big-endian crc_fail
                                    for (int i = 1; i >= 0; --i)
                                        response[insert_pos + 2 + (1 - i)] = static_cast<uint8_t>((crc_fail >> (8 * i)) & 0xFF);

                                    // write big-endian total_pkts
                                    for (int i = 3; i >= 0; --i)
                                        response[insert_pos + 4 + (3 - i)] = static_cast<uint8_t>((total_pkts >> (8 * i)) & 0xFF);

                                    if (args.verbose >= 2) std::cout << "[DEBUG] Inserted CRC_errs, pkts_in, and pkts_out into HNS packet\n";
                                } else {
                                    std::cout << "[ERROR] Not enough room to insert CRC_errs, pkts_in, and pkts_out into HNS packet\n";
                                }
                            } else {
                                response.resize(total_length - 2, 0x00);
                            }

                            handleDicePackets(args.verbose, response, total_length, backend);
                            break;
                        }
                        case 0x81: {
                            // LRT
                            TIME_DELTA_COUNTER += 1;
                            response = cdh_lrt_data;
                            total_length = 1256;

                            if (TIME_DELTA_COUNTER % 10 == 0) {
                                // Get EXACT-DICE time delta
                                TIME_DELTA = calculateTimeDelta(packet_bytes.data(), args.verbose);
                            }

                            const size_t COMMS_REGION = 128;
                            const size_t COMMS_OFFSET = 6 + COMMS_REGION; // external header + COMMS region

                            // Extract CDH tail (bytes 6+ from cdh_lrt_data) before we modify response
                            std::vector<uint8_t> cdh_tail;
                            if (response.size() > 6) {
                                cdh_tail.assign(response.begin() + 6, response.end());
                            }

                            if (args.verbose >= 2) {
                                std::cout << "[DEBUG] Initial response size: " << response.size() << std::endl;
                                std::cout << "[DEBUG] CDH tail size: " << cdh_tail.size() << std::endl;
                                if (!cdh_tail.empty()) {
                                    size_t show = std::min<size_t>(64, cdh_tail.size());
                                    std::cout << "[DEBUG] CDH tail first " << show << " bytes: ";
                                    for (size_t i = 0; i < show; ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << int(cdh_tail[i]) << " ";
                                    std::cout << std::dec << std::setfill(' ') << std::endl;
                                } else {
                                    std::cout << "[DEBUG] CDH tail is EMPTY\n";
                                }
                            }

                            // Build response structure: [6B CCSDS][128B COMMS][CDH tail]
                            response.resize(total_length, 0x00);
                            // Bytes 0-5 are already set (CCSDS header from cdh_lrt_data)

                            // Insert COMMS status fields into bytes 6-133
                            try {
                                auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend);
                                if (framer) {
                                    auto fields = framer->getCommLRTStatusFields();

                                    // add time delta to fields (long long)
                                    fields.push_back((TIME_DELTA >> 56) & 0xFF);
                                    fields.push_back((TIME_DELTA >> 48) & 0xFF);
                                    fields.push_back((TIME_DELTA >> 40) & 0xFF);
                                    fields.push_back((TIME_DELTA >> 32) & 0xFF);
                                    fields.push_back((TIME_DELTA >> 24) & 0xFF);
                                    fields.push_back((TIME_DELTA >> 16) & 0xFF);
                                    fields.push_back((TIME_DELTA >> 8) & 0xFF);
                                    fields.push_back(TIME_DELTA & 0xFF);

                                    size_t insert_pos = 6; // start of 128B COMMS section

                                    // Build EXACT internal header
                                    uint16_t seq = 0;
                                    try {
                                        seq = static_cast<uint16_t>(framer->getTotalPackets() & 0xFFFF);
                                    } catch (...) {
                                        seq = 0;
                                    }

                                    // from 50000 to 50010
                                    udp::Packet hdr_pkt = udp::Packet::create_from_buffer(std::span<char const>{}, 50010, 50000, seq, 0);
                                    std::vector<char> hdr_bytes = hdr_pkt.serialize(); // should be 12 bytes

                                    if (args.verbose >= 2) {
                                        std::cout << "[DEBUG] COMMS fields size from framer: " << fields.size() << " bytes\n";
                                        size_t show = std::min<size_t>(fields.size(), 32);
                                        if (show > 0) {
                                            std::cout << "[DEBUG] COMMS fields head: ";
                                            for (size_t i = 0; i < show; ++i)
                                                std::cout << std::hex << std::setw(2) << std::setfill('0') << (static_cast<uint8_t>(fields[i]) & 0xFF) << " ";
                                            std::cout << std::dec << std::setfill(' ') << "\n";
                                        }
                                    }

                                    // Copy header into COMMS region (truncate if larger than COMMS_REGION)
                                    size_t hdr_copy = std::min(hdr_bytes.size(), COMMS_REGION);
                                    for (size_t i = 0; i < hdr_copy; ++i) {
                                        response[insert_pos + i] = static_cast<uint8_t>(hdr_bytes[i]);
                                    }

                                    // Copy fields after header, truncating to fit inside COMMS_REGION
                                    size_t max_fields = COMMS_REGION - hdr_copy;
                                    size_t fields_copy = std::min(fields.size(), max_fields);
                                    if (fields_copy > 0) {
                                        std::copy(fields.begin(), fields.begin() + fields_copy, response.begin() + insert_pos + hdr_copy);
                                    }

                                    if (args.verbose >= 2) {
                                        std::cout << "[DEBUG] Wrote COMMS header (" << hdr_copy << "B) + fields (" << fields_copy << "B) into bytes 6-133\n";
                                    }
                                }
                            } catch (...) {
                                // If framer fails, COMMS region stays zeroed
                            }

                            // Parse and cache subsystem packets individually
                            if (!cdh_tail.empty()) {
                                // We have fresh subsystem data from CDH
                                size_t parse_offset = 0;
                                while (parse_offset < cdh_tail.size()) {
                                    // Each subsystem is a serialized udp::Packet: [2B len][2B dst][2B src][2B seq][8B time][payload...]
                                    if (parse_offset + 12 > cdh_tail.size()) break;
                                    
                                    uint16_t pkt_len = (static_cast<uint8_t>(cdh_tail[parse_offset]) << 8) | 
                                                       static_cast<uint8_t>(cdh_tail[parse_offset + 1]);
                                    uint16_t dst_port = (static_cast<uint8_t>(cdh_tail[parse_offset + 2]) << 8) | 
                                                        static_cast<uint8_t>(cdh_tail[parse_offset + 3]);
                                    
                                    if (pkt_len == 0 || parse_offset + pkt_len > cdh_tail.size()) break;
                                    
                                    // Extract and cache this subsystem packet
                                    std::vector<uint8_t> subsys_pkt(cdh_tail.begin() + parse_offset,
                                                                     cdh_tail.begin() + parse_offset + pkt_len);
                                    last_lrt_subsystems[dst_port] = subsys_pkt;
                                    
                                    if (args.verbose >= 3) {
                                        std::cout << "[DEBUG] Cached subsystem packet: dst_port=" << dst_port 
                                                  << " len=" << pkt_len << " bytes\n";
                                    }
                                    
                                    parse_offset += pkt_len;
                                }
                                
                                // Update the last known good tail
                                last_lrt_tail = cdh_tail;
                                if (args.verbose >= 2) {
                                    std::cout << "[INFO] Updated subsystem cache from fresh CDH tail (" 
                                              << last_lrt_subsystems.size() << " subsystems)\n";
                                }
                            }

                            // Reconstruct tail from cache (either fresh data or last known state)
                            std::vector<uint8_t> tail_to_append;
                            for (const auto& [dst_port, subsys_data] : last_lrt_subsystems) {
                                tail_to_append.insert(tail_to_append.end(), subsys_data.begin(), subsys_data.end());
                            }

                            // Append tail at byte 134
                            if (!tail_to_append.empty()) {
                                size_t copy_len = std::min(tail_to_append.size(), response.size() - COMMS_OFFSET);
                                if (copy_len > 0) {
                                    std::copy(tail_to_append.begin(), tail_to_append.begin() + copy_len, response.begin() + COMMS_OFFSET);
                                    if (args.verbose >= 2) {
                                        std::cout << "[DEBUG] Appended " << copy_len << " bytes of (cached) tail at offset " << COMMS_OFFSET << "\n";
                                    }
                                }
                            }

                            // Verify tail is present
                            if (args.verbose >= 1) {
                                size_t final_nonzero = 0;
                                for (size_t i = COMMS_OFFSET; i < response.size(); ++i) {
                                    if (static_cast<uint8_t>(response[i]) != 0) ++final_nonzero;
                                }
                                if (final_nonzero == 0) {
                                    std::cout << "[WARN] Post-COMMS: bytes after COMMS region are all zero (" << (response.size() - COMMS_OFFSET) << " bytes)\n";
                                } else {
                                    if (args.verbose >= 2) {
                                        std::cout << "[DEBUG] Post-COMMS: " << final_nonzero << " non-zero bytes after COMMS region\n";
                                    }
                                    if (args.verbose >= 3) {
                                        size_t show = std::min<size_t>(32, response.size() - COMMS_OFFSET);
                                        std::cout << "[DEBUG] First " << show << " bytes after COMMS region: ";
                                        for (size_t i = 0; i < show; ++i) {
                                            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(static_cast<uint8_t>(response[COMMS_OFFSET + i])) << " ";
                                        }
                                        std::cout << std::dec << std::setfill(' ') << "\n";
                                    }
                                }
                            }

                            // Send the LRT response and log an explicit send counter for diagnostics
                            handleDicePackets(args.verbose, response, total_length, backend);

                            if (args.verbose >= 2) {
                                static int lrt_sent_count = 0;
                                ++lrt_sent_count;
                                uint32_t framer_total = 0;
                                try {
                                    auto framer_ptr = std::dynamic_pointer_cast<comms::backend::ccsds>(backend);
                                    if (framer_ptr) framer_total = framer_ptr->getTotalPackets();
                                } catch (...) { framer_total = 0; }
                                std::cout << "[INFO] LRT responses sent so far: " << lrt_sent_count
                                          << " (framer total_pkts=" << framer_total << ")\n";
                            }

                            break;
                        }
                        case 0x85: {
                            // set HRT STOP
                            hrt_stop = true;

                            // Increment NORM STOP counter
                            if (auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend)) {
                                framer->incStopCmds();
                            }
                            
                            // Send framed NORM/HRT STOP command to cmd_service
                            std::string stop_cmd = "norm_stop";
                            // Create framed UDP packet for cmd_service
                            udp::Packet stop_pkt = udp::Packet::create_from_buffer(
                                stop_cmd,
                                56000,  // destination port (cmd_service)
                                50002,  // source port (cmd_socket bound address)
                                0,      // sequence number
                                0       // status
                            );
                            std::vector<char> framed_stop = stop_pkt.serialize();
                            cmd_socket->send(framed_stop, cmd_service_addr);
                            if (args.verbose >= 1)
                                std::cout << "[DEBUG] Sent framed NORM STOP command to cmd_service" << std::endl;
                            break;
                        }
                        case 0x87: {
                            // unset HRT STOP
                            hrt_stop = false;

                            // Increment NORM GO/RESUME counter
                            if (auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend)) {
                                framer->incGoCmds();
                            }
                            
                            // Send framed NORM/HRT GO command to cmd_service
                            std::string go_cmd = "norm_resume";
                            // Create framed UDP packet for cmd_service
                            udp::Packet go_pkt = udp::Packet::create_from_buffer(
                                go_cmd,
                                56000,  // destination port (cmd_service)
                                50002,  // source port (cmd_socket bound address)
                                0,      // sequence number
                                0       // status
                            );
                            std::vector<char> framed_go = go_pkt.serialize();
                            cmd_socket->send(framed_go, cmd_service_addr);
                            if (args.verbose >= 1)
                                std::cout << "[DEBUG] Sent framed NORM GO/RESUME command to cmd_service" << std::endl;
                            break;
                        }
                        default:
                            int unknown_res = forward_unknown_packet(packet_bytes, dice_forward_socket);
                            if (unknown_res == 0) {
                                if (args.verbose >= 1)
                                    std::cout << "[WARN] Unknown packet type 0x" << std::hex << int(packet_type) << std::dec << " forwarded to port 50010\n";
                            } else {
                                std::cout << "[ERROR] Failed to forward unknown packet of type 0x" << std::hex << int(packet_type) << std::dec << " to port 50010\n";
                            }
                            break;
                    }

                //  --------------------
                //  Check for CMD packet
                //  These could be actual commands or bulk commanding file transfer packets
                //  --------------------

                } else if (packet_type == 0x10) {
                    // increment incoming CMD request counter
                    if (auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend)) {
                        framer->incCMDRequests();
                    }

                    // Start with sync and then append packet type and then target ID
                    std::vector<uint8_t> response = {0x1A, 0xCF, 0xFC, 0x1D, packet_type, target_id_dice};

                    // Compute and append CRC
                    uint16_t crc = comms::backend::ccsds::compute_crc(std::vector<uint8_t>(response.begin() + 4, response.end()));
                    response.push_back((crc >> 8) & 0xFF);
                    response.push_back(crc & 0xFF);

                    // Convert to char and write
                    std::vector<char> out_packet(response.begin(), response.end());
                    backend->write(out_packet);

                    if (args.verbose >= 1)
                        std::cout << "[SEND] Sent CMD ACK packet of size: " << out_packet.size() << std::endl;

                    // Strip the DICE Header (first 12 bytes)
                    std::vector<uint8_t> cmd_header_and_data(packet_bytes.begin() + 12, packet_bytes.end());
                    
                    // Grab 105 bytes, which would include a whole BFT packet or a 93B CMD packet with EXACT header
                    uint16_t cmd_packet_length = 105;
                    std::vector<uint8_t> cmd_to_send(cmd_header_and_data.begin(), cmd_header_and_data.begin() + cmd_packet_length);

                    // Extract destination Port from EXACT or BFT header (bytes 3 and 4 in packet)
                    uint16_t dest_port = (static_cast<uint16_t>(cmd_header_and_data[2]) << 8) | static_cast<uint16_t>(cmd_header_and_data[3]);

                    // Build the destination address
                    udp::SocketAddress dest_addr = udp::SocketAddress::create("127.0.0.1:" + std::to_string(dest_port));

                    // Convert cmd_to_send to char for sending
                    std::vector<char> outgoing_cmd_packet(cmd_to_send.begin(), cmd_to_send.end());
                    
                    cmd_socket->send(outgoing_cmd_packet, dest_addr);

                    if (args.verbose >= 2) {
                        std::cout << "[DEBUG] Extracted CMD payload of length: " << outgoing_cmd_packet.size() << "\n";
                        std::cout << "[DEBUG] Destination port from EXACT header: " << dest_port << "\n";
                    }
                    if (args.verbose >= 3) {
                        std::cout << "[DEBUG] Outgoing frame: ";
                        for (char c : outgoing_cmd_packet) {
                            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(c) << " ";
                        }
                        std::cout << std::dec << "\n";
                    }
                } else {
                    // forward for future support
                    int unknown_res = forward_unknown_packet(packet_bytes, dice_forward_socket);
                    if (unknown_res == 0) {
                        if (args.verbose >= 1)
                            std::cout << "[WARN] Unknown packet type 0x" << std::hex << int(packet_type) << std::dec << " forwarded to port 50010\n";
                    } else {
                        std::cout << "[ERROR] Failed to forward unknown packet of type 0x" << std::hex << int(packet_type) << std::dec << " to port 50010\n";
                    }
                }
            }
        }

        // Handle CDH response packets coming in from the cdh_service
        if (fd_manager.areBytesWaiting(cdh_socket->fd)) {
            activity = true;
            auto opt_packet = cdh_socket->receive();

            // Packet must be present if we have reached this code
            if (!opt_packet) {
                std::cout << "[ERROR] CDH socket has bytes waiting, but a packet was not received!" << std::endl;
                continue;
            }
            auto const& packet = *opt_packet;

            // Check packet length
            if (packet.size() < 6) {
                std::cout << "[ERROR] CDH received packet too short: does not contain CCSDS header." << std::endl;
                continue;
            }

            // Look at CCSDS header from CDH to determine packet data type
            uint8_t packet_type = packet[4];
            switch (packet_type) {
            case 0xa0:
                if (args.verbose >= 1) 
                    std::cout << "[DEBUG] Received HNS data from CDH." << std::endl;
                cdh_hns_data.resize(packet.size());
                memcpy(cdh_hns_data.data(), packet.data(), packet.size());
                // Overwrite CDH-sent target ID with the local `target_id_dice` to ensure correct routing
                if (cdh_hns_data.size() > 5) {
                    cdh_hns_data[5] = target_id_dice;
                    if (args.verbose >= 1) std::cout << "[DEBUG] Overwrote CDH HNS target ID with 0x" << std::hex << int(target_id_dice) << std::dec << "\n";
                }
                break;
            case 0x81:
                if (args.verbose >= 1) 
                    std::cout << "[DEBUG] Received LRT data from CDH." << std::endl;
                cdh_lrt_data.resize(packet.size());
                memcpy(cdh_lrt_data.data(), packet.data(), packet.size());
                // Overwrite CDH-sent target ID with the local `target_id_dice` to ensure correct routing
                if (cdh_lrt_data.size() > 5) {
                    cdh_lrt_data[5] = target_id_dice;
                    if (args.verbose >= 1) std::cout << "[DEBUG] Overwrote CDH LRT target ID with 0x" << std::hex << int(target_id_dice) << std::dec << "\n";
                }
                // Cache any non-zero bytes after COMMS region so we can persist subsystem data
                {
                    const size_t COMMS_REGION = 128;
                    const size_t COMMS_OFFSET = 6 + COMMS_REGION;
                    if (packet.size() > COMMS_OFFSET) {
                        bool any_nonzero = false;
                        for (size_t i = COMMS_OFFSET; i < packet.size(); ++i) {
                            if (static_cast<uint8_t>(packet[i]) != 0) { any_nonzero = true; break; }
                        }
                        if (any_nonzero) {
                            last_lrt_tail.assign(packet.begin() + COMMS_OFFSET, packet.end());
                            if (args.verbose >= 2) std::cout << "[DEBUG] Updated last_lrt_tail from CDH LRT (" << last_lrt_tail.size() << " bytes)\n";
                        }
                    }
                }
                break;
            default:
                std::cout << "[ERROR] CDH packet had unknown packet type: " << (int)packet_type << std::endl;
                break;
            }
        }

        // Handle CMD response packets coming in from the cmd_service
        if (fd_manager.areBytesWaiting(cmd_socket->fd) && !hrt_stop) {
            activity = true;
            udp::SocketAddress sender;
            auto opt_packet = cmd_socket->receive(sender);

            if (!opt_packet) {
                std::cout << "[ERROR] CMD socket has bytes waiting, but a packet was not received!" << std::endl;
                continue;
            }

            auto const& packet = *opt_packet;
            if (args.verbose >= 1) {
                std::cout << "[DEBUG] Received CMD response of length " << packet.size() << " from " 
                          << sender.get_address() << ":" << sender.get_port() << "\n"; 
            }

            size_t total_length = 1288;
            std::vector<uint8_t> dice_hrt_packet = {0x1A, 0xCF, 0xFC, 0x1D, 0x87, target_id_dice};
            dice_hrt_packet.resize(total_length - 2);

            size_t writable_length = 1288 - 6 - 2; // total length - header - checksum
            if ((packet.size() > writable_length) && (args.verbose >= 1)) {
                std::cout << "[WARN] CMD packet longer than HRT packet data length.  CMD packet length was: " << packet.size() << std::endl;
            }

            memcpy(dice_hrt_packet.data() + 6, packet.data(), std::min(writable_length, packet.size()));

            uint16_t crc = comms::backend::ccsds::compute_crc(std::vector<uint8_t>(dice_hrt_packet.begin() + 4, dice_hrt_packet.end()));
            dice_hrt_packet.push_back((crc >> 8) & 0xFF);
            dice_hrt_packet.push_back(crc & 0xFF);

            backend->write({
                reinterpret_cast<char const*>(dice_hrt_packet.data()),
                dice_hrt_packet.size()
            });
        }

        // Handle NORM packets coming from the flight NORM sender process
        if (fd_manager.areBytesWaiting(norm_socket->fd) && !hrt_stop) {
            activity = true;
            udp::SocketAddress norm_sender;
            auto norm_packet = norm_socket->receive(norm_sender);

            if (!norm_packet) {
                std::cout << "[ERROR] NORM socket has bytes waiting, but a packet was not received!" << std::endl;
                continue;
            }

            auto const& packet = *norm_packet;
            if (args.verbose >= 1) {
                std::cout << "[DEBUG] Received NORM packet of length " << packet.size() << " from " 
                          << norm_sender.get_address() << ":" << norm_sender.get_port() << "\n"; 
            }
            if (args.verbose >= 3) {
                std::cout << "[DEBUG] NORM data: " << std::hex << std::setw(2) << std::setfill('0');
                for (size_t i = 0; i < packet.size(); ++i) {
                    std::cout << int(static_cast<uint8_t>(packet[i])) << " ";
                }
                std::cout << std::dec << std::setfill(' ') << "\n";
            }

            // process outgoing NORM packet
            size_t total_length = 1288;
            std::vector<uint8_t> dice_hrt_packet = {0x1A, 0xCF, 0xFC, 0x1D, 0x87, target_id_dice};
            dice_hrt_packet.resize(total_length - 2);

            // Build EXACT internal header
            size_t writable_length = 1288 - 6 - 2; // total length - header - checksum
            if ((packet.size() > writable_length) && (args.verbose >= 1)) {
                std::cout << "[WARN] NORM packet longer than HRT packet data length.  NORM packet length was: " << packet.size() << std::endl;
            }

            auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend);
            uint16_t seq = 0;
            try {
                seq = static_cast<uint16_t>(framer->getTotalPackets() & 0xFFFF);
            } catch (...) {
                seq = 0;
            }

            // from 50000 to 50007
            std::vector<char> dummy_packet (packet.size()); // NORM data only - Packet class takes care of the rest
            udp::Packet hdr_pkt = udp::Packet::create_from_buffer(dummy_packet, 50007, 50000, seq, 0);
            std::vector<char> hdr_bytes = hdr_pkt.serialize(); // should be 12 bytes

            // Copy header into outgoing packet
            size_t hdr_copy = std::min(hdr_bytes.size(), writable_length);
            for (size_t i = 0; i < hdr_copy; ++i) {
                dice_hrt_packet[6 + i] = static_cast<uint8_t>(hdr_bytes[i]);
            }

            if (args.verbose >= 3) {
                // print out first 18B of dice_hrt_packet (DICE header + EXACT header)
                std::cout << "[DEBUG] Prepared DICE + EXACT header: ";
                for (size_t i = 0; i < 18 && i < dice_hrt_packet.size(); ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << int(static_cast<uint8_t>(dice_hrt_packet[i])) << " ";
                }
                std::cout << std::dec << std::setfill(' ') << "for encapsulated NORM packet with seq=" << seq << "\n";
            }

            memcpy(dice_hrt_packet.data() + 18, packet.data(), std::min(writable_length, packet.size()));

            uint16_t crc = comms::backend::ccsds::compute_crc(std::vector<uint8_t>(dice_hrt_packet.begin() + 4, dice_hrt_packet.end()));
            dice_hrt_packet.push_back((crc >> 8) & 0xFF);
            dice_hrt_packet.push_back(crc & 0xFF);

            backend->write({
                reinterpret_cast<char const*>(dice_hrt_packet.data()),
                dice_hrt_packet.size()
            });

            if (args.verbose >= 3) {
                std::cout << "[DEBUG] Forwarded NORM data: ";
                for (size_t i = 0; i < dice_hrt_packet.size(); ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << int(static_cast<uint8_t>(dice_hrt_packet[i])) << " ";
                }
            }
            if (args.verbose >= 1) {
                std::cout << std::dec << std::setfill(' ') << "\n";
                std::cout << "[DEBUG] Forwarded NORM packet in HRT wrapper to backend. Packet length: " << dice_hrt_packet.size() << "\n";
            }
        }

        if (fd_manager.areBytesWaiting(dice_forward_socket->fd)) {
            activity = true;
            // Send directly to backend
            udp::SocketAddress source;
            auto opt_packet = dice_forward_socket->receive(source);

            if (!opt_packet) {
                std::cout << "[ERROR] DICE forward socket has bytes waiting, but a packet was not received!" << std::endl;
                continue;
            }

            auto const& packet = *opt_packet;
            if (args.verbose >= 2) {
                std::cout << "[DEBUG] Port 50005 received packet of length " << packet.size() << " from " 
                          << source.get_address() << ":" << source.get_port() << "\n"; 
            }
            backend->write(packet);
        }

        // Check TIME_DELTA_COUNTER for overflow
        if (TIME_DELTA_COUNTER >= 200) {
            TIME_DELTA_COUNTER %= 200;
        }

        // Prints out the status packets
        if (running && args.verbose >= 1 && activity) {
            auto framer = std::dynamic_pointer_cast<comms::backend::ccsds>(backend);
            if ( framer ) {
                framer->printByteCounters();
            }
            std::cout << "[DEBUG] Current time delta: " << TIME_DELTA << " ms\n";
            if (args.verbose >= 3) {
                std::cout << "[DEBUG] TIME_DELTA_COUNTER: " << unsigned(TIME_DELTA_COUNTER) << "\n";
            }
            std::cout << "\n\n";
        }
    
    }

    std::cout << "[INFO] Shutting down..." << std::endl;
    return 0;
}

void help() {
    std::cerr << "exact_comms_service: multiplex packets with CCSDS framing, triggered by DICE requests\n";
    std::cerr << "    -h, --help: print this message\n";
    std::cerr << "    -v, --verbose: more verbose output. Can be specified multiple times.\n";
    std::cerr << "    -f, --force: disable file checking\n";
    std::cerr << "    -u, --usec-wait: microseconds to wait after sending a packet\n";
    std::cerr << "    -l, --live-address <ip:port>: set live port address (default: " << DEFAULT_ARGS.live_address << ")\n";
    // std::cerr << "    -n, --norm-address <ip:port>: set norm port address (default: " << DEFAULT_ARGS.norm_address << ")\n";
    std::cerr << "    -N, --norm-receiver <port>: set norm receiver port (default: " << DEFAULT_ARGS.norm_receiver << ")\n";
    std::cerr << "    -s, --serial <interface>: specify serial interface as transfer mode\n";
    std::cerr << "    -b, --baud-rate <Hz>: specify the serial baud rate in Hz (default: 9600)\n";
}

// Parses CLI arguments and returns structured configuration
ProgramArguments parseArgs(int argc, char *argv[]) {
    ProgramArguments ret = DEFAULT_ARGS;

    struct option long_options[] {
        {"help",            no_argument, nullptr, 'h'},
        {"verbose",         no_argument, nullptr, 'v'},
        {"force",           no_argument, nullptr, 'f'},
        {"usec-wait",       required_argument, nullptr, 'u'},
        {"live-address",    required_argument, nullptr, 'l'},
        {"norm-receiver",   required_argument, nullptr, 'N'},
        {"serial",          required_argument, nullptr, 's'},
        {"baud-rate",       required_argument, nullptr, 'b'},
        {nullptr, 0, nullptr, 0}
    };

    int opt{};
    while ((opt = getopt_long(argc, argv, "hvfu:l:N:s:b:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'h': help(); exit(0);
            case 'v': ret.verbose++; break;
            case 'f': ret.force = true; break;
            case 'u': ret.usec_wait = atoi(optarg); break;
            case 'l': ret.live_address = udp::SocketAddress::create(optarg); break;
            case 'N': ret.norm_receiver = atoi(optarg); break;
            case 's': ret.mode = COMMS_MODE::SERIAL; ret.file = optarg; break;
            case 'b': ret.baud_raw = atoi(optarg); ret.baud_rate = parseBaudRate(ret.baud_raw); break;
            default: help(); exit(1);
        }
    }

    return ret;
}

// Maps numeric baud rate values to termios-compatible constants
speed_t parseBaudRate(int baud_rate) {
    switch (baud_rate) {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 115200: return B115200;
        case 230400: return B230400;
        case 921600: return B921600;
        default:
            std::cerr << "Unsupported baud rate: " << baud_rate << "\n";
            exit(1);
    }
}

// Prints command-line arguments for debugging and confirmation
void printArgs(ProgramArguments args) {
    std::cout << "Verbose: " << args.verbose << "\n";
    std::cout << "Live Addr: " << args.live_address << "\n";
    std::cout << "Norm Port: " << args.norm_receiver << "\n";
    std::cout << "Serial mode on " << args.file << " @ " << args.baud_raw << " baud\n";
}
