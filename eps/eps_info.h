#ifndef EPS_INFO_H
#define EPS_INFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// struct containing info about the current state of the pdu
typedef struct eps_pdup4_info_s {
    uint32_t gndwdt;
    uint16_t boot_cnt;
    uint8_t ch_on[8];
    uint8_t ch_on_actual[8];
    uint32_t ch_uptime[8];
    uint32_t ch_downtime[8];

} eps_pdup4_info_t;

#pragma pack()

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <format>

namespace eps {
    typedef eps_pdup4_info_t EpsPDUP4Info;
}

std::string infoArrayHelper(uint8_t arr[8])    { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }
std::string infoArrayHelper(int16_t arr[8])    { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }
std::string infoArrayHelper(uint16_t arr[8])   { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }
std::string infoArrayHelper(int32_t arr[8])    { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }
std::string infoArrayHelper(uint32_t arr[8])   { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }

std::string infoToString(eps::EpsPDUP4Info info)
{
    return "\nPDU-P4 Info\ngndwdt:\t\t" + std::to_string(info.gndwdt) + " s" +
    "\nboot_cnt:\t" + std::to_string(info.boot_cnt) +
    "\nch_on:\t\t" + infoArrayHelper(info.ch_on) +
    "\nch_on_actual:\t" + infoArrayHelper(info.ch_on_actual) +
    "\nch_uptime:\t" + infoArrayHelper(info.ch_uptime) + " s" +
    "\nch_downtime:\t" + infoArrayHelper(info.ch_downtime) + " s";
}

#endif

#endif
