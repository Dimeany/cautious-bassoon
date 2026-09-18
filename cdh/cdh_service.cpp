#include <chrono>
#include <sys/epoll.h>
#include <iomanip>
#include <sstream>

#include "../util/environment_util.h"
#include "../util/status_util.h"
#include "cdh_service.h"
#include "bounds_loader.h"
#include "version.h"

const udp::SocketAddress CDH_SERVICE_COMMAND_ADDRESS = udp::SocketAddress::create("0.0.0.0", atoi(util::getenv_required("CDH_COMMAND_PORT")));
const udp::SocketAddress CDH_UDPCAPTURE_ADDRESS = udp::SocketAddress::create("127.0.0.1", atoi(util::getenv_required("CDH_UDPCAPTURE_PORT")));
const udp::SocketAddress CDH_SERVICE_HEALTH_DATA_ADDRESS = udp::SocketAddress::create("0.0.0.0", atoi(util::getenv_required("CDH_HEALTH_DATA_PORT")));

using DetectorHealth = DetectorMessages::HealthPacket;
const uint16_t DET_PORT = atoi(util::getenv_required("DET_HEALTH_PORT"));

cdh::CurrentValueTable<adcs::ADCSHealth> cvt_adcs;
cdh::CurrentValueTable<eps::EpsImpressHealth> cvt_eps_impress;
cdh::CurrentValueTable<gps::GpsHealth> cvt_gps;
cdh::CurrentValueTable<DetectorHealth> cvt_det;
cdh::CurrentValueTable<cdh::CdhImpressHealth> cvt_cdh_impress;

const int HEALTH_PACKET_DELAY_SECONDS = util::getenv_int("CDH_HP_DELAY_S", 10);

int SEQUENCE_NUMBER = 0;

void print_help() {
    std::cout << "Usage: ./cdh_service [options]\n"
              << "Options:\n"
              << "  -d    Enable Debug Mode\n"
              << "  -f    Save output to file\n"
              << "  -h    Show this help message\n";
}

void fix_swapped_header(std::vector<char>& data);

int main(int argc, char* argv[]){
    // Load bounds from JSON (optional). Path may be overridden with HEALTH_BOUNDS_PATH env var.
    const char* env_path = std::getenv("HEALTH_BOUNDS_PATH");
    std::string bounds_path = env_path ? std::string(env_path) : std::string("/usr/local/bin/config/health_bounds.json");
    auto bounds_opt = loadBoundsFromJsonFile(bounds_path);
    if (bounds_opt) {
        cdh::setupCurrentValueTables(*bounds_opt);
    } else {
        // Fall back to built-in defaults
        cdh::setupCurrentValueTables();
    }

    auto command_socket = std::make_shared<udp::Socket>(CDH_SERVICE_COMMAND_ADDRESS);
    auto health_data_socket = std::make_shared<udp::Socket>(CDH_SERVICE_HEALTH_DATA_ADDRESS);
    auto initial = std::chrono::steady_clock::now();

    std::cout << "Nebula version: " << NEBULA_VERSION << std::endl;

    cdh::CommandDataHandlingService cdh_service = cdh::CommandDataHandlingService(
        std::move(command_socket),
        std::move(health_data_socket)
    );
    
    // Parse all arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.size() < 2 || arg[0] != '-') {
            std::cerr << "Invalid argument: " << arg << "\n";
            print_help();
            return 1;
        }

        for (size_t j = 1; j < arg.size(); ++j) {
            switch (arg[j]) {
                case 'd':
                    cdh_service.debug_mode = true;
                    break;
                case 'f':
                    cdh_service.save_to_file = true;
                    break;
                case 'h':
                    cdh_service.show_help = true;
                    break;
                default:
                    std::cerr << "Unknown flag: -" << arg[j] << "\n";
                    print_help();
                    return 1;
            }
        }
    }

    if (cdh_service.show_help) {
        print_help();
        return 0;
    }

    std::cout << "Running in Impress Mode\n";
    std::cout << "Debug Mode: " << (cdh_service.debug_mode ? "ON" : "OFF") << "\n";
    std::cout << "Save to File: " << (cdh_service.save_to_file ? "ON" : "OFF") << "\n";
    std::cout << "Health Packet Delay (Seconds): " << HEALTH_PACKET_DELAY_SECONDS << "\n";

    while (true) {
        try {
            std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - initial);

            //If the loop has been running longer than HEALTH_PACKET_DELAY_SECONDS since last temp sensor reading
            if (duration >= std::chrono::seconds(HEALTH_PACKET_DELAY_SECONDS)) {
                initial = std::chrono::steady_clock::now();

                cdh_service.GetHealth();           
            };
            cdh_service.ListenForPackets();
        }
        catch (const std::exception& e){
            std::cerr << "cdh service exception caught: " << e.what() << std::endl; 
        }
    }

    return 0;
}

