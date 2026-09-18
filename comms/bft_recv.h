#include <getopt.h>
#include <optional>
#include <string>
#include <vector>

#include "../udp/socket_address.h"

struct bft_file {
    std::string filename;
    std::size_t filesize;
    int expected_chunks;
    uint16_t file_crc;
    uint8_t file_id;
    std::size_t filename_len;
    int chunks_recd = 0;
    int last_chunk_recd = 0;
    std::vector<char> data;
    std::vector<bool> received_chunks;
    int crc_check_passed = 0; // -1: failed, 0: not attempted, 1: passed
    int written = 0;
    uint64_t timestamp;
};

struct bft_header_flags {
    int isData;
    int isLastBlock;
    int isRepair;
};

struct ProgramArguments {
    int verbose;
    udp::SocketAddress receive_address;
    uint64_t max_file_age_ms;
    uint64_t prune_interval_ms;
    udp::SocketAddress comms_service_cmd_socket_address;
    uint16_t report_port;
    int wipe_disk_only;
    int missing_chunks_report;
};

/**
 * @brief Gets the current timestamp in milliseconds since epoch for when a file is received or has a chunk received, to be used for pruning old files that have not completed transfer
 * 
 * @return uint64_t Current timestamp in milliseconds since epoch
 */
uint64_t getFileTimestamp();

/**
 * @brief Gets the elapsed time in milliseconds since a given timestamp, to determine how old a file is for pruning purposes
 * 
 * @param timestamp The timestamp in milliseconds since epoch to compare against the current time
 * 
 * @return int Elapsed time in milliseconds since the given timestamp
 */
int getElapsedMsSinceTimestamp(uint64_t timestamp);

/**
 * @brief Prunes the in-memory list of in-progress file transfers by removing any files that have not received a chunk within the specified maximum age threshold
 * 
 * @param max_age_ms The maximum age in milliseconds that a file can exist in memory without receiving a chunk before it is pruned
 * 
 * @return int The number of files that were pruned
 */
int pruneBFTList(int max_age_ms);

/**
 * @brief Parses the header flags from the 5-byte header of a received packet to determine if it is a data packet, if it is marked as the last block, and if it is a repair block
 * 
 * @param header The 5-byte header extracted from the received packet
 * 
 * @return std::optional<bft_header_flags> A struct containing the parsed header flags, or std::nullopt if there was an error parsing the header
 */
std::optional<bft_header_flags> parseHeaderFlags(const std::vector<char> header);

/**
 * @brief Creates a report message string containing the status of a file transfer, including the filename, 
 * filesize, number of chunks received, and whether the CRC check passed and file was written to disk
 * 
 * @param file The bft_file struct representing the file being transferred
 * 
 * @return std::string The report message string
 */
std::string createReportMessage(const bft_file& file);

/**
 * @brief Sends a report message to the COMMS service containing the status of a file transfer
 * 
 * @param file The bft_file struct representing the file being transferred, used to create the report message
 */
void sendReport(const bft_file& file);

/**
 * @brief Writes the metadata for a received file transfer to disk as a separate file
 * 
 * @param bft The bft_file struct representing the file being transferred, containing all metadata fields to be written to disk
 * 
 * @return int 0 on success, -1 on failure
 */
int writeBFTFileToDisk(const bft_file& bft);

/**
 * @brief Writes the file data for a received file transfer to disk with the given filename
 * 
 * @param filename The filename to use when writing the file data to disk
 * @param data The file data to write to disk
 * 
 * @return int 0 on success, -1 on failure
 */
int writeFileData(std::string filename, std::vector<char> data);

/**
 * @brief Loads a bft_file struct from disk given a filename, reading the binary format written by writeBFTFileToDisk
 * 
 * @param filename The filename of the BFT file metadata to load
 * 
 * @return std::optional<bft_file> The loaded bft_file struct, or std::nullopt if loading failed
 */
std::optional<bft_file> readBFTFileFromDisk(std::string filename);

/**
 * @brief Initializes the in-memory state of the program by loading any existing file metadata and chunk files from disk, allowing the program to resume in-progress file transfers after a restart
 * 
 * @return int 0 on success, -1 on failure
 */
int initializeMemoryFromDiskChunks();

/**
 * @brief Finalizes the file transfer process for a given file by computing and checking the CRC, writing the file to disk if the CRC check passes, and removing the file from memory
 * 
 * @param file The bft_file struct representing the file being transferred
 * @param file_id The file ID number associated with this file transfer, used for logging purposes
 * @param it An iterator pointing to the position of this file in the global BFT_FILES
 * 
 * @return int 0 on success, -1 on failure
 */
int finalizeFileTransfer(bft_file& file, uint8_t file_id, std::vector<bft_file>::iterator it);

/**
 * @brief Handles a received BFT info packet by extracting the file metadata from the packet, creating a new bft_file struct, and adding it to the global list of in-progress file transfers
 * 
 * @param header The 5-byte header extracted from the received packet
 * @param packet The full packet data received, including the header and body
 * 
 * @return int 0 on success, -1 on failure
 */
int handleBFTInfoPacket(std::vector<char> header, const std::vector<char> packet);

/**
 * @brief Handles a received BFT data packet by extracting the block number and file ID from the header, storing the packet data in the correct position within the corresponding bft_file struct, and finalizing the file transfer if this is the last block or a repair block and all expected blocks have been received
 * 
 * @param header The 5-byte header extracted from the received packet
 * @param header_flags The parsed header flags indicating whether this is a data packet, if it is marked as the last block, and if it is a repair block
 * @param packet The full packet data received, including the header and body
 * 
 * @return int 0 on success, -1 on failure
 */
int handleBFTDataPacket(std::vector<char> header, std::optional<bft_header_flags> header_flags, const std::vector<char> packet);

/**
 * @brief Worker function that runs in a separate thread to periodically prune the list of in-progress file transfers based on their age
 * 
 * @param interval_ms The interval in milliseconds at which to run the prune operation
 */
void prune_worker(int interval_ms);

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
 * @brief Displays the parsed program arguments for user verification
 * 
 * @param args The ProgramArguments instance to print
 */
void printArgs(ProgramArguments args);