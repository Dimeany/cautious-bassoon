#pragma once

#include "backend.h"

#include <deque>       
#include <vector>   
#include <memory>   
#include <array>

namespace comms::backend {

    class ccsds : public Backend {
        public:
            // Constructor: Takes ownership of an existing backend
            explicit ccsds(std::shared_ptr<Backend> wrapped, int verbosity = 0, uint8_t target_id_dice = 0x8C);

            // Returns the file descriptor associated with the underlying backend
            int getFd() override;

            // Read data by extracting CCSDS frames
            std::vector<char> read() override;

            // Write data to the backend (can include CCSDS framing)
            size_t write(std::span<char const> data) override;

            // Check if a valid CCSDS frame is available
            bool bytesAvailable() override;

            std::shared_ptr<Backend> inner; // Wrapped backend for communication
            std::deque<uint8_t> buffer;     // Buffer to store incoming bytes before processing

            // Frame properties
            static constexpr std::array<uint8_t, 4> SYNC_PATTERN = {0x1A, 0xCF, 0xFC, 0x1D};    // Frame synchronization pattern

            // CRC table and compute_crc
            static uint16_t compute_crc(const std::vector<uint8_t>& data);     // Computes the CRC-16 checksum for error detection

            // Internal state for streaming parser
            enum class State {
                SearchingForSync,
                ReadingFrame
            };

            State state = State::SearchingForSync;
            size_t sync_index = 0;                  // byte-by-byte sync detection
            std::vector<uint8_t> partial_frame;     // frame being built
            size_t expected_length = 0;             // expected frame length once packet type is known
            
            // Helper to get frame length based on packet type byte
            size_t frame_length_from_type(uint8_t packet_type);
            
            // Helper function
            void fill_buffer();     // Fills the buffer with incoming backend data
            
            // Byte counters for debugging in orbit
            struct ByteCounters {
                uint64_t incoming_bytes = 0;
                uint32_t incoming_packets = 0;

                uint32_t sync0_ok = 0;
                uint32_t sync0_fail = 0;
                uint32_t sync1_ok = 0;
                uint32_t sync1_fail = 0;
                uint32_t sync2_ok = 0;
                uint32_t sync2_fail = 0;
                uint32_t sync3_ok = 0;
                uint32_t sync3_fail = 0;

                uint32_t ptype_ok = 0;
                uint32_t ptype_fail = 0;
                uint32_t targetID_ok = 0;
                uint32_t targetID_fail = 0;

                uint64_t outgoing_bytes = 0;
                uint32_t crc_ok = 0;
                uint32_t crc_fail = 0;

                uint32_t HNS_packets_sent = 0;
                uint32_t LRT_packets_sent = 0;
                uint32_t HRT_packets_sent = 0;
                uint32_t CMD_ACKs_sent = 0;
                uint32_t total_packets = 0;
                // Incoming request counters
                uint32_t HNS_requests_received = 0;
                uint32_t CMD_requests_received = 0;
                uint32_t stop_cmds_received = 0;
                uint32_t go_cmds_received = 0;

                void reset() { *this = ByteCounters{}; }
            };

            // Variable to hold the byte counters
            ByteCounters stats;

            // Verbosity level for debug output (0 = quiet)
            int verbosity = 0;
            uint8_t target_id_dice = 0x8C;

            // Set/get verbosity
            void setVerbosity(int v) { verbosity = v; }
            int getVerbosity() const { return verbosity; }

            // Set/get target ID
            void setTargetID(uint8_t id) { target_id_dice = id; }
            uint8_t getTargetID() const { return target_id_dice; }

            // Serializes the stats into a binary status packet
            std::vector<char> getStatusPacket();
            // Returns 17 fields, each 7 bytes (big-endian), intended for the LRT COMMS region
            std::vector<char> getCommLRTStatusFields();

            // Safe accessors and mutators for select counters
            uint32_t getCrcFail() const;
            uint32_t getTotalPackets() const;
            uint64_t getIncomingBytes() const;
            uint64_t getOutgoingBytes() const;
            uint32_t getIncomingPackets() const;

            void incHNSRequests();
            void incCMDRequests();
            void incStopCmds();
            void incGoCmds();

            // Display the Byte Counters in a human-readable dump
            void printByteCounters();    // (for testing)
    };
}
