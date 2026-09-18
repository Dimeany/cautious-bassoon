#include <poll.h>

#include "gps_service.h"
#include "../util/status_util.h"

// Functions for sending GPS commands.  Replace this code with Edie calls?

std::vector<char> gps::create_log_command(int num, gps::MESSAGEFORMAT format) {
    std::vector<char> binary_command {};
    gps::OEM4BinaryHeader header {};
    gps::LOG message {};
    binary_command.resize(sizeof(header) + sizeof(message));

    // Filling header
    header.ucSync1 = 0xAA;
    header.ucSync2 = 0x44;
    header.ucSync3 = 0x12;
    header.ucHeaderLength = 28;
    header.usMsgNumber = 1;
    header.ucPort = (uint8_t) gps::DetailedPortIdentifier::THIS_PORT;
    header.usLength = sizeof(message);
    memcpy(binary_command.data(), &header, sizeof(header));

    // Filling message
    message.log_port_address = gps::DetailedPortIdentifier::THIS_PORT;
    message.message_id = ::CreateMsgID(static_cast<uint32_t>(num), 0, static_cast<uint32_t>(format), 0);
    message.trigger = static_cast<uint32_t>(gps::BinaryLogTriggers::ONCE);
    message.on_time = 0;
    message.offset = 0;
    message.hold = 0;
    memcpy(binary_command.data() + sizeof(header), &message, sizeof(message));

    // Filling crc
    uint32_t crc = 0;
    crc = CalculateBlockCRC32(binary_command.size(), crc, reinterpret_cast<const unsigned char*>(binary_command.data()));
    binary_command.resize(binary_command.size() + sizeof(crc));
    memcpy(binary_command.data() + sizeof(header) + sizeof(message), &crc, sizeof(crc));

    return binary_command;
}

std::string ascii_checksum(std::string_view message) {
    uint32_t crc = CalculateBlockCRC32(message.size(), 0, reinterpret_cast<unsigned char const*>(message.data()));
    std::stringstream ss;
    ss << std::hex << crc;
    return ss.str();
}

// valid cmd options are seen in cmdOpts struct in gps_service.h
gps::ServiceCmdOpts gps::parseGPSCmdOpts(int argc, char** argv){
    gps::ServiceCmdOpts GpsOpts {};
    for(int i = 1; i < argc; i++){
        std::string cmd = argv[i];
        if (cmd == "d"){
            GpsOpts.debug = true;
            std::cout<<"GPS service started with debug opt"<<std::endl;
        }
        else if(cmd == "b"){
            if(argv[i+1]){
                GpsOpts.baudRate = std::atoi(argv[i+1]);
                i++;
            }
        }
        else if(cmd == "p" || cmd=="s"){
            GpsOpts.print = true;
            
        }
        else{
            std::cout<<"Invalid GPS service command option"<<std::endl;
        }
    }
    return GpsOpts;
}

gps::GpsService::GpsService(std::shared_ptr<udp::Socket> socket_ptr, GpsServiceOptions gps_service_options, gps::ServiceCmdOpts ServiceCmdOpts){
    // Edie initialization
    edie_json_reader.LoadFile("/etc/SMALLSAT/novatel/messages_public.json");
    edie_header_decoder.LoadJsonDb(&edie_json_reader);
    edie_message_decoder.LoadJsonDb(&edie_json_reader);
    edie_encoder.LoadJsonDb(&edie_json_reader);

    uart_ptr = std::make_unique<UART>();
    uart_ptr->Open("/dev/serial1", ServiceCmdOpts.baudRate);
    uart_ptr->UpdateVTIME(0);

    this->socket_ptr = socket_ptr;
    this->gps_service_options = gps_service_options;
    this->service_cmd_opts = ServiceCmdOpts;
}

std::string gps::GpsService::success_response(std::string const& reply_body) {
    return "ack-ok\n" + reply_body;
}

std::string gps::GpsService::error_response(std::string const& reply_body) {
    return "error\n" + reply_body;
}

void gps::GpsService::set_nreset(int dir){
    // pin offset -- found using gpioinfo -c 0 --by-name -- we are offset 38 & name GPIO38
    // set gpio 38 to high to allow the gps receiver to boot using libgpiod

    gpiod::line_config gpio_obj;
    gpiod::line_settings gpio_settings{};
    gpiod::chip chip(std::filesystem::path{"/dev/gpiochip0"});   // chip object

    //set as output and as driven high
    gpio_settings.set_direction(gpiod::line::direction::OUTPUT);
    gpio_settings.set_output_value(dir == 1 ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE);
    gpio_obj.add_line_settings(gpiod::line::offset(38),gpio_settings);

    // request builder object
    gpiod::request_builder builder = chip.prepare_request();
    builder.set_line_config(gpio_obj);
    gpiod::line_request request = builder.do_request();
    request.release();

    if(dir == 1){
        std::cout << "GPS Enabled" << std::endl;
    }else if(dir == 0){
        std::cout << "GPS Disabled" <<std::endl;
    }
    
    nreset_dir = dir;
    return;
}

