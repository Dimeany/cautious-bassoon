#include <arpa/inet.h>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <syslog.h>
#include <systemd/sd-journal.h>
#include <thread>

#include "backends/ccsds.h"
#include "bft_recv.h"
#include "file_descriptor_manager.h"
#include "../udp/socket.h"

// Macros for reading and writing bft_file structs from and to disk
#define WRITE_POD(stream, val) stream.write(reinterpret_cast<const char*>(&(val)), sizeof(val))
#define READ_POD(stream, val) stream.read(reinterpret_cast<char*>(&(val)), sizeof(val))

// Default command-line parameters
const ProgramArguments DEFAULT_ARGS {
    .verbose = 0,
    .receive_address = udp::SocketAddress::create("127.0.0.1:50015"),
    .max_file_age_ms = 3 * 60 * 1000, // default to 3 minutes
    .prune_interval_ms = 1 * 60 * 1000, // default to 1 minute
    .comms_service_cmd_socket_address = udp::SocketAddress::create("127.0.0.1:50002"),
    .report_port = 50020,
    .wipe_disk_only = 0,
    .missing_chunks_report = 0
};

// Global variables
ProgramArguments ARGS {};
std::shared_ptr<udp::Socket> RECV_SOCKET;
std::vector<bft_file> BFT_FILES;
bool RUNNING = true;
std::mutex BFT_MUTEX;
std::condition_variable PRUNE_CV;
std::string FAIL_DIRECTORY = "fail/";
size_t EXPECTED_CHUNK_SIZE = 100;

// Helper function to get timestamp for received files
uint64_t getFileTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

// Helper function to get elapsed milliseconds since a given timestamp
int getElapsedMsSinceTimestamp(uint64_t timestamp) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return now_ms - timestamp;
}

// Helper function to prune the list of received files based on elapsed time since last received chunk
// Returns the number of files pruned
int pruneBFTList(int max_age_ms) {
    int num_pruned = 0;
    auto it = BFT_FILES.begin();
    while (it != BFT_FILES.end()) {
        int elapsed_ms = getElapsedMsSinceTimestamp(it->timestamp);
        if (elapsed_ms > max_age_ms) {
            // Write data and info
            writeFileData(FAIL_DIRECTORY + it->filename, it->data);
            writeBFTFileToDisk(*it);

            // Delete from memory
            it = BFT_FILES.erase(it);
            num_pruned++;
        } else {
            ++it;
        }
    }
    return num_pruned;
}

int writeFileData(std::string filename, std::vector<char> data) {
    // Write the file to disk
    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile.is_open()) {
        std::cerr << "[ERROR] Failed to open file! Check permissions or invalid characters in the filename.\n";
        return -1;
    }

    outfile.write(data.data(), data.size());
    
    if (!outfile.good()) {
        std::cerr << "[ERROR] The disk write operation failed!\n";
        return -1;
    }

    outfile.flush();
    outfile.close();

    return 0;
}

// Helper function to priuint8_t file_idnt program arguments for user verification
std::optional<bft_header_flags> parseHeaderFlags(const std::vector<char> header) {
    try {
        int isData = (header[4] >> 7) & 1;
        int isLastBlock = (header[4] >> 6) & 1;
        int isRepair = (header[4] >> 5) & 1;

        return bft_header_flags { isData, isLastBlock, isRepair };
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to interpret header fields: " << e.what() << std::endl;
        return std::nullopt;
    }
}

// Helper function to create report message
std::string createReportMessage(const bft_file& file, std::string reason) {
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[256];
    strcpy(buf, std::ctime(&now));
    buf[strlen(buf)-1] = '\0';

    std::string crc_message;
    switch (file.crc_check_passed) {
        case -1:
            crc_message = "Failed";
            break;
        case 0:
            crc_message = "Not Yet Attempted";
            break;
        case 1:
            crc_message = "Passed";
            break;
    }

    std::stringstream ss;
    ss << "Date: " << now << " / " << buf
        << "\nReport Reason: " << reason
        << "\nFile ID: " << int(file.file_id)
        << "\nFilename: " << file.filename
        << "\nFilesize: " << file.filesize << "B"
        << "\nChunks Received: " << file.chunks_recd << "/" << file.expected_chunks
        << "\nCRC Check: " << crc_message
        << "\nWritten to Disk: " << (file.written ? "Yes" : "No");
    return ss.str();
}

