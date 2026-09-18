#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sstream>
#include <sys/ioctl.h>
#include <thread>

#include "picojson.h"

#include "../util/environment_util.h"
#include "../util/status_util.h"
#include "adcs_service.h"
#include "messages_json.h"
#include "version.h"

const udp::SocketAddress SEND_SOCKET = udp::SocketAddress::create("127.0.0.1", atoi(util::getenv_required("ADCS_UDPCAPTURE_PORT")));
const udp::SocketAddress SEND_COMM_SOCKET = udp::SocketAddress::create("127.0.0.1", atoi(util::getenv_required("ADCS_COMMISSION_PORT")));
const udp::SocketAddress ADCS_SERVICE_SOCKET = udp::SocketAddress::create("0.0.0.0", atoi(util::getenv_required("ADCS_COMMAND_PORT")));

const int HEALTH_PACKET_DELAY_SECONDS = util::getenv_int("ADCS_HP_DELAY_S", 10);

// define boolean for turning Commissioning Packet on and off (default off)
bool Comm_packet = 0;

int main() {
    //Here we define the subsystem socket that we will use
    auto subsystem_socket = std::make_shared<udp::Socket>(ADCS_SERVICE_SOCKET);
    auto new_timeout =  std::chrono::seconds(1);
    subsystem_socket->set_timeout(std::chrono::seconds{new_timeout});
    auto adcs_service = adcs::AdcsService(subsystem_socket);
    std::cout << "Nebula version: " << NEBULA_VERSION << std::endl;
    auto initial = std::chrono::steady_clock::now();
    auto com_init = std::chrono::steady_clock::now();

    while (true){
        try {
            std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - initial);
            std::chrono::seconds com_dur = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - com_init);
            //If the loop has been running longer than HEALTH_PACKET_DELAY_SECONDS since last health packet
            if (duration >= std::chrono::seconds(HEALTH_PACKET_DELAY_SECONDS)) {
                initial = std::chrono::steady_clock::now();
                
                //Get the health packet
                adcs_service.getADCShealthPacket();
            }

            if (com_dur >= std::chrono::seconds(1) && Comm_packet) {
                com_init = std::chrono::steady_clock::now();
                
                // Get the commissioning packet 
               adcs_service.getCommissioningPacket();
            }

            auto sender = udp::SocketAddress {};
            auto opt_packet = subsystem_socket->receive(sender);
            if (!opt_packet) {
                // Read timed out.
                continue;
            }

            // Process packet and send response.
            auto command_string = std::string(opt_packet->data(), opt_packet->size());

            // clean exit
            if (command_string == "exit"){
                std::cout << "exited cleanly\n";
                return 0;
            }

            auto response = adcs_service.processPacket(command_string);
            subsystem_socket->send(response, sender);
            
        } catch (const std::exception& e) {
            std::cerr << "adcs service exception caught: " << e.what() << std::endl;
        } 
    }
}

adcs::AdcsService::AdcsService(std::shared_ptr<udp::Socket> s) {
    socket_ptr = s;
    int i2c_file = open("/dev/i2c-1", O_RDWR);
    if (i2c_file < 0) {
        std::cout << "Could not open I2C file: "<< strerror(errno) << std::endl;
        exit(1);
    }
    if (ioctl(i2c_file, I2C_SLAVE, 0x57) < 0) {
        std::cout << "Could not set I2C_SLAVE: " << strerror(errno) << std::endl;
        exit(1);
    }
    driver = std::make_shared<CubeSpaceDriver>(i2c_file);

    picojson::value tables_json;
    picojson::parse(tables_json, adcs::messages::get_messages_json());
    auto cube_computer_structs = CubeSpace::TablesJson::read_structs(tables_json.get<picojson::array>());
    for (auto const& cube_computer_struct : cube_computer_structs) {
        if (cube_computer_struct.set_id) {
            command_formats[*cube_computer_struct.set_id] = cube_computer_struct;
        }
        if (cube_computer_struct.get_id) {
            telemetry_formats[*cube_computer_struct.get_id] = cube_computer_struct;
        }
    }
}

