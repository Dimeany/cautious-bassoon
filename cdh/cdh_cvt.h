#ifndef CURRENT_VALUE_TABLE_H
#define CURRENT_VALUE_TABLE_H

#include <unordered_map>
#include <variant>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include "cdh_health_packet.h"
#include "../gps/gps_health.h"
#include "../adcs/adcs_health.h"
#include "../eps/eps_health.h"
#include "DetectorMessages.hh"
#include "../util/status_util.h"

namespace std {
    template<typename T1, typename T2, typename T3>
    struct hash<tuple<T1, T2, T3>> {
        std::size_t operator()(const tuple<T1, T2, T3>& t) const {
            std::size_t h1 = std::hash<T1>{}(std::get<0>(t));
            std::size_t h2 = std::hash<T2>{}(std::get<1>(t));
            std::size_t h3 = std::hash<T3>{}(std::get<2>(t));
            return h1 ^ (h2 << 1) ^ (h3 << 1); // Combine hash values
        }
    };
} // namespace std

namespace cdh{
    struct Bounds {
        double min;
        double max;
    };

    using BoundsMap = std::map<std::string, std::map<std::string, Bounds>>;

    struct Field {
        std::variant<int, float, double, int8_t, uint8_t, int16_t, uint16_t, uint32_t> value;

        // lowerBound = 0, upperBound = -1 means no bounds
        double lowerBound = 0;
        double upperBound = -1;
        int counter = 0;

        std::function<void()> lowerAction = NULL;
        std::function<void()> upperAction = NULL;
    };

    template<typename T>
    class CurrentValueTable {
    public:
        void update(const T& packet, uint32_t packet_time, int status) {
            // Update the current values based on the received packet
            // This function needs to be specialized for each packet type
        }

        std::string printCurrentValues() const {
            std::ostringstream oss;
            for (const auto& [key, field] : fields) {
                std::visit([&oss, &key](auto&& val) {
                    oss << key << ": " << std::to_string(val) << std::endl;
                }, field.value);
            }
            return oss.str();
        }

        std::string printCurrentBounds() const {
            std::ostringstream oss;
            for (const auto& [key, field] : fields) {
                oss << key << ": min=" << field.lowerBound << ", max=" << field.upperBound << std::endl;
            }
            return oss.str();
        }

        std::string printFieldCounts() const {
            std::ostringstream oss;
            for (const auto& [key, field] : fields) {
                oss << key << ": " << field.counter << std::endl;
            }
            return oss.str();
        }