// Helper function to send report message to the COMMS service
void sendReport(const bft_file& file, std::string reason) {
    std::string report_message = createReportMessage(file, reason);
    uint16_t our_port = ARGS.receive_address.get_port();
    udp::SocketAddress comms_address = ARGS.comms_service_cmd_socket_address;

    // Write report to journal
    int ret = sd_journal_send(
        "MESSAGE=Bulk File Transfer Result:\n%s",
        report_message.c_str(),
        "PRIORITY=%i", LOG_INFO,
        NULL
    );
    if (ret < 0) {
        std::cerr << "Failed to send log to systemd journal: " << std::strerror(errno) << std::endl;
    } else {
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] Successfully sent report to systemd journal for file ID " << int(file.file_id) << "\n";
        }
    }

    // Create a response packet
    std::vector<char> response_pkt = udp::Packet::create_from_buffer(
        report_message, 
        ARGS.report_port,
        our_port,
        0,
        0
    ).serialize();
    RECV_SOCKET->send(response_pkt, comms_address);
    if (ARGS.verbose >= 1) {
        std::cout << "[INFO] Successfully sent report to COMMS service for file ID " << int(file.file_id) << "\n";
    }
}

// Helper function to save a bft_file struct to disk in a binary format
int writeBFTFileToDisk(const bft_file& bft) {
    std::string info_filename = FAIL_DIRECTORY + bft.filename + "_info";
    std::ofstream out(info_filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "[ERROR] Failed to open file for writing BFT file metadata! Check permissions or invalid characters in the filename.\n";
        return -1;
    }

    size_t name_len = bft.filename.size();
    WRITE_POD(out, name_len);
    out.write(bft.filename.data(), name_len);

    WRITE_POD(out, bft.filesize);
    WRITE_POD(out, bft.expected_chunks);
    WRITE_POD(out, bft.file_crc);
    WRITE_POD(out, bft.file_id);
    WRITE_POD(out, bft.filename_len);
    WRITE_POD(out, bft.chunks_recd);
    WRITE_POD(out, bft.last_chunk_recd);
    WRITE_POD(out, bft.crc_check_passed);
    WRITE_POD(out, bft.written);
    WRITE_POD(out, bft.timestamp);

    size_t data_size = bft.data.size();
    WRITE_POD(out, data_size);
    if (data_size > 0) out.write(bft.data.data(), data_size);

    // vector<bool> doesn't guarantee contiguous memory, so we write elements individually
    size_t bool_size = bft.received_chunks.size();
    WRITE_POD(out, bool_size);
    for (bool val : bft.received_chunks) {
        uint8_t b = val ? 1 : 0;
        WRITE_POD(out, b);
    }

    return 0;
}

// Helper function to load a bft_file struct from disk given a filename, reading the binary format written by writeBFTFileToDisk
std::optional<bft_file> readBFTFileFromDisk(std::string filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "[ERROR] Failed to open file for reading BFT file metadata! Check permissions or invalid characters in the filename.\n";
        return std::nullopt;
    }

    bft_file bft;
    size_t name_len;
    READ_POD(in, name_len);
    bft.filename.resize(name_len);
    in.read(&bft.filename[0], name_len);

    READ_POD(in, bft.filesize);
    READ_POD(in, bft.expected_chunks);
    READ_POD(in, bft.file_crc);
    READ_POD(in, bft.file_id);
    READ_POD(in, bft.filename_len);
    READ_POD(in, bft.chunks_recd);
    READ_POD(in, bft.last_chunk_recd);
    READ_POD(in, bft.crc_check_passed);
    READ_POD(in, bft.written);
    READ_POD(in, bft.timestamp);

    size_t data_size;
    READ_POD(in, data_size);
    bft.data.resize(data_size);
    if (data_size > 0) in.read(bft.data.data(), data_size);

    size_t bool_size;
    READ_POD(in, bool_size);
    bft.received_chunks.resize(bool_size);
    for (size_t i = 0; i < bool_size; ++i) {
        uint8_t b;
        READ_POD(in, b);
        bft.received_chunks[i] = (b != 0);
    }

    return bft;
}

