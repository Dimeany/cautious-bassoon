#ifndef CDH_HEALTH_HEADER
#define CDH_HEALTH_HEADER

#include "cube_space_types.h"

#include <iostream>


namespace cdh {
    #pragma pack(push, 1)
    struct CdhImpressHealth {
        float avionics_temp;
        float flight_temp;
        float igse_temp;
        float cpu_temp;
        float sd_used_gb;
        float sd_avail_gb;
        int sd_usage_percent;
    };

    struct CdhExactHealth {
        float avionics_temp;
        float flight_temp;
        float cpu_temp;
        float sd_used_gb;
        float sd_avail_gb;
        int sd_usage_percent;
    };
    #pragma pack(pop)

}

#endif