void gps::GpsService::do_poll() {
    // Poll serial port and UDP socket
    struct pollfd poll_fds[] = {
        {
            .fd = uart_ptr->get_fd(),
            .events = POLLIN,
            .revents = 0,
        },
        {
            .fd = socket_ptr->fd,
            .events = POLLIN,
            .revents = 0,
        },
    };

    nfds_t nfds = 2;
    int poll_timeout_ms = 1000;
    if (0 < poll(poll_fds, nfds, poll_timeout_ms)) {
        if (poll_fds[0].revents & POLLIN) {
            receive_gps_data();
        }

        if (poll_fds[1].revents & POLLIN) {
            receive_packet();
        }
        gps::GpsService::gps_communicating = true;
    }else{
        gps::GpsService::gps_communicating = false;
    }
}

// Function that sends a command to be executed by the system
std::string exec(const char* cmd) {
    std::array<char, 1024> buffer;
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

int64_t parse_time_to_ns(const std::string& time_str) {
    try {
        std::string trimmed = time_str;
        if (trimmed.front() == '+') trimmed.erase(0, 1); // remove leading +

        std::smatch match;
        std::regex rgx(R"(([-+]?[0-9]*\.?[0-9]+)(ns|us|ms|s)?)");
        if (std::regex_match(trimmed, match, rgx)) {
            double value = std::stod(match[1].str());
            std::string unit = match[2].str();

            if (unit == "ns" || unit.empty()) return static_cast<int64_t>(value);
            if (unit == "us") return static_cast<int64_t>(value * 1e3);
            if (unit == "ms") return static_cast<int64_t>(value * 1e6);
            if (unit == "s")  return static_cast<int64_t>(value * 1e9);
        }
    } catch (const std::exception& e) {
        std::cerr << "gps service parse_time_to_ns exception caught: " << e.what() << std::endl;
    }   
    return -1; // fallback
}

uint32_t parse_last_rx_to_sec(const std::string& val) {
    char unit;
    int num;
    try {
        if (val.empty()) return 0;
        if (val == "-") return 0;
         std::regex pattern (R"(\d+[a-z])");
        std::smatch match;
        if ( std::regex_match(val,match,pattern)) {
            std::string num_with_unit = match[0];
            unit = num_with_unit.back();
            num = std::stod(num_with_unit.erase(num_with_unit.size() - 1));
        }
        else{
            unit = 's';        // default unit
            num = std::stod(val);
        }
        
        switch (unit) {
            case 's': return static_cast<uint32_t>(num);
            case 'm': return static_cast<uint32_t>(num * 60);
            case 'h': return static_cast<uint32_t>(num * 3600);
            case 'd': return static_cast<uint32_t>(num * 86400);
            case 'y': return static_cast<uint32_t>(num * 31536000);
            default:  return static_cast<uint32_t>(std::stod(val)); // assume seconds
        }
    }catch (const std::exception& e) {
        std::cerr << "gps service parse_last_rx_to_sec exception caught: " << e.what() << std::endl;
    }
    //fallback
    return -1;
}

std::map<std::string, gps::ChronySourceInfo> gps::GpsService::GetChronycSources() {
    std::map<std::string, gps::ChronySourceInfo> sources;
    std::istringstream stream(exec("chronyc sources"));
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("NMEA") == std::string::npos && line.find("PPS") == std::string::npos){
            continue;
        }

        std::istringstream iss(line);
        std::string ms, name, stratum, poll, reach, last_rx;
        std::string strCombo;
        std::string offset_adjusted, offset_measured, plusminus, offset_std;

        // istringstream works fine until we hit offset_adjusted, then there is additional characters,
        // we replace these characters with whitespace then use istringstream again.
        iss >> ms >> name >> stratum >> poll >> reach >> last_rx;
        // Get the rest of the line
        std::getline(iss, strCombo);
        std::replace(strCombo.begin(),strCombo.end(), '[', ' ');
        std::replace(strCombo.begin(),strCombo.end(), ']', ' ');

        // Use istringstream again, now we have all parameters with proper parsing
        std::istringstream iss2(strCombo);
        iss2 >> offset_adjusted >> offset_measured >> plusminus >> offset_std;

        gps::ChronySourceInfo info;
        try { info.stratum = static_cast<uint8_t>(std::stoi(stratum)); } catch (const std::exception& e) {
            std::cerr << "chronyc sources stratum exception caught: " << e.what() << std::endl;
        }
        try { info.poll = static_cast<uint8_t>(std::stoi(poll)); } catch (const std::exception& e) {
            std::cerr << "chronyc sources poll exception caught: " << e.what() << std::endl;
        }
        try { info.reach = static_cast<uint16_t>(std::stoi(reach, nullptr, 8)); } catch (const std::exception& e) {
            std::cerr << "chronyc sources reach exception caught: " << e.what() << std::endl;
        }

        info.last_rx = parse_last_rx_to_sec(last_rx);
        info.offset_adjusted_ns = parse_time_to_ns(offset_adjusted);
        info.offset_measured_ns = parse_time_to_ns(offset_measured);
        info.offset_std_ns = parse_time_to_ns(offset_std);
        sources[name] = info;
    }
    return sources;
}