void generateMissingChunksReport() {
    // Acquire mutex lock
    std::lock_guard<std::mutex> lock(BFT_MUTEX);

    int total_missing_chunks = 0;

    std::cout << "Missing Chunks Report (chunks are zero-indexed):\n";

    for (const bft_file& file : BFT_FILES) {
        if (file.chunks_recd < file.expected_chunks) {
            std::vector<int> missing_chunks;
            for (int i = 0; i < file.expected_chunks; i++) {
                if (!file.received_chunks[i]) {
                    missing_chunks.push_back(i);
                }
            }

            if (missing_chunks.empty()) continue;

            total_missing_chunks += missing_chunks.size();

            std::cout << "File: " << file.filename << " (ID: " << int(file.file_id) 
                      << ") - Missing " << (file.expected_chunks - file.chunks_recd) << " chunks\n";

            int start = missing_chunks[0];
            int end = start;

            auto printGroup = [](int s, int e) {
                if (s == e) {
                    std::cout << "  - Chunk " << s << "\n";
                } else if (e - s == 1) {
                    std::cout << "  - Chunks " << s << " and " << e << "\n";
                } else {
                    std::cout << "  - Chunks " << s << " through " << e << "\n";
                }
            };

            for (size_t i = 1; i < missing_chunks.size(); ++i) {
                if (missing_chunks[i] == end + 1) {
                    end = missing_chunks[i];
                } else {
                    printGroup(start, end);
                    start = missing_chunks[i];
                    end = start;
                }
            }
            
            // print the final group/chunk
            printGroup(start, end);
        }
    }

    if (total_missing_chunks == 0) {
        std::cout << "No missing chunks found!\n";
    }
}

int initializeMemoryFromDiskChunks() {
    // Acquire mutex lock
    std::lock_guard<std::mutex> lock(BFT_MUTEX);
    
    std::vector<std::string> info_files;
    std::vector<std::string> data_files;
    
    int successful_info_file_parsed = 0;
    int successful_data_file_parsed = 0;

    // Assemble separate vectors of info and chunk files
    for (const auto& entry : std::filesystem::directory_iterator(FAIL_DIRECTORY)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            if (path.find("_info") != std::string::npos) {
                info_files.push_back(path);
            } else {
                data_files.push_back(path);
            }
        }
    }

    if (ARGS.verbose >= 2) {
        std::cout << "[INFO] Found " << info_files.size() << " info files on disk to load into memory\n";
        std::cout << "[INFO] Found " << data_files.size() << " data files on disk to load into memory\n";
    }

    // Can exit early if no such files exist
    if (info_files.size() == 0) {
        return 1;
    }

    // Handle all info files
    for (const auto& info_file : info_files) {
        auto opt_bft = readBFTFileFromDisk(info_file);
        if (opt_bft) {
            bft_file bft = *opt_bft;

            if (ARGS.verbose >= 3) {
                std::cout << "[DEBUG] Parsed BFT file metadata from disk for file ID " << std::hex << int(bft.file_id) << std::dec << " with filename: " << bft.filename 
                          << ", filesize: " << bft.filesize << ", expected_chunks: " << bft.expected_chunks 
                          << ", chunks_recd: " << bft.chunks_recd << ", last_chunk_recd: " << bft.last_chunk_recd
                          << ", crc_check_passed: " << bft.crc_check_passed << ", written: " << bft.written
                          << ", timestamp: " << bft.timestamp << "\n";
            }

            // Only add if not written to disk and not all chunks received
            if (bft.written || bft.expected_chunks == bft.chunks_recd) {
                if (ARGS.verbose >= 1) {
                    std::cout << "[INFO] Skipping loading BFT file metadata from disk for file ID " << std::hex << int(bft.file_id) << std::dec << " with filename: " << bft.filename << " since it is already marked as written or has all chunks received\n";
                }
                continue;
            }

            BFT_FILES.push_back(bft);
            successful_info_file_parsed++;
            if (ARGS.verbose >= 2) {
                std::cout << "[INFO] Loaded BFT file metadata from disk for file ID " << std::hex << int(bft.file_id) << std::dec << " with filename: " << bft.filename << "\n";
            }
        } else {
            std::cerr << "[ERROR] Failed to load BFT file metadata from disk for file: " << info_file << "\n";
        }
    }

    // Handle existing data files
    for (const auto& data_file : data_files) {
        // Check for metadata
        // filenames will start with 'fail/'
        auto it = std::find_if(BFT_FILES.begin(), BFT_FILES.end(), [data_file](const bft_file& f) { return f.filename == data_file.substr(5); });
        if (it == BFT_FILES.end()) {
            // Disk file does not have an entry in BFT_FILES
            if (ARGS.verbose >= 1) {
                std::cout << "[WARN] File " <<  data_file << " does not exist in BFT_FILES vector\n";
            }
            return 1;
        }
        
        // Read from disk
        std::ifstream infile(data_file, std::ios::binary);

        if (!infile.is_open()) {
            std::cerr << "[ERROR] Failed to open file! Check permissions or invalid characters in the filename.\n";
            return -1;
        }

        std::vector<char> filedata((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
        it->data = filedata;
        infile.close();
        successful_data_file_parsed++;

        if (ARGS.verbose >= 2) {
            std::cout << "[INFO] Loaded file data from disk for file " << data_file << " with size: " << filedata.size() << "B\n";
        }
    }

    if (ARGS.verbose >= 1) {
        std::cout << "[INFO] Completed loading BFT files and chunks from disk. Total files in memory: " << BFT_FILES.size() << "\n";
        std::cout << "[INFO] Successfully parsed " << successful_info_file_parsed << "/" << info_files.size() << " info packet files\n";
        std::cout << "[INFO] Successfully parsed " << successful_data_file_parsed << "/" << data_files.size() << " data files\n";
    }

    return 0;
}

// Helper function to finalize the file transfer process for a given file by computing and checking the CRC, 
// writing the file to disk if the CRC check passes, and removing the file from memory
int finalizeFileTransfer(bft_file& file, uint8_t file_id, std::vector<bft_file>::iterator it) {
    // Compute and compare CRC
    uint16_t crc = comms::backend::ccsds::compute_crc(std::vector<uint8_t>(file.data.begin(), file.data.end()));
    if (crc == file.file_crc) {
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] CRC check passed for file ID " <<  int(file_id) << "\n";
        }

        file.crc_check_passed = 1;
    } else {
        std::cout << "[ERROR] CRC check failed for file ID " <<  int(file_id) << "\n";
        file.crc_check_passed = -1;
    }

    if (ARGS.verbose >= 1) {
        std::cout << "[INFO] All checks passed for file ID " <<  int(file_id) << ". Writing to disk...\n";
    }

    std::string correct_filename;

    if (file.crc_check_passed == -1) {
        correct_filename = FAIL_DIRECTORY + file.filename;
    } else if (file.crc_check_passed == 1) {
        correct_filename = file.filename;
    }

    int res = writeFileData(correct_filename, file.data);
    if (res < 0) {
        std::cout << "[ERROR] Failed to write file ID " <<  int(file_id) << " to disk!\n";
        // Keep going as this isn't a fatal error, it just means the file won't be saved to disk
    } else if (ARGS.verbose >= 1) {
        std::cout << "[INFO] Successfully wrote file ID " <<  int(file_id) << " to disk with filename: " << correct_filename << "\n";
    }

    file.written = 1;

    // Delete info file and file in FAIL_DIRECTORY
    std::string info_filename = FAIL_DIRECTORY + file.filename + "_info";
    if (std::filesystem::exists(info_filename)) {
        std::filesystem::remove(info_filename);
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] Deleted info file for file ID " <<  int(file_id) << " with filename: " << info_filename << "\n";
        }
    }
    if (std::filesystem::exists(FAIL_DIRECTORY + file.filename)) {
        std::filesystem::remove(FAIL_DIRECTORY + file.filename);
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] Deleted fail directory file for file ID " <<  int(file_id) << " with filename: " << FAIL_DIRECTORY + file.filename << "\n";
        }
    }

    // Send and print report
    if (ARGS.verbose >= 1) {
        std::cout << "[INFO] Sending report for file ID " <<  int(file_id) << "...\n";
    }
    sendReport(file, "File Transfer Finalization");

    // File written, remove the file from memory and disk (if applicable)
    if (file.written) {
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] Removing file ID " <<  int(file_id) << " from memory and disk...\n";
        }

        // Reacquire lock
        std::lock_guard<std::mutex> lock(BFT_MUTEX);
        BFT_FILES.erase(it);
    }
    
    return 0;
}

