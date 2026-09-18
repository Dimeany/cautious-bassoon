// csp_parameters.c
// Simeon Shaffar - May 1, 2025
// This file makes declarations of all the parameters used in the driver
// They are defined using the 

#include "csp_parameters.h"


// Define the node addresses of the different hardware devices for the below definitions
uint16_t PCDU = 3;
uint16_t BATT = 2;
uint16_t MPPT = 1;

// For p4 EXACT
uint16_t PDU = 5;


// Here below, we define all of our parameters, there are a few details to note:
// - First, notice that we are using a macro called PARAM_DEFINE_REMOTE, which was defined back in Space Inventor's code
//		it effectively just takes what you give it and plops it into a param_t struct
// - Second, notice the difference between _temp and temp (and the equivalents for all other parameters)
//		_temp is the raw integer value of the parameter, temp is the parameter, which is the struct with a bunch of other information
//		_temp is passed into the temp initializer as a reference, so when we pull telemetry, the value gets put into _temp
// - Third, notice the random numbers for each parameter, this is the ID for that specific parameter,
//		we get it from the print_param_fields() function. For a list of these, go to this link:
//		https://docs.google.com/document/d/1XeLAGGvOiDOC16z9gDCepyFvwq1XCtRQc4FCeiZBu_g/edit

// P3 STUFF FOR IMPRESS

//PCDU PARAMETERS

//Watchdog timer
uint32_t _pcdu_gndwdt = 0;
PARAM_DEFINE_REMOTE(1, pcdu_gndwdt, &PCDU, PARAM_TYPE_UINT32, 1, sizeof(uint32_t), PM_CONF, &_pcdu_gndwdt, "")

//Scalar parameters
int16_t _pcdu_temp = 0;
PARAM_DEFINE_REMOTE(30, pcdu_temp, &PCDU, PARAM_TYPE_UINT16, 1, sizeof(int16_t), PM_CONF, &_pcdu_temp, "")
int16_t _pcdu_temp0 = 0;
PARAM_DEFINE_REMOTE(400, pcdu_temp0, &PCDU, PARAM_TYPE_UINT16, 1, sizeof(int16_t), PM_CONF, &_pcdu_temp0, "")
int16_t _pcdu_temp1 = 0;
PARAM_DEFINE_REMOTE(401, pcdu_temp1, &PCDU, PARAM_TYPE_UINT16, 1, sizeof(int16_t), PM_CONF, &_pcdu_temp1, "")
uint16_t _pcdu_vbatt = 0;
PARAM_DEFINE_REMOTE(402, pcdu_vbatt, &PCDU, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_pcdu_vbatt, "")
uint8_t _pcdu_tlm_reset = 0;
PARAM_DEFINE_REMOTE(410, pcdu_tlm_reset, &PCDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pcdu_tlm_reset, "")

