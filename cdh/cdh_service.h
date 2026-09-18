#ifndef CDH_SUBSYSTEM_PROCESS_HEADER
#define CDH_SUBSYSTEM_PROCESS_HEADER

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
    class CommandDataHandlingService {
        private:
            PCT2075TP avionics_sensor;
            PCT2075TP flight_sensor;
            PCT2075TP igse_sensor;
            // PCT2075TP burn_sensor;
            int epoll_fd = 0;
            std::shared_ptr<udp::Socket> command_socket;
            std::shared_ptr<udp::Socket> health_data_socket;
            std::vector<char> string_response(std::string message);
            std::vector<char> error_response(std::string message);

            // Cached health data
            cdh::CdhImpressHealth cached_cdh_health_impress{};
            eps::EpsImpressHealth cached_eps_health_impress{};
            adcs::ADCSHealth cached_adcs_health{};
            gps::GpsHealth cached_gps_health{};
            DetectorMessages::HealthPacket cached_det_health{};
            std::vector<char> cached_eps_packet;
            std::vector<char> cached_gps_packet;
            std::vector<char> cached_det_packet;

            std::vector<char> full_packet;

        public:
            CommandDataHandlingService(
                std::shared_ptr<udp::Socket> health_data_socket_,
                std::shared_ptr<udp::Socket> command_socket_
            );

            void ListenForPackets();
            void ReceiveCommandPacket();
            void ReceiveHealthDataPacket();

            std::vector<char> processPacket(std::string command_string);
            std::string GetTemps(std::string argument);
            float GetCpuTemp();
            std::tuple<float, float, int> GetSdCardUsage();
            void GetHealth();
            void writePacketToFile(const std::vector<char>& packet, const std::string& filePath);

            bool debug_mode = false;
            bool save_to_file = false;
            bool show_help = false;
    };

} // namespace cdh

#endif
