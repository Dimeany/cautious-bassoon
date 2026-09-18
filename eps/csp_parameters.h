// csp_parameters.h
// Simeon Shaffar - May 1, 2025
// This header file does the extern declarations of all the parameters, and defines some
// other declarations used to reference parameters.


#ifndef CSP_PARAMETERS_H
#define CSP_PARAMETERS_H

#include <param/param.h>

#ifdef __cplusplus
extern "C" {
#endif


// For the node declarations
#define PCDU_ARRAY_SIZE 12
#define BATT_ARRAY_SIZE 4
#define MPPT_ARRAY_SIZE 7

#define PDUP4_CH_CNT 8
#define PDU_CONF_ARRAY_SIZE 9

typedef enum {
    EXACT,
    IMPRESS,
} Mission;

// For node declarations, where these ints are the CSP node addresses of each device
extern uint16_t PCDU;
extern uint16_t BATT;
extern uint16_t MPPT;

//P4 PDU for EXACT

extern uint16_t PDU;

// These are the master arrays which will allow us to check over all the parameters
extern param_t* impress_params_a[];
extern param_t* pdup4_params_a[];


// Here, we define an enum which we can use to feed into the array to select a parameter
typedef enum {
    PCDU_GNDWDT,
	PCDU_TEMP,
	PCDU_TEMP0,
	PCDU_TEMP1,
    PCDU_VBATT,
    PCDU_TLM_RESET,
    PCDU_CH_ON,
    PCDU_DFL_ON,
    PCDU_DFL_ON_IN,
    PCDU_DFL_OFF_IN,
    PCDU_CH_ILIM,
    PCDU_DFL_ILIM,
    PCDU_CH_PROTECT,
    PCDU_DFL_VPROTECT,
    PCDU_DFL_VPROTECT_HYS,
    PCDU_FAULT_OC,
    PCDU_FAULT_IO,
    PCDU_P_AVG,
    PCDU_ENERGY,
    PCDU_P_CUR,
    PCDU_I_CUR,
    PCDU_V_CUR,
    PCDU_P_MAX,
    PCDU_I_MAX,
    PCDU_V_MAX,
    PCDU_V_MIN,

    BATT_GNDWDT,
    BATT_TEMP,
    BATT_BAT1_TEMP,
    BATT_BAT2_TEMP,
    BATT_BAL_TEMP,
    BATT_OUT_TEMP,
    BATT_INT_TEMP,
    BATT_HEATER_ON,
    BATT_HEATER_AUTO,
    BATT_HEATER_AUTO_ON_AT,
    BATT_HEATER_AUTO_OFF_AT,
    BATT_POWERCYCLE,
    BATT_BALANCE_CELL,
    BATT_BALANCE_AUTO,
    BATT_BALANCE_THR,
    BATT_VIN,
    BATT_VOUT,
    BATT_IIN,
    BATT_IOUT,
    BATT_VCELL,
    BATT_RSENSE_IN,
    BATT_RSENSE_OUT,
    BATT_FAULT_IO,

    MPPT_GNDWDT,
    MPPT_TEMP,
    MPPT_TEMP0,
    MPPT_TEMP1,
    MPPT_MODE,
    MPPT_VFIXED,
    MPPT_TLM_RESET,
    MPPT_STEP_SIZE,
    MPPT_VEOC,
    MPPT_IOUT_MAX,
    MPPT_VOUT,
    MPPT_IOUT,
    MPPT_VIN,
    MPPT_IIN,
    MPPT_EFF,
    MPPT_DAC_EN,
    MPPT_DAC_VALUE,
    
    // "Sentinel Value" at the end so we can easily grab the total number of params
    IMPRESS_PARAMS_COUNT
} impress_params_e;

// Defined in the order listed in the PDU-P4 SWICD Issue 1.4-6 (skips all listed as 'internal use')
typedef enum {
    P4_GNDWDT,
    P4_CSP_CAN0_ADDR,
    P4_CSP_CAN0_MASK,
    P4_CSP_PRINT_CNF,
    P4_CSP_CAN0_PROMISE,
    P4_CSP_DEDUP,
    P4_BOOT_IMG1,
    P4_BOOT_IMG0,
    P4_BOOT_CUR,
    P4_BOOT_CNT,
    P4_BOOT_ERR,
    P4_CSP_CAN1_PROMISC,
    P4_MCU_TEMP,
    P4_CSP_CAN1_ADDR,
    P4_CSP_CAN1_MASK,
    P4_CSP_CAN0_ISDFL,
    P4_CSP_CAN1_ISDFL,
    P4_CH_ON,
    P4_CH_ON_INIT,
    P4_CH_TYPE,
    P4_CH_ON_ACTUAL,
    P4_REBOOT_MODE,
    P4_CH_OC_RETRY,
    P4_CH_LATCHUP_TIME,
    P4_CH_OC_CNT,
    P4_CH_OC_CNT_HW,
    P4_CH_TEMP,
    P4_CH_VOLTAGE,
    P4_CH_CURRENT,
    P4_BUS_VOLTAGE,
    P4_CH_CURLIM,
    P4_CROSS_KILL,
    P4_MASTER_MODE,
    P4_MASTER_MODE_INIT,
    P4_WINDOW_UV,
    P4_WINDOW_OV,
    P4_WINDOW_HYST,
    P4_CH_ON_IN,
    P4_CH_OFF_IN,
    P4_WINDOW_UV_ACTIVE,
    P4_WINDOW_OV_ACTIVE,
    P4_CH_UPTIME,
    P4_CH_DOWNTIME,
    P4_CH_WDT_INITIAL,
    P4_CH_WDT_IN,
    P4_CH_WDT_ON_TIMER_CUR,
    P4_CH_LATCHED_UP_HW,
    P4_CH_ON_CNT,

    // Sentinel value to track the total number of parameters
    PDUP4_PARAMS_COUNT
} pdup4_params_e;


#ifdef __cplusplus
}
#endif

#endif