// Helper function to handle a received BFT info packet by extracting the file metadata from the packet, 
// creating a new bft_file struct, and adding it to the global list of in-progress file transfers
int handleBFTInfoPacket(std::vector<char> header, const std::vector<char> packet) {
    // Extract header fields for CRC and file ID number, ignoring destination port
    uint16_t file_crc = ntohs(*reinterpret_cast<uint16_t*>(header.data())); // first 2 bytes
    uint8_t file_id = header[4] & 0x1F; // last (5th) byte, masking out the top 3 bits which are used for flags

    if (ARGS.verbose >= 2) {
        std::cout << "[DEBUG] Interpreted header fields:\n";
        std::cout << "    file_crc:" << std::hex << int(file_crc) << std::dec << "\n";
        std::cout << "    file_id: " << int(file_id) << "\n";
    }

    // Extract filename length, file size, and file name from packet body
    std::vector<char> body;
    body.resize(packet.size() - 5);
    std::copy(packet.begin() + 5, packet.end(), body.begin());

    uint8_t filename_len = *reinterpret_cast<uint8_t*>(body.data()); // first byte
    uint32_t filesize = ntohl(*reinterpret_cast<uint32_t*>(body.data() + 1)); // next 4 bytes
    std::string filename = std::string(body.data() + 5, body.data() + 5 + filename_len); // next filename_len bytes

    if (ARGS.verbose >= 1) {
        std::cout << "[DEBUG] Interpreted body fields:\n";
        std::cout << "    filename_len: " << std::hex << int(filename_len) << std::dec << "\n";
        std::cout << "    filesize: " << filesize << "\n";
        std::cout << "    filename: " << filename << "\n";
    }

    std::lock_guard<std::mutex> lock(BFT_MUTEX);
    auto it = std::find_if(BFT_FILES.begin(), BFT_FILES.end(), [file_id](const bft_file& f) { return f.file_id == file_id; });
    if (it != BFT_FILES.end()) {
        // Info packet is for a file that already exists in memory
        if (ARGS.verbose >= 1) {
            std::cout << "[WARN] Received info packet for file ID " <<  int(file_id) << " which already exists in BFT_FILES vector\n";
        }
        return 1;
    }

    // Create new bft_file struct and add to list of files being received
    bft_file new_file {
        .filename = filename,
        .filesize = filesize,
        .expected_chunks = static_cast<int>((filesize + EXPECTED_CHUNK_SIZE - 1) / EXPECTED_CHUNK_SIZE),
        .file_crc = file_crc,
        .file_id = file_id,
        .filename_len = filename_len,
        .data = std::vector<char>(filesize),
        .received_chunks = std::vector<bool>((filesize + EXPECTED_CHUNK_SIZE - 1) / EXPECTED_CHUNK_SIZE, false),
        .timestamp = getFileTimestamp()
    };

    // Write info packet to disk
    if (writeBFTFileToDisk(new_file) < 0) {
        std::cout << "[ERROR] Failed to write info packet to disk for file ID " << int(file_id) << "!\n";
        // Keep going as this isn't a fatal error, it just means we won't be able to reinitialize the file struct from disk
    } else if (ARGS.verbose >= 2) {
        std::cout << "Wrote info packet for file ID " << int(file_id) << " to disk with filename: " << filename << "_info\n";
    }

    if (ARGS.verbose >= 1) {
        std::cout << "[DEBUG] Created new file entry for file ID " << int(file_id) << " with filename: " << filename << ", filesize: " << filesize << ", expected chunks: " << new_file.expected_chunks << "\n";
    }

    BFT_FILES.push_back(new_file);
    return 0;
}