void gps::GpsService::receive_gps_data() {
    // Write all available data to Novatel framer
    std::vector<unsigned char> buffer(1024);
    while (auto bytes_read = uart_ptr->ReadOnly(buffer.data(), buffer.size())) {
        edie_framer.Write(buffer.data(), bytes_read);
    }

    // Read all available frames

    // MESSAGE_SIZE_MAX is defined in a Novatel header and used in its
    // Parser class for the buffer passed to GetFrame.
    std::vector<unsigned char> frame_buffer(MESSAGE_SIZE_MAX);

    // meta_data must be persistent across framer invocations - the
    // framer is stateful, and leaves in-progress work in the
    // meta_data object when it receives part of a frame
    static auto meta_data = novatel::edie::oem::MetaDataStruct {};

    auto intermediate_header = novatel::edie::oem::IntermediateHeader {};
    auto intermediate_message = novatel::edie::IntermediateMessage {};

    auto message_data = novatel::edie::MessageDataStruct {};

    while (true) {
        auto status = edie_framer.GetFrame(frame_buffer.data(), frame_buffer.size(), meta_data);
        if (status == novatel::edie::STATUS::UNKNOWN) {
            continue;
        }
        else if (status != novatel::edie::STATUS::SUCCESS) {
            break;
        }

        if (meta_data.eFormat == novatel::edie::HEADERFORMAT::NMEA) {
            handle_nmea_sentence(std::string(reinterpret_cast<char*>(frame_buffer.data()), meta_data.uiLength));
            continue;
        }
        

        status = edie_header_decoder.Decode(frame_buffer.data(), intermediate_header, meta_data);
        if (status != novatel::edie::STATUS::SUCCESS) {
            std::cout << "Novatel EDIE Could not decode header: " << status << std::endl;
            headerDecodeErr++;
        }

        status = edie_message_decoder.Decode(frame_buffer.data() + meta_data.uiHeaderLength, intermediate_message, meta_data);
        if (status != novatel::edie::STATUS::SUCCESS) {
            std::cout << "Novatel EDIE Could not decode message: " << status << std::endl;
            mssgDecodeErr++;
        }

        // Dispatch messages...
        std::vector<unsigned char> encode_buffer(MESSAGE_SIZE_MAX);
        unsigned char* encode_buffer_ptr = encode_buffer.data(); // Edie seems to want a pointer-to-a-pointer...

        if (service_cmd_opts.debug) {
            std::cout << "Received message from GPS:" << std::endl;
            status = edie_encoder.Encode(
                &encode_buffer_ptr, encode_buffer.size(),
                intermediate_header, intermediate_message,
                message_data, meta_data,
                novatel::edie::ENCODEFORMAT::JSON
            );
            if (status != novatel::edie::STATUS::SUCCESS) {
                std::cout << "Could not encode message: " << status << std::endl;
            }
            else {
                std::cout << std::string(reinterpret_cast<char*>(message_data.pucMessage), message_data.uiMessageLength) << std::endl;
            }
        }

        // A binary command response is for a log command.  Other
        // binary messages are for the health packet.
        if (meta_data.eFormat == novatel::edie::HEADERFORMAT::BINARY && !meta_data.bResponse) {
            status = edie_encoder.Encode(
                &encode_buffer_ptr, encode_buffer.size(),
                intermediate_header, intermediate_message,
                message_data, meta_data,
                novatel::edie::ENCODEFORMAT::BINARY
            );
            if (status != novatel::edie::STATUS::SUCCESS) {
                std::cout << "Novatel EDIE Could not encode binary message: " << status << std::endl;
                mssgEncodeErr++;
            }
            handle_binary_message(meta_data, message_data);
        }
        // If there is a pending UDP command, presumably the message
        // is a response to that command.
        else if (pending_udp_reply) {
            auto encode_format = pending_udp_reply->encode_format;
            if (pending_udp_reply->reply_message_state == ReplyMessageState::ExpectingLogCommandResponse) {
                // Put all command responses in ASCII
                encode_format = novatel::edie::ENCODEFORMAT::ASCII;
            }

            status = edie_encoder.Encode(
                &encode_buffer_ptr, encode_buffer.size(),
                intermediate_header, intermediate_message,
                message_data, meta_data,
                encode_format
            );
            if (status != novatel::edie::STATUS::SUCCESS) {
                std::cout << "Novatel EDIE Could not encode reply message: " << status << std::endl;
                mssgEncodeErr++;
            }
            handle_other_message(meta_data, message_data);
        }
        //adding handling for ID:94
        else if(meta_data.usMessageID == 94){
            std::cout << "RXSTATUSEVENT recieved:" << std::endl;

            status = edie_encoder.Encode(
                &encode_buffer_ptr, encode_buffer.size(),
                intermediate_header, intermediate_message,
                message_data, meta_data,
                novatel::edie::ENCODEFORMAT::ASCII
            );

            if (status != novatel::edie::STATUS::SUCCESS) {
                std::cout << "Novatel EDIE Could not encode RXSTATUSEVENT: " << status << std::endl;
                mssgEncodeErr++;
            }
            
            std::cout << std::string(reinterpret_cast<char*>(message_data.pucMessage), message_data.uiMessageLength) << std::endl;
        }

        //add handling for ID:128 for checking rxconfig
        else if(meta_data.usMessageID == 128){
            status = edie_encoder.Encode(
                &encode_buffer_ptr, encode_buffer.size(),
                intermediate_header, intermediate_message,
                message_data, meta_data,
                novatel::edie::ENCODEFORMAT::ASCII
            );

            if (status != novatel::edie::STATUS::SUCCESS) {
            std::cout << "Novatel EDIE Could not encode RXCONFIG MESSAGE: " << status << std::endl;
                mssgEncodeErr++;
            }
            
            std::cout << std::string(reinterpret_cast<char*>(message_data.pucMessage), message_data.uiMessageLength) << std::endl;
        }

        // add handling for ID:1412
        else if(meta_data.usMessageID == 1412){
            // std::cout << "PROFILEINFO recieved" << std::endl;

            status = edie_encoder.Encode(
                &encode_buffer_ptr, encode_buffer.size(),
                intermediate_header, intermediate_message,
                message_data, meta_data,
                novatel::edie::ENCODEFORMAT::ASCII
            );

            if (status != novatel::edie::STATUS::SUCCESS) {
                std::cout << "Novatel EDIE Could not encode PROFILEINFO: " << status << std::endl;
                mssgEncodeErr++;
            }
        }
        else {
            std::cout << "Received unexpected GPS message.  ID: " << meta_data.usMessageID << std::endl;
            std::cout << std::string(reinterpret_cast<char*>(message_data.pucMessage), message_data.uiMessageLength) << std::endl;
        }
    }
}