void cdh::CommandDataHandlingService::ListenForPackets() {
    std::vector<struct epoll_event> events(2);

    // int ready_fds = epoll_wait(epoll_fd, events.data(), events.size(), options.udp_listening_timeout_sec * 1000);
    int ready_fds = epoll_wait(epoll_fd, events.data(), events.size(), 1000);
    if (ready_fds < 0) {
        std::cerr << "ListenForPackets: epoll_wait call failed: " << strerror(errno) << std::endl;

        if (errno == EINTR) {
            // This signal is not fatal, and occurs e.g. when strace
            // is attached
            return;
        }
        exit(1);
    }

    for (int i = 0; i < ready_fds; i++) {
        // Maybe this logic could be shorter if we stored some "data"
        // other than the file descriptor
        if (events[i].data.fd == command_socket->fd) {
            return ReceiveCommandPacket();
        }
        else if (events[i].data.fd == health_data_socket->fd) {
            return ReceiveHealthDataPacket();
        }
        else {
            std::cout << "Got epoll ready event for unknown file descriptor: " << events[i].data.fd << std::endl;
        }
    }
}

void cdh::CommandDataHandlingService::ReceiveCommandPacket() {
    try {
        udp::SocketAddress sender;
        auto opt_packet = command_socket->receive(sender);
        if (!opt_packet) {
            return;
        }

        auto command_string = std::string(opt_packet->data(), opt_packet->size());
        auto response = cdh::CommandDataHandlingService::processPacket(command_string);

        command_socket->send(response, sender);
    }
    catch (const udp::SocketException& e){
        std::cerr << "socket exception caught in ReceiveCommandPacket(): " << e.what() << std::endl;
    }
}

