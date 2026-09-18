#ifndef EXPERIMENT_HS_HEADER
#define EXPERIMENT_HS_HEADER

#include <cstdint>

namespace cdh {
    #pragma pack(push, 1)
    struct CDHHS {
        int16_t eps_temp;
        int16_t gps_temp;
        int16_t det_temp;
        int16_t cpu_temp;
        int16_t in_volts;
        int16_t gps_mA;
        int16_t det_5_mA;
        int16_t det_6_mA;
        int16_t det_7_mA;
        int16_t det_8_mA;
        int16_t cpu_mA;
    };
    #pragma pack(pop)
}

#endif