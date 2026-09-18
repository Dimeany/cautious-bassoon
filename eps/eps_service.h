// eps_service.h
// Written by Simeon Shaffar on 11/28/2023
// This file defines the service object that will be used to handle all EPS service functionality
#ifndef EPS_SUBSYSTEM_PROCESS_HEADER
#define EPS_SUBSYSTEM_PROCESS_HEADER

#include <iostream>
#include <chrono>
#include <string_view>
#include <memory>

#include "csp_driver.h"

#include "socket.h"
#include "udp.h"

#include "eps_health.h"
#include "eps_settings.h"
#include "eps_info.h"

#include "../util/environment_util.h"


using std::optional, std::nullopt, std::pair, std::vector;
using std::unique_ptr, std::make_unique;
using std::string, std::__cxx11::stoi, std::to_string;
using std::cout, std::cerr, std::endl, std::istringstream, std::ostringstream;
using std::exception;

using namespace udp;
using namespace std::chrono;
using namespace std::chrono_literals;


//This command tells c++ to pack the data so that it is more useful in binary
#pragma pack(1)

namespace eps {


    struct CommandLineOptions {
        Mission mission = Mission::IMPRESS; // 1 for impress 0 for exact
        bool safe = true;
        int attempt_count = 1;
        seconds health_rate = std::chrono::seconds(util::getenv_int("EPS_HP_DELAY_S", 10));
        seconds socket_timeout = 1s;
        SocketAddress socket_addr = SocketAddress::create("127.0.0.1", atoi(util::getenv_required("EPS_COMMAND_PORT")));
        SocketAddress health_dest = SocketAddress::create("127.0.0.1", atoi(util::getenv_required("EPS_UC_PORT")));
    };
    
    void usage(); 
    CommandLineOptions parse_cmd_opts(int argc, char *argv[]);

    class EpsService {
        private:
            CommandLineOptions opts;

            unique_ptr<Socket> socket;

            uint16_t health_seq_number = 0;
            uint16_t cmd_seq_number = 0;

        public:
            // Initializer
            EpsService(CommandLineOptions cmd_opt);

            // Getters
            seconds get_health_rate() { return this->opts.health_rate; }
            SocketAddress get_health_dest() { return this->opts.health_dest; }
        
            // Health packet generation
            void generate_health();
            void set_watchdogs();
            
            // Generalized command processing functions
            optional<pair<string, SocketAddress>> listen_for_commands();
            void process_command(pair<string, SocketAddress>);
            vector<string> pull_out_command(string, uint8_t);
            void return_to_sender(string response, SocketAddress sender);

            // Command functions
            void command_send_health(SocketAddress sender);
            void command_get_param(SocketAddress sender, string raw_command_string);
            void command_set_param(SocketAddress sender, string raw_command_string);
    };

    //Error handling stuff
    class EPSException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
};

std::ostream& operator<<(std::ostream& os, const eps::CommandLineOptions);

#pragma pack()

#endif
