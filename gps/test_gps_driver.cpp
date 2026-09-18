
#include "gps_service.h"

struct TestGPSService : public gps::GpsService{
    //create member variable ports to reduce passed variables later
    const udp::SocketAddress SEND_SOCKET;
    const udp::SocketAddress PPS_CONFIG;

    //TestService Constructor calls GpsService Constructor to allow TestService to inherit GpsService
   TestGPSService(std::shared_ptr<udp::Socket> socket_ptr, gps::GpsServiceOptions gps_service_options, gps::ServiceCmdOpts options)
   :gps::GpsService(socket_ptr,gps_service_options,options), SEND_SOCKET(gps_service_options.send_socket), PPS_CONFIG(gps_service_options.pps_config) {   
    }

    void test_command_ids(int& iters){
        int commandCtr=0;
        int poll_flag;

        // vector of command strings
        const std::vector<std::string> commandStrs = {
            "command ascii SERIALCONFIG;COM2,115200,N,8,1,N,ON",
            "log ascii 5",
            "log ascii 1412",
            "command ascii LOG;COM2,BESTXYZB,ONCE,0,0,NOHOLD",
            "command ascii LOG;COM2,HWMONITORB,ONCE,0,0,NOHOLD",
            "command ascii LOG;COM2,PSRDOPB,ONCE,0,0,NOHOLD",
            "command ascii LOG;COM2,TIMEB,ONCE,0,0,NOHOLD",
            "command ascii UNLOGALL;COM2",
            "command ascii PPSCONTROL;enable,positive,1.0,1000"
        };

        // run test, first loop is number of iterations, second loop runs through the entire commandStrs vector
        for (int iter = 0; iter < iters; iter++){
            for (size_t cmd = 0; cmd < commandStrs.size(); cmd++){
                commandCtr++;
                poll_flag = cmdResponse;
                process_command(commandStrs[cmd],SEND_SOCKET);
                while(cmdResponse == poll_flag){
                do_poll();
                }       
            }
        }

        // print test results:
        std::cout << "Command Spam Test:" << std::endl;
                std::cout << "Total Commands Sent: " << commandCtr<<std::endl;
                std::cout << "Accepted Commands: " << accepted_cmds << std::endl;
                std::cout << "Total Responses Recieved: " << cmdResponse << std::endl; 
                std::cout << "Encode Errors: " << mssgEncodeErr << std::endl;
                std::cout << "Header Decode Errors: " << headerDecodeErr << std::endl;
                std::cout << "Message Decode Errors: " << mssgDecodeErr << std::endl;
                std::cout << std::endl;
        return;
    }