//Channel parameters
uint8_t _pcdu_ch_on[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(200, pcdu_ch_on, &PCDU, PARAM_TYPE_UINT8, PCDU_ARRAY_SIZE, sizeof(uint8_t), PM_CONF, &_pcdu_ch_on, "")
uint8_t _pcdu_dfl_on[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(230, pcdu_dfl_on, &PCDU, PARAM_TYPE_UINT8, PCDU_ARRAY_SIZE, sizeof(uint8_t), PM_CONF, &_pcdu_dfl_on, "")

//Timer parameters
uint32_t _pcdu_dfl_on_in[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(236, pcdu_dfl_on_in, &PCDU, PARAM_TYPE_UINT32, PCDU_ARRAY_SIZE, sizeof(uint32_t), PM_CONF, &_pcdu_dfl_on_in, "")
uint32_t _pcdu_dfl_off_in[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(237, pcdu_dfl_off_in, &PCDU, PARAM_TYPE_UINT32, PCDU_ARRAY_SIZE, sizeof(uint32_t), PM_CONF, &_pcdu_dfl_off_in, "")

//Current limit parameters
uint16_t _pcdu_ch_ilim[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(202, pcdu_ch_ilim, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_ch_ilim, "")
uint16_t _pcdu_dfl_ilim[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(234, pcdu_dfl_ilim, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_dfl_ilim, "")

//Voltage protection parameters
uint8_t _pcdu_ch_protect[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(201, pcdu_ch_protect, &PCDU, PARAM_TYPE_UINT8, PCDU_ARRAY_SIZE, sizeof(uint8_t), PM_CONF, &_pcdu_ch_protect, "")
uint16_t _pcdu_dfl_vprotect[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(232, pcdu_dfl_vprotect, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_dfl_vprotect, "")
uint16_t _pcdu_dfl_vprotect_hys[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(231, pcdu_dfl_vprotect_hys, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_dfl_vprotect_hys, "")

//Overcurrent and IO fault parameters
uint32_t _pcdu_fault_oc[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(302, pcdu_fault_oc, &PCDU, PARAM_TYPE_UINT32, PCDU_ARRAY_SIZE, sizeof(uint32_t), PM_CONF, &_pcdu_fault_oc, "")
uint32_t _pcdu_fault_io[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(301, pcdu_fault_io, &PCDU, PARAM_TYPE_UINT32, PCDU_ARRAY_SIZE, sizeof(uint32_t), PM_CONF, &_pcdu_fault_io, "")

//Net power and energy parameters
uint32_t _pcdu_p_avg[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(421, pcdu_p_avg, &PCDU, PARAM_TYPE_UINT32, PCDU_ARRAY_SIZE, sizeof(uint32_t), PM_CONF, &_pcdu_p_avg, "")
uint64_t _pcdu_energy[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(411, pcdu_energy, &PCDU, PARAM_TYPE_UINT64, PCDU_ARRAY_SIZE, sizeof(uint64_t), PM_CONF, &_pcdu_energy, "")

//Current PIV parameters
uint16_t _pcdu_p_cur[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(420, pcdu_p_cur, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_p_cur, "")
uint16_t _pcdu_i_cur[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(417, pcdu_i_cur, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_i_cur, "")
uint16_t _pcdu_v_cur[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(414, pcdu_v_cur, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_v_cur, "")


//Max PIV parameters
uint16_t _pcdu_p_max[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(419, pcdu_p_max, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_p_max, "")
uint16_t _pcdu_i_max[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(416, pcdu_i_max, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_i_max, "")
uint16_t _pcdu_v_max[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(413, pcdu_v_max, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_v_max, "")
uint16_t _pcdu_v_min[PCDU_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(412, pcdu_v_min, &PCDU, PARAM_TYPE_UINT16, PCDU_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_pcdu_v_min, "")


//BATT PARAMETERS

//Watchdog timer
uint32_t _batt_gndwdt = 0;
PARAM_DEFINE_REMOTE(1, batt_gndwdt, &BATT, PARAM_TYPE_UINT32, 1, sizeof(uint32_t), PM_CONF, &_batt_gndwdt, "")

//Temp parameters
int16_t _batt_temp = 0;
PARAM_DEFINE_REMOTE(30, batt_temp, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_temp, "")
int16_t _batt_bat1_temp = 0;
PARAM_DEFINE_REMOTE(120, batt_bat1_temp, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_bat1_temp, "")
int16_t _batt_bat2_temp = 0;
PARAM_DEFINE_REMOTE(129, batt_bat2_temp, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_bat2_temp, "")
int16_t _batt_bal_temp = 0;
PARAM_DEFINE_REMOTE(121, batt_bal_temp, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_bal_temp, "")
int16_t _batt_out_temp = 0;
PARAM_DEFINE_REMOTE(128, batt_out_temp, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_out_temp, "")
int16_t _batt_int_temp = 0;
PARAM_DEFINE_REMOTE(127, batt_int_temp, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_int_temp, "")

//Heater parameters
uint8_t _batt_heater_on = 0;
PARAM_DEFINE_REMOTE(160, batt_heater_on, &BATT, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_batt_heater_on, "")
uint8_t _batt_heater_auto = 0;
PARAM_DEFINE_REMOTE(140, batt_heater_auto, &BATT, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_batt_heater_auto, "")
int8_t _batt_heater_auto_on_at = 0;
PARAM_DEFINE_REMOTE(141, batt_heater_auto_on_at, &BATT, PARAM_TYPE_INT8, 1, sizeof(int8_t), PM_CONF, &_batt_heater_auto_on_at, "")
int8_t _batt_heater_auto_off_at = 0;
PARAM_DEFINE_REMOTE(142, batt_heater_auto_off_at, &BATT, PARAM_TYPE_INT8, 1, sizeof(int8_t), PM_CONF, &_batt_heater_auto_off_at, "")

//Balancing and power cycle parameters
uint8_t _batt_powercycle = 0;
PARAM_DEFINE_REMOTE(170, batt_powercycle, &BATT, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_batt_powercycle, "")
uint8_t _batt_balance_cell = 0;
PARAM_DEFINE_REMOTE(162, batt_balance_cell, &BATT, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_batt_balance_cell, "")
uint8_t _batt_balance_auto = 0;
PARAM_DEFINE_REMOTE(145, batt_balance_auto, &BATT, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_batt_balance_auto, "")
uint16_t _batt_balance_thr = 0;
PARAM_DEFINE_REMOTE(146, batt_balance_thr, &BATT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_batt_balance_thr, "")

//Current PIV parameters
uint16_t _batt_vin = 0;
PARAM_DEFINE_REMOTE(123, batt_vin, &BATT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_batt_vin, "")
uint16_t _batt_vout = 0;
PARAM_DEFINE_REMOTE(125, batt_vout, &BATT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_batt_vout, "")
int16_t _batt_iin = 0;
PARAM_DEFINE_REMOTE(124, batt_iin, &BATT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_batt_iin, "")
uint16_t _batt_iout = 0;
PARAM_DEFINE_REMOTE(126, batt_iout, &BATT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_batt_iout, "")
uint16_t _batt_vcell[BATT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(122, batt_vcell, &BATT, PARAM_TYPE_UINT16, BATT_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_batt_vcell, "")

//RSense Parameters (I have no clue what the heck these are)
double _batt_rsense_in = 0;
PARAM_DEFINE_REMOTE(147, batt_rsense_in, &BATT, PARAM_TYPE_DOUBLE, 1, sizeof(double), PM_CONF, &_batt_rsense_in, "")
double _batt_rsense_out = 0;
PARAM_DEFINE_REMOTE(148, batt_rsense_out, &BATT, PARAM_TYPE_DOUBLE, 1, sizeof(double), PM_CONF, &_batt_rsense_out, "")

//Faultio parameter
uint32_t _batt_fault_io = 0;
PARAM_DEFINE_REMOTE(250, batt_fault_io, &BATT, PARAM_TYPE_UINT32, 1, sizeof(uint32_t), PM_CONF, &_batt_fault_io, "")


//MPPT PARAMETERS

//Watch dog timer
uint32_t _mppt_gndwdt = 0;
PARAM_DEFINE_REMOTE(1, mppt_gndwdt, &MPPT, PARAM_TYPE_UINT32, 1, sizeof(uint32_t), PM_CONF, &_mppt_gndwdt, "")

//Temp parameters
int16_t _mppt_temp = 0;
PARAM_DEFINE_REMOTE(30, mppt_temp, &MPPT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_mppt_temp, "")
int16_t _mppt_temp0 = 0;
PARAM_DEFINE_REMOTE(180, mppt_temp0, &MPPT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_mppt_temp0, "")
int16_t _mppt_temp1 = 0;
PARAM_DEFINE_REMOTE(181, mppt_temp1, &MPPT, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_CONF, &_mppt_temp1, "")

//Mode parameters
uint8_t _mppt_mode[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(113, mppt_mode, &MPPT, PARAM_TYPE_UINT8, MPPT_ARRAY_SIZE, sizeof(uint8_t), PM_CONF, &_mppt_mode, "")
uint16_t _mppt_vfixed[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(115, mppt_vfixed, &MPPT, PARAM_TYPE_UINT16, MPPT_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_mppt_vfixed, "")
uint8_t _mppt_tlm_reset = 0;
PARAM_DEFINE_REMOTE(400, mppt_tlm_reset, &MPPT, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_mppt_tlm_reset, "")
uint8_t _mppt_step_size[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(111, mppt_step_size, &MPPT, PARAM_TYPE_UINT8, MPPT_ARRAY_SIZE, sizeof(uint8_t), PM_CONF, &_mppt_step_size, "")

//Protection parameters
uint16_t _mppt_veoc = 0;
PARAM_DEFINE_REMOTE(130, mppt_veoc, &MPPT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_mppt_veoc, "")
uint16_t _mppt_iout_max = 0;
PARAM_DEFINE_REMOTE(129, mppt_iout_max, &MPPT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_mppt_iout_max, "")

//State parameters
uint16_t _mppt_vout = 0;
PARAM_DEFINE_REMOTE(151, mppt_vout, &MPPT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_mppt_vout, "")
uint16_t _mppt_iout = 0;
PARAM_DEFINE_REMOTE(153, mppt_iout, &MPPT, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_mppt_iout, "")
uint16_t _mppt_vin[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(150, mppt_vin, &MPPT, PARAM_TYPE_UINT16, MPPT_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_mppt_vin, "")
uint16_t _mppt_iin[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(152, mppt_iin, &MPPT, PARAM_TYPE_UINT16, MPPT_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_mppt_iin, "")
double _mppt_eff = 0;
PARAM_DEFINE_REMOTE(154, mppt_eff, &MPPT, PARAM_TYPE_DOUBLE, 1, sizeof(double), PM_CONF, &_mppt_eff, "")

//DAC parameters
uint8_t _mppt_dac_en[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(140, mppt_dac_en, &MPPT, PARAM_TYPE_UINT8, MPPT_ARRAY_SIZE, sizeof(uint8_t), PM_CONF, &_mppt_dac_en, "")
uint16_t _mppt_dac_value[MPPT_ARRAY_SIZE] = {0};
PARAM_DEFINE_REMOTE(142, mppt_dac_value, &MPPT, PARAM_TYPE_UINT16, MPPT_ARRAY_SIZE, sizeof(uint16_t), PM_CONF, &_mppt_dac_value, "")


// This is the master array which will contain pointers to each parameter
param_t* impress_params_a[IMPRESS_PARAMS_COUNT] = {
    &pcdu_gndwdt,
    &pcdu_temp,
    &pcdu_temp0,
    &pcdu_temp1,
    &pcdu_vbatt,
    &pcdu_tlm_reset,
    &pcdu_ch_on,
    &pcdu_dfl_on,
    &pcdu_dfl_on_in,
    &pcdu_dfl_off_in,
    &pcdu_ch_ilim,
    &pcdu_dfl_ilim,
    &pcdu_ch_protect,
    &pcdu_dfl_vprotect,
    &pcdu_dfl_vprotect_hys,
    &pcdu_fault_oc,
    &pcdu_fault_io,
    &pcdu_p_avg,
    &pcdu_energy,
    &pcdu_p_cur,
    &pcdu_i_cur,
    &pcdu_v_cur,
    &pcdu_p_max,
    &pcdu_i_max,
    &pcdu_v_max,
    &pcdu_v_min,

    &batt_gndwdt,
    &batt_temp,
    &batt_bat1_temp,
    &batt_bat2_temp,
    &batt_bal_temp,
    &batt_out_temp,
    &batt_int_temp,
    &batt_heater_on,
    &batt_heater_auto,
    &batt_heater_auto_on_at,
    &batt_heater_auto_off_at,
    &batt_powercycle,
    &batt_balance_cell,
    &batt_balance_auto,
    &batt_balance_thr,
    &batt_vin,
    &batt_vout,
    &batt_iin,
    &batt_iout,
    &batt_vcell,
    &batt_rsense_in,
    &batt_rsense_out,
    &batt_fault_io,
    
    &mppt_gndwdt,
    &mppt_temp,
    &mppt_temp0,
    &mppt_temp1,
    &mppt_mode,
    &mppt_vfixed,
    &mppt_tlm_reset,
    &mppt_step_size,
    &mppt_veoc,
    &mppt_iout_max,
    &mppt_vout,
    &mppt_iout,
    &mppt_vin,
    &mppt_iin,
    &mppt_eff,
    &mppt_dac_en,
    &mppt_dac_value
};



// P4 STUFF

// Parameter declarations for PDU-P4 based on SWICD parameter list
// All of these were double checked by Noah Nieman and Simeon Shaffar on 5/21/2025
// Updated on 6/16/2026 by Liam Foley, based on PDU-P4 SWICD Issue 1.4-6 (after updating firmware)

uint32_t _pdu_gndwdt = 0;
uint16_t _pdu_csp_can0_addr = 0;
uint8_t _pdu_csp_can0_mask = 0;
uint8_t _pdu_csp_print_cnf = 0;
uint8_t _pdu_csp_can0_promisc = 0;
uint8_t _pdu_csp_dedup = 0;
uint8_t _pdu_boot_img1 = 0;
uint8_t _pdu_boot_img0 = 0;
uint8_t _pdu_boot_cur = 0;
uint16_t _pdu_boot_cnt = 0;
uint16_t _pdu_boot_err = 0;
uint8_t _pdu_csp_can1_promisc = 0;
int16_t _pdu_mcu_temp = 0;
uint16_t _pdu_csp_can1_addr = 0;
uint8_t _pdu_csp_can1_mask = 0;
uint8_t _pdu_csp_can0_isdfl = 0;
uint8_t _pdu_csp_can1_isdfl = 0;
uint8_t _pdu_ch_on[PDUP4_CH_CNT] = {0};
uint8_t _pdu_ch_on_init[PDUP4_CH_CNT] = {0};
uint8_t _pdu_ch_type[PDUP4_CH_CNT] = {0};
int8_t _pdu_ch_on_actual[PDUP4_CH_CNT] = {0};
uint8_t _pdu_reboot_mode[PDUP4_CH_CNT] = {0};
uint16_t _pdu_ch_oc_retry[PDUP4_CH_CNT] = {0};
uint64_t _pdu_ch_latchup_time[PDUP4_CH_CNT] = {0};
uint16_t _pdu_ch_oc_cnt[PDUP4_CH_CNT] = {0};
uint16_t _pdu_ch_oc_cnt_hw[PDUP4_CH_CNT] = {0};
int16_t _pdu_ch_temp[PDUP4_CH_CNT] = {0};
uint16_t _pdu_ch_voltage[PDUP4_CH_CNT] = {0};
int32_t _pdu_ch_current[PDUP4_CH_CNT] = {0};
uint16_t _pdu_bus_voltage = 0;
int16_t _pdu_ch_curlim[PDUP4_CH_CNT] = {0};
uint8_t _pdu_cross_kill = 0;
uint8_t _pdu_master_mode = 0;
uint8_t _pdu_master_mode_init = 0;
uint16_t _pdu_window_uv[PDUP4_CH_CNT] = {0};
uint16_t _pdu_window_ov[PDUP4_CH_CNT] = {0};
uint16_t _pdu_window_hyst[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_on_in[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_off_in[PDUP4_CH_CNT] = {0};
uint8_t _pdu_window_uv_active[PDUP4_CH_CNT] = {0};
uint8_t _pdu_window_ov_active[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_uptime[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_downtime[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_wdt_initial[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_wdt_in[PDUP4_CH_CNT] = {0};
uint8_t _pdu_ch_wdt_on_timer_cur[PDUP4_CH_CNT] = {0};
uint8_t _pdu_ch_latched_up_hw[PDUP4_CH_CNT] = {0};
uint32_t _pdu_ch_on_cnt[PDUP4_CH_CNT] = {0};

PARAM_DEFINE_REMOTE(1, pdu_gndwdt, &PDU, PARAM_TYPE_UINT32, 1, sizeof(uint32_t), PM_CONF, &_pdu_gndwdt, "Max value: 3 days + 8 hours (288000 s)")
PARAM_DEFINE_REMOTE(10, pdu_csp_can0_addr, &PDU, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_pdu_csp_can0_addr, "Set according to your network topology")
PARAM_DEFINE_REMOTE(11, pdu_csp_can0_mask, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_can0_mask, "Set according to your network topology")
PARAM_DEFINE_REMOTE(13, pdu_csp_print_cnf, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_print_cnf, "0=no effect, 1=print csp interface configuration to stdout")
PARAM_DEFINE_REMOTE(14, pdu_csp_can0_promisc, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_can0_promisc, "Should be set to 0")
PARAM_DEFINE_REMOTE(17, pdu_csp_dedup, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_dedup, "0=off, 1=on forwarding only, 2=incoming only, 3=on incoming and forwarding")
PARAM_DEFINE_REMOTE(20, pdu_boot_img1, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_SYSINFO, &_pdu_boot_img1, "Number of times to boot image 0") // theres some strange stuff going on with this and the next param
PARAM_DEFINE_REMOTE(21, pdu_boot_img0, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_SYSINFO, &_pdu_boot_img0, "Number of times to boot image 1") // but this is how its listed in the SWICD
PARAM_DEFINE_REMOTE(24, pdu_boot_cur, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_SYSINFO, &_pdu_boot_cur, "Current image running")
PARAM_DEFINE_REMOTE(25, pdu_boot_cnt, &PDU, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_SYSINFO, &_pdu_boot_cnt, "Total number of boots")
PARAM_DEFINE_REMOTE(26, pdu_boot_err, &PDU, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_SYSINFO, &_pdu_boot_err, "Number of failed boot attempts")
PARAM_DEFINE_REMOTE(27, pdu_csp_can1_promisc, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_can1_promisc, "Should be set to 0")
PARAM_DEFINE_REMOTE(30, pdu_mcu_temp, &PDU, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_READONLY | PM_TELEM, &_pdu_mcu_temp, "MCU temperature (Cx100)")
PARAM_DEFINE_REMOTE(38, pdu_csp_can1_addr, &PDU, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_CONF, &_pdu_csp_can1_addr, "Set according to your network topology")
PARAM_DEFINE_REMOTE(39, pdu_csp_can1_mask, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_can1_mask, "Set according to your network topology")
PARAM_DEFINE_REMOTE(48, pdu_csp_can0_isdfl, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_can0_isdfl, "CAN 0 default flag. Active HIGH")
PARAM_DEFINE_REMOTE(49, pdu_csp_can1_isdfl, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_csp_can1_isdfl, "CAN 1 default flag. Active HIGH")
PARAM_DEFINE_REMOTE(120, pdu_ch_on, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_CONF, &_pdu_ch_on, "User desired state of pdu channels")
PARAM_DEFINE_REMOTE(121, pdu_ch_on_init, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_CONF, &_pdu_ch_on_init, "Initial state of pdu channels")
PARAM_DEFINE_REMOTE(122, pdu_ch_type, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_CONF, &_pdu_ch_type, "0=NORMAL Tries to follow ch_on, 1=CRITICAL Always on but power-cycles after GNDWDT (Overrules all other config), 2=PROTECTED")
PARAM_DEFINE_REMOTE(123, pdu_ch_on_actual, &PDU, PARAM_TYPE_INT8, PDUP4_CH_CNT, sizeof(int8_t), PM_READONLY | PM_TELEM, &_pdu_ch_on_actual, "Actual pin level set for the power channels. -1 for input/slave")
PARAM_DEFINE_REMOTE(124, pdu_reboot_mode, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_CONF, &_pdu_reboot_mode, "0=maintain channels after reboot, 1=restore channels to init settings")
PARAM_DEFINE_REMOTE(125, pdu_ch_oc_retry, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_CONF, &_pdu_ch_oc_retry, "Time left where channel is forced off after SW over-current (s)")
PARAM_DEFINE_REMOTE(127, pdu_ch_latchup_time, &PDU, PARAM_TYPE_UINT64, PDUP4_CH_CNT, sizeof(uint64_t), PM_CONF, &_pdu_ch_latchup_time, "Time taken to disable a power channel after an alert is detected (microseconds)")
PARAM_DEFINE_REMOTE(128, pdu_ch_oc_cnt, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_READONLY | PM_TELEM, &_pdu_ch_oc_cnt, "Counter for SW over-current events")
PARAM_DEFINE_REMOTE(129, pdu_ch_oc_cnt_hw, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_CONF, &_pdu_ch_oc_cnt_hw, "Counter for HW over-current/latch-up events")
PARAM_DEFINE_REMOTE(130, pdu_ch_temp, &PDU, PARAM_TYPE_INT16, PDUP4_CH_CNT, sizeof(int16_t), PM_READONLY | PM_TELEM, &_pdu_ch_temp, "PDU output channel INA temperature. (Cx100)")
PARAM_DEFINE_REMOTE(131, pdu_ch_voltage, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_READONLY | PM_TELEM, &_pdu_ch_voltage, "PDU output channel voltage. (mV)")
PARAM_DEFINE_REMOTE(132, pdu_ch_current, &PDU, PARAM_TYPE_INT32, PDUP4_CH_CNT, sizeof(int32_t), PM_READONLY | PM_TELEM, &_pdu_ch_current, "PDU output channel current. (mA)")
PARAM_DEFINE_REMOTE(133, pdu_bus_voltage, &PDU, PARAM_TYPE_UINT16, 1, sizeof(uint16_t), PM_READONLY | PM_TELEM, &_pdu_bus_voltage, "Measured supply voltage (mV)")
PARAM_DEFINE_REMOTE(137, pdu_ch_curlim, &PDU, PARAM_TYPE_INT16, PDUP4_CH_CNT, sizeof(int16_t), PM_CONF, &_pdu_ch_curlim, "Configure over-current limit (mA)")
PARAM_DEFINE_REMOTE(185, pdu_cross_kill, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_cross_kill, "=0 inactive. =123 other MCU powered down")
PARAM_DEFINE_REMOTE(190, pdu_master_mode, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_master_mode, "Disable INA and PDU control in slave mode.")
PARAM_DEFINE_REMOTE(191, pdu_master_mode_init, &PDU, PARAM_TYPE_UINT8, 1, sizeof(uint8_t), PM_CONF, &_pdu_master_mode_init, "Whether to assume master mode when booting")
PARAM_DEFINE_REMOTE(201, pdu_window_uv, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_CONF, &_pdu_window_uv, "Minimum voltage for power channels (mV)")
PARAM_DEFINE_REMOTE(202, pdu_window_ov, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_CONF, &_pdu_window_ov, "Maximum voltage for power channels (mV)")
PARAM_DEFINE_REMOTE(203, pdu_window_hyst, &PDU, PARAM_TYPE_UINT16, PDUP4_CH_CNT, sizeof(uint16_t), PM_CONF, &_pdu_window_ov, "Hysterisis for enabling power channels after window (mV)")
PARAM_DEFINE_REMOTE(204, pdu_ch_on_in, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_CONF, &_pdu_ch_on_in, "Countdown for turning on the power channels, paused when the channel is on (s)")
PARAM_DEFINE_REMOTE(205, pdu_ch_off_in, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_CONF, &_pdu_ch_off_in, "Countdown for turning off the power channels, paused when the channel is off (s)")
PARAM_DEFINE_REMOTE(206, pdu_window_uv_active, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_DEBUG, &_pdu_window_uv_active, "Whether the power channels is currently in under-voltage protection")
PARAM_DEFINE_REMOTE(207, pdu_window_ov_active, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_DEBUG, &_pdu_window_ov_active, "Whether the power channels is currently in over-voltage protection")
PARAM_DEFINE_REMOTE(208, pdu_ch_uptime, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_READONLY | PM_TELEM, &_pdu_ch_uptime, "Number of seconds the channel has been on (s)")
PARAM_DEFINE_REMOTE(209, pdu_ch_downtime, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_READONLY | PM_TELEM, &_pdu_ch_downtime, "Number of seconds the channel has been off (s)")
PARAM_DEFINE_REMOTE(210, pdu_ch_wdt_initial, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_CONF, &_pdu_ch_wdt_initial, "Initial value for watchdog timeout per power channel (s)")
PARAM_DEFINE_REMOTE(211, pdu_ch_wdt_in, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_CONF, &_pdu_ch_wdt_in, "Current value for the watchdog timeout countdown. Channel will power cycle when this reaches 0 (s)")
PARAM_DEFINE_REMOTE(212, pdu_ch_wdt_on_timer_cur, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_READONLY | PM_TELEM, &_pdu_ch_wdt_on_timer_cur, "on timer for the current power cycle")
PARAM_DEFINE_REMOTE(220, pdu_ch_latched_up_hw, &PDU, PARAM_TYPE_UINT8, PDUP4_CH_CNT, sizeof(uint8_t), PM_READONLY | PM_TELEM, &_pdu_ch_latched_up_hw, "Whether we suspect the channel HW of currently being locked up")
PARAM_DEFINE_REMOTE(225, pdu_ch_on_cnt, &PDU, PARAM_TYPE_UINT32, PDUP4_CH_CNT, sizeof(uint32_t), PM_READONLY | PM_TELEM, &_pdu_ch_on_cnt, "How many times a channel has been turned on. Particularly useful for deployment channels")

param_t* pdup4_params_a[PDUP4_PARAMS_COUNT] = {
    &pdu_gndwdt,
    &pdu_csp_can0_addr,
    &pdu_csp_can0_mask,
    &pdu_csp_print_cnf,
    &pdu_csp_can0_promisc,
    &pdu_csp_dedup,
    &pdu_boot_img1,
    &pdu_boot_img0,
    &pdu_boot_cur,
    &pdu_boot_cnt,
    &pdu_boot_err,
    &pdu_csp_can1_promisc,
    &pdu_mcu_temp,
    &pdu_csp_can1_addr,
    &pdu_csp_can1_mask,
    &pdu_csp_can0_isdfl,
    &pdu_csp_can1_isdfl,
    &pdu_ch_on,
    &pdu_ch_on_init,
    &pdu_ch_type,
    &pdu_ch_on_actual,
    &pdu_reboot_mode,
    &pdu_ch_oc_retry,
    &pdu_ch_latchup_time,
    &pdu_ch_oc_cnt,
    &pdu_ch_oc_cnt_hw,
    &pdu_ch_temp,
    &pdu_ch_voltage,
    &pdu_ch_current,
    &pdu_bus_voltage,
    &pdu_ch_curlim,
    &pdu_cross_kill,
    &pdu_master_mode,
    &pdu_master_mode_init,
    &pdu_window_uv,
    &pdu_window_ov,
    &pdu_window_hyst,
    &pdu_ch_on_in,
    &pdu_ch_off_in,
    &pdu_window_uv_active,
    &pdu_window_ov_active,
    &pdu_ch_uptime,
    &pdu_ch_downtime,
    &pdu_ch_wdt_initial,
    &pdu_ch_wdt_in,
    &pdu_ch_wdt_on_timer_cur,
    &pdu_ch_latched_up_hw,
    &pdu_ch_on_cnt
};