void gps::GpsService::receive_packet() {
    try {
        //std::cout << "About to listen for a packet" << std::endl;
        //std::cerr << "timeout for the socket is " << socket_ptr->timeout().count() << std::endl;
        udp::SocketAddress sender;
        auto opt_packet = socket_ptr->receive(sender);
        if (!opt_packet) {
            // Read timed out.
            return;
        }

        // Process UDP command
        auto command = std::string(opt_packet->data(), opt_packet->size());
        
        // I know this is a bad way to handle string cmds and we should use an unordered map,
        // but there is only 3 so oh well
        if (command == "exit"){
            cleanKillFlag = 1;
            std::string resp = "killed";
            socket_ptr->send(resp,sender);
            return;
        }
        else if(command == "boot enable"){
            set_nreset(1);
            std::string resp = "GPS boot enabled via nreset";
            socket_ptr->send(resp,sender);
        }
        else if(command == "boot disable"){
            set_nreset(0);
            std::string resp = "GPS boot disabled via nreset";
            socket_ptr->send(resp,sender);
        }
        process_command(command, sender);
    }
    catch (const udp::SocketException& e){
        std::cerr << "socket exception caught: " << e.what() << std::endl;
    }
}

void gps::GpsService::update_chrony_info(std::map<std::string, gps::ChronySourceInfo>* sources){

    if (auto it = sources->find("NMEA"); it != sources->end()) {
        cached_NMEA_info = it->second;
    }

    if (auto it = sources->find("PPS0"); it != sources->end()) {
        cached_pps_info = it->second;
    }

    if(chrony_packet_is_cached == false){
        chrony_packet_is_cached = true;
    }
    if(service_cmd_opts.debug){
        std::cout << "GPSSERVICE::Updated cached Chrony packet"<<std::endl;
    }
}