        std::string printFunctionalTest(std::string subsystem)
        {   
            std::ostringstream oss;

            const auto currentTime = std::chrono::system_clock::now();
            uint32_t currentSecondsUnix = std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count();
            uint32_t secondsSinceLastPacket = currentSecondsUnix - getUnixTime();
            int status = -1;

            oss << subsystem << " packet age: " << secondsSinceLastPacket << " seconds" << std::endl;
            if (fields.contains("status")) 
            { 
                status = std::get<int>(fields["status"].value);
                oss << "Status: " << status << " (" << util::intStatusToString(status) << ")" << std::endl; 
            }
            else { oss << "Subsystem doesn't have a status field." << std::endl; }

            if (secondsSinceLastPacket > 15) 
            { 
                oss << "FAILURE: Data too stale!" << std::endl; 
                return oss.str();
            }

            int successes = 0;
            int checked = 0;

            auto checkField = [&oss, &successes, &checked](const std::string& key, const Field& field) {
                std::visit([&oss, &key, &field, &successes](auto&& val) {
                    if (val < field.lowerBound) { oss << "FAILURE: " << key << ": " << std::to_string(val) << " LESS THAN BOUNDS (min=" << field.lowerBound << ")" << std::endl; }
                    else if (val > field.upperBound) { oss << "FAILURE: " << key << ": " << std::to_string(val) << " MORE THAN BOUNDS (max=" << field.upperBound << ")" << std::endl; }
                    else { oss << "Success: " << key << ": " << std::to_string(val) << std::endl; successes++; }
                }, field.value);

                checked++;
            };

            auto getAndCheckField = [&](std::string fieldName) {
                checkField(fieldName, fields[fieldName]);
            };

            if (std::is_same_v<T, eps::EpsPDUP4Health>)
            {
                getAndCheckField("pdu_mcu_temp");
                getAndCheckField("pdu_bus_voltage");

                for (int i = 0; i < 8; i++)
                {
                    std::string stringIndex = std::to_string(i);
                    if (std::get<uint8_t>(fields["pdu_ch_on_" + stringIndex].value) == 0) { oss << "pdu_ch " << i << " is off" << std::endl; }
                    else
                    {
                        getAndCheckField("pdu_ch_temp_" + stringIndex);
                        getAndCheckField("pdu_ch_voltage_" + stringIndex);
                        getAndCheckField("pdu_ch_current_" + stringIndex);
                    }
                }
            }
            else if (std::is_same_v<T, DetectorMessages::HealthPacket>) {
                auto checkDetectorChannel = [&](std::string detName) {
                    Field armTempField = fields[detName + "_arm_temp"];
                    if (std::get<double>(armTempField.value) == -273.15) { oss << "ch " << detName << " is off" << std::endl; }
                    else {
                        getAndCheckField(detName + "_arm_temp");
                        getAndCheckField(detName + "_sipm_temp");
                        getAndCheckField(detName + "_oper_voltage");
                    }
                };

                checkDetectorChannel("c1");
                checkDetectorChannel("m1");
                checkDetectorChannel("m5");
                checkDetectorChannel("x1");
            }
            else if (std::is_same_v<T, gps::GpsHealth>)
            {
                Field rcvr_status = fields["rcvr_status"];
                if (std::get<uint32_t>(rcvr_status.value) % 2 == 0) {
                    successes++;
                    oss << "Success: rcvr_status: no errors" << std::endl;
                }
                else { oss << "FAILURE: rcvr_status: error reported!" << std::endl; }
                checked++;

                getAndCheckField("number_satellites_tracked");
                getAndCheckField("temperature");
                getAndCheckField("secondary_temp");
            }
            else
            {
                for (const auto& [key, field] : fields) {
                    if (!(field.lowerBound == 0 && field.upperBound == -1)) { checkField(key, field); }
                }
            }

            bool failed = false;
            oss << std::endl << subsystem << " Successes: " << successes << "/" << checked << std::endl;
            if (successes != checked) { oss << "Failed to meet all bounds checks!" << std::endl; failed = true; }

            if (status == -1) { oss << subsystem << " reports no status, assuming OK" << std::endl; }
            else if (status != 0) { oss << subsystem << " reports error status!" << std::endl; failed = true; }

            oss << "\033[1;37m" << subsystem << " overall: " << (failed ? "\033[1;31mFAILED" : "\033[1;32mPassed") << "!\033[0m" << std::endl;

            return oss.str();
        }

        uint32_t getUnixTime() const {
            if (fields.find("unix_time") != fields.end()) {
                return std::get<uint32_t>(fields.at("unix_time").value);
            }
            return 0;
        }

