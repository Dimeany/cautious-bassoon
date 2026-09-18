#include "struct_operator_decodes.h"

namespace RXStatusBitMask{
    namespace ReceiverError {
        const char* bit_mask_string[] = {
            "Dynamic Random Access Memory", // 0
            "Invalid firmware", // 1
            "ROM status", // 2
            "Reserved", // 3
            "Electronic Serial Number (ESN) access status", // 4
            "Authorization code status", // 5
            "Reserved", // 6
            "Supply voltage status", // 7
            "Reserved", // 8
            "Temperature status (as compared against acceptable limits)", // 9
            "MINOS status", // 10
            "PLL RF status", // 11
            "Reserved", // 12
            "Reserved", // 13
            "Reserved", // 14
            "NVM status", // 15
            "Software resource limit exceeded", // 16
            "Model invalid for this receiver", // 17
            "Reserved", // 18
            "Reserved", // 19
            "Remote loading has begun", // 20
            "Export restriction", // 21
            "Safe Mode", // 22
            "Reserved", // 23
            "Reserved", // 24
            "Reserved", // 25
            "Reserved", // 26
            "Reserved", // 27
            "Reserved", // 28
            "Reserved", // 29
            "Reserved", // 30
            "Component hardware failure", // 31
            };
    }
    namespace ReceiverStatus {
        const char* bit_mask_string[] = {
            "Error flag", // 0
            "Temperature status", // 1
            "Voltage supply status", // 2
            "Primary antenna power status", // 3
            "LNA Failure", // 4
            "Primary antenna open circuit flag", // 5
            "Primary antenna short circuit flag", // 6
            "CPU overload flag", // 7
            "COM port transmit buffer overrun.", // 8
            "Spoofing Detection Status", // 9
            "Reserved", // 10
            "Link overrun flag", // 11
            "Input overrun flag", // 12
            "Aux transmit overrun flag", // 13
            "Antenna gain state", // 14
            "Jammer Detected", // 15
            "INS reset flag", // 16
            "IMU communication failure", // 17
            "GPS almanac flag/UTC known", // 18
            "Position solution flag", // 19
            "Position fixed flag", // 20
            "Clock steering status", // 21
            "Clock model flag", // 22
            "External oscillator locked flag", // 23
            "Software resource", // 24
            "Version bit 0", // 25
            "Version bit 1", // 26
            "Tracking mode", // 27
            "Digital Filtering Enabled", // 28
            "Auxiliary 3 status event flag", // 29
            "Auxiliary 2 status event flag", // 30
            "Auxiliary 1 status event flag", // 31
            };
    }
    namespace Aux1Status {
        const char* bit_mask_string[] = {
            "Jammer detected on RF1", // 0
            "Jammer detected on RF2", // 1
            "Jammer detected on RF3", // 2
            "Position averaging", // 3
            "Jammer detected on RF4", // 4
            "Jammer detected on RF5", // 5
            "Jammer detected on RF6", // 6
            "USB connection status", // 7
            "USB1 buffer overrun flag", // 8
            "USB2 buffer overrun flag", // 9
            "USB3 buffer overrun flag", // 10
            "Reserved", // 11
            "Profile Activation Bit", // 12
            "Throttled Ethernet Reception", // 13
            "Reserved", // 14
            "Reserved", // 15
            "Reserved", // 16
            "Reserved", // 17
            "Ethernet not connected", // 18
            "ICOM1 buffer overrun flag", // 19
            "ICOM2 buffer overrun flag", // 20
            "ICOM3 buffer overrun flag", // 21
            "NCOM1 buffer overrun flag", // 22
            "NCOM2 buffer overrun flag", // 23
            "NCOM3 buffer overrun flag", // 24
            "Reserved", // 25
            "Reserved", // 26
            "Reserved", // 27
            "Reserved", // 28
            "Reserved", // 29
            "Status error reported by the IMU. May be treated as a notice unless the issue persists.", // 30
            "IMU measurement outlier detected.", // 31
            };
    }
    namespace Aux2Status {
        const char* bit_mask_string[] = {
            "SPI Communication Failure", // 0
            "I2C Communication Failure", // 1
            "COM4 buffer overrun flag", // 2
            "COM5 buffer overrun flag", // 3
            "Reserved", // 4
            "Reserved", // 5
            "Reserved", // 6
            "Reserved", // 7
            "Reserved", // 8
            "COM1 buffer overrun flag", // 9
            "COM2 buffer overrun flag", // 10
            "COM3 buffer overrun flag", // 11
            "PLL RF1 unlock flag", // 12
            "PLL RF2 unlock flag", // 13
            "PLL RF3 unlock flag", // 14
            "PLL RF4 unlock flag", // 15
            "PLL RF5 unlock flag", // 16
            "PLL RF6 unlock flag", // 17
            "CCOM1 buffer overrun", // 18
            "CCOM2 buffer overrun", // 19
            "CCOM3 buffer overrun", // 20
            "CCOM4 buffer overrun", // 21
            "CCOM5 buffer overrun", // 22
            "CCOM6 buffer overrun", // 23
            "ICOM4 buffer overrun", // 24
            "ICOM5 buffer overrun", // 25
            "ICOM6 buffer overrun", // 26
            "ICOM7 buffer overrun", // 27
            "Secondary antenna power status", // 28
            "Secondary antenna open circuit", // 29
            "Secondary antenna short circuit", // 30
            "Reset loop detected", // 31
            };
    }
    namespace Aux3Status {
        const char* bit_mask_string[] = {
            "SCOM buffer overrun flag.", // 0
            "WCOM1 buffer overrun flag", // 1
            "FILE buffer overrun flag", // 2
            "Reserved", // 3
            "Antenna 1 Gain State bit 0", // 4
            "Antenna 1 Gain State bit 1", // 5
            "Antenna 2 Gain State bit 0", // 6
            "Antenna 2 Gain State bit 1", // 7
            "GPS reference time is incorrect. Reset the receiver to correct it", // 8
            "Reserved", // 9
            "Reserved", // 10
            "Reserved", // 11
            "Reserved", // 12
            "Reserved", // 13
            "Reserved", // 14
            "Reserved", // 15
            "DMI hardware failure detection flag", // 16
            "Reserved", // 17
            "Reserved", // 18
            "Reserved", // 19
            "Reserved", // 20
            "Reserved", // 21
            "Reserved", // 22
            "Reserved", // 23
            "Spoofing Calibration Status", // 24
            "Spoofing Calibration Required", // 25
            "Reserved", // 26
            "Reserved", // 27
            "Reserved", // 28
            "Web content is corrupt or does not exist", // 29
            "RF Calibration Data is present and in error", // 30
            "RF Calibration Data is present", // 31
            };
    }
   namespace Aux4Status {
        const char* bit_mask_string[] = {
            "less than 60 percent of available satellites are tracked well", // 0
            "less than 15 percent of available satellites are tracked well", // 1
            "Reserved", // 2
            "Reserved", // 3
            "Reserved", // 4
            "Reserved", // 5
            "Reserved", // 6
            "Reserved", // 7
            "Reserved", // 8
            "Reserved", // 9
            "Reserved", // 10
            "Reserved", // 11
            "Clock freewheeling due to bad position integrity", // 12
            "Reserved", // 13
            "less than 60 percent of expected corrections available RTK Corrections", // 14
            "less than 15 percent of expected corrections available RTK Corrections", // 15
            "Bad RTK Geometry", // 16
            "Reserved", // 17
            "Reserved", // 18
            "Long RTK Baseline Baseline >50 km", // 19
            "Poor RTK COM Link (poor correction quality) Corrections quality lteq 60 percent", // 20
            "Poor ALIGN COM Link (poor correction quality) Corrections quality lteq 60 percent", // 21
            "GLIDE Not Active", // 22
            "Bad PDP Geometry", // 23
            "No TerraStar Subscription", // 24
            "Reserved", // 25
            "Reserved", // 26
            "Reserved", // 27
            "Bad PPP Geometry", // 28
            "Reserved", // 29
            "No INS Alignment", // 30
            "INS not converged", // 31
            };
    }
}

