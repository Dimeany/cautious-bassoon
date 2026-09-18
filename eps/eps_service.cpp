// eps_service.cpp
// Created by Simeon Shaffar on 11/28/2023
//
// This is where the official EPS subsystem process will live
// The EPS subsystem process primarily exists to abstract away
// the details of talking to the EPS stack from the rest of the satellite.
// It also abstracts the details of what systems are powered by what channels


#include "eps_service.h"
#include "../util/status_util.h"
#include "version.h"

using namespace eps;
int main(int argc, char* argv[]) {
    CommandLineOptions opts = eps::parse_cmd_opts(argc, argv);

    cout << "Nebula version: " << NEBULA_VERSION << endl;
    cout << opts << endl;

    EpsService eps_service = EpsService(opts);
    std::chrono::_V2::steady_clock::time_point initial = steady_clock::now();
    
    while(true) {

        try {
            std::optional<std::pair<std::string, udp::SocketAddress>> opt_cmd = eps_service.listen_for_commands();

            // If a valid command is received
            if (opt_cmd.has_value()) {
                eps_service.process_command(*opt_cmd);
            }

            seconds duration = duration_cast<seconds>(steady_clock::now() - initial);

            // If the loop has been running longer than health_rate seconds since last health packet
            if (duration >= eps_service.get_health_rate()) {
                initial = steady_clock::now();
                
                eps_service.generate_health();
                eps_service.set_watchdogs();
            }

        } 
        catch (const exception& e) {
            cerr << "eps service exception caught: " << e.what() << endl;
        }
    }
}

EpsService::EpsService(CommandLineOptions cmd_opt) {
    opts = cmd_opt;
    socket = make_unique<Socket>(opts.socket_addr);

    socket->set_timeout(duration_cast<seconds>(opts.socket_timeout));

    start_csp();
}

void eps::usage() {
    CommandLineOptions opts;

    cerr << "EPS Service usage: " << endl;
    cerr << "   -h: Prints this help message" << endl;
    cerr << "   -r <rate>: Health packet generation rate in seconds"
                            << " (default: " << opts.health_rate<< ")" << endl;
    cerr << "   -t <timeout>: Timeout for the socket while listening for commands in seconds"
                            << " (default: " << opts.socket_timeout << ")" << endl;
    cerr << "   -p <port>: The port that the EPS Service will listen for commands on"
                            << " (default: " << opts.socket_addr.get_port() << ")" << endl;
    cerr << "   -d <ip:port>: The destination address and port you want the EPS service to send health packets to"
                            << " (default: " << opts.health_dest << ")" << endl;
    cerr << "   -u: Unsafe - EPS service won't block you from messing with the flight computer power"
                            << " (default: safe)" << endl;
    cerr << "   -c <count>: Attempt count for how many times the service will try before giving up on a command"
                            << " (default: " << opts.attempt_count << ")" << endl;
    cerr << "   -m <exact/impress>: The SSRL mission that is selected"
                            << " (default: impress)" << endl;
    cerr << "   -e: Quickly set the mission to exact"
                            << " (default: impress)" << endl;
    exit(1);
}

CommandLineOptions eps::parse_cmd_opts(int argc, char *argv[]) {
    CommandLineOptions options;

    int opt = 0;
    while ((opt = getopt(argc, argv, "hut:r:p:d:c:m:e")) != -1) {
        switch (opt) {
            case 'h':
                eps::usage();
                break;
            case 'r':
                options.health_rate = seconds(stoi(optarg));
                break;
            case 't':
                options.socket_timeout = seconds(stoi(optarg));
                break;
            case 'p':
                options.socket_addr = options.socket_addr.create("127.0.0.1", stoi(optarg));
                break;
            case 'd':
                options.health_dest = options.health_dest.create(optarg);
                break;
            case 'u':
                options.safe = false;
                break;
            case 'c':
                options.attempt_count = stoi(optarg);
                break;
            case 'm':
                // Using strcmp to compare cstrings, it returns 0 for success
                if (!strcmp(optarg, "exact")) options.mission = Mission::EXACT;
                else if (!strcmp(optarg, "impress")) options.mission = Mission::IMPRESS;
                else {
                    cerr << "Invalid Mission: " << optarg << endl;
                    eps::usage();
                }
                break;
            case 'e':
                options.mission = Mission::EXACT;
                break;
            default:
                eps::usage();
        }
    }

    return options;
} // parse_cmd_opts

std::ostream& operator<<(std::ostream& os, const CommandLineOptions opts) {
    os << "EPS Command Line Options:" << endl;
    os << "\tSafe: " << opts.safe << endl;
    os << "\tAttempt count: " << opts.attempt_count << endl;
    os << "\tHealth generation rate: " << opts.health_rate << endl;
    os << "\tSocket timeout: " << opts.socket_timeout << endl;
    os << "\tSocket address: " << opts.socket_addr << endl;
    os << "\tHealth destination: " << opts.health_dest << endl;

    os << "\tMission: ";
    switch(opts.mission) {
        case Mission::IMPRESS:
            os << "IMPRESS" << endl; break;
        case Mission::EXACT:
            os << "EXACT" << endl; break;
    }

    return os;
} // opts extraction operator overload