    void test_command_format(int& iters){
        int good_cmds = 0;
        int bad_cmds = 0;
        int prevCmdCt;
        int poll_flag;
        int serv_acc_cmds = 0;

        const std::vector<std::string> commandStrs = {
            "command ascii SERIALCONFIG;COM2,115200,N,8,1,N,ON",
            "log ascii 5",
            "log ascii 1412",
            "command ascii LOG;COM2,BESTXYZB,ONCE,0,0,NOHOLD",
            "command ascii LOG;COM2,HWMONITORB,ONCE,0,0,NOHOLD",
            "command ascii LOG;COM2,PSRDOPB,ONCE,0,0,NOHOLD",
            "command ascii LOG;COM2,TIMEB,ONCE,0,0,NOHOLD",
            "command ascii UNLOGALL;COM2",
            "command ascii PPSCONTROL;enable,positive,1.0,1000"
        };

        const std::vector<std::string> bad_commandStrs = {
            "command SERIALCONFIG;COM2,115200,N,8,1,N,ON",
            "log ascii 500000",
            "ascii 1412",
            "command asci k;COM2,BESTYZB,ONCE,0,0,NOOLD",
            "comand ascii LOC;OM2,HRB",
            "command ascii LOG;COM2,PSOPB,ONCE",
            "command ascii LOG;COM2,OCE,0,0,NOHLD",
            "command ascii UNALL;COm",
            "command ascii PPSCONTROL;posive,,1000"
        };

        for (int iter = 0; iter<iters;iter++){
            // loop through commandStrs vector
            for(size_t good_command = 0; good_command < commandStrs.size();good_command++){
                prevCmdCt = accepted_cmds;
                process_command(commandStrs[good_command],SEND_SOCKET);
                good_cmds++;
                if (prevCmdCt != accepted_cmds){
                    serv_acc_cmds++;
                    poll_flag = cmdResponse;
                    while(poll_flag == cmdResponse){
                        do_poll();
                    }
                }
            }

            // loop through bad_commandStrs vector
            for(size_t bad_command = 0; bad_command < bad_commandStrs.size();bad_command++){
                prevCmdCt = accepted_cmds;
                process_command(bad_commandStrs[bad_command],SEND_SOCKET);
                bad_cmds++;
                if (prevCmdCt != accepted_cmds){
                    serv_acc_cmds++;
                    poll_flag = cmdResponse;
                    while(poll_flag == cmdResponse){
                        do_poll();
                    }
                }
            }
        }

        // print test results:
        std::cout << "Format Spam Test:" << std::endl;
        std::cout << "Total Commands Sent: "<<(good_cmds+bad_cmds)<<std::endl;
        std::cout << "Total GPS Responses: " << cmdResponse << std::endl;
        std::cout << "Properly Formatted Messages Sent: " << good_cmds << std::endl;
        std::cout << "Improperly Formatted Messgaes Sent: " << bad_cmds << std::endl;
        std::cout << "Commands returned from GPS with no errors: " << accepted_cmds << std::endl;
        std::cout << "commands accepted by the service: " << serv_acc_cmds << std::endl;
        std::cout << "commands rejected by the service: " << (good_cmds+bad_cmds)-serv_acc_cmds << std::endl;
        std::cout << "Encode Errors: " << mssgEncodeErr << std::endl;
        std::cout << "Header Decode Errors: " << headerDecodeErr << std::endl;
        std::cout << "Message Decode Erros: " << mssgDecodeErr << std::endl;  
        std::cout << std::endl;
        return;
    }


    void test_health_packet(int numPackets){
        int poll_flag;

        // command string vector
        const std::vector<std::string> set_packet_logs = {
            "command ascii SERIALCONFIG;COM2,115200,N,8,1,N,ON",
            "command ascii LOG,THISPORT,0,0,UNKNOWN,0,0.0,0,0;COM2,BESTXYZB,ONTIME,1,0,NOHOLD",
            "command ascii LOG;COM2,HWMONITORB,ONTIME,1,0,NOHOLD",
            "command ascii LOG;COM2,PSRDOPB,ONTIME,1,0,NOHOLD",
            "command ascii LOG;COM2,TIMEB,ONTIME,1,0,NOHOLD",
        };

        // gather responses from the buffer
        for (size_t cmd = 0; cmd < set_packet_logs.size(); cmd++){
            poll_flag = cmdResponse;
            process_command(set_packet_logs[cmd],SEND_SOCKET);
            while(cmdResponse == poll_flag){
            do_poll();
            }       
        }

        // make do_poll() loop duration the same time as the number of packets we want to compile
        // packets should be compiled every 1 second
        auto duration = std::chrono::seconds(numPackets);
        auto startTime = std::chrono::steady_clock::now();
        while((std::chrono::steady_clock::now() - startTime) < duration){
            do_poll();
        }

        // unlog all the logs we dont want permanently
        const std::string unlog = "command ascii UNLOGALL;COM2";  
        process_command(unlog,SEND_SOCKET);
        poll_flag = cmdResponse;
        while(poll_flag == cmdResponse){
            do_poll();
        }

        // print results
        std::cout<<"Health Packet Compliation Test: \n";
        std::cout << "Number of Packets that should have been sent: " << numPackets<<std::endl;
        std::cout << "Number of packets that were sent: " << packetCt << std::endl;
        std::cout << "header decode errors: " << headerDecodeErr << std::endl;
        std::cout << "message decode errors: " << mssgDecodeErr << std::endl;
        std::cout << "message encode error: " << mssgEncodeErr << std::endl;
    }

