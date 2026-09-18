#include "serial.h"

comms::backend::Serial::Serial(std::string interface, speed_t baud_rate) {
    fd = open(interface.c_str(), O_RDWR | O_NOCTTY);
    if (fd < 0) {
        std::cerr << "Failed to open serial interface: " << interface << "\n";
        exit(1);
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Failed to get terminal attributes\n";
        close(fd);
        exit(1);
    }

    cfsetspeed(&tty, baud_rate);

    cfmakeraw(&tty);
    tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls, enable reading
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cc[VMIN]  = 0;                            // read doesn't block
    tty.c_cc[VTIME] = 5;                            // 0.5 seconds read timeout


    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Failed to set terminal attributes\n";
        close(fd);
        exit(1);
    }
}

comms::backend::Serial::~Serial() {
    close(fd);
}


int comms::backend::Serial::getFd() {
    return fd;
}

size_t comms::backend::Serial::write(std::span<char const> data) {
    return ::write(fd, data.data(), data.size());
}

std::vector<char> comms::backend::Serial::read() {
    std::vector<char> buffer(1024);
    ssize_t n = ::read(fd, buffer.data(), buffer.size());
    
    // std::cout << "[Serial::read] Read returned n = " << n << std::endl; // (for testing)

    if (n > 0) {
        buffer.resize(n);
        // std::cout << "[Serial::read] Read " << n << " bytes\n"; // (for testing)
    } else {
        buffer.clear();
        // std::cout << "[Serial::read] No bytes read (n == 0)\n"; // (for testing)
    }
    return buffer;
}

bool comms::backend::Serial::bytesAvailable() {
    int bytes;
    ioctl(fd, FIONREAD, &bytes);
    // std::cout << "[Serial::bytesAvailable] " << bytes << " bytes available\n";
    return bytes > 0;
}
