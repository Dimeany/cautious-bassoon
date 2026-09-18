#ifndef EPS_HEALTH_H
#define EPS_HEALTH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// We want to pack the data so that it is consistent with our documentation
#pragma pack(1)

typedef struct eps_pdup3_health_s {
    // PCDU VARIABLES
    int16_t pcdu_temp;
    uint16_t pcdu_vbatt;
    bool pcdu_ch_on[12];
    uint16_t pcdu_icur[12];
    uint16_t pcdu_vcur[12];

    // BATT VARIABLES
    bool batt_heater_on;
    bool batt_balance_cell;
    int16_t batt_temp;
    uint16_t batt_vin;
    uint16_t batt_vout;
    int16_t batt_iin;
    uint16_t batt_iout;

    // MPPT VARIABLES
    int16_t mppt_temp;
    uint16_t mppt_iin[7];
    uint16_t mppt_vin[7];
} eps_pdup3_health_t;


typedef struct eps_pdup4_health_s {
    int16_t pdu_mcu_temp;
    uint8_t pdu_ch_on[8];
    uint16_t pdu_ch_voltage[8];
    uint16_t pdu_ch_current[8];
    int16_t pdu_ch_temp[8];
    uint16_t pdu_bus_voltage;
} eps_pdup4_health_t;



#pragma pack()

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace eps {
    typedef eps_pdup3_health_t EpsImpressHealth;
    typedef eps_pdup4_health_t EpsPDUP4Health;
}

std::ostream& operator<<(std::ostream& os, const eps::EpsImpressHealth);
std::ostream& operator<<(std::ostream& os, const eps::EpsPDUP4Health);

#endif

#endif