    void test_nmea_sentc(int num){
        int poll_flag;
        const std::vector<std::string> nmea_logs = {
            "command ascii LOG;GPRMCB,ONTIME,2,0,NOHOLD",
            "command ascii LOG;GPGSAB,ONTIME,2,0,NOHOLD"
        };

        // gather responses from the buffer
        for (size_t cmd = 0; cmd < nmea_logs.size(); cmd++){
            poll_flag = cmdResponse;
            process_command(nmea_logs[cmd],SEND_SOCKET);
            while(cmdResponse == poll_flag){
            do_poll();
            }       
        }
        
        auto duration = std::chrono::seconds(num);
        auto startTime = std::chrono::steady_clock::now();
        
        while ((std::chrono::steady_clock::now() - startTime) < duration){
            do_poll();
        }

        process_command("command ascii unlogall;com2",SEND_SOCKET);
        poll_flag = cmdResponse;
        while(poll_flag == cmdResponse){
            do_poll();
        }

        std::cout<<"NMEA sentence test:\n";
        std::cout<<"Number of commands sent: 3\n";
        std::cout<<"Number of commands received: "<<accepted_cmds<<std::endl;
        std::cout<<"Number of command responses: "<<cmdResponse<<std::endl;
        std::cout<<"Number of NMEA sentences received: "<<recNmea<<std::endl;
        std::cout<<"Number of NMEA sentences expected: "<<num<<std::endl;

    }
};


/* ******* Test Arguments are shown *******
-c  -- spams commands for our used NovAtel command ids
-f  -- sends improperly formated messages to gps_request
-h  -- compiles health packets VERY fast
*/
int main(int argc, char* argv[]) {
    Logger::InitLogger();
    //ports
    const udp::SocketAddress SEND_SOCKET = udp::SocketAddress::create("127.0.0.1:55001");
    const udp::SocketAddress GPS_SERVICE_SOCKET = udp::SocketAddress::create("0.0.0.0:55000");
    const udp::SocketAddress NMEA_SOCKET = udp::SocketAddress::create("0.0.0.0:2947");
    const udp::SocketAddress PPS_CONFIG = udp::SocketAddress::create("127.0.0.1:55004");
    
    // for use in GpsService Constructor
    auto options = gps::parseGPSCmdOpts(argc, argv);
    auto subsystem_socket = std::make_shared<udp::Socket>(GPS_SERVICE_SOCKET);

    using namespace std::chrono_literals;
    subsystem_socket->set_timeout(1s);

    gps::GpsServiceOptions gps_service_options {
        SEND_SOCKET,
        NMEA_SOCKET,
        PPS_CONFIG,
    };

    // check for input arguments for specific tests
    for (int i=1;i<argc;i++){
        std::string arg = argv[i];
        // create a new object each time a test is run, this resets the counts by calling the GpsService constructor within the 
        // TestService constructor.
        TestGPSService test_service = TestGPSService(subsystem_socket,gps_service_options,options);
        if (arg == "c"){
            int iters = 8;
            test_service.test_command_ids(iters);
        }
        else if (arg == "f"){
            int iters = 5;
            test_service.test_command_format(iters);
        }
        else if (arg == "h"){
            int packetNum = 10;
            test_service.test_health_packet(packetNum);
        }
        else if (arg == "n"){
            int stcNum = 10;
            test_service.test_nmea_sentc(stcNum);
        }
        else{
            std::cout << "Invalid argument\n";
        }
    }
}