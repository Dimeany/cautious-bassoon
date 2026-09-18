#include "comms_service.h"
#include "version.h"

const ProgramArguments DEFAULT_ARGS {
    .verbose = 0,
    .ppp = false,
    .force = false,
    .dump = false,
    .usec_wait = 0,
    .live_address = udp::SocketAddress::create("0.0.0.0:50000"),
    .norm_address = udp::SocketAddress::create("0.0.0.0:56000"),
    .local_norm_receiver = 57000,
    .remote_norm_receiver = 57000,
    .mode = COMMS_MODE::UNSET,
    .baud_rate = B9600,
    .file {},
    .ethernet_source {},
    .ethernet_dest {},
};

ProgramArguments args {};
bool running = true;
uint16_t norm_sequence_number = 0;
std::shared_ptr<comms::backend::Backend> backend;

size_t packets_sent = 0;
size_t packets_received = 0;


void signalHandler(int signum) {
    (void)signum;
    running = false;
}

int main(int argc, char *argv[]) {
    args = parseArgs(argc, argv);

    std::cout << "Nebula version: " << NEBULA_VERSION << std::endl;

    auto live_socket = std::make_shared<udp::Socket>(args.live_address);
    auto norm_socket = std::make_shared<udp::Socket>(args.norm_address);

    if (args.mode == COMMS_MODE::FILEIN)
        backend = std::make_shared<comms::backend::FileIn>(args.file, args.ppp, args.force);
    else if (args.mode == COMMS_MODE::FILEOUT)
        backend = std::make_shared<comms::backend::FileOut>(args.file, args.force);
    else if (args.mode == COMMS_MODE::ETHERNET)
        backend = std::make_shared<comms::backend::Ethernet>(args.ethernet_source, args.ethernet_dest, args.force);
    else if (args.mode == COMMS_MODE::SERIAL) {
        backend = std::make_shared<comms::backend::Serial>(args.file, args.baud_rate);
        if (!args.ppp) {
            // The main loop assumes that the backend returns one full
            // packet at a time.
            std::cout << "Serial without PPP framing (-p flag) is not supported" << std::endl;
            exit(1);
        }
    }

    // If we are using PPP framing, wrap the backend in the PPP backend
    if (args.ppp)
        backend = std::make_shared<comms::backend::PPP>(std::move(backend));

    FileDescriptorManager fd_manager {live_socket->fd};
    fd_manager.addFileDescriptor(norm_socket->fd);

    if (backend->getFd() >= 0)
        fd_manager.addFileDescriptor(backend->getFd());

    if (args.verbose >= 1)
        printArgs(args);

    if (args.verbose >= 2)
        std::cout << "\tsize\tdest\tsrc\tseq #\ttime\n";


    SignalManager signal_manager {signalHandler, SIGINT, SIGTERM};
    while (running) {
        int status = fd_manager.wait();
        if (status == 0)
            continue;

        if (fd_manager.areBytesWaiting(backend->getFd())) {
            while (backend->bytesAvailable()) {
                auto packet_bytes = backend->read();
                if (packet_bytes.size() == 0) {
                    break;
                }

                // Every backend implements framing such that
                // backend->read() returns a full packet
                processFromBackend(live_socket, udp::Packet::deserialize(packet_bytes));
            }
        }

        else if (fd_manager.areBytesWaiting(live_socket->fd)) {
            processFromSocket(live_socket, false);
        }

        else if (fd_manager.areBytesWaiting(norm_socket->fd)) {
            processFromSocket(norm_socket, true);
        }
    }

    if (args.verbose >= 1) {
        std::cout << "\nShutting down" << std::endl;
        if (args.dump) {
            std::cout << "Total packets in file: " << packets_received << std::endl;
        } else {
            std::cout << "Total packets sent: " << packets_sent << std::endl;
            std::cout << "Total packets received: " << packets_received << std::endl;
        }
    }
    exit(0);
}

void processFromBackend(std::shared_ptr<udp::Socket> socket, udp::Packet packet) {
    udp::SocketAddress dest = udp::SocketAddress::create("127.0.0.1", packet.header.destination_port);

    if (packet.header.destination_port == args.live_address.get_port() || packet.header.destination_port == args.norm_address.get_port())
        return;

    if (!args.dump) {
        // strip header if sending to norm
        if (packet.header.destination_port == args.local_norm_receiver)
            socket->send(packet.payload, dest);
        else
            socket->send(packet.serialize(), dest);
    }

    if (args.verbose >= 2)
        printHeader(false, packet);

    packets_received++;
    usleep(args.usec_wait);
}

