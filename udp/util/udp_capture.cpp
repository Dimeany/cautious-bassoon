#include <systemd/sd-daemon.h>

#include "udp_capture.h"

static const char *TIME_FMT = "%Y-%j-%H-%M-%S";

static Output out;
static ProgramArgs args;

int main(int argc, char *argv[]) {
  init_traps();
  parse_args(argc, argv);
  listen_write_loop();
  return 0;
}

void usage(const char *program) {
  std::cerr
      << "Usage:\n"
      << "    " << program
      << " -l listen_port -t listen_timeout -T abs_timeout -b base_fn -m"
         " max_fsz -p post_process_prog [-f forward_ip_port. . .]\n"
      << "\t-l listen_port: port to listen on for data\n"
      << "\t-d debug print to std::cout\n"
      << "\t-t listen_timeout: int # of seconds to wait after not receiving "
         "data before closing file\n"
      << "\t-T abs_timeout: int # of seconds to keep file open before "
         "closing\n"
      << "\t-b base_fn: base filename to use\n"
      << "\t-m max_fsz: max binary filze size in bytes\n"
      << "\t-p post_process_prog: program to run on file after it has been "
         "closed\n"
      << "\t[-f forward_ip_port . . .]: optional (many) UDP ip:port to "
         "forward data to\n";
}

sockaddr_in extract_sockaddr_in(const std::string &address) {
  auto port_div_pos = address.find(':');
  if (port_div_pos == std::string::npos) {
    throw std::runtime_error("can't find port from ip string");
  }

  auto ip = address.substr(0, port_div_pos);
  if (ip == "localhost") {
    ip = "127.0.0.1";
  }

  auto port = std::stoi(address.substr(port_div_pos + 1, std::string::npos));

  return sockaddr_in{
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr = {.s_addr = inet_addr(ip.c_str())},
      .sin_zero = {0},
  };
}

int initialize_socket() {
  int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_socket < 0) {
    throw std::runtime_error("cannot open listen socket");
  }

  const sockaddr_in listen_addr{
      .sin_family = AF_INET,
      .sin_port = htons(args.listen_port),
      .sin_addr =
          {
              .s_addr = htonl(INADDR_ANY),
          },
      .sin_zero = {0},
  };
  if (bind(udp_socket, (sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
    throw std::runtime_error("cannot bind socket to listener");
  }

  if (args.listen_timeout) {
    timeval tv{.tv_sec = *args.listen_timeout, .tv_usec = 0};
    if (setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
      throw std::runtime_error("cannot set socket timeout to " +
                               std::to_string(tv.tv_sec));
    }
  }

  sd_notify(0, "READY=1");

  return udp_socket;
}

void listen_write_loop() {
  auto udp_socket = initialize_socket();
  int32_t open_time = 0;

  constexpr size_t BUF_SZ{65535};
  std::string buf(BUF_SZ, 0);

  while (true) {
    int bytes_read = recvfrom(udp_socket, buf.data(), BUF_SZ, 0, nullptr, 0);

    if (args.debug_mode) {
      udp::PacketHeader *packet_header = (udp::PacketHeader *)buf.data();
      std::cout << packet_header;
    }

    bool close_file = false;
    if (bytes_read < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        // On timeout, close any open file
        close_file = out.file.is_open();
      } else if (errno != EINTR) {
        throw std::runtime_error{std::string{"socket recv died: "} +
                                 strerror(errno)};
      }
    }

    // Log to file
    if (!args.base_fn.empty()) {
      // Decide whether to open a new file
      size_t current_file_size = 0;
      if (out.file.is_open()) {
        current_file_size = static_cast<size_t>(out.file.tellp());
        close_file = close_file ||
                     (args.absolute_timeout &&
                      *args.absolute_timeout < time(nullptr) - open_time) ||
                     (current_file_size > args.max_fsz);
      }

      // Possibly close & open a new file
      if (close_file) {
        out.close();
        post_process(args.post_process, out.name);
      }

      bool open_new_file = (bytes_read > 0) && !out.file.is_open();
      if (open_new_file) {
        out.open(args.base_fn);
        open_time = time(nullptr);
      }

      // Write to file
      if (bytes_read > 0)
        out.write(buf.data(), bytes_read);
    }

    // Forward to all destinations
    for (const auto &fwd_addr : args.forward_to) {
      sendto(udp_socket, buf.data(), bytes_read, 0, (sockaddr *)&fwd_addr,
             sizeof(fwd_addr));
    }
  }

  close(udp_socket);
}