void cdh::CommandDataHandlingService::ReceiveHealthDataPacket() {
    adcs::ADCSHealth adcs_packet{};
    eps::EpsImpressHealth eps_packet_impress{};
    gps::GpsHealth gps_packet{};
    DetectorHealth det_packet{};

    try {
        udp::SocketAddress sender;
        std::optional<std::vector<char>> opt_packet = health_data_socket->receive(sender);
        if (!opt_packet) {
            // Read timed out.
            return;
        }
        // std::optional<udp::Packet> packet_opt;

        udp::Packet packet = udp::Packet::deserialize(*opt_packet);
        uint32_t packet_time = packet.header.time;
        uint8_t status = packet.header.status;
        uint16_t packet_source_port = packet.header.source_port;

        if (sender.get_port() == 53001) { // ADCS
            if (packet.payload.size() != sizeof(adcs_packet))
                return;
            memcpy(&adcs_packet, packet.payload.data(), packet.payload.size());
            cached_adcs_health = adcs_packet;
            cvt_adcs.update(adcs_packet, packet_time, status);
        } else if (sender.get_port() == 52001 || packet_source_port == 52001) { // EPS
            if (packet.payload.size() != sizeof(eps_packet_impress))
                return;
            memcpy(&eps_packet_impress, packet.payload.data(), packet.payload.size());
            cvt_eps_impress.update(eps_packet_impress, packet_time, status);

            std::vector<char> full_packet;
            full_packet.reserve(sizeof(packet.header) + packet.payload.size());

            // Copy header
            const char* hdr_ptr = reinterpret_cast<const char*>(&packet.header);
            full_packet.insert(full_packet.end(), hdr_ptr, hdr_ptr + sizeof(packet.header));

            // Copy payload
            full_packet.insert(full_packet.end(), packet.payload.begin(), packet.payload.end());

            // Cache it as-is for LRT forwarding
            cached_eps_packet = std::move(full_packet);
            cached_eps_health_impress = eps_packet_impress;

            std::vector<char> eps_corrected = cached_eps_packet;
            fix_swapped_header(eps_corrected);
            writePacketToFile(eps_corrected, "health_packets/eps_health_packet.bin");

            if (debug_mode) std::cout << "Received EPS health packet." << std::endl;
        } else if (sender.get_port() == 55001 || packet_source_port == 55000) { // GPS
            if (packet.payload.size() != sizeof(gps_packet))
                return;
            memcpy(&gps_packet, packet.payload.data(), packet.payload.size());
            cvt_gps.update(gps_packet, packet_time, status);

            std::vector<char> full_packet;
            full_packet.reserve(sizeof(packet.header) + packet.payload.size());

            // Copy header
            const char* hdr_ptr = reinterpret_cast<const char*>(&packet.header);
            full_packet.insert(full_packet.end(), hdr_ptr, hdr_ptr + sizeof(packet.header));

            // Copy payload
            full_packet.insert(full_packet.end(), packet.payload.begin(), packet.payload.end());

            // Cache it as-is for LRT forwarding
            cached_gps_packet = std::move(full_packet);
            cached_gps_health = gps_packet;

            std::vector<char> gps_corrected = cached_gps_packet;
            fix_swapped_header(gps_corrected);
            writePacketToFile(gps_corrected, "health_packets/gps_health_packet.bin");

            if (debug_mode) std::cout << "Received GPS health packet." << std::endl;
        } else if (sender.get_port() == DET_PORT || packet_source_port == 62009) { // DET
            if (packet.payload.size() != sizeof(det_packet))
                return;
            memcpy(&det_packet, packet.payload.data(), packet.payload.size());
            cvt_det.update(det_packet, packet_time, util::Status::UNKNOWN); // hardcoded because no detector rn

            std::vector<char> full_packet;
            full_packet.reserve(sizeof(packet.header) + packet.payload.size());

            // Copy header
            const char* hdr_ptr = reinterpret_cast<const char*>(&packet.header);
            full_packet.insert(full_packet.end(), hdr_ptr, hdr_ptr + sizeof(packet.header));

            // Copy payload
            full_packet.insert(full_packet.end(), packet.payload.begin(), packet.payload.end());

            // Cache it as-is for LRT forwarding
            cached_det_packet = std::move(full_packet);
            cached_det_health = det_packet;

            std::vector<char> det_corrected = cached_det_packet;
            fix_swapped_header(det_corrected);
            writePacketToFile(det_corrected, "health_packets/det_health_packet.bin");

            if (debug_mode) std::cout << "Received DET health packet." << std::endl;
        } else {
            std::cerr << "Received a health data packet from an unknown port: " << packet_source_port << std::endl;
        }
    }
    catch (const udp::SocketException& e){
        std::cerr << "socket exception caught in ReceiveHealthDataPacket(): " << e.what() << std::endl;
    }
}

// EXACT Version
cdh::CommandDataHandlingService::CommandDataHandlingService(
    std::shared_ptr<udp::Socket> command_socket_,
    std::shared_ptr<udp::Socket> health_data_socket_
)
    : avionics_sensor(TEMP_BUS, AVIONICS_ADDRESS),
      flight_sensor(TEMP_BUS, FLIGHT_ADDRESS),
      igse_sensor(TEMP_BUS, IGSE_ADDRESS) {
    command_socket = command_socket_;
    health_data_socket = health_data_socket_;

    epoll_fd = epoll_create(1 /* this argument is ignored (see: man 2 epoll_create) */);
    if (epoll_fd == -1) {
        std::cerr << "Could not create epoll file descriptor: " << strerror(errno) << std::endl;
        exit(1);
    }

    struct epoll_event event {};
    event.events = EPOLLIN;

    event.data.fd = command_socket->fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, command_socket->fd, &event)) {
        std::cerr << "epoll_ctl returned nonzero for command socket: " << strerror(errno) << std::endl;
        exit(1);
    }

    event.data.fd = health_data_socket->fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, health_data_socket->fd, &event)) {
        std::cerr << "epoll_ctl returned nonzero for health data socket: " << strerror(errno) << std::endl;
        exit(1);
    }
}

