#include "ccsds.h"

#include <algorithm>    
#include <iostream>
#include <iomanip>

namespace comms::backend {

    constexpr std::array<uint8_t, 4> ccsds::SYNC_PATTERN;

    // Constructor: Wraps the existing backend
    ccsds::ccsds(std::shared_ptr<Backend> wrapped, int verbosity, uint8_t target_id_dice) : inner(std::move(wrapped)), verbosity(verbosity), target_id_dice(target_id_dice) {
    }

    // Retrieves the file descriptor from the underlying backend 
    int ccsds::getFd() {
        // std::cout << "[CCSDS] getFd() called.\n"; // (for testing)
        return inner->getFd();
    }

    // Frame length mapping by packet type byte (first byte after sync)
    size_t ccsds::frame_length_from_type(uint8_t packet_type) {
        switch (packet_type) {
            case 0x10:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a CMD packet.\n";
                return 120;  // CMD
            case 0xFB:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a File Transfer packet.\n";
                return 574;  // File Transfer
            case 0xC0:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a FT fault packet.\n";
                return 22;   // FT fault
            case 0xA0:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is an HNS request packet.\n";
                return 14;   // HNS request
            case 0x81:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is an LRT request packet.\n";
                return 14;   // LRT request
            case 0x85:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a DICE STOP command packet.\n";
                return 14;   // DICE STOP command
            case 0x87:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a DICE GO command packet.\n";
                return 14;   // DICE GO command
            case 0xD0:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a FT req ACK.\n";
                return 14;   // FT req ACK
            case 0xE0:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a DT data req.\n";
                return 120;  // DT data
            case 0xEC:
                if (verbosity >= 2) std::cout << "[CCSDS] packet_type is a TIMsrc req.\n";
                return 16;   // TIMsrc
            default: return 0;
        }
    }