// Helper function to handle a received BFT data packet by extracting the block number and file ID from the header, 
// storing the packet data in the correct position within the corresponding bft_file struct, and finalizing the 
// file transfer if this is the last block or a repair block and all expected blocks have been received
int handleBFTDataPacket(std::vector<char> header, std::optional<bft_header_flags> header_flags, const std::vector<char> packet) {
    // Extract header fields for block number and file ID number, ignoring destination port
    uint16_t block_num = ntohs(*reinterpret_cast<uint16_t*>(header.data())); // first 2 bytes
    uint8_t file_id = header[4] & 0x1F; // last (5th) byte, masking out the top 3 bits which are used for flags

    if (ARGS.verbose >= 1) {
        std::cout << "[DEBUG] Interpreted header fields:\n";
        std::cout << "    block_num: " << block_num << "\n";
        std::cout << "    file_id: " << int(file_id) << "\n";
    }

    // Find the corresponding bft_file struct based on file ID number
    std::lock_guard<std::mutex> lock(BFT_MUTEX);

    auto it = std::find_if(BFT_FILES.begin(), BFT_FILES.end(), [file_id](const bft_file& f) { return f.file_id == file_id; });
    if (it == BFT_FILES.end()) {
        std::cout << "[ERROR] Received data packet for file ID " << int(file_id) << " but no corresponding file metadata found!" << std::endl;
        return -1;
    }

    bft_file& file = *it;

    // Check if the block number is within the expected range for this file
    if (block_num >= file.expected_chunks) {
        std::cout << "[ERROR] Received block number " << block_num 
                  << " which exceeds expected chunks for file ID " 
                  << int(file_id) << "!\n";
        return -1;
    }

    // Check if the received chunk already exists in the file
    // Ignore and update timestamp if chunk already exists unless received packet is a repair packet
    if (file.received_chunks[block_num] && !header_flags->isRepair) {
        if (ARGS.verbose >= 1) {
            std::cout << "[WARN] Received non-repair duplicate chunk for file ID " << int(file_id) << ", block number " << block_num << ". Ignoring this chunk.\n";
        }
        file.timestamp = getFileTimestamp();
        return 0;
    }

    // Calculate where this chunk belongs in the flat buffer
    size_t offset = block_num * EXPECTED_CHUNK_SIZE;
    size_t payload_size = packet.size() - 5;
    size_t bytes_to_copy = std::min<size_t>(payload_size, file.data.size() - offset);

    // Store the packet data in the correct block number position within the file's data vector if it fits
    if (block_num < file.expected_chunks) {
        std::copy(packet.begin() + 5, packet.begin() + 5 + bytes_to_copy, file.data.begin() + offset);
        file.chunks_recd++;
        file.last_chunk_recd = block_num;
        file.received_chunks[block_num] = true;

        // Update the timestamp for the file to reflect the receipt of a new chunk
        file.timestamp = getFileTimestamp();
    } else {
        std::cout << "[ERROR] Received block number " << block_num << " which exceeds expected number of chunks for file ID " << int(file_id) << "!" << std::endl;
        return -1;
    }

    if (ARGS.verbose >= 1) {
        std::cout << "[DEBUG] Stored data for file ID " << int(file_id) << ", block number " << block_num << ". Total chunks received for this file: " << file.chunks_recd << "/" << file.expected_chunks << "\n";
    }

    // If this is the last block or is a repair block, check if all blocks have been received and print a message
    if (ARGS.verbose >= 1) {   
        if (header_flags->isLastBlock && !(file.chunks_recd == file.expected_chunks)) {
            std::cout << "[WARN] Received last block for file ID " << int(file_id) << " but only " << file.chunks_recd << "/" << file.expected_chunks << " chunks have been received.\n";
        } else if (header_flags->isRepair && !(file.chunks_recd == file.expected_chunks)) {
            std::cout << "[WARN] Received repair block for file ID " << int(file_id) << " but only " << file.chunks_recd << "/" << file.expected_chunks << " chunks have been received.\n";
        } else if (header_flags->isRepair && file.chunks_recd == file.expected_chunks) {
            std::cout << "[INFO] Received repair block for file ID " << int(file_id) << " but all expected blocks have already been received.\n";
        } else if (header_flags->isLastBlock && file.chunks_recd == file.expected_chunks) {
            std::cout << "[INFO] Received last block for file ID " << int(file_id) << " and all expected blocks have been received.\n";
        }
    }

    // Unlock BFT mutex
    BFT_MUTEX.unlock();

    if (header_flags->isLastBlock) {
        sendReport(file, "Last Block Received");
    }
    
    if (header_flags->isLastBlock || header_flags->isRepair) {
        return finalizeFileTransfer(file, file_id, it);
    }

    return 0;
}