//This updates the cached gps health packet
void gps::GpsService::update_health_packet(const gps::TIME& time_log, const gps::BESTXYZ& best_xyz, const gps::OEM4BinaryHeader& header_bestxyz, const gps::PSRDOP& psrdop, const gps::HWMONITOR& hwmonitor) {

    // check if chrony sources thread is ready to pass out information
    if(chrony_future.valid()){
        if(chrony_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready){
            auto raw_chrony = chrony_future.get();
            update_chrony_info(&raw_chrony);
        }
    }
    else{
        if(service_cmd_opts.debug){
            std::cout<<"chron future is invalid"<<std::endl;
        }
    }

    cached_health_packet.percent_idle = header_bestxyz.ucIdleTime;
    cached_health_packet.time_status = header_bestxyz.ucTimeStatus;
    cached_health_packet.ref_week_num = header_bestxyz.usWeekNo;
    cached_health_packet.gps_week_ms = header_bestxyz.uiWeekMSec;
    cached_health_packet.rcvr_status = header_bestxyz.uiStatus;

    // Populate all the fields from bestxyz
    cached_health_packet.solution_status = best_xyz.solution_status;
    cached_health_packet.position_type = best_xyz.position_type;
    cached_health_packet.diff_age = best_xyz.diff_age;
    cached_health_packet.solution_age = best_xyz.solution_age;
    cached_health_packet.number_satellites_tracked = best_xyz.num_svs;
    cached_health_packet.num_sats_used_in_solution = best_xyz.num_soln_svs;
    cached_health_packet.x = best_xyz.x;
    cached_health_packet.y = best_xyz.y;
    cached_health_packet.z = best_xyz.z;
    cached_health_packet.x_std_dev = best_xyz.x_std_dev;
    cached_health_packet.y_std_dev = best_xyz.y_std_dev;
    cached_health_packet.z_std_dev = best_xyz.z_std_dev;
    cached_health_packet.v_sol_status = best_xyz.status;
    cached_health_packet.vel_type = best_xyz.type;
    cached_health_packet.vel_x = best_xyz.x_10;
    cached_health_packet.vel_y = best_xyz.y_11;
    cached_health_packet.vel_z = best_xyz.z_12;

    // Populate all the fields from hwmonitor
    cached_health_packet.hwmonitor_arraylength = hwmonitor.measurements_arraylength;
    cached_health_packet.temperature = hwmonitor.measurements[0].value;
    cached_health_packet.temp_status = hwmonitor.measurements[0].status;
    cached_health_packet.antenna_current = hwmonitor.measurements[1].value;
    cached_health_packet.antenna_current_status = hwmonitor.measurements[1].status;
    cached_health_packet.core_3v3_voltage = hwmonitor.measurements[2].value;
    cached_health_packet.core_3v3_status = hwmonitor.measurements[2].status;
    cached_health_packet.antenna_voltage = hwmonitor.measurements[3].value;
    cached_health_packet.antenna_voltage_status = hwmonitor.measurements[3].status;
    cached_health_packet.core_1v2_voltage = hwmonitor.measurements[4].value;
    cached_health_packet.core_1v2_status = hwmonitor.measurements[4].status;
    cached_health_packet.regulated_supply_voltage = hwmonitor.measurements[5].value;
    cached_health_packet.regulated_supply_voltage_status = hwmonitor.measurements[5].status;
    cached_health_packet.one_v_8 = hwmonitor.measurements[6].value;
    cached_health_packet.one_v_8_status = hwmonitor.measurements[6].status;
    cached_health_packet.voltage_5v = hwmonitor.measurements[7].value;
    cached_health_packet.voltate_5v_status = hwmonitor.measurements[7].status;
    cached_health_packet.secondary_temp = hwmonitor.measurements[8].value;
    cached_health_packet.secondary_temp_status = hwmonitor.measurements[8].status;

    // Dilution of precision with PSR filter
    cached_health_packet.psr_pdop = psrdop.pdop;
    cached_health_packet.psr_tdop = psrdop.tdop;
    cached_health_packet.psr_cutoff= psrdop.gps_elev_mask;
    cached_health_packet.clock_model_status = time_log.clock_model_status;
    cached_health_packet.offset = time_log.offset;
    cached_health_packet.offset_std = time_log.offset_std;
    cached_health_packet.utc_offset = time_log.utc_offset;
    cached_health_packet.utc_year = time_log.utc_year;
    cached_health_packet.utc_month = time_log.utc_month;
    cached_health_packet.utc_day = time_log.utc_day;
    cached_health_packet.utc_hour = time_log.utc_hour;
    cached_health_packet.utc_minute = time_log.utc_minute;
    cached_health_packet.utc_millisecond = time_log.utc_millisecond;
    cached_health_packet.utc_time_status = time_log.utc_time_status;

    if(health_packet_is_cached == false){
        health_packet_is_cached = true;
    }

    if(service_cmd_opts.debug){
        std::cout<<"GPSSERVICE::Updated cached gps health packet"<<std::endl;
    }
}