void cdh::CommandDataHandlingService::GetHealth() {
    // Collect CDH health data
    auto [used, available, usage_percent] = GetSdCardUsage();
    cdh::CdhImpressHealth cdh_packet{
        .avionics_temp = avionics_sensor.readTemperature(),
        .flight_temp = flight_sensor.readTemperature(),
        .igse_temp = igse_sensor.readTemperature(),
        .cpu_temp = GetCpuTemp(),
        .sd_used_gb = used,
        .sd_avail_gb = available,
        .sd_usage_percent = usage_percent
    };
    
    // Update CVT for CDH health
    cvt_cdh_impress.update(cdh_packet, std::time(nullptr), util::Status::OK);
    cached_cdh_health_impress = cdh_packet;

    auto packet = udp::Packet::create_from_struct(
        cdh_packet,
        CDH_UDPCAPTURE_ADDRESS.get_port(),
        CDH_SERVICE_COMMAND_ADDRESS.get_port(),
        SEQUENCE_NUMBER++,
        util::Status::OK
    ).serialize();
    command_socket->send(packet, CDH_UDPCAPTURE_ADDRESS);
    writePacketToFile(packet, "health_packets/cdh_health_packet.bin");
}

void fix_swapped_header(std::vector<char>& data) {
    if (data.size() < 12) {
        return;
    }

    // Swap each of the 4 uint16_t fields (bytes 0–7)
    for (size_t i = 0; i < 8; i += 2) {
        std::swap(data[i], data[i + 1]);
    }

    // Swap the uint32_t field (bytes 8–11)
    std::swap(data[8], data[11]);
    std::swap(data[9], data[10]);
}

void cdh::CommandDataHandlingService::writePacketToFile(const std::vector<char>& packet, const std::string& filePath) {
    if (!save_to_file) {
        return;
    }

    namespace fs = std::filesystem;

    // Convert to filesystem path
    fs::path pathObj(filePath);

    // Ensure the parent directory exists
    if (pathObj.has_parent_path()) {
        fs::create_directories(pathObj.parent_path());
    }

    std::ofstream outFile(filePath, std::ios::out);
    if (!outFile) {
        std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
        return;
    }

    // Convert each byte to hex
    for (unsigned char byte : packet) {
        outFile << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(byte);
    }

    outFile << std::endl;
    outFile.close();

    if (debug_mode) std::cout << "File written successfully: " << filePath << std::endl;
}

std::string cdh::CommandDataHandlingService::GetTemps(std::string argument) {
    std::ostringstream oss;
    auto process_sensor = [](bool &read_flag, float temperature) -> std::string {
        if (read_flag) {
            return "unavailable\n";
        } else {
            return std::to_string(temperature) + " °C\n";
        }
    };    
    
    if (argument == "all_sensor") 
    {   
        float avionics_temp = avionics_sensor.readTemperature();
        float flight_temp = flight_sensor.readTemperature();
        float igse_temp = igse_sensor.readTemperature();     

        std::string avionics_str = process_sensor(avionics_sensor.read_flag, avionics_temp);
        std::string flight_str = process_sensor(flight_sensor.read_flag, flight_temp);
        std::string igse_str = process_sensor(igse_sensor.read_flag, igse_temp);

        oss << "Avionics Temperature: " << avionics_str
            << "Flight Computer Temperature: " << flight_str
            << "IGSE Temperature: " << igse_str << "\n";
        return oss.str();
    }
    else if (argument == "avionics_sensor") {
        float avionics_temp = avionics_sensor.readTemperature();
        std::string avionics_str = process_sensor(avionics_sensor.read_flag, avionics_temp);
        oss << "Avionics Temperature: " << avionics_str << "\n";
        return oss.str();
    }
    else if (argument == "flight_sensor") {
        float flight_temp = flight_sensor.readTemperature();
        std::string flight_str = process_sensor(flight_sensor.read_flag, flight_temp);
        oss << "Flight Computer Temperature: " << flight_str << "\n";
        return oss.str();
    } 
    else if (argument == "igse_sensor") {
        float igse_temp = igse_sensor.readTemperature();
        std::string igse_str = process_sensor(igse_sensor.read_flag, igse_temp);
        oss << "IGSE Temperature: " << igse_str << "\n";
        return oss.str();
    } 
    else {
        oss << "Invalid measurement type.";
        return oss.str();
    }
}