// Worker function to periodically prune the list of received files based on their age
void prune_worker(int interval_ms) {
    std::unique_lock<std::mutex> lock(BFT_MUTEX);
    
    while (RUNNING) {
        // Sleep for interval_ms, but wake up immediately if RUNNING becomes false
        if (PRUNE_CV.wait_for(lock, std::chrono::milliseconds(interval_ms), []{ return !RUNNING; })) {
            break; 
        }

        // Prune
        int pruned = pruneBFTList(ARGS.max_file_age_ms);
        if (ARGS.verbose >= 1) {
            std::cout << "[DEBUG] Prune worker ran. Pruned " << pruned << " files. " << BFT_FILES.size() << " files remain.\n";
            if (pruned > 0 && ARGS.verbose >= 2) {
                // Print IDs of remaining files
                std::vector<uint8_t> remaining_IDs;

                for (const auto& pair : BFT_FILES) {
                    remaining_IDs.push_back(pair.file_id);
                }

                if (BFT_FILES.size() == 0) {
                    std::cout << "[DEBUG] No remaining files after prune.\n";
                } else {
                    std::cout << "[DEBUG] IDs of remaining files: ";
                    for (const auto& remaining_file : BFT_FILES) {
                        std::cout << static_cast<int>(remaining_file.file_id) << " ";
                    }
                    std::cout << "\n";
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Parse CLI arguments into config
    ARGS = parseArgs(argc, argv);

    // Create fail directory if it doesn't exist
    if (!std::filesystem::exists(FAIL_DIRECTORY)) {
        if (std::filesystem::create_directory(FAIL_DIRECTORY)) {
            if (ARGS.verbose >= 1) {
                std::cout << "[INFO] Created fail directory: " << FAIL_DIRECTORY << "\n";
            }
        } else {
            std::cerr << "[ERROR] Failed to create fail directory: " << FAIL_DIRECTORY << "\n";
            return 1;
        }
    } else if (ARGS.verbose >= 1) {
        std::cout << "[INFO] Fail directory already exists: " << FAIL_DIRECTORY << "\n";
    }

    if (ARGS.verbose >= 1) {
        printArgs(ARGS);
        // Also print working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::cout << "Working directory: " << cwd << "\n";
        } else {
            std::cerr << "[ERROR] Failed to get working directory.\n";
        }
    }

    if (ARGS.wipe_disk_only) {
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] Wipe disk only flag is set. Attempting to wipe files from disk without starting the receive service...\n";
        }
        int total_files_removed = 0;
        for (const auto& entry : std::filesystem::directory_iterator(FAIL_DIRECTORY)) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();
                if (std::remove(path.c_str()) == 0) {
                    if (ARGS.verbose >= 2) {
                        std::cout << "[DEBUG] Successfully removed file: " << path << "\n";
                    }
                    total_files_removed++;
                } else {
                    std::cerr << "[ERROR] Failed to remove file: " << path << "\n";
                }
            }
        }

        if (ARGS.verbose >= 1) {
            if (total_files_removed == 0) {
                std::cout << "[INFO] No info files found on disk to remove.\n";
            } else {
                std::cout << "[INFO] Finished wiping disk. Removed " << total_files_removed << " files from " << FAIL_DIRECTORY << ".\n";
            }
        }
        
        return 0;
    }

    // Try to initialize memory state from disk, but keep going even if this fails since it isn't a fatal error
    int initialization_res = initializeMemoryFromDiskChunks();
    if (initialization_res < 0) {
        std::cerr << "[ERROR] Failed to initialize memory state from disk! Check permissions and ensure the chunk directory exists and contains valid chunk and info files.\n";
    } else if (initialization_res == 0 && ARGS.verbose >= 1) {
        std::cout << "[INFO] Successfully initialized memory state from disk.\n";
    } else if (initialization_res == 1 && ARGS.verbose >= 1) {
        std::cout << "[INFO] No chunk or info files found on disk to initialize memory state from. Starting with empty state.\n";
    }

    // Send missing chunks report
    if (ARGS.missing_chunks_report) {
        if (ARGS.verbose >= 1) {
            std::cout << "[INFO] Missing chunks report flag is set. Printing list of missing chunks for each file represented in the chunks directory...\n";
        }
        generateMissingChunksReport();

        return 0;
    }

    // Open the socket to receive packets
    RECV_SOCKET = std::make_shared<udp::Socket>(ARGS.receive_address);
    if (!RECV_SOCKET || RECV_SOCKET->fd < 0) {
        std::cerr << "[ERROR] Failed to create or bind receive socket (" << ARGS.receive_address.get_port() << ")\n";
        return 1;
    }

    // Create file descriptor manager for the receive socket
    FileDescriptorManager fd_manager {RECV_SOCKET->fd};

    // Create pruner thread
    std::thread pruner(prune_worker, ARGS.prune_interval_ms);

    // Main loop
    while(RUNNING) {

        // Wait for data on the receive socket
        if (fd_manager.wait() == 0)
        continue;

        // Handle incoming packets on the receive socket
        if (fd_manager.areBytesWaiting(RECV_SOCKET->fd)) {
            std::optional<std::vector<char>> opt_packet = RECV_SOCKET->receive();

            if (!opt_packet) {
                std::cout << "[ERROR] Receive socket has bytes waiting, but a packet was not received!" << std::endl;
                continue;
            }

            const std::vector<char>& packet = *opt_packet;

            // Check packet length
            if (packet.size() < 5) {
                std::cout << "[ERROR] received socket packet too short: does not have room for bulk file transfer header." << std::endl;
                continue;
            }

            // Get header fields from the packet
            std::vector<char> header(5);
            std::copy(packet.begin(), packet.begin() + 5, header.begin());
            std::optional<bft_header_flags> header_flags = parseHeaderFlags(header);

            if (!header_flags) {
                std::cout << "[ERROR] Failed to parse header flags. Skipping packet.\n";
                continue;
            }

            if (!header_flags->isData && header_flags->isLastBlock) {
                std::cout << "[ERROR] Received packet with invalid header: info packets cannot be marked as last block.\n";
                continue;
            }

            if (ARGS.verbose >= 1) {
                std::cout << "[DEBUG] Received packet of length " << packet.size() << " with header byte: 0x" 
                          << std::hex << int(header[4]) << std::dec << "\n";
                if (ARGS.verbose >= 2) {
                    std::cout << "    isData: " << header_flags->isData << "\n";
                    std::cout << "    isLastBlock: " << header_flags->isLastBlock << "\n";
                    std::cout << "    isRepair: " << header_flags->isRepair << "\n";
                }
            }

            // Handle the packet based on header fields
            int res;
            if (!header_flags->isData) {
                res = handleBFTInfoPacket(header, packet);
            } else {
                res = handleBFTDataPacket(header, header_flags, packet);
            }

            if (res < 0) {
                std::cout << "[ERROR] Failed to handle received packet.\n";
            } else if (res == 1 && ARGS.verbose >= 1) {
                std::cout << "[WARN] Received info packet for file ID which already exists in BFT_FILES vector\n";
            }
        }
    }

    // Clean shutdown
    RUNNING = false;
    PRUNE_CV.notify_all();
    if (pruner.joinable()) {
        pruner.join();
    }

    return 0;
};

