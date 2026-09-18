#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <poll.h>

#include "gps_service.h"
#include "../util/environment_util.h"
#include "version.h"

const udp::SocketAddress SEND_SOCKET = udp::SocketAddress::create("127.0.0.1", atoi(util::getenv_required("GPS_UDPCAPTURE_PORT")));
const udp::SocketAddress GPS_SERVICE_SOCKET = udp::SocketAddress::create("0.0.0.0", atoi(util::getenv_required("GPS_COMMAND_PORT")));
const udp::SocketAddress NMEA_SOCKET = udp::SocketAddress::create("0.0.0.0", atoi(util::getenv_required("GPS_NMEA_SOCKET_PORT")));
const udp::SocketAddress PPS_CONFIG = udp::SocketAddress::create("127.0.0.1", atoi(util::getenv_required("GPS_PPS_CONFIG_PORT")));
//const udp::SocketAddress PROFILE_CONFIG = udp::SocketAddress::create("127.0.0.1", atoi(util::getenv_required("GPS_PROFILE_CONFIG_PORT")));

const int HEALTH_PACKET_DELAY_SECONDS = util::getenv_int("GPS_HP_DELAY_S", 10);

//receiver setup and helper functions
bool setBaudRate();
bool setupConfig();
std::string readResponse(int fd);
bool verifyMessage(int fd, const std::string& cmd, bool settingBaud);

//profile setup
int failure_counter = 0;
bool profile_flag = false;
//auto service_start_time = std::chrono::system_clock::now();

int main(int argc, char *argv[]) {
    // Initialize edie logger
    Logger::InitLogger();

    std::cout << "Nebula version: " << NEBULA_VERSION << std::endl;

    auto options = gps::parseGPSCmdOpts(argc, argv);
    auto subsystem_socket = std::make_shared<udp::Socket>(GPS_SERVICE_SOCKET);

    using namespace std::chrono_literals;
    subsystem_socket->set_timeout(1s);

    gps::GpsServiceOptions gps_service_options {
        SEND_SOCKET,
        NMEA_SOCKET,
        PPS_CONFIG,
    };

    gps::GpsService gps_service = gps::GpsService(std::move(subsystem_socket), gps_service_options, options);


    //start with reciever off
    gps_service.set_nreset(0);

    std::cout << "GPS Service initialized" << std::endl;

    while (true) {
        //set configurations when the gps is enabled and not already set
        if(!profile_flag && gps_service.gps_communicating){
            std::this_thread::sleep_for(std::chrono::seconds(10));//wait for the receiver to enable

            //set receiver baud rate to 115200
            gps_service.serial_connection_setup = setBaudRate();

            //set up profiles and configure receiver
            setupConfig();

            gps_service.configure_pps(PPS_CONFIG);
            std::cout << "PPS Polarity Configured" << std::endl;

            profile_flag = true;

            std::cout << "GPS card properly configured" << std::endl;

            // //run main config function
            // if(!setupConfig() && failure_counter < 2){
            //     //disable gps if config fails
            //     gps_service.set_nreset(0);
            //     std::cerr << "Failed to configure GPS" << std::endl;
            //     failure_counter++;
            // }else{
            //     //configure with the correct polarity
            //     gps_service.configure_pps(PPS_CONFIG);
            //     //std::cout << "PPS Polarity Configured" << std::endl;

            //     profile_flag = true;

            //     std::cout << "GPS card properlly configured" << std::endl;
            // }
        }


        try {
            gps_service.do_poll();
        } catch (const std::exception& e) {
            std::cerr << "gps service exception caught: " << e.what() << std::endl;
        }

        // if chrony hasn't been cached and chrony getter thread isn't running
        if(!gps_service.chrony_packet_is_cached){
            if(!gps_service.chrony_future.valid()){
                gps_service.chrony_future = std::async(std::launch::async,&gps::GpsService::GetChronycSources,&gps_service);   
                if(gps_service.service_cmd_opts.debug){
                    std::cout<<"GPSSERVICE::Called chronyc sources thread\n";
                }
            }
            else if(gps_service.chrony_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready){
                std::map<std::string, gps::ChronySourceInfo> raw_chrony = gps_service.chrony_future.get();
                gps_service.update_chrony_info(&raw_chrony);                
            }
        }

        // check if it has been 10 sec since last health packet got sent and there is a cached packet to send
        else if(gps_service.chrony_packet_is_cached){
            if(gps_service.health_packet_sequence_number == 0 || (std::chrono::system_clock::now() - gps_service.packet_timer) >= std::chrono::seconds(HEALTH_PACKET_DELAY_SECONDS)){
                gps_service.send_health_packet();
                gps_service.packet_timer = std::chrono::system_clock::now();

                // check if chrony reader thread is available (not actively reading or waiting for .get() method)
                if(!gps_service.chrony_future.valid()){
                    gps_service.chrony_future = std::async(std::launch::async,&gps::GpsService::GetChronycSources,&gps_service);
                    if(gps_service.service_cmd_opts.debug){
                        std::cout<<"GPSSERVICE::Called chronyc sources thread\n";
                    }
                }
            }        
        }

        // clean exit
        if (gps_service.cleanKillFlag == 1){
            std::cout<<"exited cleanly\n";
            return 0;
        }
    }    
}

