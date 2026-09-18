#ifndef _FILEIN_H_
#define _FILEIN_H_

#include <fstream>
#include <iostream>

#include "backend.h"

namespace comms {
    namespace backend {

        class FileIn : public Backend {
            int fd = -1;
          public:
            std::fstream file;

            FileIn(std::string filename, bool ppp, bool force);

            int getFd() override;
            size_t write(std::span<char const> data) override;
            std::vector<char> read() override;
            bool bytesAvailable() override;
        };
    }
}

#endif