void adcs::AdcsService::getADCShealthPacket() {
    auto adcs_unix_time = driver->read<CubeSpace::CurrentUnixTimeMessageFormat>();
    // auto adcs_state = driver->read<CubeSpace::CurrentADCSStateTelemetryFormat>();
    auto adcs_state = driver->read <CubeSpace::ADCSStateTelemetryFormat>();
    auto adcs_measurements = driver->read<CubeSpace::ADCSMeasurementsTelemetryFormat>();
    auto estimation_data = driver->read<CubeSpace::EstimationDataTelemetryFormat>();
    auto power_temp_measurements = driver->read<CubeSpace::PowerandTemperatureMeasurementsTelemetryFormat>();

    adcs::ADCSHealth health_packet{
        .CurrentUnixTime = adcs_unix_time.CurrentUnixTime,
        // Health V2 added between here
        .ADCSRunMode = static_cast<uint8_t>(adcs_state.ADCSRunMode),
        .CubeControlSignalEnabled = adcs_state.CubeControlSignalEnabled,
        .CubeControlMotorEnabled = adcs_state.CubeControlMotorEnabled,
        .AttitudeEstimationMode = static_cast<uint8_t>(adcs_state.AttitudeEstimationMode),
        .ControlMode = static_cast<uint8_t>(adcs_state.ControlMode),
        .CubeSense1Enabled = adcs_state.CubeSense1Enabled, 
        .CubeSense2Enabled = adcs_state.CubeSense2Enabled, 
        .CubeWheel1Enabled = adcs_state.CubeWheel1Enabled, 
        .CubeWheel2Enabled = adcs_state.CubeWheel2Enabled, 
        .CubeWheel3Enabled = adcs_state.CubeWheel3Enabled, 
        .MotorDriverEnabled = adcs_state.MotorDriverEnabled,
        .ASGP4Mode = static_cast<uint8_t>(adcs_state.ASGP4Mode),
        // And here
        .SunisAboveLocalHorizon = adcs_state.SunisAboveLocalHorizon,
        .CubeSense1CommunicationsError = adcs_state.CubeSense1CommunicationsError,
        .CubeSense2CommunicationsError = adcs_state.CubeSense2CommunicationsError,
        .CubeControlSignalCommunicationsError = adcs_state.CubeControlSignalCommunicationsError,
        .CubeControlMotorCommunicationsError = adcs_state.CubeControlMotorCommunicationsError,
        .CubeWheel1CommunicationsError = adcs_state.CubeWheel1CommunicationsError,
        .CubeWheel2CommunicationsError = adcs_state.CubeWheel2CommunicationsError,
        .CubeWheel3CommunicationsError = adcs_state.CubeWheel3CommunicationsError,
        .MagnetometerRangeError = adcs_state.MagnetometerRangeError,
        .Cam1SRAMOvercurrentDetected = adcs_state.Cam1SRAMOvercurrentDetected,
        .Cam13V3OvercurrentDetected = adcs_state.Cam13V3OvercurrentDetected,
        .SunSensorRangeError = adcs_state.SunSensorRangeError,
        .Cam2SRAMOvercurrentDetected = adcs_state.Cam2SRAMOvercurrentDetected,
        .Cam23V3OvercurrentDetected = adcs_state.Cam23V3OvercurrentDetected,
        .RateSensorRangeError = adcs_state.RateSensorRangeError,
        .WheelSpeedRangeError = adcs_state.WheelSpeedRangeError, 
        .CoarseSunSensorError = adcs_state.CoarseSunSensorError, 
        .Modelledandmeasuredmagneticfielddiffersinsize = adcs_state.Modelledandmeasuredmagneticfielddiffersinsize, 
        .NodeRecoveryError = adcs_state.NodeRecoveryError, 
        .CubeSense1RuntimeError = adcs_state.CubeSense1RuntimeError, 
        .CubeSense2RuntimeError = adcs_state.CubeSense2RuntimeError, 
        .CubeControlSignalRuntimeError = adcs_state.CubeControlSignalRuntimeError, 
        .CubeControlMotorRuntimeError = adcs_state.CubeControlMotorRuntimeError, 
        .CubeWheel1RuntimeError = adcs_state.CubeWheel1RuntimeError, 
        .CubeWheel2RuntimeError = adcs_state.CubeWheel2RuntimeError, 
        .CubeWheel3RuntimeError = adcs_state.CubeWheel3RuntimeError, 
        .MagnetometerError = adcs_state.MagnetometerError, 
        .RateSensorFailure = adcs_state.RateSensorFailure, 
        .Estimatedq1 = adcs_state.Estimatedq1, 
        .Estimatedq2 = adcs_state.Estimatedq2, 
        .Estimatedq3 = adcs_state.Estimatedq3, 
        // adcs measurments
        .MagneticFieldX = adcs_measurements.MagneticFieldX, 
        .MagneticFieldY = adcs_measurements.MagneticFieldY, 
        .MagneticFieldZ = adcs_measurements.MagneticFieldZ, 
        .XAngularRate = adcs_measurements.XAngularRate, 
        .YAngularRate = adcs_measurements.YAngularRate, 
        .ZAngularRate = adcs_measurements.ZAngularRate, 
        // estimated data
        .EstimatedXgyroBias = estimation_data.EstimatedXgyroBias, 
        .EstimatedYgyroBias = estimation_data.EstimatedYgyroBias, 
        .EstimatedZgyroBias = estimation_data.EstimatedZgyroBias, 
        .QuaternionErrorQ1 = estimation_data.QuaternionErrorQ1, 
        .QuaternionErrorQ2 = estimation_data.QuaternionErrorQ2, 
        .QuaternionErrorQ3 = estimation_data.QuaternionErrorQ3, 
        // power and temp measurements 
        .CubeControl3V3Current = power_temp_measurements.CubeControl3V3Current, 
        .CubeControl5VCurrent = power_temp_measurements.CubeControl5VCurrent, 
        .CubeControlVbatCurrent = power_temp_measurements.CubeControlVbatCurrent, 
        .Wheel1Current = power_temp_measurements.Wheel1Current,
        .Wheel2Current = power_temp_measurements.Wheel2Current,
        .Wheel3Current = power_temp_measurements.Wheel3Current,
        .MCUTemperature = power_temp_measurements.MCUTemperature,
        .MagnetometerTemp = power_temp_measurements.MagnetometerTemperature, 
        .RedundantMagnetometerTemp = power_temp_measurements.RedundantMagnetometerTemperature, 
        .XRateSensorTemp = power_temp_measurements.XRateSensorTemperature,
        .YRateSensorTemp = power_temp_measurements.YRateSensorTemperature,
        .ZRateSensorTemp = power_temp_measurements.ZRateSensorTemperature,
    };

    auto packet = udp::Packet::create_from_struct(
        health_packet,
        SEND_SOCKET.get_port(),
        ADCS_SERVICE_SOCKET.get_port(),
        health_packet_sequence_number++,
        0
    ).serialize();
    socket_ptr->send(packet, SEND_SOCKET);
}

