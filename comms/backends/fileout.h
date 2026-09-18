#ifndef _FILEOUT_H_
#define _FILEOUT_H_

#include <fstream>
#include <filesystem>
#include <iostream>

#include "backend.h"

namespace comms {
    namespace backend {

        class FileOut : public Backend {
            int fd = -1;
          public:
            std::fstream file;

            FileOut(std::string filename, bool force);

            int getFd() override;
            size_t write(std::span<char const> data) override;
            std::vector<char> read() override;
            bool bytesAvailable() override;
        };
    }
}

#endif