bool setBaudRate(){
    std::cout << "Starting to configure GPS serial connection...\n"; 
    
    int fd = open("/dev/serial1", O_RDWR | O_NOCTTY); 
    if (fd < 0) { 
        std::cerr << "Error: Couldn't open GPS serial interface\n"; 
        return false;
    } 

    struct termios tty; 
    if (tcgetattr(fd, &tty) != 0) { 
        std::cerr << "Error: Couldn't get terminal attributes\n"; 
        close(fd); 
        return false; 
    } 

    //Set initial 9600 Speed
    cfsetspeed(&tty, B9600); 

    //Configure raw mode
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_cflag &= ~(CSIZE | PARENB);
    tty.c_cflag |= CS8;

    tty.c_cflag |= (CLOCAL | CREAD); 
    tty.c_cflag &= ~CRTSCTS; 
    tty.c_cc[VMIN] = 0; 
    tty.c_cc[VTIME] = 5; 

    if (tcsetattr(fd, TCSANOW, &tty) != 0) { 
        std::cerr << "Error: Couldn't set terminal attributes\n"; 
        close(fd); 
        return false; 
    } 
    
    //command to set baud rate to 115200
    std::string baud_command = "SERIALCONFIG COM1 115200 N 8 1 N ON\r\n"; 
    if(!verifyMessage(fd, baud_command, true)) {
        std::cerr << "Error writing GPS baud rate to 115200" << std::endl;
        close(fd);
        return false;
    }

    tcdrain(fd); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

    //Change pi to 115200
    cfsetspeed(&tty, B115200); 
    if (tcsetattr(fd, TCSANOW, &tty) != 0) { 
        std::cerr << "Error: Couldn't change terminal attributes to 115200\n"; 
        close(fd); 
        return false; 
    } 

    close(fd); 
    std::cout << "GPS receiver COM1 successfully set to 115200 baud" << std::endl; 
    return true;
}

