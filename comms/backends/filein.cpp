#include "ppp.h"
#include "filein.h"

comms::backend::FileIn::FileIn(std::string filename, bool ppp, bool force) : file(filename, std::ios_base::in | std::ios_base::binary) {
    if (!file.is_open())
        throw std::runtime_error("File does not exist");

    char first_char = file.peek();
    if (ppp && !force && first_char != comms::backend::PPP::FLAG) {
        std::cerr << filename << " does not contain PPP packets!\n";
        std::cerr << "Run without -p to process as UDP packets, or run with -f to process anyway.\n";
        exit(1);
    }

    file.seekg(-1, std::ios_base::end);
    char last_char = file.peek();
    file.seekg(0);
    if (!ppp && !force && first_char == comms::backend::PPP::FLAG && last_char == comms::backend::PPP::FLAG) {
        std::cerr << filename << " likely contains PPP packets!\n";
        std::cerr << "Run with -p to process as PPP packets, or run with -f to process anyway.\n";
        exit(1);
    }
}

int comms::backend::FileIn::getFd() {
    return fd;
}

size_t comms::backend::FileIn::write(std::span<char const> data) {
    (void) data;
    return 0;
}

std::vector<char> comms::backend::FileIn::read() {
    size_t size = 65536;

    char *buffer = new char[size];
    file.read(buffer, size);
    size_t read_size = file.gcount();

    std::vector<char> output {};
    output.reserve(read_size);
    output.assign(buffer, buffer + read_size);

    delete[] buffer;
    return output;
}

bool comms::backend::FileIn::bytesAvailable() {
    return file.peek() != EOF;
}
