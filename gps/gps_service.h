#ifndef INCLUDE_GPS_GPS_SERVICE_H
#define INCLUDE_GPS_GPS_SERVICE_H

// Novatel headers
#include "encoder.hpp"
#include "framer.hpp"
#include "header_decoder.hpp"
#include "message_decoder.hpp"

#include "novatel_common.hpp"
#include "novatel_enums.hpp"
#include "novatel_structs.hpp"

#include "socket.h"
#include "uart.h"
#include "udp.h"

#include "gps_health.h"
#include <chrono>

#include <future>
#include <gpiod.hpp>
#include <filesystem>

#pragma pack(1)

std::string ascii_checksum(std::string_view message);
namespace gps
{
    std::vector<char> create_log_command(int num, gps::MESSAGEFORMAT format);

    struct GpsServiceOptions {
        udp::SocketAddress send_socket;
        udp::SocketAddress nmea_socket;
        udp::SocketAddress pps_config;
    };

    //CmdOptions
    struct ServiceCmdOpts {
        bool debug = false;        // 'd' arg
        int baudRate = 115200;      // 'b' arg
        bool print = false;
    };
    ServiceCmdOpts parseGPSCmdOpts(int argc, char** argv);

    class GPSException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    enum class ReplyMessageState {
        // For "command ..."
        ExpectingCommandResponse,

        // For "log ..."
        ExpectingLogCommandResponse,
        ExpectingLog,
    };

    struct PendingUdpReply {
        udp::SocketAddress sender;
        novatel::edie::ENCODEFORMAT encode_format;
        ReplyMessageState reply_message_state;
    };

    // gps Service Class
    struct GpsService {
            std::shared_ptr<udp::Socket> socket_ptr;

            //gps power gpio set -- need to do this with new libgpiod
            void set_nreset(int dir);
            int nreset_dir;

            GpsServiceOptions gps_service_options;
            ServiceCmdOpts service_cmd_opts;
            JsonReader edie_json_reader;
            novatel::edie::oem::Framer edie_framer;
            novatel::edie::oem::HeaderDecoder edie_header_decoder;
            novatel::edie::MessageDecoder edie_message_decoder;
            novatel::edie::oem::Encoder edie_encoder;

            //cached gps health packets
            GpsHealth cached_health_packet {};
            ChronySourceInfo cached_pps_info{};
            ChronySourceInfo cached_NMEA_info{};
            std::chrono::system_clock::time_point packet_timer = std::chrono::system_clock::now();
            bool serial_connection_setup = false;
            bool health_packet_is_cached = false;
            bool chrony_packet_is_cached = false;
            std::future<std::map<std::string, gps::ChronySourceInfo>> chrony_future;

            // initialize counts for service testing and possible additional telemetry field
            int headerDecodeErr = 0;
            int mssgDecodeErr = 0;
            int mssgEncodeErr = 0;
            int cmdResponse = 0;
            int accepted_cmds = 0;
            int packetCt = 0;
            int recNmea = 0;

            //clean exit flag
            int cleanKillFlag = 0;

            std::unique_ptr<UART> uart_ptr;
            std::optional<PendingUdpReply> pending_udp_reply;

            std::string success_response(std::string const& reply_body);
            std::string error_response(std::string const& reply_body);

            GpsService(std::shared_ptr<udp::Socket> s, GpsServiceOptions gps_service_options, ServiceCmdOpts ServiceCmdOpts);

            void do_poll();
            bool gps_communicating = false;
            void receive_gps_data();
            void receive_packet();

            // For pulling the chronyc sources info into the health packet
            std::map<std::string, ChronySourceInfo> GetChronycSources();

            void update_health_packet(const gps::TIME& time_log, const gps::BESTXYZ& best_xyz, const gps::OEM4BinaryHeader& header_bestxyz, const gps::PSRDOP& psrdop, const gps::HWMONITOR& hwmonitor);
            void update_chrony_info(std::map<std::string, gps::ChronySourceInfo>* sources);
            void send_health_packet();
            void print_health_packet();

            void create_profiles();
            void activate_profiles();
            void configure_pps(udp::SocketAddress sender);


            void process_command(std::string const& command_string, udp::SocketAddress sender);
            
            void handle_nmea_sentence(const std::string& sentence);
            void handle_binary_message(novatel::edie::oem::MetaDataStruct &meta_data, novatel::edie::MessageDataStruct &message_data);
            void handle_other_message(novatel::edie::oem::MetaDataStruct &meta_data, novatel::edie::MessageDataStruct &message_data);
            bool health_packet_ready();
            void reset_flags();
            bool flag_time = false;
            bool flag_psrdop = false;
            bool flag_bestxyz = false;
            bool flag_hwmonitor = false;

            bool flag_pps = true;

            uint16_t health_packet_sequence_number = 0;
            gps::BESTXYZ best_xyz;
            gps::OEM4BinaryHeader header_bestxyz;
            gps::TIME time_log;
            gps::PSRDOP psrdop;
            gps::HWMONITOR hwmonitor;
    };
} // namespace gps

template<typename T>
T get_struct(std::span<unsigned char> message) {
    T t {};
    if (message.size() > sizeof(t)) {
        std::stringstream error_string;
        error_string << "Message length greater than size of destination struct.  Message length is " << message.size()
                << " but struct size is " << sizeof(t);
        throw gps::GPSException(error_string.str());
    }
    memcpy(&t, message.data(), message.size());
    return t;
}

#pragma pack()
#endif