void adcs::AdcsService::getCommissioningPacket() {
    auto adcs_unix_time = driver->read<CubeSpace::CurrentUnixTimeMessageFormat>();
    auto adcs_fine_estimated_ang_rates = driver->read<CubeSpace::FineEstimatedAngularRatesTelemetryFormat>();
    auto adcs_state = driver->read <CubeSpace::ADCSStateTelemetryFormat>();
    auto adcs_commanded_wheel_speed = driver->read <CubeSpace::WheelSpeedCommandsTelemetryFormat>();
    auto adcs_measurements = driver->read<CubeSpace::ADCSMeasurementsTelemetryFormat>();
    auto adcs_estimation_data = driver->read<CubeSpace::EstimationDataTelemetryFormat>();
    auto adcs_raw_sensor_measurements = driver->read<CubeSpace::RawSensorMeasurementsTelemetryFormat>();
    auto adcs_power_temp_measurements = driver->read<CubeSpace::PowerandTemperatureMeasurementsTelemetryFormat>();

    adcs::ADCSCommissioning commissioning_packet{
        .CurrentUnixTime = adcs_unix_time.CurrentUnixTime,
        .FineEstimatedXAngularRate = adcs_fine_estimated_ang_rates.EstimatedXAngularRate,
        .FineEstimatedYAngularRate = adcs_fine_estimated_ang_rates.EstimatedYAngularRate,
        .FineEstimatedZAngularRate = adcs_fine_estimated_ang_rates.EstimatedZAngularRate,
        .EstimatedXAngularRate = adcs_state.EstimatedXAngularRate,
        .EstimatedYAngularRate = adcs_state.EstimatedYAngularRate,
        .EstimatedZAngularRate = adcs_state.EstimatedZAngularRate,
        .EstimatedRollAngle = adcs_state.EstimatedRollAngle,
        .EstimatedPitchAngle = adcs_state.EstimatedPitchAngle,
        .EstimatedYawAngle = adcs_state.EstimatedYawAngle,
        .Latitude = adcs_state.Latitude,
        .Longitude = adcs_state.Longitude,
        .Altitude = adcs_state.Altitude,
        .MagX = adcs_raw_sensor_measurements.MagX,
        .MagY = adcs_raw_sensor_measurements.MagY,
        .MagZ = adcs_raw_sensor_measurements.MagZ,
        .CSS1 = adcs_raw_sensor_measurements.CSS1,
        .CSS2 = adcs_raw_sensor_measurements.CSS2,
        .CSS3 = adcs_raw_sensor_measurements.CSS3,
        .CSS4 = adcs_raw_sensor_measurements.CSS4,
        .CSS5 = adcs_raw_sensor_measurements.CSS5,
        .CSS6 = adcs_raw_sensor_measurements.CSS6,
        .CSS7 = adcs_raw_sensor_measurements.CSS7,
        .CSS8 = adcs_raw_sensor_measurements.CSS8,
        .CSS9 = adcs_raw_sensor_measurements.CSS9,
        .CSS10 = adcs_raw_sensor_measurements.CSS10,
        .Cam2centroidX = adcs_raw_sensor_measurements.Cam2centroidX,
        .Cam2centroidY = adcs_raw_sensor_measurements.Cam2centroidY,
        .Cam2Capturestatus = static_cast<uint8_t>(adcs_raw_sensor_measurements.Cam2Capturestatus),
        .Cam2Detectionresult = static_cast<uint8_t>(adcs_raw_sensor_measurements.Cam2Detectionresult),
        .Cam1centroidX = adcs_raw_sensor_measurements.Cam1centroidX,
        .Cam1centroidY = adcs_raw_sensor_measurements.Cam1centroidY,
        .Cam1Capturestatus = static_cast<uint8_t>(adcs_raw_sensor_measurements.Cam1Capturestatus),
        .Cam1Detectionresult = static_cast<uint8_t>(adcs_raw_sensor_measurements.Cam1Detectionresult),
        .CommandedXWheelSpeed = adcs_commanded_wheel_speed.CommandedXWheelSpeed,
        .CommandedYWheelSpeed = adcs_commanded_wheel_speed.CommandedYWheelSpeed, 
        .CommandedZWheelSpeed = adcs_commanded_wheel_speed.CommandedZWheelSpeed,
        .InnovationVectorX = adcs_estimation_data.InnovationVectorX,
        .InnovationVectorY = adcs_estimation_data.InnovationVectorY,
        .InnovationVectorZ = adcs_estimation_data.InnovationVectorZ,
        .IGRFModelledMagneticFieldX = adcs_estimation_data.IGRFModelledMagneticFieldX,
        .IGRFModelledMagneticFieldY = adcs_estimation_data.IGRFModelledMagneticFieldY,
        .IGRFModelledMagneticFieldZ = adcs_estimation_data.IGRFModelledMagneticFieldZ,
        .QuaternionErrorQ1 = adcs_estimation_data.QuaternionErrorQ1,
        .QuaternionErrorQ2 = adcs_estimation_data.QuaternionErrorQ2,
        .QuaternionErrorQ3 = adcs_estimation_data.QuaternionErrorQ3,
        .EstimatedXgyroBias = adcs_estimation_data.EstimatedXgyroBias,
        .EstimatedYgyroBias = adcs_estimation_data.EstimatedYgyroBias,
        .EstimatedZgyroBias = adcs_estimation_data.EstimatedZgyroBias,
        .XWheelSpeed = adcs_measurements.XWheelSpeed,
        .YWheelSpeed = adcs_measurements.YWheelSpeed,
        .ZWheelSpeed = adcs_measurements.ZWheelSpeed,
        .MagneticFieldX = adcs_measurements.MagneticFieldX,
        .MagneticFieldY = adcs_measurements.MagneticFieldY,
        .MagneticFieldZ = adcs_measurements.MagneticFieldZ,
        .XAngularRate = adcs_measurements.XAngularRate,
        .YAngularRate = adcs_measurements.YAngularRate,
        .ZAngularRate = adcs_measurements.ZAngularRate,
        .SunX = adcs_measurements.SunX,
        .SunY = adcs_measurements.SunY,
        .SunZ = adcs_measurements.SunZ,
        .NadirX = adcs_measurements.NadirX,
        .NadirY = adcs_measurements.NadirY,
        .NadirZ = adcs_measurements.NadirZ,
        .CubeControl3V3Current = adcs_power_temp_measurements.CubeControl3V3Current,
        .CubeControl5VCurrent = adcs_power_temp_measurements.CubeControl5VCurrent,
        .CubeControlVbatCurrent = adcs_power_temp_measurements.CubeControlVbatCurrent,
    };

    auto packet = udp::Packet::create_from_struct(
        commissioning_packet,
        SEND_COMM_SOCKET.get_port(),
        ADCS_SERVICE_SOCKET.get_port(),
        commissioning_packet_sequence_number++,
        0
    ).serialize();
    socket_ptr->send(packet, SEND_COMM_SOCKET);
}
std::vector<char> adcs::AdcsService::string_response(std::string message) {
    message = "ack-ok\n" + message;
    std::vector<char> response(message.size());
    memcpy(response.data(), message.data(), message.size());
    return response;
}