void gps::GpsService::print_health_packet() {
    std::cout << std::endl << "-< GPS Health >-"   << std::endl;

    std::cout << "Percent Idle: " << cached_health_packet.percent_idle << std::endl;
    std::cout << "header time_status: " << cached_health_packet.time_status << std::endl;
    std::cout << "header ref_week_num: " << cached_health_packet.ref_week_num << std::endl;
    std::cout << "header gps_week_ms: " << cached_health_packet.gps_week_ms << std::endl;
    std::cout << "header rcvr_status: " << cached_health_packet.rcvr_status << std::endl;
    std::cout << std::endl;

    std::cout << "bestxyz solution_status: " << cached_health_packet.solution_status << std::endl;
    std::cout << "bestxyz position_type: " << cached_health_packet.position_type << std::endl;
    std::cout << "bestxyz differential age: " << cached_health_packet.diff_age << std::endl;
    std::cout << "bestxyz solution age: " << cached_health_packet.solution_age << std::endl;
    std::cout << "bestxyz number_satellites_tracked: " << cached_health_packet.number_satellites_tracked << std::endl;
    std::cout << "bestxyz num_sats_used_in_solution: " << cached_health_packet.num_sats_used_in_solution << std::endl;

    std::cout << "bestxyz xyz: (" << cached_health_packet.x << ", "
                                      << cached_health_packet.y << ", "
                                      << cached_health_packet.z << ")" << std::endl;

    std::cout << "bestxyz standard deviations: (" << cached_health_packet.x_std_dev << ", "
                                      << cached_health_packet.y_std_dev << ", "
                                      << cached_health_packet.z_std_dev << ")" << std::endl;

    std::cout << "bestxyz v_sol_status: " << cached_health_packet.v_sol_status << std::endl;
    std::cout << "bestxyz vel_type: " << cached_health_packet.vel_type << std::endl;

    std::cout << "bestxyz velocity: (" << cached_health_packet.vel_x << ", "
                                      << cached_health_packet.vel_y << ", "
                                      << cached_health_packet.vel_z << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "Hardware Monitor:" << std::endl;
    std::cout << "temperature: " << cached_health_packet.temperature << std::endl;
    std::cout << "temp_status: " << cached_health_packet.temp_status << std::endl;
    std::cout << "antenna_current: " << cached_health_packet.antenna_current << std::endl;
    std::cout << "antenna_current_status: " << cached_health_packet.antenna_current_status << std::endl;
    std::cout << "core_3v3_voltage: " << cached_health_packet.core_3v3_voltage << std::endl;
    std::cout << "core_3v3_status: " << cached_health_packet.core_3v3_status << std::endl;
    std::cout << "antenna_voltage: " << cached_health_packet.antenna_voltage << std::endl;
    std::cout << "antenna_voltage_status: " << cached_health_packet.antenna_voltage_status << std::endl;
    std::cout << "core_1v2_voltage: " << cached_health_packet.core_1v2_voltage << std::endl;
    std::cout << "core_1v2_status: " << cached_health_packet.core_1v2_status << std::endl;
    std::cout << "regulated_supply_voltage: " << cached_health_packet.regulated_supply_voltage << std::endl;
    std::cout << "regulated_supply_voltage_status: " << cached_health_packet.regulated_supply_voltage_status << std::endl;
    std::cout << "one_v_8: " << cached_health_packet.one_v_8 << std::endl;
    std::cout << "one_v_8_status: " << cached_health_packet.one_v_8_status << std::endl;
    std::cout << "voltage_5v: " << cached_health_packet.voltage_5v << std::endl;
    std::cout << "voltate_5v_status: " << cached_health_packet.voltate_5v_status << std::endl;
    std::cout << "secondary_temp: " << cached_health_packet.secondary_temp << std::endl;
    std::cout << "secondary_temp_status: " << cached_health_packet.secondary_temp_status << std::endl;

    // NMEA source info as seen by chrony
    std::cout << "nmea_stratum: " << cached_health_packet.nmea_info.stratum << std::endl;
    std::cout << "nmea_poll: " << cached_health_packet.nmea_info.poll << std::endl;
    std::cout << "nmea_reach: " << cached_health_packet.nmea_info.reach << std::endl;
    std::cout << "nmea_last_rx: " << cached_health_packet.nmea_info.last_rx << std::endl;
    std::cout << "nmea_offset_measured_ns: " << cached_health_packet.nmea_info.offset_measured_ns << std::endl;
    std::cout << "nmea_offset_adjusted_ns: " << cached_health_packet.nmea_info.offset_adjusted_ns << std::endl;
    std::cout << "nmea_offset_std_ns: " << cached_health_packet.nmea_info.offset_std_ns << std::endl;

    // PPS source info as seen by chrony
    std::cout << "pps_stratum: " << cached_health_packet.pps_info.stratum << std::endl;
    std::cout << "pps_poll: " << cached_health_packet.pps_info.poll << std::endl;
    std::cout << "pps_reach: " << cached_health_packet.pps_info.reach << std::endl;
    std::cout << "pps_last_rx: " << cached_health_packet.pps_info.last_rx << std::endl;
    std::cout << "pps_offset_measured_ns: " << cached_health_packet.pps_info.offset_measured_ns << std::endl;
    std::cout << "pps_offset_adjusted_ns: " << cached_health_packet.pps_info.offset_adjusted_ns << std::endl;
    std::cout << "pps_offset_std_ns: " << cached_health_packet.pps_info.offset_std_ns << std::endl;
    std::cout << std::endl << std::endl;

    std::cout << "-< End GPS Health >- " << std::endl << std::endl;
}


