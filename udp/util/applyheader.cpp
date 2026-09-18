#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udp.h"

const char *executable_name = "applyheader";
constexpr size_t buffer_size = 65536;

struct ProgramArguments {
    uint16_t destination_port;
    uint16_t source_port;
    uint16_t sequence_number;
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
            if (input.length() > 0) {
                break;
            }
            exit(2);
        } else {
            std::cerr << "select() error: " << std::strerror(errno) << "\n";
            exit(1);
        }
    }

    udp::PacketHeader header{};
    header.length = sizeof(header) + input.size();
    header.destination_port = args.destination_port;
    header.source_port = args.source_port;
    header.sequence_number = args.sequence_number;
    header.time = time(NULL);

    if (args.network_byte_order) {
        header.length = htons(header.length);
        header.destination_port = htons(header.destination_port);
        header.source_port = htons(header.source_port);
        header.sequence_number = htons(header.sequence_number);
        header.time = htonl(header.time);
    }

    for (size_t i = 0; i < sizeof(header); i++) {
        std::cout << reinterpret_cast<char *>(&header)[i];
    }
    std::cout << input;
}

ProgramArguments parse_args(int argc, char *argv[])
{
    ProgramArguments returned_struct{};

    int opt{};
    /**
     * -p: source (p)ort
     * -d: (d)estination port
     * -t: packet (t)ype
     * -q: se(q)uence number
     * -n: (n)etwork bytes order
     */
    while ((opt = getopt(argc, argv, "nhp:d:t:q:")) != -1) {
        switch (opt) {
        case 'h':
            std::cerr << "-p: source (p)ort\n";
            std::cerr << "-d: (d)estination port\n";
            std::cerr << "-q: se(q)uence number\n";
            std::cerr << "-n: (n)etwork bytes order\n";
            break;
        case 'n':
            returned_struct.network_byte_order = true;
            break;
        case 'p':
            returned_struct.source_port = atoi(optarg);
            break;
        case 'd':
            returned_struct.destination_port = atoi(optarg);
            break;
        case 'q':
            returned_struct.sequence_number = atoi(optarg);
            break;
        }
    }

    return returned_struct;
}