void processFromSocket(std::shared_ptr<udp::Socket> socket, bool applyHeader) {
    udp::SocketAddress source {};
    auto opt_packet = socket->receive(source);
    if (!opt_packet)
        return;
    std::vector<char>& packet = *opt_packet;

    if (applyHeader) {
        std::vector<char> ready_packet = udp::Packet::create_from_buffer(packet, args.remote_norm_receiver, socket->bound_address.get_port(), norm_sequence_number, 0).serialize();

        if (args.verbose >= 2)
            printHeader(true, ready_packet);

        norm_sequence_number++;

        backend->write(ready_packet);
    } else {
        if (args.verbose >= 2)
            printHeader(true, packet);

        uint16_t& size = *((uint16_t *) packet.data());
        if (htons(size) != packet.size()) {
            std::cerr << "ERROR: Packet of " << packet.size() << " bytes received without proper header from port " << source.get_port() << "\n";
            return;
        }

        backend->write(packet);
    }

    packets_sent++;
}

void printHeader(bool input, udp::Packet& packet) {
    printHeader(input, packet.header);
}

void printHeader(bool input, std::vector<char>& packet) {
    udp::PacketHeader header {};
    udp::PacketHeader& net_byte_order_header = *((udp::PacketHeader *) packet.data());

    header.length = ntohs(net_byte_order_header.length);
    header.destination_port = ntohs(net_byte_order_header.destination_port);
    header.source_port = ntohs(net_byte_order_header.source_port);
    header.sequence_number = ntohs(net_byte_order_header.sequence_number);
    header.time = ntohl(net_byte_order_header.time);

    printHeader(input, header);
}

void printHeader(bool input, udp::PacketHeader& header) {
    if (input)
        std::cout << ">";
    else
        std::cout << "<";

    time_t time = header.time;
    std::tm tm = *gmtime(&time);
    std::cout << "\t" << header.length << "\t" << header.destination_port << "\t" << header.source_port << "\t" << header.sequence_number << "\t" << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S%Z") << "\n";
}

void help() {
    std::cerr << "comms_service: multiplex and demultiplex packets with a serial, ethernet, and file backend\n";
    std::cerr << "    -h, --help: print this message\n";
    std::cerr << "    -v, --verbose: more verbose output. Can be specified multiple times.\n";
    std::cerr << "    -p, --ppp: enable ppp framing\n";
    std::cerr << "    -f, --force: disable file checking\n";
    std::cerr << "    -u, --usec-wait: microseconds to wait after sending a packet\n";
    std::cerr << "    -l, --live-address <ip:port>: set live port address (default: " << DEFAULT_ARGS.live_address << ")\n";
    std::cerr << "    -n, --norm-address <ip:port>: set norm port address (default: " << DEFAULT_ARGS.norm_address << ")\n";
    std::cerr << "    -M, --local-norm-receiver <port>: set local norm receiver port (default: " << DEFAULT_ARGS.local_norm_receiver << ")\n";
    std::cerr << "    -N, --remote-norm-receiver <port>: set remote norm receiver port (default: " << DEFAULT_ARGS.remote_norm_receiver << ")\n";
    std::cerr << "    -s, --serial <interface>: specify serial interface as transfer mode\n";
    std::cerr << "    -b, --baud-rate <Hz>: specify the serial baud rate in Hz (default: 9600)\n";
    std::cerr << "    -e, --ethernet-source <ip:port>: specify ip address and port number to send from as transfer mode\n";
    std::cerr << "    -d, --ethernet-dest <ip:port>: specify ip address and port number to send to as transfer mode\n";
    std::cerr << "    -i, --file-in <file>: specify filename to read from as transfer mode\n";
    std::cerr << "    -o, --file-out <file>: specify filename to write to as transfer mode\n";
    std::cerr << "    -D, --dump <file>: specify filename to read without sending out packets\n";
}