void gps::GpsService::configure_pps(udp::SocketAddress sender){
    flag_pps = false;
    std::string pps_config = "command ascii PPSCONTROL,THISPORT,0,0,UNKNOWN,0,0.0,0,0;enable,positive,1.0,1000";

    // Strip prefix. Parse number as int.
    std::string prefix = "command ascii ";
    std::string  command_str = pps_config.substr(prefix.size());
    std::string  full_command_str = '#' + command_str + '*' + ascii_checksum(command_str) + "\r\n";
    uart_ptr->Write(full_command_str);

    std::cout << "Configured PPS with Positive Polarity\n";
    
    pending_udp_reply = PendingUdpReply {
        sender,
        novatel::edie::ENCODEFORMAT::ASCII,
        ReplyMessageState::ExpectingCommandResponse,
    };
}


void gps::GpsService::process_command(std::string const& command_string, udp::SocketAddress sender) {
    if(service_cmd_opts.debug){
        std::cout << "string command: " << command_string << "\n";
    }

    if (command_string.starts_with("log ascii ")) {
        // Strip prefix.  Parse number as int.
        std::string prefix = "log ascii ";
        int message_id = std::stoi(command_string.substr(prefix.size()));
        std::vector<char> command = create_log_command(message_id, gps::MESSAGEFORMAT::ASCII);
        uart_ptr->Write(command);

        pending_udp_reply = PendingUdpReply {
            sender,
            novatel::edie::ENCODEFORMAT::ASCII,
            ReplyMessageState::ExpectingLogCommandResponse,
        };
        accepted_cmds++;
    }

    else if (command_string.starts_with("log json ")) {
        // Strip prefix.  Parse number as int.
        std::string prefix = "log json ";
        int message_id = std::stoi(command_string.substr(prefix.size()));
        std::vector<char> command = create_log_command(message_id, gps::MESSAGEFORMAT::ASCII);
        uart_ptr->Write(command);

        pending_udp_reply = PendingUdpReply {
            sender,
            novatel::edie::ENCODEFORMAT::JSON,
            ReplyMessageState::ExpectingLogCommandResponse,
        };
        accepted_cmds++;
    }

    else if (command_string.starts_with("command ascii ")) {
        // Strip prefix.  Parse number as int.
        std::string prefix = "command ascii ";
        std::string command_str = command_string.substr(prefix.size());
        std::string full_command_str = '#' + command_str + '*' + ascii_checksum(command_str) + "\r\n";
        uart_ptr->Write(full_command_str);

        pending_udp_reply = PendingUdpReply {
            sender,
            novatel::edie::ENCODEFORMAT::ASCII,
            ReplyMessageState::ExpectingCommandResponse,
        };
        accepted_cmds++;
    }

    // else if (command_string.starts_with("command ascii abrev ")) {
    //     // Strip prefix.  Parse number as int.
    //     std::string prefix = "command ascii abrev ";
    //     std::string command_str = command_string.substr(prefix.size());
    //     std::string full_command_str = command_str + "\r\n";
    //     uart_ptr->Write(full_command_str);

    //     pending_udp_reply = PendingUdpReply {
    //         sender,
    //         novatel::edie::ENCODEFORMAT::ASCII,
    //         ReplyMessageState::ExpectingCommandResponse,
    //     };
    //     accepted_cmds++;
    // }

    else {
        auto response = error_response("string command not recognized");
        socket_ptr->send(response, sender);
        return;
    }
}

void gps::GpsService::handle_nmea_sentence(const std::string& sentence) {
    if (service_cmd_opts.debug) {
        std::cout << "Received NMEA sentence: " << sentence << std::endl;
    }

    // Send the UDP packet
    try {
        socket_ptr->send(sentence, gps_service_options.nmea_socket);
    } catch (const std::exception& e) {
        std::cerr << "Failed to send NMEA packet: " << e.what() << std::endl;
    }
    recNmea++;
}