std::vector<char> adcs::AdcsService::error_response(std::string message) {
    message = "error\n" + message;
    std::vector<char> response(message.size());
    memcpy(response.data(), message.data(), message.size());
    return response;
}

std::vector<char> adcs::AdcsService::processPacket(std::string command_string) {
    try {

       if (command_string.starts_with("Comm Packet On"))
       {
            std::vector<char> comm;
            Comm_packet = 1;
	    std::string str = "Commissioning packet on";
	    comm.assign(str.begin(), str.end());
	    return comm;
       }
       if (command_string.starts_with("Comm Packet Off"))
       {
            std::vector<char> comm;
	    Comm_packet = 0;
	    std::string str = "Commissioning packet off";
	    comm.assign(str.begin(), str.end());
	    return comm;
	}
        // Parse the string command.
        if (command_string.starts_with("telemetry binary ")) {
            // Telemetry request.
            std::string prefix = "telemetry binary ";
            auto id = std::stoi(command_string.substr(prefix.size()));
            return processPacket_telemetry(id);
        }

        if (command_string.starts_with("telemetry json ")) {
            // Telemetry request.  JSON response.
            std::string prefix = "telemetry json ";
            auto id = std::stoi(command_string.substr(prefix.size()));
            return processPacket_telemetry_json(id);
        }

        if (command_string.starts_with("command binary ")) {
            // Command in JSON format.

            // Strip off "command "
            std::string prefix = "command binary ";
            auto id_and_data = command_string.substr(prefix.size());

            // Split into ID and binary data
            auto space_pos = id_and_data.find_first_of(' ');
            if (space_pos == std::string::npos) {
                return error_response("Expected 'command <ID> <binary data>'");
            }

            auto id = std::stoi(id_and_data.substr(0, space_pos));
            auto data = id_and_data.substr(space_pos + 1);
            return processPacket_command(id, data);
        }

        if (command_string.starts_with("command json ")) {
            // Command in JSON format.
            std::string prefix = "command json ";
            auto id_and_data = command_string.substr(prefix.size());

            // Split into ID and JSON string
            auto space_pos = id_and_data.find_first_of(' ');
            if (space_pos == std::string::npos) {
                return error_response("Expected 'command json <ID> <JSON data>'");
            }

            auto id = std::stoi(id_and_data.substr(0, space_pos));
            auto json_str = id_and_data.substr(space_pos + 1);
	    
	    return processPacket_command_json(id, json_str);
        }

        // Could not parse the string command.
        return error_response("Could not parse command: " + command_string);
    }
    catch (std::exception const& e) {
        // Error occurred handling packet.
        std::stringstream response_message_ss;
        response_message_ss << "Error occurred handling packet: " << e.what() << std::endl;
        return error_response(response_message_ss.str());
    }
}