std::ostream& operator<<(std::ostream& os, gps::BESTXYZ const& s){
   os << "BestXYZ:\n";
   os << "position_solution_status: " << s.solution_status << "\n";
   os << "position_type: " << s.position_type << "\n";
   os << "pos x: " << s.x << "\n";
   os << "pos y: " << s.y << "\n";
   os << "pos z: " << s.z << "\n";
   os << "pos x standard_deviation: " << s.x_std_dev << "\n";
   os << "pos y standard_deviation: " << s.y_std_dev << "\n";
   os << "pos z standard_deviation: " << s.z_std_dev << "\n";
   os << "velocity_solution_status: " << s.status << "\n";
   os << "velocity_type: " << s.type << "\n";
   os << "vel x: " << s.x_10 << "\n";
   os << "vel y: " << s.y_11 << "\n";
   os << "vel z: " << s.z_12 << "\n";
   os << "vel x standard_deviation: " << s.x_std_dev_13 << "\n";
   os << "vel y standard_deviation: " << s.y_std_dev_14 << "\n";
   os << "vel z standard_deviation: " << s.z_std_dev_15 << "\n";
   os << "velocity_latency: " << s.latency << "\n";
   os << "differential_age: " << s.diff_age << "\n";
   os << "solution_age: " << s.solution_age << "\n";
   os << "number_satellites_tracked: " << (int) s.num_svs << "\n";
   os << "number_satellites_in_solution: " << (int) s.num_soln_svs << "\n";
   os << "number_L1E1B1_signals_satellites_in_solution: " << (int) s.num_soln_L1_svs << "\n";
   os << "number_multi_frequency_satellites_in_solution: " << (int) s.num_soln_multi_svs << "\n";
   os << "extended_solution_status: " << (int) s.ext_sol_stat << "\n";
   os << "Galileo_and_BeiDou_signals_used_mask: " << (int) s.gal_and_bds_mask << "\n";
   os << "GPS_and_GLONASS_signals_used_mask: " << (int) s.gps_and_glo_mask << "\n";
   return os;
}

