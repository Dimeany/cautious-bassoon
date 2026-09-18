#include "fileout.h"

comms::backend::FileOut::FileOut(std::string filename, bool force) : file() {

    if (!force && std::filesystem::exists(filename)) {
        std::cout << filename << " already exists!\n";
        std::cout << "Run again with -f to overwrite file, or change the filename to something new.\n";
        exit(1);
    }

    file.open(filename, std::ios_base::out | std::ios_base::binary);
    if (!file.is_open())
        throw std::runtime_error("File could not be created");
}

int comms::backend::FileOut::getFd() {
    return fd;
}

size_t comms::backend::FileOut::write(std::span<char const> data) {
    file.write(data.data(), data.size());
    return data.size();
}

std::vector<char> comms::backend::FileOut::read() {
    return {};
}

bool comms::backend::FileOut::bytesAvailable() {
    return false;
}
