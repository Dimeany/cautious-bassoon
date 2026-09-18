#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr size_t BUFFER_SIZE = 65536;

struct ProgramArguments {
    sockaddr_in address;
    in_port_t from_port;
    bool verbose;
    bool just_send;
    timeval tv;
    std::string message;
};

ProgramArguments parse_args(int argc, char *argv[]);

int main(int argc, char **argv)
{
    ProgramArguments args = parse_args(argc, argv);
    if (args.verbose) {
        std::cerr << "address to send to " << inet_ntoa(args.address.sin_addr)
                  << ":" << args.address.sin_port << "\n";
    }

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "socket() error: " << std::strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    // IF the args stated that we should send from a particular port
    if (args.from_port != 0) {
        // Declare an input socketaddr
        auto storage = sockaddr_in {};

        // Assign the given port to the address
        storage.sin_family = AF_INET;
        storage.sin_port = htons(args.from_port);

        // Assign local host to the address
        if (inet_pton(AF_INET, "127.0.0.1", &storage.sin_addr) < 0) {
            std::cout << "Failed to assign local address to 127.0.0.1\n";
            exit(EXIT_FAILURE);
        }

        // Bind the socket to the given address
        int result = bind(
            socket_fd,
            (sockaddr *)&storage,
            sizeof(storage)
        );
        // Handle errors with that binding (taken straight from socket.cpp)
        if (result < 0) {
            // Construct nice error string
            auto errno_str = std::strerror(errno);

            char ip_addr_str[INET_ADDRSTRLEN] {};
            inet_ntop(
                AF_INET,
                &(storage.sin_addr),
                ip_addr_str,
                sizeof(ip_addr_str)
            );

            std::cerr << "Failed to bind socket internet address.\n"
                << "Error: " << errno_str << "\n"
                << "address: " << ip_addr_str << "\n"
                << "port: " << args.from_port;

            exit(EXIT_FAILURE);
        }
    } // binding to a given port

    if (args.verbose) {
        std::cerr << "message to send: " << args.message << "\n";
    }

    sockaddr_in send_addr = args.address;
    send_addr.sin_port = htons(send_addr.sin_port);

    int bytes_sent = sendto(
        socket_fd, args.message.data(), args.message.size(), 0, 
        reinterpret_cast<sockaddr*>(&send_addr),
        sizeof(send_addr)
    );
    if (bytes_sent < 0) {
        std::cerr << "sendto() error: " << std::strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
    else if (args.verbose) {
        std::cerr << "bytes sent: " << bytes_sent << "\n";
    }
    if (args.just_send) {
        exit(EXIT_SUCCESS);
    }

    if (args.tv.tv_usec > 0 || args.tv.tv_sec) {
        int setsockopt_result = setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &args.tv, sizeof(args.tv));
        if (setsockopt_result < 0) {
            std::cerr << "setsockopt() error: " << std::strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
    }

    char buffer[BUFFER_SIZE];
    int bytes_recv = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, nullptr, 0);
    if (bytes_recv < 0) {
        std::cerr << "recvfrom() error: " << std::strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
    else if (bytes_recv == 0 && args.verbose) {
        std::cerr << "recvfrom() timed out\n";
        exit(EXIT_FAILURE + 1);
    }
    else {
        std::string response(buffer, bytes_recv);
        std::cout << response;
    }
    return 0;
}

ProgramArguments parse_args(int argc, char *argv[])
{
    ProgramArguments returned_struct{};
    bool p_opt_parsed = false;
    bool a_opt_parsed = false;
    returned_struct.address.sin_family = AF_INET;
    returned_struct.tv.tv_sec = 3;

    int opt{};
    while ((opt = getopt(argc, argv, "hvsp:a:f:m:t:")) != -1) {
        switch (opt) {
            case 'h':
                std::cerr << "-p [port number]: the port number you are sending too.\n"
                    << "-a [ipv4 address string]: the address you are sending too.\n"
                    << "-p: the port number you are sending to.\n"
                    << "-f: the port number you are sending from\n"
                    << "-s: send only, do not call recvfrom()\n"
                    << "-v: verbose output (to STDERR file)\n"
                    << "-t [ms]: recvfrom() timeout in MILLISECONDS\n"
                    << "-h: help (print to STDERR file)\n"
                    << "-m [message string]: message you are sending. This is not the recommended way to pass a message. Use a pipe instead especially if you need to use binary\n";
                break;
            case 's':
                returned_struct.just_send = true;
                break;
            case 't':
                // specify value in ms
                returned_struct.tv.tv_usec = (atoi(optarg) * 1000) % (1000 * 1000);
                returned_struct.tv.tv_sec = (atoi(optarg) * 1000) / (1000 * 1000);
                break;
            case 'v':
                returned_struct.verbose = true;
                break;
            case 'p':
                returned_struct.address.sin_port = atoi(optarg);
                p_opt_parsed = true;
                break;
            case 'f':
                returned_struct.from_port = atoi(optarg);
                break;
            case 'a': {
                    if (inet_aton(optarg, &returned_struct.address.sin_addr) == 0) {
                        std::cerr << "failed to parse adress\n";
                        exit(EXIT_FAILURE);
                    }
                    a_opt_parsed = true;
                }
                break;
            case 'm':
                returned_struct.message = std::string(optarg);
                break;
        }
    }

    if (!p_opt_parsed) {
        std::cerr << argv[0] << " requires a port to send to. Use the -p option: -p [port number]\n";
        exit(EXIT_FAILURE);
    }
    if (!a_opt_parsed) {
        std::cerr << argv[0] << " requires an address to send to. Use the -a option: -a [ipv4 address string]\n";
        exit(EXIT_FAILURE);
    }

    if (returned_struct.message.length() != 0) {
        return returned_struct;
    }

    char buffer[BUFFER_SIZE];
    while (true) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        int select_result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (select_result < 0) {
            std::cerr << "select() error: " << std::strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if (select_result == 0) {
            std::cerr << "reading from STDIN timed out\n";
            if (returned_struct.message.length() > 0) {
                return returned_struct;
            }
            else {
                exit(EXIT_FAILURE);
            }
        } else {
            // There is something in stdin
            int bytes_from_stdin = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (bytes_from_stdin < 0) {
                std::cerr << "read() from stdin error: " << std::strerror(errno) << "\n";
                exit(EXIT_FAILURE);
            }
            else if (bytes_from_stdin == 0) {
                return returned_struct;
            } else {
                returned_struct.message +=
                    std::string(buffer, bytes_from_stdin);
            }
        }
    }
}