std::vector<char> cdh::CommandDataHandlingService::string_response(std::string message) {
    message = "ack-ok\n" + message;
    std::vector<char> response(message.size());
    memcpy(response.data(), message.data(), message.size());
    return response;
}

std::vector<char> cdh::CommandDataHandlingService::error_response(std::string message) {
    message = "error\n" + message;
    std::vector<char> response(message.size());
    memcpy(response.data(), message.data(), message.size());
    return response;
}

std::vector<char> cdh::CommandDataHandlingService::processPacket(std::string command_string) {
    try {

        if (command_string.starts_with("measure")) {
            // Extract the part after "measure"
            std::string argument = command_string.substr(8); // Extract the part after "measure "

            // Remove any leading or trailing whitespace
            argument.erase(argument.find_last_not_of(" \n\r\t") + 1);
            argument.erase(0, argument.find_first_not_of(" \n\r\t"));

            std::string response;

            response = cdh::CommandDataHandlingService::GetTemps(argument);
            
            if (response == "Invalid measurement type.") {
                return cdh::CommandDataHandlingService::error_response(response);
            }
            return cdh::CommandDataHandlingService::string_response(response);     
        }        

        if (command_string.starts_with("get_counters"))
        {
            std::string argument = command_string.substr(13); // Extract the part after "get_counters "
            std::string response;

            // Remove any leading or trailing whitespace
            argument.erase(argument.find_last_not_of(" \n\r\t") + 1);
            argument.erase(0, argument.find_first_not_of(" \n\r\t"));

            if (argument == "eps") 
            {
                response = cvt_eps_impress.printFieldCounts();
            }
            else if (argument == "adcs") 
            {
                response = cvt_adcs.printFieldCounts();
            } 
            else if (argument == "gps") 
            {
                response = cvt_gps.printFieldCounts();
            } 
            else if (argument == "det") 
            {
                response = cvt_det.printFieldCounts();
            }
            else if (argument == "cdh") 
            {
                response = cvt_cdh_impress.printFieldCounts();
            } 
            else 
            {
                response = "Invalid argument for get_counters.";
                return cdh::CommandDataHandlingService::error_response(response);
            }
            return cdh::CommandDataHandlingService::string_response(response);        
        }


        if (command_string.starts_with("get_values"))
        {
            std::string argument = command_string.substr(11); // Extract the part after "get_counters "
            std::string response;

            // Remove any leading or trailing whitespace
            argument.erase(argument.find_last_not_of(" \n\r\t") + 1);
            argument.erase(0, argument.find_first_not_of(" \n\r\t"));

            if (argument == "eps") 
            {
                response = cvt_eps_impress.printCurrentValues();
            }
            else if (argument == "adcs") 
            {
                response = cvt_adcs.printCurrentValues();
            } 
            else if (argument == "gps") 
            {
                response = cvt_gps.printCurrentValues();
            } 
            else if (argument == "det") 
            {
                response = cvt_det.printCurrentValues();
            }
            else if (argument == "cdh") 
            {
                response = cvt_cdh_impress.printCurrentValues();
            }
            else 
            {
            response = "Invalid argument for get_values.";
            return cdh::CommandDataHandlingService::error_response(response);
            }
            return cdh::CommandDataHandlingService::string_response(response);        
        }
        
        if (command_string.starts_with("get_bounds"))
        {
            std::string argument = command_string.substr(11); // Extract the part after "get_bounds "
            std::string response;

            // Remove any leading or trailing whitespace
            argument.erase(argument.find_last_not_of(" \n\r\t") + 1);
            argument.erase(0, argument.find_first_not_of(" \n\r\t"));

            if (argument == "eps") 
            {
                response = cvt_eps_impress.printCurrentBounds();
            }
            else if (argument == "adcs") 
            {
                response = cvt_adcs.printCurrentBounds();
            } 
            else if (argument == "gps") 
            {
                response = cvt_gps.printCurrentBounds();
            } 
            else if (argument == "det") 
            {
                response = cvt_det.printCurrentBounds();
            }
            else if (argument == "cdh") 
            {
                response = cvt_cdh_impress.printCurrentBounds();
            }
            else 
            {
                response = "Invalid argument for get_bounds.";
                return cdh::CommandDataHandlingService::error_response(response);
            }
            return cdh::CommandDataHandlingService::string_response(response);        
        }

        return cdh::CommandDataHandlingService::error_response("Unknown command");
    }
    catch (std::exception const& e) {
        // Error occurred handling packet.
        std::stringstream response_message_ss;
        response_message_ss << "Error occurred handling packet: " << e.what() << std::endl;
        return error_response(response_message_ss.str());
    }
}