std::vector<char> adcs::AdcsService::processPacket_telemetry(int id) {
    if (!telemetry_formats.count(id)) {
        return error_response("No such Telemetry ID: " + std::to_string(id));
    }

    // Create a buffer of the correct size for the telemetry ID we are receiving
    auto size = telemetry_formats[id].length_bytes;
    std::vector<char> result(size);

    driver->read(id, result);

    return result;
}

std::vector<char> adcs::AdcsService::processPacket_telemetry_json(int id) {
    // Call "binary" telemetry function with ID
    auto bytes = processPacket_telemetry(id);

    // Convert binary data into a JSON string...

    // Get the telemetry format
    auto telemetry_format = telemetry_formats.at(id);

    // Convert byte array to an array of "field values" using the telemetry format
    auto field_values = CubeSpace::TablesJson::from_byte_array(bytes, telemetry_format);

    // Add each "field value" to a JSON object
    picojson::object obj;
    for (size_t i = 0; i < telemetry_format.fields.size(); i++) {
        obj[telemetry_format.fields[i].name] = CubeSpace::TablesJson::cube_computer_field_value_to_json(field_values[i]);
    }

    // send JSON string
    return string_response(picojson::value(obj).serialize());
}

std::vector<char> adcs::AdcsService::processPacket_command(int id, std::span<char> binary_data) {
    if (!command_formats.count(id)) {
        return error_response("No such Command ID: " + std::to_string(id));
    }

    // Check that data size is correct for the command ID we are sending
    auto size = command_formats[id].length_bytes;
    if (binary_data.size() != size) {
        std::stringstream error_message;
        error_message << "Command data had an incorrect size.  Expected length: "
                      << size
                      << ".  Actual length: "
                      << binary_data.size();
        return error_response(error_message.str());
    }

    driver->write(id, binary_data);

    // Poll for up to 2 seconds waiting for command to be acknowledged
    CubeSpace::TelecommandAcknowledgeTelemetryFormat tc_ack {};
    for (size_t i = 0; i < 20; i++) {
        tc_ack = driver->read<CubeSpace::TelecommandAcknowledgeTelemetryFormat>();
        if (tc_ack.Processedflag == 1) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (tc_ack.Processedflag != 1 || static_cast<int>(tc_ack.TCerrorstatus) != 0) {
        std::stringstream error_message;
        error_message << "CubeComputer could not acknowledge the command."
                      << "\n\tLast Telecommand ID: " << tc_ack.LastTCID
                      << "\n\tProcessed flag: " << tc_ack.Processedflag
                      << "\n\tTC error status: " << static_cast<int>(tc_ack.TCerrorstatus)
                      << "\n\tTC parameter error index: " << tc_ack.TCparametererrorindex;
        return error_response(error_message.str());
    }

    return string_response("Successfully processed command: " + std::to_string(id));
}

std::vector<char> adcs::AdcsService::processPacket_command_json(int id, std::string json_data) {
    if (!command_formats.count(id)) {
        return error_response("No such Command ID: " + std::to_string(id));
    }

    auto command_format = command_formats.at(id);

    picojson::value json;
    auto err = picojson::parse(json, json_data);
    if (!err.empty()) {
        return error_response("JSON could not be parsed.  Error: " + err);
    }

    if (!json.is<picojson::object>()) {
        return error_response("Expected command data to be a JSON object");
    }
    auto json_obj = json.get<picojson::object>();

    // Convert from JSON object to an array of "message field values"
    std::vector<std::optional<CubeSpace::TablesJson::CubeComputerFieldValue>> field_values;
    for (auto const& field : command_format.fields) {
        // A missing field will be passed to the CubeComputer as 0's
        if (!json_obj.contains(field.name)) {
            field_values.push_back({});
            continue;
        }

        auto json_val = json_obj[field.name];
        switch (field.type) {
        case CubeSpace::TablesJson::CubeComputerDataType::Uint:
            if (!json_val.is<double>()) {
                return error_response("Expected double for field: " + field.name);
            }
            field_values.push_back(static_cast<uint64_t>(json_val.get<double>()));
            break;
        case CubeSpace::TablesJson::CubeComputerDataType::Int:
        case CubeSpace::TablesJson::CubeComputerDataType::Enum:
        case CubeSpace::TablesJson::CubeComputerDataType::Bool:
            if (!json_val.is<double>()) {
                return error_response("Expected double for field: " + field.name);
            }
            field_values.push_back(static_cast<int64_t>(json_val.get<double>()));
            break;
        case CubeSpace::TablesJson::CubeComputerDataType::Array:
            if (!json_val.is<std::string>()) {
                return error_response("Expected string for field: " + field.name);
            }
            field_values.push_back(json_val.get<std::string>());
            break;
        case CubeSpace::TablesJson::CubeComputerDataType::Float:
            if (!json_val.is<double>()) {
                return error_response("Expected double for field: " + field.name);
            }
            field_values.push_back(static_cast<float>(json_val.get<double>()));
            break;
        case CubeSpace::TablesJson::CubeComputerDataType::Double:
            if (!json_val.is<double>()) {
                return error_response("Expected double for field: " + field.name);
            }
            field_values.push_back(json_val.get<double>());
            break;
        }
    }

    // Convert array of "message field values" to byte array using the command format
    auto bytes = CubeSpace::TablesJson::to_byte_array(field_values, command_format);
    return processPacket_command(id, bytes);
}