void help() {
    std::cerr << "bft_recv: receives DICE bulk file transfer (BFT) packets forwarded by the COMMS service\n";
    std::cerr << "    -h, --help: print this message\n";
    std::cerr << "    -v, --verbose: more verbose output. Can be specified multiple times.\n";
    std::cerr << "    -a, --receive-address <ip:port>: set receive port address (default: " << DEFAULT_ARGS.receive_address << ")\n";
    std::cerr << "    -m, --max-file-age-ms <milliseconds>: set maximum age of in-progress files before they are pruned (default: "
                    << DEFAULT_ARGS.max_file_age_ms << " ms)\n";
    std::cerr << "    -p, --prune-interval-ms <milliseconds>: set interval for pruning in-progress files (default: " 
                    << DEFAULT_ARGS.prune_interval_ms << " ms)\n";
    std::cerr << "    -c, --comms-service-cmd-socket-address <ip:port>: set the address for sending reports to the COMMS service "
                    << "(default: " << DEFAULT_ARGS.comms_service_cmd_socket_address << ")\n";
    std::cerr << "    -r, --report-port <port>: set the port for sending reports to the COMMS service (default: " 
                    << DEFAULT_ARGS.report_port << ")\n";
    std::cerr << "    -w, --wipe-disk-only: wipe files from fail directory without starting the receive service (default: " 
                    << DEFAULT_ARGS.wipe_disk_only << "). This option causes the service to exit upon completion of wiping.\n";
    std::cerr << "    -x, --missing-chunks-report: print a report of missing chunks for each file represented in the chunks "
                    << "directory and exit without starting the receive service (default: " << DEFAULT_ARGS.missing_chunks_report 
                    << "). This option causes the service to exit upon completion of printing the report.\n";
}