void init_traps() {
  signal(SIGINT, sig_handle);
  signal(SIGILL, sig_handle);
  signal(SIGFPE, sig_handle);
  signal(SIGSTOP, sig_handle);
  signal(SIGQUIT, sig_handle);
  signal(SIGSEGV, sig_handle);
  signal(SIGTERM, sig_handle);
}

void sig_handle(int sig) {
  out.close();
  post_process(args.post_process, out.name);
  std::cerr << "Caught signal " << sig << ": " << strsignal(sig) << std::endl;
  std::terminate();
}

void parse_args(int argc, char *argv[]) {
  args = {.listen_port = 0,
          .listen_timeout = {},
          .absolute_timeout = {},
          .base_fn = "",
          .max_fsz = SIZE_MAX,
          .post_process = "",
          .forward_to = {},
          .debug_mode = false};

  int opt{0};
  while ((opt = getopt(argc, argv, "l:t:T:b:m:p:f:d")) != -1) {
    switch (opt) {
    case 'm':
      args.max_fsz = static_cast<size_t>(atoi(optarg));
      break;

    case 'T':
      args.absolute_timeout = static_cast<int>(abs(atoi(optarg)));
      break;
    case 't':
      args.listen_timeout = static_cast<int>(abs(atoi(optarg)));
      break;

    case 'f':
      args.forward_to.push_back(extract_sockaddr_in(optarg));
      break;

    case 'l':
      args.listen_port = static_cast<int>(abs(atoi(optarg)));
      break;
    case 'b':
      args.base_fn = optarg;
      break;
    case 'p':
      args.post_process = optarg;
      break;

    case 'd':
      args.debug_mode = true;
      break;

    default:
      usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (!args.base_fn.empty() && !args.absolute_timeout) {
    std::cerr << "** absolute timeout (-T) is required if logging to files"
              << std::endl;
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (!args.listen_timeout) {
    args.listen_timeout = args.absolute_timeout;
  }

  if (args.listen_port == 0) {
    std::cerr << "** listen port (-l) is required!" << std::endl;
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (args.forward_to.empty() && args.base_fn.empty()) {
    std::cerr << "** need either file name (-b) or forward addresses (-f)!"
              << std::endl;
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
}

void post_process(const std::string &prog, const std::string &fn) {
  if (prog.empty() || fn.empty())
    return;

  auto run = prog + " \"" + fn + '"';
  int res = system(run.c_str());
  if (res < 0) {
    std::cerr << "failed to execute given process: status code " << res
              << std::endl;
  }
}

void Output::write(const char *dat, size_t size) {
  if (!file)
    return;
  file.write(dat, size);
}

void Output::open(const std::string &base_fname) {
  if (base_fname.empty())
    return;
  name = "";
  file = std::ofstream{};

  auto now = std::chrono::system_clock::now();
  auto to_fmt = std::chrono::system_clock::to_time_t(now);

  size_t repeat_num = 0;
  std::stringstream ss;
  do {
    ss.str("");
    ss << base_fname << '_' << std::put_time(std::gmtime(&to_fmt), TIME_FMT)
       << '_' << std::to_string(repeat_num) << ".bin";
  } while (std::filesystem::exists(ss.str()) && (++repeat_num));

  name = ss.str();
  file = std::ofstream{name, std::ios::binary};
  if (!file)
    throw std::runtime_error{"cannot open binary file at " + name};
}

void Output::close() {
  if (file) {
    file.close();
  }
}
