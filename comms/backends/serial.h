#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <cstdlib>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "backend.h"

namespace comms {
    namespace backend {

        class Serial : public Backend {
            int fd = -1;
          public:
            Serial(std::string interface, speed_t);
            ~Serial();

            int getFd() override;
            size_t write(std::span<char const> data) override;
            std::vector<char> read() override;
            bool bytesAvailable() override;
        };
    }
}

#endif
