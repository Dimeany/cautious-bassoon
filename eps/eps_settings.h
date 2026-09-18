#ifndef EPS_SETTINGS_H
#define EPS_SETTINGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

// struct containing configurable settings
typedef struct eps_pdup4_settings_s {
    uint8_t ch_on_init[8];
    uint8_t ch_type[8];
    uint16_t ch_curlim[8];
    uint16_t window_uv[8];
    uint16_t window_ov[8];
    uint16_t window_hyst[8];
} eps_pdup4_settings_t;

#pragma pack()

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <format>

namespace eps {
    typedef eps_pdup4_settings_t EpsPDUP4Settings;
}

std::string settingsArrayHelper(uint8_t arr[8])    { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }
std::string settingsArrayHelper(uint16_t arr[8])   { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }
std::string settingsArrayHelper(uint32_t arr[8])   { return std::format("[{} {} {} {} {} {} {} {}]", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]); }

std::string settingsToString(eps::EpsPDUP4Settings settings)
{
    return "\nPDU-P4 Settings\nch_on_init:\t" + settingsArrayHelper(settings.ch_on_init) +
    "\nch_type:\t" + settingsArrayHelper(settings.ch_type) + " 0=normal, 1=critical, 2=protected" +
    "\nch_curlim:\t" + settingsArrayHelper(settings.ch_curlim) + " mA" +
    "\nwindow_uv:\t" + settingsArrayHelper(settings.window_uv) + " mV" +
    "\nwindow_ov:\t" + settingsArrayHelper(settings.window_ov) + " mV" +
    "\nwindow_hyst:\t" + settingsArrayHelper(settings.window_hyst) + " mV";
}

#endif

#endif