        void addField(const std::string& subsystem, const std::string& key, const cdh::BoundsMap& boundsMap, double defaultLower, double defaultUpper, std::function<void()> lowerAction = NULL, std::function<void()> upperAction = NULL)
        {
            double lower = defaultLower;
            double upper = defaultUpper;

            cdh::BoundsMap::const_iterator boundsMapIterator = boundsMap.find(subsystem);
            if (boundsMapIterator != boundsMap.end())
            {
                std::map<std::string, cdh::Bounds>::const_iterator fieldIterator = boundsMapIterator->second.find(key);
                if (fieldIterator != boundsMapIterator->second.end())
                {
                    lower = fieldIterator->second.min;
                    upper = fieldIterator->second.max;
                }
            }

            // value, lowerBound, upperBound, counter, lowerAction, upperAction
            fields[key] = { -1, lower, upper, 0, lowerAction, upperAction };
        }
    private:
        void checkAllBounds()
        {
            if (fields.contains("status")) 
            { 
                int status = std::get<int>(fields["status"].value);
                if (status != util::Status::OK) { return; } // we dont care about checking bounds if there is some error in the header - error indicates the subsystem will put out bad telem
            }

            auto checkBoundsForField = [&](Field field) {
                std::visit([&field](auto&& val) {
                    if (val < field.lowerBound || val > field.upperBound) 
                    {   
                        if (val < field.lowerBound) { if (field.lowerAction != NULL) { field.lowerAction();} }
                        else { if (field.upperAction != NULL) { field.upperAction(); } }
                    } 
                }, field.value);
            };

            if (std::is_same_v<T, DetectorMessages::HealthPacket>) {
                 auto checkDetectorChannel = [&](std::string detName) {
                    Field armTempField = fields[detName + "_arm_temp"];
                    if (std::get<double>(armTempField.value) == -273.15) { return; }

                    Field sipmTempField = fields[detName + "_sipm_temp"];
                    Field voltField = fields[detName + "_oper_voltage"];

                    checkBoundsForField(armTempField);
                    checkBoundsForField(sipmTempField);
                    checkBoundsForField(voltField);
                };

                 checkDetectorChannel("c1");
                 checkDetectorChannel("m1");
                 checkDetectorChannel("m5");
                 checkDetectorChannel("x1");

                 return;
            }

            for (auto& [key, field] : fields) {
                if (!(field.lowerBound == 0 && field.upperBound == -1))
                {
                   checkBoundsForField(field); 
                }
            }
        }

        std::unordered_map<std::string, Field> fields;
    };

    template<>
    inline void cdh::CurrentValueTable<adcs::ADCSHealth>::update(const adcs::ADCSHealth& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;
        fields["MCUTemperature"].value = packet.MCUTemperature;
        fields["MagnetometerTemp"].value = packet.MagnetometerTemp;
        fields["XRateSensorTemp"].value = packet.XRateSensorTemp;
        fields["YRateSensorTemp"].value = packet.YRateSensorTemp;
        fields["ZRateSensorTemp"].value = packet.ZRateSensorTemp;
        fields["CubeControl3V3Current"].value = packet.CubeControl3V3Current;
        fields["CubeControl5VCurrent"].value = packet.CubeControl5VCurrent;
        fields["CubeControlVbatCurrent"].value = packet.CubeControlVbatCurrent;
        fields["Wheel1Current"].value = packet.Wheel1Current;
        fields["Wheel2Current"].value = packet.Wheel2Current;
        fields["Wheel3Current"].value = packet.Wheel3Current;

        checkAllBounds();
    }

