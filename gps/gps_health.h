#ifndef INCLUDE_GPS_GPS_HEALTH_H
#define INCLUDE_GPS_GPS_HEALTH_H

namespace gps {    
// this is defined before GpsHealth, becaue GpsHealth calls it
#pragma pack(1)
struct ChronySourceInfo {
                uint8_t stratum = 0;
                uint8_t poll = 0;
                uint16_t reach = 0;
                uint32_t last_rx = 0;
                int64_t offset_adjusted_ns = -1;
                int64_t offset_measured_ns = -1;
                int64_t offset_std_ns = -1;
            };
struct GpsHealth {
        uint8_t percent_idle;
        uint8_t time_status;
        uint16_t ref_week_num;
        uint32_t gps_week_ms;
        uint32_t rcvr_status;
        int32_t solution_status;
        int32_t position_type;
        float diff_age;
        float solution_age;
        uint8_t number_satellites_tracked;
        uint8_t num_sats_used_in_solution;
        double x;
        double y;
        double z;
        float x_std_dev;
        float y_std_dev;
        float z_std_dev;
        int32_t v_sol_status;
        int32_t vel_type;
        double vel_x;
        double vel_y;
        double vel_z;
        uint32_t hwmonitor_arraylength;
        float temperature;
        uint32_t temp_status;
        float antenna_current;
        uint32_t antenna_current_status;
        float core_3v3_voltage;
        uint32_t core_3v3_status;
        float antenna_voltage;
        uint32_t antenna_voltage_status;
        float core_1v2_voltage;
        uint32_t core_1v2_status;
        float regulated_supply_voltage;
        uint32_t regulated_supply_voltage_status;
        float one_v_8;
        uint32_t one_v_8_status;
        float voltage_5v;
        uint32_t voltate_5v_status;
        float secondary_temp;
        uint32_t secondary_temp_status;
        float psr_pdop;
        float psr_tdop;
        float psr_cutoff;
        int32_t clock_model_status;
        double offset;
        double offset_std;
        double utc_offset;
        uint32_t utc_year;
        uint8_t utc_month;
        uint8_t utc_day;
        uint8_t utc_hour;
        uint8_t utc_minute;
        uint32_t utc_millisecond;
        int32_t utc_time_status;
        //chronyc sources
        ChronySourceInfo nmea_info;
        ChronySourceInfo pps_info;
    };
    #pragma pack()

}

#endif