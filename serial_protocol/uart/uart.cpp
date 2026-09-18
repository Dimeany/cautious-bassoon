#include "uart.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <sstream>
#include <termios.h>
#include <unistd.h>

UART::UART() {
}

UART::~UART()
{
    close(fd);
}

void log_error(std::string const& msg) {
    std::cerr << "ERROR" << msg << std::endl;
}

void log_warning(std::string const& msg) {
    std::cerr << "WARNING" << msg << std::endl;
}

int UART::Open(const char *port, int baud_rate) {
    fd = open(port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        throw UARTException("Failed to open UART file descriptor for file " + std::string(port) + ": " + std::string(strerror(errno)));
    }
    SetSettings(baud_rate);
    return 0;
}

size_t UART::ReadOnly(void *dest, size_t num_bytes) {
    size_t total_bytes_read = 0;

    while (total_bytes_read < num_bytes) {
        int bytes_read = read(fd, reinterpret_cast<char *>(dest) + total_bytes_read, num_bytes - total_bytes_read);
        if (bytes_read < 0) {
            throw UARTException(std::string("UART::Read(): UART read() returned -1.  Error: ") + strerror(errno));
        } else if (bytes_read == 0) {
            // No more data to read
            break;
        }
        total_bytes_read += bytes_read;
    }

    // Return the total number of bytes read
    return total_bytes_read;
}

void UART::Read(void *dest, size_t num_bytes) {
    size_t total_bytes_read = 0;

    while (total_bytes_read != num_bytes) {
        int bytes_read = read(fd, reinterpret_cast<char *>(dest) + total_bytes_read, num_bytes - total_bytes_read);
        if (bytes_read < 0) {
            throw UARTException(std::string("UART::Read(): UART read() returned -1.  Error: ") + strerror(errno));
        }
        else if (bytes_read == 0) {
            break;
        }
        total_bytes_read += bytes_read;
    }

    if (total_bytes_read != num_bytes) {
        std::stringstream error_message;
        error_message << "UART::Read(): read() did not return enough bytes.  Expected: " << num_bytes
                      << ", got: " << total_bytes_read;
        throw UARTException(error_message.str());
    }
}

std::string UART::ReadLine(std::string line_end_sequence) {
    std::string buffer;
    buffer.resize(1000);

    std::string result;

    while (!result.ends_with(line_end_sequence)) {
        int bytes_read = read(fd, buffer.data(), buffer.size());
        if (bytes_read < 0) {
            throw UARTException(std::string("UART::ReadLine(): read() returned -1.  Error: ") + strerror(errno));
        }
        else if (bytes_read == 0) {
            break;
        }

        auto result_previous_size = result.size();
        result.resize(result.size() + bytes_read);
        memcpy(result.data() + result_previous_size, buffer.data(), bytes_read);
    }

    if (!result.ends_with(line_end_sequence)) {
        throw UARTException("UART::ReadLine(): read() timed out without the expected line end sequence.");
    }

    return result;
}


int UART::Write(std::span<char const> data_to_write) {
    // flushes the received yet unread data
    // ensures that there is no data left from previous messages
    int return_val_from_tcflush = tcflush(fd, TCIFLUSH);
    if (return_val_from_tcflush < 0){
        log_error( "UART write tcflush() error, errno: " + std::to_string(errno));
    }
    int return_val_from_write = write(fd, data_to_write.data(), data_to_write.size());
    if (return_val_from_write < 0){
        log_error("UART write write() error error, errno: " + std::to_string(errno));
    }
    return return_val_from_write;
}

int UART::get_fd() {
    return fd;
}