    template<>
    inline void cdh::CurrentValueTable<eps::EpsImpressHealth>::update(const eps::EpsImpressHealth& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;
        fields["pcdu_temp"].value = packet.pcdu_temp;
        fields["pcdu_vbatt"].value = packet.pcdu_vbatt;
        fields["pcdu_ch_on_0"].value = packet.pcdu_ch_on[0];
        fields["pcdu_ch_on_1"].value = packet.pcdu_ch_on[1];
        fields["pcdu_ch_on_2"].value = packet.pcdu_ch_on[2];
        fields["pcdu_ch_on_3"].value = packet.pcdu_ch_on[3];
        fields["pcdu_ch_on_4"].value = packet.pcdu_ch_on[4];
        fields["pcdu_ch_on_5"].value = packet.pcdu_ch_on[5];
        fields["pcdu_ch_on_6"].value = packet.pcdu_ch_on[6];
        fields["pcdu_ch_on_7"].value = packet.pcdu_ch_on[7];
        fields["pcdu_ch_on_8"].value = packet.pcdu_ch_on[8];
        fields["pcdu_ch_on_9"].value = packet.pcdu_ch_on[9];
        fields["pcdu_ch_on_10"].value = packet.pcdu_ch_on[10];
        fields["pcdu_ch_on_11"].value = packet.pcdu_ch_on[11];
        fields["pcdu_icur_0"].value = packet.pcdu_icur[0];
        fields["pcdu_icur_1"].value = packet.pcdu_icur[1];
        fields["pcdu_icur_2"].value = packet.pcdu_icur[2];
        fields["pcdu_icur_3"].value = packet.pcdu_icur[3];
        fields["pcdu_icur_4"].value = packet.pcdu_icur[4];
        fields["pcdu_icur_5"].value = packet.pcdu_icur[5];
        fields["pcdu_icur_6"].value = packet.pcdu_icur[6];
        fields["pcdu_icur_7"].value = packet.pcdu_icur[7];
        fields["pcdu_icur_8"].value = packet.pcdu_icur[8];
        fields["pcdu_icur_9"].value = packet.pcdu_icur[9];
        fields["pcdu_icur_10"].value = packet.pcdu_icur[10];
        fields["pcdu_icur_11"].value = packet.pcdu_icur[11];
        fields["pcdu_vcur_0"].value = packet.pcdu_vcur[0];
        fields["pcdu_vcur_1"].value = packet.pcdu_vcur[1];
        fields["pcdu_vcur_2"].value = packet.pcdu_vcur[2];
        fields["pcdu_vcur_3"].value = packet.pcdu_vcur[3];
        fields["pcdu_vcur_4"].value = packet.pcdu_vcur[4];
        fields["pcdu_vcur_5"].value = packet.pcdu_vcur[5];
        fields["pcdu_vcur_6"].value = packet.pcdu_vcur[6];
        fields["pcdu_vcur_7"].value = packet.pcdu_vcur[7];
        fields["pcdu_vcur_8"].value = packet.pcdu_vcur[8];
        fields["pcdu_vcur_9"].value = packet.pcdu_vcur[9];
        fields["pcdu_vcur_10"].value = packet.pcdu_vcur[10];
        fields["pcdu_vcur_11"].value = packet.pcdu_vcur[11];
        fields["batt_heater_on"].value = packet.batt_heater_on ? 1 : 0;
        fields["batt_balance_cell"].value = packet.batt_balance_cell ? 1 : 0;
        fields["batt_temp"].value = packet.batt_temp;
        fields["batt_vin"].value = packet.batt_vin;
        fields["batt_vout"].value = packet.batt_vout;
        fields["batt_iin"].value = packet.batt_iin;
        fields["batt_iout"].value = packet.batt_iout;
        fields["mppt_temp"].value = packet.mppt_temp;
        fields["mppt_iin_0"].value = packet.mppt_iin[0];
        fields["mppt_iin_1"].value = packet.mppt_iin[1];
        fields["mppt_iin_2"].value = packet.mppt_iin[2];
        fields["mppt_iin_3"].value = packet.mppt_iin[3];
        fields["mppt_iin_4"].value = packet.mppt_iin[4];
        fields["mppt_iin_5"].value = packet.mppt_iin[5];
        fields["mppt_iin_6"].value = packet.mppt_iin[6];
        fields["mppt_vin_0"].value = packet.mppt_vin[0];
        fields["mppt_vin_1"].value = packet.mppt_vin[1];
        fields["mppt_vin_2"].value = packet.mppt_vin[2];
        fields["mppt_vin_3"].value = packet.mppt_vin[3];
        fields["mppt_vin_4"].value = packet.mppt_vin[4];
        fields["mppt_vin_5"].value = packet.mppt_vin[5];
        fields["mppt_vin_6"].value = packet.mppt_vin[6];
        
        checkAllBounds();
    }
    template<>
    inline void cdh::CurrentValueTable<eps::EpsPDUP4Health>::update(const eps::EpsPDUP4Health& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;
        fields["pdu_mcu_temp"].value = packet.pdu_mcu_temp;
        fields["pdu_ch_on_0"].value = packet.pdu_ch_on[0];
        fields["pdu_ch_on_1"].value = packet.pdu_ch_on[1];
        fields["pdu_ch_on_2"].value = packet.pdu_ch_on[2];
        fields["pdu_ch_on_3"].value = packet.pdu_ch_on[3];
        fields["pdu_ch_on_4"].value = packet.pdu_ch_on[4];
        fields["pdu_ch_on_5"].value = packet.pdu_ch_on[5];
        fields["pdu_ch_on_6"].value = packet.pdu_ch_on[6];
        fields["pdu_ch_on_7"].value = packet.pdu_ch_on[7];
        fields["pdu_ch_current_0"].value = packet.pdu_ch_current[0];
        fields["pdu_ch_current_1"].value = packet.pdu_ch_current[1];
        fields["pdu_ch_current_2"].value = packet.pdu_ch_current[2];
        fields["pdu_ch_current_3"].value = packet.pdu_ch_current[3];
        fields["pdu_ch_current_4"].value = packet.pdu_ch_current[4];
        fields["pdu_ch_current_5"].value = packet.pdu_ch_current[5];
        fields["pdu_ch_current_6"].value = packet.pdu_ch_current[6];
        fields["pdu_ch_current_7"].value = packet.pdu_ch_current[7];
        fields["pdu_ch_voltage_0"].value = packet.pdu_ch_voltage[0];
        fields["pdu_ch_voltage_1"].value = packet.pdu_ch_voltage[1];
        fields["pdu_ch_voltage_2"].value = packet.pdu_ch_voltage[2];
        fields["pdu_ch_voltage_3"].value = packet.pdu_ch_voltage[3];
        fields["pdu_ch_voltage_4"].value = packet.pdu_ch_voltage[4];
        fields["pdu_ch_voltage_5"].value = packet.pdu_ch_voltage[5];
        fields["pdu_ch_voltage_6"].value = packet.pdu_ch_voltage[6];
        fields["pdu_ch_voltage_7"].value = packet.pdu_ch_voltage[7];
        fields["pdu_ch_temp_0"].value = packet.pdu_ch_temp[0];
        fields["pdu_ch_temp_1"].value = packet.pdu_ch_temp[1];
        fields["pdu_ch_temp_2"].value = packet.pdu_ch_temp[2];
        fields["pdu_ch_temp_3"].value = packet.pdu_ch_temp[3];
        fields["pdu_ch_temp_4"].value = packet.pdu_ch_temp[4];
        fields["pdu_ch_temp_5"].value = packet.pdu_ch_temp[5];
        fields["pdu_ch_temp_6"].value = packet.pdu_ch_temp[6];
        fields["pdu_ch_temp_7"].value = packet.pdu_ch_temp[7];
        fields["pdu_bus_voltage"].value = packet.pdu_bus_voltage;

        checkAllBounds();
    }