bool setupConfig() { 
    std::cout << "Setting up GPS\n"; 
    
    int fd = open("/dev/serial1", O_RDWR | O_NOCTTY); 
    if (fd < 0) { 
        std::cerr << "Error: Couldn't open GPS serial interface\n"; 
        return false; 
    } 
    struct termios tty; 
    if (tcgetattr(fd, &tty) != 0) { 
        std::cerr << "Error: Couldn't get terminal attributes\n"; 
        close(fd); 
        return false; 
    } 
    
    std::vector<std::string> configs;
    configs.push_back("PROFILE CREATE nominal1\r\n"); 
    std::string nominal1_gnss_config[14] = { 
        "PROFILE CREATEELEMENT nominal1 \"SERIALCONFIG COM1 115200 N 8 1 N ON\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"PPSCONTROL enable positive 1.0 1000\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"DATADECODESIGNAL GPSL2C ENABLE\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"DATADECODESIGNAL GPSL5 ENABLE\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"NMEATALKER AUTO\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"SETIONOTYPE L1L2\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"PDPFILTER ENABLE\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"ASSIGNALL ALL AUTO\"\r\n",
        "PROFILE CREATEELEMENT nominal1 \"ELEVATIONCUTOFF ALL -17\"\r\n",
        "PROFILE CREATEELEMENT nominal1 \"DOPPLERWINDOW ALL USER 50000\"\r\n",
        "PROFILE CREATEELEMENT nominal1 \"DYNAMICS AIR\"\r\n",
        "PROFILE CREATEELEMENT nominal1 \"SETTROPOMODEL NONE\"\r\n",
        "PROFILE CREATEELEMENT nominal1 \"POSTIMEOUT 10\"\r\n",
        "PROFILE CREATEELEMENT nominal1 \"APPROXPOSTIMEOUT 5\"\r\n",
    }; 
    for(const auto& cmd : nominal1_gnss_config) configs.push_back(cmd);

    std::string nominal1_log_config[10] = { 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_5 GPRMC ONTIME 5 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_6 GPGSA ONTIME 5 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_7 GNGSA ONTIME 5 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_8 GNRMC ONTIME 5 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_9 GARMC ONTIME 5 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_10 GAGSA ONTIME 5 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_1 TIMEB ONTIME 10 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_3 BESTXYZB ONTIME 10 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_4 HWMONITORB ONTIME 10 0 HOLD\"\r\n", 
        "PROFILE CREATEELEMENT nominal1 \"LOG COM1_2 PSRDOPB ONTIME 10 0 HOLD\"\r\n" 
    }; 
    for(const auto& cmd : nominal1_log_config) configs.push_back(cmd);
 
    configs.push_back("PROFILE activate nominal1\r\n");

    //Write profile setup vector
    for (size_t i = 0; i < configs.size(); i++) {
        if(!verifyMessage(fd, configs[i], false)) {
            std::cerr << "Profile Error: command index " << i << " failed\n";
            break;
        }
    }

    std::vector<std::string> commands;

    //enable all constelations
    
    

    //Write receiver configs
    for (size_t i = 0; i < commands.size(); i++) {
        if(!verifyMessage(fd, commands[i], false)) {
            std::cerr << "Config Error: command index " << i << " failed\n";
            close(fd);
            return false;
        }
    }

    //Save Config
    verifyMessage(fd, "SAVECONFIG\r\n", false);

    
    tcdrain(fd); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

    close(fd); 
    std::cout << "GPS receiver fully configured" << std::endl; 
    return true; 
}


std::string readResponse(int fd){
    int timeout = 5000;
    std::string result;
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    auto time_left = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);

    while(true){
        int time_remaining = std::chrono::duration_cast<std::chrono::milliseconds>(time_left - std::chrono::steady_clock::now()).count();
        //if timeout
        if (time_remaining <= 0) {
            break;
        }
        int ret = poll(&pfd, 1, time_remaining);

        //poll error
        if(ret < 0){
            std::cerr << "Error: Failed to poll for data\n" << std::endl;
            break;
        }

        //no data received
        if(ret == 0){
            std::cerr << "Error: Timeout while waiting for data\n" << std::endl;
            break;
        }

        if(pfd.revents & (POLLERR | POLLHUP)){
            std::cerr << "Serial port error\n" << std::endl;
            break;
        }

        if(pfd.revents & POLLIN){
            char buffer[256];
            ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
            if (bytes_read > 0){
                result.append(buffer, bytes_read);
                //a response from the gps should contain [COM1]
                if (result.find("[COM") != std::string::npos){
                    //std::cout << "Received response from GPS: " << std::endl;
                    break;
                }
    
            }
            else{
                std::cerr << "Error: Failed to read from receiver\n" << std::endl;
                break;
            }
        }
    }
    return result;
}


bool verifyMessage(int fd, const std::string& cmd, bool settingBaud){
    ssize_t bytes_written = write(fd, cmd.c_str(), cmd.length());
    if(bytes_written < 0){
        std::cerr << "Error: Failed to write command to receiver\n" << std::endl;
        return false;
    }
    
    tcdrain(fd);

    std::string response = readResponse(fd);

    if(!settingBaud){
        if(response.find("<ERROR") != std::string::npos){
            std::cerr << "Command failed: " << cmd << " | Response: " << response << std::endl;
            return false;
        }
        
        if(response.find("<OK") == std::string::npos){
            std::cerr << "OK not received for: " << cmd << " | Response: " << response << std::endl;
            return false;
        }
        else{
            std::cout << "OK received for: " << cmd << " | Response: " << response << std::endl;
        }
        return true;
    }
    else{
        if(response.find("<OK") != std::string::npos){
            std::cerr << "Baud rate set for 115200" << std::endl;
            return true;
        }
        else{
            std::cerr << "Baud rate already set to 115200" << std::endl;
            return true;
        }
    }
    
}
