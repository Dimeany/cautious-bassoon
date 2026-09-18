#ifndef EXACT_CDH_SUBSYSTEM_PROCESS_HEADER
#define EXACT_CDH_SUBSYSTEM_PROCESS_HEADER

#include <iostream>
#include <unistd.h>
#include <string>
#include <random>
#include <poll.h>
#include <bits/stdc++.h>
#include <bitset>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <tuple>

#include "../gps/gps_health.h"
#include "../adcs/adcs_health.h"
#include "../eps/eps_health.h"
#include "packet.h"

// udp includes
#include "socket.h"

// adcs includes
#include "cube_space_types.h"
#include "struct_decode_operators.h"

// cdh includes
#include "cdh_health_packet.h"
#include "cdh_temp_sensor.h"
#include "cdh_cvt.h"
#include "cdh_cvt_setup.h"

// det includes
#include "DetectorMessages.hh"

#include "cdh_hs_packet.h"
#include "cdh_lrt_packet.h"

namespace cdh {
    // CDH Service Class
    class EXACT_CommandDataHandlingService {
        private:
            PCT2075TP avionics_sensor;
            PCT2075TP flight_sensor;
            int epoll_fd = 0;
            std::shared_ptr<udp::Socket> command_socket;
            std::shared_ptr<udp::Socket> health_data_socket;
            std::vector<char> string_response(std::string message);
            std::vector<char> error_response(std::string message);

            // Cached health data
            cdh::CdhExactHealth cached_cdh_health_exact{};
            eps::EpsPDUP4Health cached_eps_health_exact{};
            adcs::ADCSHealth cached_adcs_health{};
            gps::GpsHealth cached_gps_health{};
            DetectorMessages::HealthPacket cached_det_health{};
            std::vector<char> cached_eps_packet;
            std::vector<char> cached_gps_packet;
            std::vector<char> cached_det_packet;

            const size_t lrt_full_packet_size = 1256;
            std::vector<char> full_packet;
            std::chrono::steady_clock::time_point last_lrt_time;

            const uint32_t packet_staleness_threshold_sec = 15;

        public:
            EXACT_CommandDataHandlingService(
                std::shared_ptr<udp::Socket> health_data_socket_,
                std::shared_ptr<udp::Socket> command_socket_
            );

            void ListenForPackets();
            void ReceiveCommandPacket();
            void ReceiveHealthDataPacket();
            void InitializeLRTPacket();

            std::vector<char> processPacket(std::string command_string);
            std::string GetTemps(std::string argument);
            float GetCpuTemp();
            std::tuple<float, float, int> GetSdCardUsage();
            void GetHealth();
            void PrintHealthDebug(cdh::CdhExactHealth health);
            void SendHSPacket();
            void SendLRTPacket();
            void writePacketToFile(const std::vector<char>& packet, const std::string& filePath);

            bool debug_mode = false;
            bool save_to_file = false;
            bool show_help = false;
            bool dont_use_i2c = false;
    };

} // namespace cdh

#endif
