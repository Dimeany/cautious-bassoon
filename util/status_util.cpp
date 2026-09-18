#include "status_util.h"

#include <iostream>

namespace util {
    std::string statusToString(Status status)
    {
        switch (status)
        {
            case Status::UNKNOWN: return "Unknown";
            case Status::OK: return "Ok";
            case Status::HARDWARE_ERROR: return "Hardware Error";
            case Status::SOFTWARE_ERROR: return "Software Error";
        }

        return "Undefined Status";
    }

    std::string intStatusToString(int status) { return statusToString(static_cast<Status>(static_cast<uint8_t>(status))); }
}