    template<>
    inline void cdh::CurrentValueTable<gps::GpsHealth>::update(const gps::GpsHealth& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;
        fields["temperature"].value = packet.temperature;
        fields["antenna_current"].value = packet.antenna_current;
        fields["core_3v3_voltage"].value = packet.core_3v3_voltage;
        fields["antenna_voltage"].value = packet.antenna_voltage;
        fields["core_1v2_voltage"].value = packet.core_1v2_voltage;
        fields["regulated_supply_voltage"].value = packet.regulated_supply_voltage;
        fields["one_v_8"].value = packet.one_v_8;
        fields["voltage_5v"].value = packet.voltage_5v;
        fields["secondary_temp"].value = packet.secondary_temp;
        fields["number_satellites_tracked"].value = packet.number_satellites_tracked;
        fields["rcvr_status"].value = packet.rcvr_status; 
        
        checkAllBounds();
    }

    template<>
    inline void cdh::CurrentValueTable<DetectorMessages::HealthPacket>::update(const DetectorMessages::HealthPacket& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;

        // Lambda function to process temperatures
        auto process_value = [&](const auto& temp, const std::string& key, double divisor = 100.0, double offset = 273.15) {
            double value = static_cast<double>(temp) / divisor - offset;
            fields[key].value = value;
        };       

        // Process each temperature and voltage
        process_value(packet.c1.arm_temp, "c1_arm_temp");
        process_value(packet.c1.sipm_temp, "c1_sipm_temp");
        fields["c1_oper_voltage"].value = packet.c1.sipm_operating_voltage;

        process_value(packet.m1.arm_temp, "m1_arm_temp");
        process_value(packet.m1.sipm_temp, "m1_sipm_temp");
        fields["m1_oper_voltage"].value = packet.m1.sipm_operating_voltage;

        process_value(packet.m5.arm_temp, "m5_arm_temp");
        process_value(packet.m5.sipm_temp, "m5_sipm_temp");
        fields["m5_oper_voltage"].value = packet.m5.sipm_operating_voltage;

        process_value(packet.x1.arm_temp, "x1_arm_temp");
        process_value(packet.x1.sipm_temp, "x1_sipm_temp");
        fields["x1_oper_voltage"].value = packet.x1.sipm_operating_voltage;

        fields["x123_board_temp"].value = packet.x123.board_temp;

        process_value(packet.x123.det_temp, "x123_det_temp", 10.0);
        
        fields["x123_high_voltage"].value = packet.x123.det_high_voltage;

        checkAllBounds();
    }