// Functions that send a command to be executed by the system
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

float cdh::CommandDataHandlingService::GetCpuTemp() {
    std::string result = exec("vcgencmd measure_temp");
    std::string temp_str = result.substr(result.find('=') + 1, result.find('\'') - result.find('=') - 1);
    return std::stof(temp_str);
}

std::tuple<float, float, int> cdh::CommandDataHandlingService::GetSdCardUsage() {
    std::string result = exec("df -h /");
    std::istringstream iss(result);
    std::string line;
    std::getline(iss, line); // Skip the header line
    std::getline(iss, line); // Read the second line with the actual data

    // Parse the second line to extract the memory usage details
    std::istringstream iss_line(line);
    std::string filesystem, size, used, available, percent, mountpoint;
    iss_line >> filesystem >> size >> used >> available >> percent >> mountpoint;

    // Remove the '%' character from the percent string
    percent.pop_back();

    // Convert strings to numeric values
    float used_gb = std::stof(used);
    float available_gb = std::stof(available);
    int usage_percent = std::stoi(percent);

    return std::make_tuple(used_gb, available_gb, usage_percent);
}

void cdh::setupCurrentValueTables(const cdh::BoundsMap& bounds) {
    cvt_adcs.addField("ADCS", "MCUTemperature", bounds, -10.0, 60.0, lowMCUTemperatureAction, highMCUTemperatureAction);
    cvt_adcs.addField("ADCS", "MagnetometerTemp", bounds, -10.0, 60.0, lowMagnetometerTempAction, highMagnetometerTempAction);
    cvt_adcs.addField("ADCS", "XRateSensorTemp", bounds, -10.0, 60.0, lowXRateSensorTempAction, highXRateSensorTempAction); 
    cvt_adcs.addField("ADCS", "YRateSensorTemp", bounds, -10.0, 60.0, lowYRateSensorTempAction, highYRateSensorTempAction);
    cvt_adcs.addField("ADCS", "ZRateSensorTemp", bounds, -10.0, 60.0, lowZRateSensorTempAction, highZRateSensorTempAction);

    cvt_eps_impress.addField("EPS", "pcdu_temp", bounds, 0.0, 4000.0, lowPcduTempAction, highPcduTempAction);

    cvt_gps.addField("GPS", "temperature", bounds, -40.0, 85.0, lowGpsTemperatureAction, highGpsTemperatureAction);
    cvt_gps.addField("GPS", "secondary_temp", bounds, -40.0, 85.0, lowSecondaryTempAction, highSecondaryTempAction);

    cvt_det.addField("DET", "c1_arm_temp", bounds, -40.0, 60.0, lowC1ArmTempAction, highC1ArmTempAction);
    cvt_det.addField("DET", "c1_sipm_temp", bounds, -40.0, 60.0, lowC1SipmTempAction, highC1SipmTempAction);
    cvt_det.addField("DET", "m1_arm_temp", bounds, -40.0, 60.0, lowM1ArmTempAction, highM1ArmTempAction);
    cvt_det.addField("DET", "m1_sipm_temp", bounds, -40.0, 60.0, lowM1SipmTempAction, highM1SipmTempAction);
    cvt_det.addField("DET", "m5_arm_temp", bounds, -40.0, 60.0, lowM5ArmTempAction, highM5ArmTempAction);
    cvt_det.addField("DET", "m5_sipm_temp", bounds, -40.0, 60.0, lowM5SipmTempAction, highM5SipmTempAction);
    cvt_det.addField("DET", "x1_arm_temp", bounds, -40.0, 60.0, lowX1ArmTempAction, highX1ArmTempAction);
    cvt_det.addField("DET", "x1_sipm_temp", bounds, -40.0, 60.0, lowX1SipmTempAction, highX1SipmTempAction);
    cvt_det.addField("DET", "x123_board_temp", bounds, -30.0, 80.0, lowX123BoardTempAction, highX123BoardTempAction);
    cvt_det.addField("DET", "x123_det_temp", bounds, -30.0, 80.0, lowX123DetTempAction, highX123DetTempAction);

    cvt_cdh_impress.addField("CDH", "avionics_temp", bounds, -40.0, 35.0, lowAvionicsTempAction, highAvionicsTempAction);
    cvt_cdh_impress.addField("CDH", "flight_temp", bounds, 0.0, 70.0, lowFlightTempAction, highFlightTempAction);
    cvt_cdh_impress.addField("CDH", "igse_temp", bounds, -30.0, 60.0, lowIgseTempAction, highIgseTempAction);
    cvt_cdh_impress.addField("CDH", "cpu_temp", bounds, -25.0, 80.0, lowCpuTempAction, highCpuTempAction);
    cvt_cdh_impress.addField("CDH", "sd_usage_percent", bounds, 0, 80.0, NULL, highSdUsagePercentAction);
}