// listens for packets, returning the sender and command string if it gets something, otherwise it will return nullopt
optional<pair<string, SocketAddress>> EpsService::listen_for_commands() {
    try {
        SocketAddress sender;
        std::optional<std::vector<char>> opt_packet = socket->receive(sender);
        if (!opt_packet) { return std::nullopt; } // If the socket timed out.

        string command_string(opt_packet->data(), opt_packet->size());

        return pair<string, SocketAddress>{command_string, sender};
    } 
    catch (const SocketException& e) {
        cout << "EPS SocketException caught: " << e.what() << endl; 
        return nullopt;
    }
}

void EpsService::process_command(pair<string, SocketAddress> cmd) {
    string command_string = cmd.first;
    SocketAddress sender = cmd.second;

    cout << "command_string: \"" << command_string << "\"" << endl;

    try {

        if (command_string == "ping") {
            string message = "pong";
            return_to_sender(message, sender);
        }

        else if (command_string == "get_health") {
            command_send_health(sender);
        }

        else if (command_string.substr(0, 9) == "get_param") {
            command_get_param(sender, command_string);
        }

        else if (command_string.substr(0,9) == "set_param") {
            command_set_param(sender, command_string);
        }

        else if (command_string == "get_settings") {
            EpsPDUP4Settings settings = EpsPDUP4Settings();
            if (pdup4_get_settings(&settings))
            {
                return_to_sender(settingsToString(settings), sender);
            }
            else { return_to_sender("EPS error while trying to retrieve settings!", sender); }
        }

        else if (command_string == "get_info") {
            EpsPDUP4Info info = EpsPDUP4Info();
            if (pdup4_get_info(&info))
            {
                return_to_sender(infoToString(info), sender);
            }
            else { return_to_sender("EPS error while trying to retrieve info!", sender); }
        }

        else if (command_string == "too_many_params")
        {
            if (try_pull_too_many_params()) { return_to_sender("Succeeded in getting all params!", sender); }
            else { return_to_sender("EPS failed to pull all params!", sender); }
        }
        
        else throw EPSException("String command not recognized");
    }
    catch (const exception& e) {
        string message = "error\n" + string(e.what());
        cerr << message << endl;

        socket->send(message, sender);
    }
}


void EpsService::generate_health() {
    int result;

    // FOR IMPRESS
    if (opts.mission == Mission::IMPRESS) {
        EpsImpressHealth health_packet = {};

        for(int i = 1; i <= opts.attempt_count; i++) {
            result = build_impress_health_queue(&health_packet);
            if (result == 0) { break; }
        }

        util::Status status = (result < 0 ? util::Status::HARDWARE_ERROR : util::Status::OK);

        std::vector<char> packet = Packet::create_from_struct(
            health_packet,
            get_health_dest().get_port(),
            socket->bound_address.get_port(),
            cmd_seq_number++,
            status
        ).serialize();
        socket->send(packet, get_health_dest());

        if (result < 0) { throw EPSException("Driver failed to create health packet"); }
    } 

    // FOR EXACT
    else {
        EpsPDUP4Health health_packet = {};

        for(int i = 1; i <= opts.attempt_count; i++) {
            result = build_exact_health_queue(&health_packet); // For now we ignore the queue system because it was having issues
            if (result == 0) { break; }
        }

        util::Status status = (result < 0 ? util::Status::HARDWARE_ERROR : util::Status::OK);

        std::vector<char> packet = Packet::create_from_struct(
            health_packet,
            get_health_dest().get_port(),
            socket->bound_address.get_port(),
            cmd_seq_number++,
            status
        ).serialize();
        socket->send(packet, get_health_dest());

        if (result < 0) { throw EPSException("Driver failed to create health packet"); }
    }

}


void EpsService::set_watchdogs() {
    uint32_t watchdog_timeout = 300;

    int result = 0;

    // FOR IMPRESS
    if (opts.mission == Mission::IMPRESS) {
        for(int i = 1; i <= opts.attempt_count; i++) {
            result = push_parameter(impress_params_a[PCDU_GNDWDT], &watchdog_timeout, 0);
            if (result == 0) { break; }
        }
        if (result < 0) throw EPSException("Driver failed to set PCDU watchdog timer");

        for(int i = 1; i <= opts.attempt_count; i++) {
            result = push_parameter(impress_params_a[BATT_GNDWDT], &watchdog_timeout, 0);
            if (result == 0) { break; }
        }
        if (result < 0) throw EPSException("Driver failed to set BATT watchdog timer");

        for(int i = 1; i <= opts.attempt_count; i++) {
            result = push_parameter(impress_params_a[MPPT_GNDWDT], &watchdog_timeout, 0);
            if (result == 0) { break; }
        }
        if (result < 0) throw EPSException("Driver failed to set MPPT watchdog timer");
    }

    // FOR EXACT
    else {
        for(int i = 1; i <= opts.attempt_count; i++) {
            result = push_parameter(pdup4_params_a[P4_GNDWDT], &watchdog_timeout, 0);
            if (result == 0) { break; }
        }
        if (result < 0) throw EPSException("Driver failed to set PDU watchdog timer");
    }
}