    template<>
    inline void cdh::CurrentValueTable<cdh::CdhExactHealth>::update(const cdh::CdhExactHealth& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;
        fields["avionics_temp"].value = packet.avionics_temp;
        fields["flight_temp"].value = packet.flight_temp;
        fields["cpu_temp"].value = packet.cpu_temp;
        fields["sd_usage_percent"].value = packet.sd_usage_percent;
        
        checkAllBounds();
    }
    template<>
    inline void cdh::CurrentValueTable<cdh::CdhImpressHealth>::update(const cdh::CdhImpressHealth& packet, uint32_t packet_time, int status) {
        fields["status"].value = status;
        fields["unix_time"].value = packet_time;
        fields["avionics_temp"].value = packet.avionics_temp;
        fields["flight_temp"].value = packet.flight_temp;
        fields["igse_temp"].value = packet.igse_temp;
        fields["cpu_temp"].value = packet.cpu_temp;
        fields["sd_usage_percent"].value = packet.sd_usage_percent;
        
        checkAllBounds();
    }
    
    void setupCurrentValueTables();
    void setupCurrentValueTables(const BoundsMap& bounds);
    void highPcduTempAction();
    void lowPcduTempAction();
    void highMCUTemperatureAction();
    void lowMCUTemperatureAction();
    void highMagnetometerTempAction();
    void lowMagnetometerTempAction();
    void highXRateSensorTempAction();
    void lowXRateSensorTempAction();
    void highYRateSensorTempAction();
    void lowYRateSensorTempAction();
    void highZRateSensorTempAction();
    void lowZRateSensorTempAction();
    void highGpsTemperatureAction();
    void highGpsCurrentAction();
    void lowGpsTemperatureAction();
    void highSecondaryTempAction();
    void lowSecondaryTempAction();
    void highC1ArmTempAction();
    void lowC1ArmTempAction();
    void highC1SipmTempAction();
    void lowC1SipmTempAction();
    void highM1ArmTempAction();
    void lowM1ArmTempAction();
    void highM1SipmTempAction();
    void lowM1SipmTempAction();
    void highM5ArmTempAction();
    void lowM5ArmTempAction();
    void highM5SipmTempAction();
    void lowM5SipmTempAction();
    void highX1ArmTempAction();
    void lowX1ArmTempAction();
    void highX1SipmTempAction();
    void lowX1SipmTempAction();
    void lowC1OperVoltageAction();
    void highC1OperVoltageAction();
    void lowM1OperVoltageAction();
    void highM1OperVoltageAction();
    void lowM5OperVoltageAction();
    void highM5OperVoltageAction();
    void lowX1OperVoltageAction();
    void highX1OperVoltageAction();
    void highX123BoardTempAction();
    void lowX123BoardTempAction();
    void highX123DetTempAction();
    void lowX123DetTempAction();
    void highAvionicsTempAction();
    void lowAvionicsTempAction();
    void highFlightTempAction();
    void lowFlightTempAction();
    void highIgseTempAction();
    void lowIgseTempAction();
    void highCpuTempAction();
    void lowCpuTempAction();
    void highSdUsagePercentAction();
    void lowSdUsagePercentAction();
} // namespace cdh

#endif // CURRENT_VALUE_TABLE_H
