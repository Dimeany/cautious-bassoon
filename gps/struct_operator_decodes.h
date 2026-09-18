#ifndef GPS_STRUCTS_HEADER
#define GPS_STRUCTS_HEADER
#include <stdint.h>
#include <sstream>
#include <iostream>

#include "novatel/novatel_structs.hpp"
#include "novatel/novatel_enums.hpp"
#include "novatel/common.hpp"
#include "novatel/crc32.hpp"

std::ostream& operator<<(std::ostream& os, gps::BESTXYZ const& s);
std::ostream& operator<<(std::ostream& os, gps::HWMONITOR const& s);
std::ostream& operator<<(std::ostream& os, gps::RXSTATUS const& s);
std::ostream& operator<<(std::ostream& os, gps::PDPDOP const& s);
std::ostream& operator<<(std::ostream& os, gps::PSRDOP const& s);
std::ostream& operator<<(std::ostream& os, gps::OEM4BinaryHeader const& s);

#endif