ProgramArguments parseArgs(int argc, char *argv[]) {
    ProgramArguments returned_struct {DEFAULT_ARGS};

    struct option long_options[] = {
        {"help",             no_argument, NULL, 'h'},
        {"verbose",          no_argument, NULL, 'v'},
        {"ppp",              no_argument, NULL, 'p'},
        {"force",            no_argument, NULL, 'f'},
        {"usec-wait",        required_argument, NULL, 'u'},
        {"live-address",     required_argument, NULL, 'l'},
        {"norm-address",     required_argument, NULL, 'n'},
        {"norm-receiver",    required_argument, NULL, 'N'},
        {"serial",           required_argument, NULL, 's'},
        {"baud-rate",        required_argument, NULL, 'b'},
        {"ethernet-source",  required_argument, NULL, 'e'},
        {"ethernet-dest",    required_argument, NULL, 'd'},
        {"file-in",          required_argument, NULL, 'i'},
        {"file-out",         required_argument, NULL, 'o'},
        {"dump",             required_argument, NULL, 'D'}
    };

    int opt {};
    while ((opt = getopt_long(argc, argv, "hvpfu:l:n:N:M:s:b:e:d:i:o:D:", long_options, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help();
            exit(0);
        case 'v':
            returned_struct.verbose++;
            break;
        case 'p':
            returned_struct.ppp = true;
            break;
        case 'f':
            returned_struct.force = true;
            break;
        case 'u':
            returned_struct.usec_wait = atoi(optarg);
            break;
        case 'l':
            returned_struct.live_address = udp::SocketAddress::create(optarg);
            break;
        case 'n':
            returned_struct.norm_address = udp::SocketAddress::create(optarg);
            break;
        case 'M':
            returned_struct.local_norm_receiver = atoi(optarg);
            break;
        case 'N':
            returned_struct.remote_norm_receiver = atoi(optarg);
            break;
        case 's':
            if (returned_struct.mode != COMMS_MODE::UNSET) {
                std::cerr << "Cannot add serial port: already in other transfer mode\n";
                exit(1);
            }
            returned_struct.mode = COMMS_MODE::SERIAL;
            returned_struct.file = optarg;
            break;
        case 'b':
            if (returned_struct.mode != COMMS_MODE::SERIAL) {
                std::cerr << "Cannot set baud rate: must be in serial transfer mode (-s)\n";
                exit(1);
            }
            returned_struct.baud_rate = parseBaudRate(atoi(optarg));
            break;
        case 'e':
            if (returned_struct.mode != COMMS_MODE::UNSET && returned_struct.mode != COMMS_MODE::ETHERNET) {
                std::cerr << "Cannot add source ethernet port: already in other transfer mode\n";
                exit(1);
            }
            returned_struct.mode = COMMS_MODE::ETHERNET;
            returned_struct.ethernet_source = udp::SocketAddress::create(optarg);
            break;
        case 'd':
            if (returned_struct.mode != COMMS_MODE::UNSET && returned_struct.mode != COMMS_MODE::ETHERNET) {
                std::cerr << "Cannot add destination ethernet port: already in other transfer mode\n";
                exit(1);
            }
            returned_struct.mode = COMMS_MODE::ETHERNET;
            returned_struct.ethernet_dest = udp::SocketAddress::create(optarg);
            break;
        case 'D':
            returned_struct.verbose = 2;
            returned_struct.dump = true;
            __attribute__((fallthrough));
        case 'i':
            if (returned_struct.mode != COMMS_MODE::UNSET) {
                std::cerr << "Cannot add file: already in other transfer mode\n";
                exit(1);
            }
            returned_struct.mode = COMMS_MODE::FILEIN;
            returned_struct.file = optarg;
            break;
        case 'o':
            if (returned_struct.mode != COMMS_MODE::UNSET) {
                std::cerr << "Cannot add file: already in other transfer mode\n";
                exit(1);
            }
            returned_struct.mode = COMMS_MODE::FILEOUT;
            returned_struct.file = optarg;
            break;
        default:
            std::cerr << "Invalid argument\n";
            help();
            exit(1);
        }
    }

    if (returned_struct.mode == COMMS_MODE::UNSET) {
        std::cerr << "Must have operation mode specified as ethernet, file, or serial\n";
        help();
        exit(1);
    }

    if (returned_struct.mode == COMMS_MODE::ETHERNET && (returned_struct.ethernet_source.get_port() == 0 || returned_struct.ethernet_dest.get_port() == 0)) {
        std::cerr << "Ethernet operation mode must have ethernet-source and ethernet-dest set\n";
        exit(1);
    }

    return returned_struct;
}

speed_t parseBaudRate(int baud_rate) {
    // Got these constanst from "man 3 cfsetspeed"
    switch (baud_rate) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        std::cerr << "Unrecognized baud rate: " << baud_rate << ".  See 'man 3 cfsetspeed' for options." << std::endl;
        exit(1);
    }
}

void printArgs(ProgramArguments args) {
    std::cout << "Verbose Level: " << args.verbose << std::endl;
    std::cout << "PPP Framing: " << args.ppp << std::endl;

    if (!args.dump) {
        std::cout << "Live Address: " << args.live_address << std::endl;
        std::cout << "Norm Address: " << args.norm_address << std::endl;
    }

    if (args.dump) {
        std::cout << "Operation Mode: dump" << std::endl;
    }

    else if (args.mode == COMMS_MODE::ETHERNET) {
        std::cout << "Operation Mode: ethernet" << std::endl;
        std::cout << "    Source: " << args.ethernet_source << std::endl;
        std::cout << "    Dest: " << args.ethernet_dest << std::endl;
    }

    else if (args.mode == COMMS_MODE::SERIAL) {
        std::cout << "Operation Mode: serial" << std::endl;
        std::cout << "    Interface: " << args.file << std::endl;
    }

    else if (args.mode == COMMS_MODE::FILEIN) {
        std::cout << "Operation Mode: file-in" << std::endl;
        std::cout << "    File: " << args.file << std::endl;
    }

    else if (args.mode == COMMS_MODE::FILEOUT) {
        std::cout << "Operation Mode: file-out" << std::endl;
        std::cout << "    File: " << args.file << std::endl;
    }

    std::cout << std::endl;
}