std::ostream &operator<<(std::ostream &os, gps::HWMONITOR const &s)
{
   os << "HWMonitor:\n";
   os << "number of measurements: " << s.measurements_arraylength << "\n";
   for (unsigned int i = 0; i < s.measurements_arraylength; i++){
      os << "measurement " << i + 1 << ":\n" 
         << "\tvalue: " << s.measurements[i].value << "\n"
         << "\tstatus: " << s.measurements[i].status << "\n";
   }
   return os;
}

#define VERBOSE_RXSTATUS false

std::ostream &operator<<(std::ostream &os, gps::RXSTATUS const &s)
{
   os << "RXStatus: \n";
   os << "number of stat masks: " << s.status_words_arraylength << "\n"; 
   os << "Receiver error:\n"; 
   for (unsigned int i = 0; i < 32; i++){
      if ( VERBOSE_RXSTATUS || (s.rx_error & (1 << i)) ){
         os << "bit " << i << " " << ((s.rx_error & (1 << i)) ? "set: " : "not set: ") << RXStatusBitMask::ReceiverError::bit_mask_string[i] << "\n";
      }
   }
   os << "Receiver Status:\n"; 
   if (s.status_words_arraylength >= 1) {
      uint32_t word = s.status_words[0].status_word;
      for (unsigned int i = 0; i < 32; i++){
         if ( VERBOSE_RXSTATUS || (word & (1 << i)) ){
            os << "bit " << i << " " << ((word & (1 << i)) ? "set: " : "not set: ") << RXStatusBitMask::ReceiverStatus::bit_mask_string[i] << "\n";
         }
      }
   }
   os << "Aux Status 1:\n"; 
   if (s.status_words_arraylength >= 2) {
      uint32_t word = s.status_words[1].status_word;
      for (unsigned int i = 0; i < 32; i++){
         if ( VERBOSE_RXSTATUS || (word & (1 << i)) ){
            os << "bit " << i << " " << ((word & (1 << i)) ? "set: " : "not set: ") << RXStatusBitMask::Aux1Status::bit_mask_string[i] << "\n";
         }
      }
   }
   os << "Aux Status 2:\n"; 
   if (s.status_words_arraylength >= 3) {
      uint32_t word = s.status_words[2].status_word;
      for (unsigned int i = 0; i < 32; i++){
         if ( VERBOSE_RXSTATUS || (word & (1 << i)) ){
            os << "bit " << i << " " << ((word & (1 << i)) ? "set: " : "not set: ") << RXStatusBitMask::Aux2Status::bit_mask_string[i] << "\n";
         }
      }
   }
   os << "Aux Status 3:\n"; 
   if (s.status_words_arraylength >= 4) {
      uint32_t word = s.status_words[3].status_word;
      for (unsigned int i = 0; i < 32; i++){
         if ( VERBOSE_RXSTATUS || (word & (1 << i)) ){
            os << "bit " << i << " " << ((word & (1 << i)) ? "set: " : "not set: ") << RXStatusBitMask::Aux3Status::bit_mask_string[i] << "\n";
         }
      }
   }
   os << "Aux Status 4:\n"; 
   if (s.status_words_arraylength >= 5) {
      uint32_t word = s.status_words[4].status_word;
      for (unsigned int i = 0; i < 32; i++){
         if ( VERBOSE_RXSTATUS || (word & (1 << i)) ){
            os << "bit " << i << " " << ((word & (1 << i)) ? "set: " : "not set: ") << RXStatusBitMask::Aux4Status::bit_mask_string[i] << "\n";
         }
      }
   }
   return os;
}

std::ostream &operator<<(std::ostream &os, gps::OEM4BinaryHeader const &s)
{
    os << "Binary Header\n";
    os << "sync byte 1: " << (int)s.ucSync1 << "\n";
    os << "sync byte 2: " << (int)s.ucSync2 << "\n";
    os << "sync byte 3: " << (int)s.ucSync3 << "\n";
    os << "header length: " << (int)s.ucHeaderLength << "\n";
    os << "message_id: " << s.usMsgNumber << "\n";
    os << "message_type: " << (int)s.ucMsgType << "\n";
    os << "port_address: " << (int)s.ucPort << "\n";
    os << "message_length: " << s.usLength << "\n";
    os << "sequence: " << s.usSequenceNumber << "\n";
    os << "idle_time: " << (int)s.ucIdleTime << "\n";
    os << "enum_time_status: " << (int)s.ucTimeStatus << "\n";
    os << "week: " << s.usWeekNo << "\n";
    os << "ms: " << s.uiWeekMSec << "\n";
    os << "receiver_status: " << s.uiStatus << "\n";
    os << "Message Def CRC: " << s.usMsgDefCRC << "\n";
    os << "receiver_version: " << s.usReceiverSWVersion;
    return os;
}