void gps::GpsService::handle_binary_message(novatel::edie::oem::MetaDataStruct &meta_data, novatel::edie::MessageDataStruct &message_data) {
    auto message_header = std::span<unsigned char> {
        message_data.pucMessageHeader,
        message_data.uiMessageHeaderLength,
    };
    auto message_body = std::span<unsigned char> {
        message_data.pucMessageBody,
        message_data.uiMessageBodyLength - 4, // Subtract length of binary checksum
    };
    switch (meta_data.usMessageID) {
        case static_cast<int>(gps::MessageID::TIME):
            // Extract and store TIME data
            time_log = get_struct<gps::TIME>(message_body);
            flag_time = true;
            break;
        case static_cast<int>(gps::MessageID::PSRDOP):
            // Extract and store PSRDOP data
            psrdop = get_struct<gps::PSRDOP>(message_body);
            flag_psrdop = true;
            break;
        case static_cast<int>(gps::MessageID::BESTXYZ):
            // Extract and store BESTXYZ data
            best_xyz = get_struct<gps::BESTXYZ>(message_body);
            header_bestxyz = get_struct<gps::OEM4BinaryHeader>(message_header);
            flag_bestxyz = true;
            break;
        case static_cast<int>(gps::MessageID::HWMONITOR):
            // Extract and store HWMONITOR data
            hwmonitor = get_struct<gps::HWMONITOR>(message_body);
            flag_hwmonitor = true;
            break;
        default:
            std::cout << "Unexpected binary message with ID: " << static_cast<int>(meta_data.usMessageID) << std::endl;
            break;
    }
    if (health_packet_ready()) {
        update_health_packet(time_log, best_xyz, header_bestxyz, psrdop, hwmonitor);
        reset_flags();
    }
}
bool gps::GpsService::health_packet_ready() {
    return flag_time && flag_psrdop && flag_bestxyz && flag_hwmonitor;
}

void gps::GpsService::handle_other_message(novatel::edie::oem::MetaDataStruct &meta_data, novatel::edie::MessageDataStruct &message_data) {
    std::string response_message = std::string(reinterpret_cast<char *>(message_data.pucMessage), message_data.uiMessageLength);

    switch (pending_udp_reply->reply_message_state) {
    case ReplyMessageState::ExpectingCommandResponse:
    case ReplyMessageState::ExpectingLogCommandResponse: {
        if (!meta_data.bResponse) {
            std::cout << "Expecting a command response but, got a message that was not a command response.  ID: " << meta_data.usMessageID << std::endl;
            std::cout << response_message << std::endl;
            return;
        }

        // If the command response was not OK, send an error response
        // and reinitialize the pending reply message
        if (message_data.uiMessageBodyLength < 3 || memcmp("OK*", message_data.pucMessageBody, 3)) {
            std::string reply_body = error_response(response_message);
            socket_ptr->send(reply_body, pending_udp_reply->sender);
            pending_udp_reply = {};
            accepted_cmds--;
            cmdResponse++;
        }
        // If the command response was for a log command, switch state
        // to expecting log.
        else if (pending_udp_reply->reply_message_state == ReplyMessageState::ExpectingLogCommandResponse) {
            pending_udp_reply->reply_message_state = ReplyMessageState::ExpectingLog;
        }
        // Otherwise, the command response was for a command.  Send a
        // success response.    
        else {
            std::string reply_body = success_response(response_message);
            socket_ptr->send(reply_body, pending_udp_reply->sender);
            pending_udp_reply = {};
            cmdResponse++;
        }
        break;
    }
    case ReplyMessageState::ExpectingLog: {
        std::string reply_body = success_response(response_message);
        socket_ptr->send(reply_body, pending_udp_reply->sender);
        cmdResponse++;
        pending_udp_reply = {};
        break;
    }
    }
}

void gps::GpsService::send_health_packet(){
    //Wrap the packet and send it
    packetCt++;
    cached_health_packet.pps_info = cached_pps_info;

    util::Status status;

    if(!gps::GpsService::gps_communicating){
        status = util::Status::HARDWARE_ERROR;
    }
    else if(!serial_connection_setup || !health_packet_is_cached){
        status = util::Status::SOFTWARE_ERROR;
    }
    else{
        status = util::Status::OK;
    }

    cached_health_packet.nmea_info = cached_NMEA_info;
    std::vector<char> packet = udp::Packet::create_from_struct(
        cached_health_packet,
        gps_service_options.send_socket.get_port(),
        socket_ptr->bound_address.get_port(),
        health_packet_sequence_number++,
        status

    ).serialize();
    socket_ptr->send(packet, gps_service_options.send_socket);

    if(service_cmd_opts.debug){
        std::cout<<"Sent Health Packet"<<std::endl;
        if(service_cmd_opts.print){
            print_health_packet();
        }
    }
}

void gps::GpsService::reset_flags() {
    flag_time = false;
    flag_psrdop = false;
    flag_bestxyz = false;
    flag_hwmonitor = false;
}