void cdh::setupCurrentValueTables() {
    cdh::BoundsMap empty;
    setupCurrentValueTables(empty);
}

void cdh::highPcduTempAction() {
    std::cout << "Warning: PCDU temperature is too high!" << std::endl;
}

void cdh::lowPcduTempAction() {
    std::cout << "Warning: PCDU temperature is too low!" << std::endl;
}

void cdh::highMCUTemperatureAction() {
    std::cout << "Warning: MCU temperature is too high!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowMCUTemperatureAction() {
    std::cout << "Warning: MCU temperature is too low!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highMagnetometerTempAction() {
    std::cout << "Warning: Magnetometer temperature is too high!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowMagnetometerTempAction() {
    std::cout << "Warning: Magnetometer temperature is too low!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highXRateSensorTempAction() {
    std::cout << "Warning: X Rate Sensor temperature is too high!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowXRateSensorTempAction() {
    std::cout << "Warning: X Rate Sensor temperature is too low!" << std::endl;
}

void cdh::highYRateSensorTempAction() {
    std::cout << "Warning: Y Rate Sensor temperature is too high!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowYRateSensorTempAction() {
    std::cout << "Warning: Y Rate Sensor temperature is too low!" << std::endl;
}

void cdh::highZRateSensorTempAction() {
    std::cout << "Warning: Z Rate Sensor temperature is too high!" << std::endl;
    std::cout << "Shutting down ADCS!" << std::endl;
    std::string result = exec("eps_adcs_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowZRateSensorTempAction() {
    std::cout << "Warning: Z Rate Sensor temperature is too low!" << std::endl;
}

void cdh::highGpsTemperatureAction() {
    std::cout << "Warning: GPS temperature is too high!" << std::endl;
    std::cout << "Shutting down GPS!" << std::endl;
    std::string result = exec("eps_gps_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowGpsTemperatureAction() {
    std::cout << "Warning: GPS temperature is too low!" << std::endl;
    std::cout << "Shutting down GPS!" << std::endl;
    std::string result = exec("eps_gps_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highSecondaryTempAction() {
    std::cout << "Warning: GPS Secondary temperature is too high!" << std::endl;
    std::cout << "Shutting down GPS!" << std::endl;
    std::string result = exec("eps_gps_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowSecondaryTempAction() {
    std::cout << "Warning: GPS Secondary temperature is too low!" << std::endl;
    std::cout << "Shutting down GPS!" << std::endl;
    std::string result = exec("eps_gps_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highC1ArmTempAction() {
    std::cout << "Warning: C1 Arm temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowC1ArmTempAction() {
    std::cout << "Warning: C1 Arm temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highC1SipmTempAction() {
    std::cout << "Warning: C1 SiPM temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowC1SipmTempAction() {
    std::cout << "Warning: C1 SiPM temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highM1ArmTempAction() {
    std::cout << "Warning: M1 Arm temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowM1ArmTempAction() {
    std::cout << "Warning: M1 Arm temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highM1SipmTempAction() {
    std::cout << "Warning: M1 SiPM temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowM1SipmTempAction() {
    std::cout << "Warning: M1 SiPM temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highM5ArmTempAction() {
    std::cout << "Warning: M5 Arm temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowM5ArmTempAction() {
    std::cout << "Warning: M5 Arm temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highM5SipmTempAction() {
    std::cout << "Warning: M5 SiPM temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowM5SipmTempAction() {
    std::cout << "Warning: M5 SiPM temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highX1ArmTempAction() {
    std::cout << "Warning: X1 Arm temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowX1ArmTempAction() {
    std::cout << "Warning: X1 Arm temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highX1SipmTempAction() {
    std::cout << "Warning: X1 SiPM temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowX1SipmTempAction() {
    std::cout << "Warning: X1 SiPM temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highX123BoardTempAction() {
    std::cout << "Warning: X123 Board temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowX123BoardTempAction() {
    std::cout << "Warning: X123 Board temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highX123DetTempAction() {
    std::cout << "Warning: X123 Detector temperature is too high!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::lowX123DetTempAction() {
    std::cout << "Warning: X123 Detector temperature is too low!" << std::endl;
    // std::cout << "Shutting down Detector!" << std::endl;
    // std::string result = exec("eps_det_off;");
    // std::cout << result << std::endl;
    // std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highAvionicsTempAction() {
    std::cout << "Warning: Avionics temperature is too high!" << std::endl;
    std::cout << "Shutting down Avionics!" << std::endl;
    std::string result = exec("eps_gps_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;

}

void cdh::lowAvionicsTempAction() {
    std::cout << "Warning: Avionics temperature is too low!" << std::endl;
    std::cout << "Shutting down Avionics!" << std::endl;
    std::string result = exec("eps_gps_off;");
    std::cout << result << std::endl;
    std::cout << "Operator commanded restart required!" << std::endl;
}

void cdh::highFlightTempAction() {
    std::cout << "Warning: Flight temperature is too high!" << std::endl;
}

void cdh::lowFlightTempAction() {
    std::cout << "Warning: Flight temperature is too low!" << std::endl;
}

void cdh::highIgseTempAction() {
    std::cout << "Warning: IGSE temperature is too high!" << std::endl;
}

void cdh::lowIgseTempAction() {
    std::cout << "Warning: IGSE temperature is too low!" << std::endl;
}

void cdh::highCpuTempAction() {
    std::cout << "Warning: CPU temperature is too high!" << std::endl;
}

void cdh::lowCpuTempAction() {
    std::cout << "Warning: CPU temperature is too low!" << std::endl;
}

void cdh::highSdUsagePercentAction() {
    std::cout << "Warning: SD usage percent is too high!" << std::endl;
    std::cout << "Deleting 1GB of old health data from storage!" << std::endl;
    
}
