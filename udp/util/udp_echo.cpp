#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const char *executable_name = "echo";
constexpr size_t buffer_size = 65536;

struct ProgramArguments {
    sockaddr_in address;
    bool verbose;
};

ProgramArguments parse_args(int argc, char *argv[]);

int main(int argc, char **argv)
{
    ProgramArguments args = parse_args(argc, argv);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        std::cerr << "socket() error: " << std::strerror(errno) << "\n";
        exit(1);
    }

    int bind_result =
        bind(socket_fd, (sockaddr *)(&args.address), sizeof(args.address));
    if (bind_result == -1) {
        std::cerr << "bind() error: " << std::strerror(errno) << "\n";
        exit(1);
    }

    socklen_t bind_address_size = sizeof(args.address);
    int getsockname_result =
        getsockname(socket_fd, (sockaddr *)(&args.address), &bind_address_size);
    if (getsockname_result == -1) {
        std::cerr << "getsockname() error: " << std::strerror(errno) << "\n";
        exit(1);
    }
    if (args.verbose) {
        std::cerr << "address bound to " << inet_ntoa(args.address.sin_addr)
                  << ":" << ntohs(args.address.sin_port) << "\n";
    }

    while (true) {
        sockaddr_in recv_address{};
        char buffer[buffer_size];
        socklen_t recv_address_size{sizeof(struct sockaddr_in)};
        int bytes_recv = recvfrom(
            socket_fd,
            buffer,
            buffer_size,
            0,
            (sockaddr *)(&recv_address),
            &recv_address_size
        );
        if (bytes_recv == -1) {
            std::cerr << "recvfrom() error: " << std::strerror(errno) << "\n";
            exit(1);
        }
        if (args.verbose) {
            std::cerr << "address recieved from "
                      << inet_ntoa(recv_address.sin_addr) << ":"
                      << ntohs(recv_address.sin_port) << "\n";
        }

        int bytes_sent = sendto(
            socket_fd,
            buffer,
            bytes_recv,
            0,
            (sockaddr *)(&recv_address),
            sizeof(recv_address)
        );
        if (bytes_sent == -1) {
            std::cerr << "sendto() error: " << std::strerror(errno) << "\n";
            exit(1);
        } else if (args.verbose) {
            std::cerr << "bytes sent: " << bytes_sent << "\n";
        }
    }
}

ProgramArguments parse_args(int argc, char *argv[])
{
    ProgramArguments returned_struct{};
    bool a_opt_parsed = false;
    returned_struct.address.sin_family = AF_INET;
    returned_struct.address.sin_port = 0;

    int opt{};
    while ((opt = getopt(argc, argv, "vp:a:m:")) != -1) {
        switch (opt) {
        case 'v':
            returned_struct.verbose = true;
            break;
        case 'p':
            returned_struct.address.sin_port = htons(atoi(optarg));
            break;
        case 'a': {
            if (inet_aton(optarg, &returned_struct.address.sin_addr) == 0) {
                std::cerr << "failed to parse address\n";
                exit(1);
            }
            a_opt_parsed = true;
        } break;
        }
    }

    if (!a_opt_parsed) {
        std::cerr << executable_name
                  << " requires an address to bind too. Use the -a option: -a "
                     "[ipv4 address string]\n";
        exit(1);
    }
    return returned_struct;
}
