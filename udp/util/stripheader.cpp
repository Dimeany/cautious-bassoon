#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udp.h"

const char *executable_name = "stripheader";
constexpr size_t buffer_size = 65536;

struct ProgramArguments {
    bool verbose;
    bool network_byte_order;
};

ProgramArguments parse_args(int argc, char *argv[]);

int main(int argc, char **argv)
{
    ProgramArguments args = parse_args(argc, argv);
    std::string input{};
    char buffer[buffer_size];
    while (true) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        int select_result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (select_result > 0) {
            // There is something in stdin
            int bytes_from_stdin = read(STDIN_FILENO, buffer, buffer_size);
            if (bytes_from_stdin < 0) {
                std::cerr << "read() from stdin error: " << std::strerror(errno)
                          << "\n";
                exit(1);
            } else if (bytes_from_stdin == 0) {
                break;
            } else {
                input += std::string(buffer, bytes_from_stdin);
            }
        } else if (select_result == 0) {
            std::cerr << "timedout reading from STDIN\n";
            if (input.length() > 0) {
                break;
            }
            exit(1);
        } else {
            std::cerr << "select() error: " << std::strerror(errno) << "\n";
            exit(1);
        }
    }

    if (args.verbose) {
        udp::PacketHeader header{};
        memcpy(&header, input.data(), sizeof(header));
        // assuming packet header is in network byte order
        if (args.network_byte_order) {
            header.length = ntohs(header.length);
            header.destination_port = ntohs(header.destination_port);
            header.source_port = ntohs(header.source_port);
            header.sequence_number = ntohs(header.sequence_number);
            header.time = ntohl(header.time);
        }
        std::cerr << "length: " << header.length << "\n";
        std::cerr << "source port: " << header.source_port << "\n";
        std::cerr << "destination port: " << header.destination_port
                  << "\n";
        std::cerr << "sequence number: " << header.sequence_number << "\n";
        auto tm = *std::gmtime((time_t *)(&header.time));
        std::cerr << std::put_time(&tm, "%B %d, %Y %H:%M:%S") << "\n";
    }
    for (size_t i = sizeof(udp::PacketHeader); i < input.length(); i++) {
        std::cout << input.data()[i];
    }
}

ProgramArguments parse_args(int argc, char *argv[])
{
    ProgramArguments returned_struct{};

    int opt{};
    while ((opt = getopt(argc, argv, "hnv")) != -1) {
        switch (opt) {
        case 'h':
            std::cerr << "-v: print the contents of the header to STDERR\n";
            std::cerr << "-n: print in (n)etwork bytes order\n";
            break;
        case 'n':
            returned_struct.network_byte_order = true;
            break;
        case 'v':
            returned_struct.verbose = true;
            break;
        }
    }

    return returned_struct;
}