void UART::SetSettings(int baud_rate){
    struct termios options;
    tcgetattr(fd, &options);
    speed_t baud;
    switch (baud_rate) {
        case 9600: baud = B9600;
        break;
        case 19200: baud = B19200; 
        break;
        case 38400: baud = B38400; 
        break;
        case 57600: baud = B57600; 
        break;
        case 115200: baud = B115200; 
        break;
        default: 
            throw std::invalid_argument("Unsupported baud rate");
    }
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    // INPUT FLAGS!
    // NOT (IGN)ore (BR)ea(K) condition on input (does gps offer a BREAK condition: check on ocilliscope?)
    options.c_iflag &= ~IGNBRK;
    options.c_iflag &= ~BRKINT;
    // IGNBRK and BRKINT are both not set 
    //     -> A BREAK reads as a null byte ('\0') if PARMRK is not set 
    //     -> we will set PARMRK so BREAK reads as \377 \0 \0
    options.c_iflag &= ~PARMRK; // Marks parity or framing errors (don't care about parity errors)
    // BREAK is marked with \377 \0 \0 (do care about BREAK which is a framing error)
    // All framing or parity errors (known as erroneous bytes) are prefixed with \377 \0 
    options.c_iflag &= ~ISTRIP; // NOT strip the 8th bit (it's meaningful because no parity by default)
    // The nice thing about PARMRK is if ISTRIP is not set (its not) then if \377 is send it will be prefixed by \377
    //     -> \377 = \377 \377

    options.c_iflag &= ~IGNPAR; // NOT (IGN)ore (PAR)ity errors and *framing errors*
    // By default there is no parity for OEM719 but there is a framing bit! WE WANT TO CHECK FRAMING ERRORS
    options.c_iflag &= ~INPCK;
    
    options.c_iflag &= ~INLCR; // Dont translate NL (new line) to CR (carrier return) on input
    options.c_iflag &= ~ICRNL; // Dont translate CR (carrier return) to NL (new line) on input
    options.c_iflag &= ~IGNCR; // set IGNORE CR anyways this makes the above line useless

    // not sure what XON and XOFF flow control is
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // OUTPUT FLAGS
    // Disable input/output processing
    options.c_oflag &= ~OPOST;

    options.c_oflag &= ~ONLCR;  // NOT convert new line to carriage return
    // THIS IS NEW AND MAY CAUSE ERRORS FOR OEM719
    options.c_oflag &= ~OCRNL;  // Not convert carriage return to newline

    // CONTROL MODES
    // Set data bits: 8N1
    options.c_cflag &= ~CSIZE; // clears size
    options.c_cflag |= CS8; // sets size to 8
    options.c_cflag &= ~PARENB; // We dont want parity checking or parity bit on output
    options.c_cflag &= ~CSTOPB; // one stop bit (OEM719 specification)
    
    // Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;
    
    // Enable receiver and ignore modem control lines
    options.c_cflag |= CREAD | CLOCAL;
    
    // LOGIC MODES
    options.c_lflag &= ~ICANON; // Non canonical mode
    options.c_lflag &= ~ECHO; // Dont echo input
    options.c_lflag &= ~ECHOE; // Disable canonical option (probably meaningless because in non canonical mode)
    options.c_lflag &= ~ECHONL; // Disable canonical option (probably meaningless because in non canonical mode)
    options.c_lflag &= ~ISIG; // Turn off signal handling for INTR, QUIT, SUSP, or DSUSP characters This isn't a terminal
    
    // SPECIAL CHARACTERS
    // Set timeout settings
    // ref https://www.gnu.org/software/libc/manual/html_node/Noncanonical-Input.html
    options.c_cc[VTIME] = 30; // 3 second read timeout
    options.c_cc[VMIN] = 0;
    
    // Apply the new settings
    tcsetattr(fd, TCSANOW, &options);

    // sleep(2);
    // // Makes absolutely certain the I/O buffers are flushed.
    // int return_val_from_tcflush = tcflush(fd, TCIFLUSH);
    // if (return_val_from_tcflush < 0){
    //     perror("UART write tcflush() error");
    // }
}

void UART::UpdateVMIN(size_t new_vmin)
{
    struct termios options;
    tcgetattr(fd, &options);
    options.c_cc[VMIN] = new_vmin;
    tcsetattr(fd, TCSANOW, &options);
}

void UART::UpdateVTIME(size_t new_vtime)
{
    struct termios options;
    tcgetattr(fd, &options);
    options.c_cc[VTIME] = new_vtime;
    tcsetattr(fd, TCSANOW, &options);
}