// takes a message and sends it back to the command source
void EpsService::return_to_sender(string response, SocketAddress sender) {
    socket->send(response, sender);
}

// turns a string into a vector of strings separated by spaces
vector<string> EpsService::pull_out_command(string raw_message, uint8_t expected_count) {
    vector<string> parsed_command;

    istringstream iss(raw_message);
    string token;

    while(iss >> token) {
        parsed_command.push_back(token);
    }

    if (parsed_command.size() < expected_count) {
        throw EPSException("Incorrect number of command arguments: expected " + std::to_string(expected_count) + ", got " + std::to_string(parsed_command.size()));
    }

    return parsed_command;
}

// sends a human readable printout of the EPS health packet
void EpsService::command_send_health(SocketAddress sender) {
    int result;
    ostringstream os;

    // FOR IMPRESS
    if (opts.mission == Mission::IMPRESS) {
        EpsImpressHealth health_packet = {};

        for(int i = 1; i <= opts.attempt_count; i++) {
            result = build_impress_health_queue(&health_packet); 
            if (result == 0) { break; } // Break out of this loop if it was successful
        }
        if (result < 0) { throw EPSException("Driver failed to create health packet"); }

        os << "ack-ok\n" << health_packet;
    }

    // FOR EXACT
    else {
        EpsPDUP4Health health_packet = {};

        for(int i = 1; i <= opts.attempt_count; i++) {
            result = build_exact_health_queue(&health_packet); 
            if (result == 0) { break; } // Break out of this loop if it was successful
        }
        if (result < 0) { throw EPSException("Driver failed to create health packet"); }

        os << "ack-ok\n" << health_packet;
    };
        
    return_to_sender(os.str(), sender);
} // command_send_health


// goes into the driver to pull an arbitrary parameter
void EpsService::command_get_param(SocketAddress sender, string raw_command_string) {
    vector<string> parsed_command = pull_out_command(raw_command_string, 2);
    string param_name = parsed_command.at(1);

    // Initialize an offset if given, otherwise set it to -1
    int offset;
    if (parsed_command.size() == 3) {
        offset = stoi(parsed_command.at(2));
    } else offset = -1;
    
    const char* output;
    for(int i = 1; i <= opts.attempt_count; i++) {
        output = pull_parameter_string(param_name.c_str(), opts.mission, offset);
        if (output != NULL) { break; }     // Break out of this loop if it was successful or if it was a user issue
    }
    // If we reached the end of the attempts count and there is still an issue
    if (output == NULL) { throw EPSException("Serial communication error getting param: " + param_name); }
    if (string(output) == "") { throw EPSException("User error, offset out of bounds or no parameter for " + param_name); }
    
    string message;
    if (offset == -1) { message = "ack-ok\n" + param_name + " value: " + output; }
    else  { message = "ack-ok\n" + string(output); } // If they give a particular offset, assume there is bash script automation, simple output

    return_to_sender(message, sender);
}

// goes into the driver to set an arbitrary parameter
void EpsService::command_set_param(SocketAddress sender, string raw_command_string) {
    vector<string> parsed_command = pull_out_command(raw_command_string, 3);
    string param_name = parsed_command.at(1);
    string offset_str;
    string new_val_name;

    if (parsed_command.size() == 3) {
        offset_str = "-1"; // Driver throws error for "-1" offset on anything non-scalar
        new_val_name = parsed_command.at(2);
    } else {
        offset_str = parsed_command.at(2);
        new_val_name = parsed_command.at(3);
    }

    // Error handling for a few particular cases of concern
    if (opts.safe) {
        if (param_name == "pcdu_ch_on" && offset_str == "0") { throw EPSException("User error, you should not mess with flight computer power"); }
        if (param_name == "pcdu_dfl_on" && offset_str == "0") { throw EPSException("User error, you should not mess with flight computer power"); }
    }

    int result;
    for(int i = 1; i <= opts.attempt_count; i++) {
        result = push_parameter_string(param_name.c_str(), opts.mission, offset_str.c_str(), new_val_name.c_str());
        if (result != -1) { break; } // Break out of the loop if it was successful or had some user error
    }

    // If we reached the end of the attempts count and there is still an issue
    if (result == -1) { throw EPSException("Serial communication error setting param: " + param_name); }
    else if (result == -2) { throw EPSException("No valid parameter exists with name: " + param_name); }
    else if (result == -3) { throw EPSException("Invalid offset value for param: " + offset_str); }
    else if (result == -4) { throw EPSException("Couldn't cast param new value: " + new_val_name); }
    else if (result < 0) { throw EPSException("push_parameter_string failed on param: " + param_name); }

    string message = "ack-ok\nSuccessfully set " + param_name + "[" + offset_str + "] to " + new_val_name;
    return_to_sender(message, sender);
}