    // Fill the buffer with incoming backend data
    void ccsds::fill_buffer() {
        while (inner->bytesAvailable()) {
            auto chunk = inner->read();

            // (entire for loop is for testing)
            if (verbosity >= 3) {
                std::cout << "[CCSDS] Read chunk: ";

                if (chunk.size() == 0) {
                    std::cout << "empty chunk" << std::endl;
                    break;
                }

                for (uint8_t b : chunk) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << int(b) << " ";
                }
                std::cout << std::dec << "\n";
            }
                
            for (uint8_t b : chunk) {
                stats.incoming_bytes++;    // count every byte coming from DICE to Experiment
                buffer.push_back(b);
            }
        }
    }

    // Read function: Extracts and returns a valid CCSDS frame
    std::vector<char> ccsds::read() {
        // std::cout << "[CCSDS] read() called.\n"; // (for testing)
        
        // Read more bytes if buffer empty
        if (buffer.empty()) {
            // refill into buffer
            fill_buffer();
            if (buffer.empty())
                return {};
        }

        while (!buffer.empty()) {
            uint8_t byte = buffer.front();

            if (state == State::SearchingForSync) {
                // Byte-by-byte sync detection using sync_index
                if (byte == SYNC_PATTERN[sync_index]) {
                    switch (sync_index) {
                        case 0: stats.sync0_ok++; break;
                        case 1: stats.sync1_ok++; break;
                        case 2: stats.sync2_ok++; break;
                        case 3: stats.sync3_ok++; break;
                    }

                    partial_frame.push_back(byte);
                    sync_index++;

                    if (sync_index == 4) {
                        // Full sync pattern matched
                        if (verbosity >= 2) std::cout << "[CCSDS] Sync Detected\n";
                        state = State::ReadingFrame;
                        expected_length = 0;
                    }

                } else {
                    // Failed to match the expected sync byte
                    if (sync_index == 0) {
                        stats.sync0_fail++;
                        // (for testing)
                        // std::cout << "[SYNC0 FAIL] Saw 0x" << std::hex << std::setw(2) << std::setfill('0') 
                        //           << static_cast<int>(byte) << " instead of 0x1A" << std::dec << std::endl;
                    } else {
                        switch (sync_index) {
                            case 1: stats.sync1_fail++; break;
                            case 2: stats.sync2_fail++; break;
                            case 3: stats.sync3_fail++; break;
                        }
                    }

                    // Reset sync state, but allow immediate restart if byte == 0x1A
                    partial_frame.clear();
                    sync_index = (byte == 0x1A) ? 1 : 0;
                    if (sync_index == 1) {
                        stats.sync0_ok++;
                        partial_frame.push_back(byte);
                    }
                }

                buffer.pop_front();
                continue;
            }

            else if (state == State::ReadingFrame) {
                partial_frame.push_back(byte);
                buffer.pop_front();

                // Try to determine packet type and target ID once we have enough bytes
                if (partial_frame.size() >= 12 && expected_length == 0) {
                    uint8_t packet_type = 0;
                    uint8_t target_ID = 0;
                    
                    // All packets coming from DICE have the packet type at the 11th byte and the target ID at the 12th byte
                    if (partial_frame.size() > 11) {
                        packet_type = partial_frame[10];
                        expected_length = frame_length_from_type(packet_type);
                        target_ID = partial_frame[11];
                    }

                    // --- Handle bad packet type ---
                    // if (expected_length == 0 && !(packet_type == 0x85 || packet_type == 0x87)) {
                    if (expected_length == 0) {
                        stats.ptype_fail++;
                        if (verbosity >= 1) std::cout << "[CCSDS] Unknown packet type 0x" << std::hex << int(packet_type) << std::dec << ". Expected length is "<< expected_length << ". Aborting frame.\n";
                        
                        // Restart sync detection from target ID byte
                        partial_frame.clear();
                        sync_index = (target_ID == 0x1A) ? 1 : 0;
                        if (sync_index == 1) {
                            stats.sync0_ok++;
                            partial_frame.push_back(target_ID);
                        } else {
                            stats.sync0_fail++;
                            // for testing
                            // std::cout << "[SYNC0 FAIL] Saw 0x" << std::hex << std::setw(2) << std::setfill('0') 
                            //           << static_cast<int>(target_ID) << " instead of 0x1A" << std::dec << std::endl;
                        }

                        state = State::SearchingForSync;
                        continue;
                    } else {
                        stats.ptype_ok++;
                    }

                    // --- Handle bad target ID ---
                    if (target_ID != target_id_dice) {
                        stats.targetID_fail++;
                        if (verbosity >= 1) std::cout << "[CCSDS] Invalid target ID 0x" << std::hex << int(target_ID) << " (expected 0x" << std::hex << int(target_id_dice) << "). Dropping frame.\n" << std::dec;

                        // Skip rest of frame
                        while (partial_frame.size() < expected_length && !buffer.empty()) {
                            partial_frame.push_back(buffer.front());
                            buffer.pop_front();
                        }

                        state = State::SearchingForSync;
                        sync_index = 0;
                        partial_frame.clear();
                        continue;
                    } else {
                        stats.targetID_ok++;
                    }
                }

                // Max frame guard
                if (partial_frame.size() > 1600) {
                    if (verbosity >= 1) std::cout << "[CCSDS] Frame too large. Aborting.\n";    // (for testing)
                    state = State::SearchingForSync;
                    partial_frame.clear();
                    //sliding_window.clear();
                    sync_index = 0;
                    continue;
                }

                if (expected_length > 0 && partial_frame.size() == expected_length) {
                    // Frame complete, so check length of packet and check CRC
                    if (expected_length < 4) {
                        if (verbosity >= 1) std::cout << "[CCSDS] Frame too short, discarding.\n";  // (for testing)
                        state = State::SearchingForSync;
                        //sliding_window.clear();
                        sync_index = 0;
                        partial_frame.clear();
                        continue;
                    }

                    uint16_t crc_received = (partial_frame[expected_length - 2] << 8) | partial_frame[expected_length - 1];
                    // CRC calculated over bytes after sync up to (excluding) CRC
                    std::vector<uint8_t> crc_data(partial_frame.begin() + 4, partial_frame.end() - 2);
                    uint16_t crc_calculated = compute_crc(crc_data);
                
                    if (crc_received != crc_calculated) {
                        stats.crc_fail++;

                        // (for testing)
                        if (verbosity >= 1) std::cout << "[CCSDS] CRC mismatch (received 0x" << std::hex << crc_received << ", calculated 0x" << crc_calculated << std::dec << "). Dropping frame.\n";

                        state = State::SearchingForSync;
                        //sliding_window.clear();
                        sync_index = 0;
                        partial_frame.clear();
                        continue;
                    }

                    stats.crc_ok++;

                    // Count a successfully received packet
                    stats.incoming_packets++;

                    // Valid frame found, reset state and return it as vector<char>
                    if (verbosity >= 2) std::cout << "[CCSDS] Valid frame received.\n"; // (for testing)
                    state = State::SearchingForSync;
                    //sliding_window.clear();
                    sync_index = 0;

                    std::vector<char> result(partial_frame.begin(), partial_frame.end());
                    partial_frame.clear();

                    return result;
                }
            }
        }

        // No full frame ready yet
        return {};
    }

    // Checks if there's a valid CCSDS frame available
    bool ccsds::bytesAvailable() {
        // std::cout << "[CCSDS] bytesAvailable() called.\n"; // (for testing)
        
        // Pull in any new bytes
        fill_buffer();

        // If there's at least the smallest possible frame, report true
        if (!buffer.empty() || inner->bytesAvailable()) {
            // std::cout << "[CCSDS] Buffer has " << buffer.size() << " bytes - ready to parse\n\n";   // (for testing)
            return true;
        }

        // std::cout << "[CCSDS] Buffer has " << buffer.size() << " bytes - not enough for a frame\n\n";   // (for testing)
        return false;
    }

    // Write data to the backend
    size_t ccsds::write(std::span<char const> packet) {
        if (verbosity >= 2) std::cout << "[CCSDS] write() called with " << packet.size() << " bytes.\n"; // (for testing)
        // FOR RIGHT NOW THE ENTIRE HNS AND LRT PACKETS ARE BEING CREATED IN exact_comms_service (may need to change this later)

        // (entire for loop is for testing)
        if (verbosity >= 3) {
            std::cout << "[CCSDS] Outgoing frame: ";
            for (char c : packet) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << int(c) << " ";
            }
            std::cout << std::dec << "\n";

            std::cout << "[CCSDS] Sending framed packet of size " << packet.size() << "\n"; // (for testing)
        }
        
        // Track bytes and total packets from experiment to DICE
        stats.outgoing_bytes += packet.size();
        stats.total_packets++;
        
        // Track HNS packets and LRT packets sent from experiment to DICE
        uint8_t packet_type = static_cast<uint8_t>(packet[4]);

        switch (packet_type) {
            case 0xA0:
                stats.HNS_packets_sent++;
                break;
            case 0x81:
                stats.LRT_packets_sent++;
                break;
            case 0x10:
                stats.CMD_ACKs_sent++;
                break;
            case 0x87:
                stats.HRT_packets_sent++;
                break;
            default:
                break; // only counting HNS/LRT packets for now
        }
        
        return inner->write(packet);
    }

    // Create and return the Byte Counters status packet
    std::vector<char> ccsds::getStatusPacket() {
        std::vector<char> packet;

        auto append_uint32 = [&](uint32_t val) {
            for (int i = 3; i >= 0; --i)
                packet.push_back(static_cast<char>((val >> (8 * i)) & 0xFF));
        };

        auto append_uint64 = [&](uint64_t val) {
            for (int i = 7; i >= 0; --i)
                packet.push_back(static_cast<char>((val >> (8 * i)) & 0xFF));
        };

        append_uint64(stats.incoming_bytes);

        append_uint32(stats.sync0_ok);
        append_uint32(stats.sync0_fail);
        append_uint32(stats.sync1_ok);
        append_uint32(stats.sync1_fail);
        append_uint32(stats.sync2_ok);
        append_uint32(stats.sync2_fail);
        append_uint32(stats.sync3_ok);
        append_uint32(stats.sync3_fail);

        append_uint32(stats.ptype_ok);
        append_uint32(stats.ptype_fail);
        append_uint32(stats.targetID_ok);
        append_uint32(stats.targetID_fail);

        append_uint32(stats.crc_ok);
        append_uint32(stats.crc_fail);

        append_uint64(stats.outgoing_bytes);
        append_uint32(stats.HNS_packets_sent);
        append_uint32(stats.LRT_packets_sent);
        append_uint32(stats.HRT_packets_sent);
        append_uint32(stats.CMD_ACKs_sent);
        append_uint32(stats.total_packets);

        return packet;
    }

    // Return 17 fields packed as: 4B pkts_in, 4B pkts_out, 8B bytes_in, 8B bytes_out,
    // then 10x4B fields (CRC_errs, sync_fail[0..3], bad_pkt_type, bad_target_address,
    // HNSreq, LRTreq, HRTreq, CMD, STOP, GO). All big-endian.
    std::vector<char> ccsds::getCommLRTStatusFields() {
        std::vector<char> out;
        out.reserve(80);

        auto append_uint32 = [&](uint32_t v) {
            for (int i = 3; i >= 0; --i)
                out.push_back(static_cast<char>((v >> (8 * i)) & 0xFF));
        };
        auto append_uint64 = [&](uint64_t v) {
            for (int i = 7; i >= 0; --i)
                out.push_back(static_cast<char>((v >> (8 * i)) & 0xFF));
        };

        // pkts_in
        append_uint32(static_cast<uint32_t>(stats.incoming_packets));
        // pkts_out (reuse total_packets)
        append_uint32(static_cast<uint32_t>(stats.total_packets));
        // bytes_in
        append_uint64(static_cast<uint64_t>(stats.incoming_bytes));
        // bytes_out
        append_uint64(static_cast<uint64_t>(stats.outgoing_bytes));

        // CRC_errs
        append_uint32(stats.crc_fail);

        // sync_fail for 4 sync bytes
        append_uint32(stats.sync0_fail);
        append_uint32(stats.sync1_fail);
        append_uint32(stats.sync2_fail);
        append_uint32(stats.sync3_fail);

        // bad_pkt_type
        append_uint32(stats.ptype_fail);
        // bad_target_address
        append_uint32(stats.targetID_fail);

        // HNSreq incoming HNS requests received
        append_uint32(stats.HNS_requests_received);
        // LRTreq reuse LRT_packets_sent
        append_uint32(stats.LRT_packets_sent);
        // HRT packets sent 
        append_uint32(stats.HRT_packets_sent);
        // CMD incoming CMD requests received
        append_uint32(stats.CMD_requests_received);
        // STOP cmds received
        append_uint32(stats.stop_cmds_received);
        // GO cmds received
        append_uint32(stats.go_cmds_received);

        return out;
    }

    // Accessors
    uint32_t ccsds::getCrcFail() const { return stats.crc_fail; }
    uint32_t ccsds::getTotalPackets() const { return stats.total_packets; }
    uint64_t ccsds::getIncomingBytes() const { return stats.incoming_bytes; }
    uint64_t ccsds::getOutgoingBytes() const { return stats.outgoing_bytes; }
    uint32_t ccsds::getIncomingPackets() const { return stats.incoming_packets; }

    // Increment helpers
    void ccsds::incHNSRequests() { stats.HNS_requests_received++; }
    void ccsds::incCMDRequests() { stats.CMD_requests_received++; }
    void ccsds::incStopCmds() { stats.stop_cmds_received++; }
    void ccsds::incGoCmds() { stats.go_cmds_received++; }

    // This entire function is for testing
    void ccsds::printByteCounters() {
        std::cout << "----- Byte Counters Stats -----\n\n";
        
        std::cout << "From DICE to Experiment\n";
        std::cout << "Total Bytes: " << stats.incoming_bytes << "\n\n";
        
        std::cout << "Sync0 OK/Fail: " << stats.sync0_ok << "/" << stats.sync0_fail << "\n";
        std::cout << "Sync1 OK/Fail: " << stats.sync1_ok << "/" << stats.sync1_fail << "\n";
        std::cout << "Sync2 OK/Fail: " << stats.sync2_ok << "/" << stats.sync2_fail << "\n";
        std::cout << "Sync3 OK/Fail: " << stats.sync3_ok << "/" << stats.sync3_fail << "\n\n";
        
        std::cout << "Packet Type OK/Fail: " << stats.ptype_ok << "/" << stats.ptype_fail << "\n";
        std::cout << "Target ID OK/Fail: " << stats.targetID_ok << "/" << stats.targetID_fail << "\n\n";
        
        std::cout << "CRC OK/Fail: " << stats.crc_ok << "/" << stats.crc_fail << "\n\n";

        std::cout << "From Experiment to DICE\n";
        std::cout << "Total Bytes: " << stats.outgoing_bytes << "\n";
        std::cout << "HNS packets sent: " << stats.HNS_packets_sent << "\n";
        std::cout << "LRT packets sent: " << stats.LRT_packets_sent << "\n";
        std::cout << "HRT packets sent: " << stats.HRT_packets_sent << "\n";
        std::cout << "CMD ACKs sent: " << stats.CMD_ACKs_sent << "\n";
        std::cout << "Total packets sent: " << stats.total_packets << "\n\n";

        std::cout << "Incoming Requests Received\n";
        std::cout << "HNS requests: " << stats.HNS_requests_received << "\n";
        std::cout << "CMD requests: " << stats.CMD_requests_received << "\n";
        std::cout << "STOP commands: " << stats.stop_cmds_received << "\n";
        std::cout << "GO commands: " << stats.go_cmds_received << "\n";

        std::cout << "-------------------------------\n\n";
    }

    // Precomputed CRC-16 (polynomial 0x1021) lookup table
    static const uint16_t CRCtable[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
        0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
        0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
        0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
        0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
        0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
        0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
        0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
        0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
        0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
        0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
        0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
        0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
        0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
        0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
        0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
        0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
        0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
        0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
        0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
        0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
        0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
        0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
        0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
        0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
        0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
        0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
        0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
        0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
        0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
        0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
        0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
        0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };

    // Compute CRC-16 checksum for frame validation
    uint16_t ccsds::compute_crc(const std::vector<uint8_t>& data) {
        uint16_t crc = 0xFFFF;  // Initialize CRC value
        for (uint8_t byte : data) {     // Process each byte
            uint8_t index = static_cast<uint8_t>((crc >> 8) ^ byte);
            crc = static_cast<uint16_t>((crc << 8) ^ CRCtable[index]);
            crc &= 0xFFFF;
        }
        return crc; // Return computed CRC
    }
    
}