// Parse CLI arguments and return structured configuration
ProgramArguments parseArgs(int argc, char *argv[]) {
    ProgramArguments ret = DEFAULT_ARGS;

    struct option long_options[] {
        {"help",            no_argument, nullptr, 'h'},
        {"verbose",         no_argument, nullptr, 'v'},
        {"receive-address", required_argument, nullptr, 'a'},
        {"max-file-age-ms", required_argument, nullptr, 'm'},
        {"prune-interval-ms", required_argument, nullptr, 'p'},
        {"comms-service-cmd-socket-address", required_argument, nullptr, 'c'},
        {"report-port", required_argument, nullptr, 'r'},
        {"wipe-disk-only", no_argument, nullptr, 'w'},
        {"missing-chunks-report", no_argument, nullptr, 'x'},
        {nullptr, 0, nullptr, 0}
    };

    int opt{};
    while ((opt = getopt_long(argc, argv, "hvwxa:m:p:c:r:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'h': help(); exit(0);
            case 'v': ret.verbose++; break;
            case 'a': ret.receive_address = udp::SocketAddress::create(optarg); break;
            case 'm': ret.max_file_age_ms = std::stoull(optarg); break;
            case 'p': ret.prune_interval_ms = std::stoull(optarg); break;
            case 'c': ret.comms_service_cmd_socket_address = udp::SocketAddress::create(optarg); break;
            case 'r': ret.report_port = std::stoi(optarg); break;
            case 'w': ret.wipe_disk_only = 1; break;
            case 'x': ret.missing_chunks_report = 1; break;
            default: help(); exit(1);
        }
    }

    return ret;
}

// Print command-line arguments for debugging and confirmation
void printArgs(ProgramArguments args) {
    std::cout << "Verbose: " << args.verbose << "\n";
    std::cout << "Receive Address: " << args.receive_address << "\n";
    std::cout << "Max File Age (ms): " << args.max_file_age_ms << "\n";
    std::cout << "Prune Interval (ms): " << args.prune_interval_ms << "\n";
    std::cout << "COMMS Service Command Socket Address: " << args.comms_service_cmd_socket_address << "\n";
    std::cout << "Report Port: " << args.report_port << "\n";
    std::cout << "Wipe Disk Only: " << args.wipe_disk_only << "\n";
    std::cout << "Missing Chunks Report: " << args.missing_chunks_report << "\n";
}