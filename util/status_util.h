#ifndef STATUS_UTIL_H
#define STATUS_UTIL_H

#include <cstdint>
#include <string>

namespace util {
    enum Status : uint8_t {
        UNKNOWN = 1,
        OK = 0,
        HARDWARE_ERROR = 2,
        SOFTWARE_ERROR = 3
    };

    std::string statusToString(Status status);
    std::string intStatusToString(int status);
} // namespace util

#endif