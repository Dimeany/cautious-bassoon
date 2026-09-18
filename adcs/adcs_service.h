#include <unordered_map>
#include <utility>

#include "tables_json.h"
#include "cube_space_driver.h"
#include "socket.h"
#include "udp.h"
#include "adcs_health.h"

namespace adcs
{
    // #pragma pack(push,1)
    // struct ADCSHealth // Health packet
    // {
    //   uint32_t CurrentUnixTime;
    //   uint8_t ADCSRunMode : 2;
    //   uint8_t CubeControlSignalEnabled : 1;
    //   uint8_t CubeControlMotorEnabled : 1;
    //   uint8_t AttitudeEstimationMode : 4;
    //   uint8_t ControlMode : 4;
    //   uint8_t CubeSense1Enabled : 1;
    //   uint8_t CubeSense2Enabled : 1;
    //   uint8_t CubeWheel1Enabled : 1;
    //   uint8_t CubeWheel2Enabled : 1;
    //   uint8_t CubeWheel3Enabled : 1;
    //   uint8_t MotorDriverEnabled : 1;
    //   uint8_t ASGP4Mode : 2;
    //   uint8_t SunisAboveLocalHorizon : 1;
    //   uint8_t CubeSense1CommunicationsError : 1;
    //   uint8_t CubeSense2CommunicationsError : 1;
    //   uint8_t CubeControlSignalCommunicationsError : 1;
    //   uint8_t CubeControlMotorCommunicationsError : 1;
    //   uint8_t CubeWheel1CommunicationsError : 1;
    //   uint8_t CubeWheel2CommunicationsError : 1;
    //   uint8_t CubeWheel3CommunicationsError : 1;
    //   uint8_t MagnetometerRangeError : 1;
    //   uint8_t Cam1SRAMOvercurrentDetected : 1;
    //   uint8_t Cam13V3OvercurrentDetected : 1;
    //   uint8_t SunSensorRangeError : 1;
    //   uint8_t Cam2SRAMOvercurrentDetected : 1;
    //   uint8_t Cam23V3OvercurrentDetected : 1;
    //   uint8_t RateSensorRangeError : 1;
    //   uint8_t WheelSpeedRangeError : 1;
    //   uint8_t CoarseSunSensorError : 1;
    //   uint8_t Modelledandmeasuredmagneticfielddiffersinsize : 1;
    //   uint8_t NodeRecoveryError : 1;
    //   uint8_t CubeSense1RuntimeError : 1;
    //   uint8_t CubeSense2RuntimeError : 1;
    //   uint8_t CubeControlSignalRuntimeError : 1;
    //   uint8_t CubeControlMotorRuntimeError : 1;
    //   uint8_t CubeWheel1RuntimeError : 1;
    //   uint8_t CubeWheel2RuntimeError : 1;
    //   uint8_t CubeWheel3RuntimeError : 1;
    //   uint8_t MagnetometerError : 1;
    //   uint8_t RateSensorFailure : 1;
    //   int16_t Estimatedq1;
    //   int16_t Estimatedq2;
    //   int16_t Estimatedq3;
    //   int16_t MagneticFieldX;
    //   int16_t MagneticFieldY;
    //   int16_t MagneticFieldZ;
    //   int16_t XAngularRate;
    //   int16_t YAngularRate;
    //   int16_t ZAngularRate;
    //   int16_t EstimatedXgyroBias;
    //   int16_t EstimatedYgyroBias;
    //   int16_t EstimatedZgyroBias;
    //   int16_t QuaternionErrorQ1;
    //   int16_t QuaternionErrorQ2;
    //   int16_t QuaternionErrorQ3;
    //   uint16_t CubeControl3V3Current;
    //   uint16_t CubeControl5VCurrent;
    //   uint16_t CubeControlVbatCurrent;
    //   uint16_t Wheel1Current;
    //   uint16_t Wheel2Current;
    //   uint16_t Wheel3Current;
    //   int16_t MCUTemperature;
    //   int16_t MagnetometerTemp;
    //   int16_t RedundantMagnetometerTemp;
    //   int16_t XRateSensorTemp;
    //   int16_t YRateSensorTemp;
    //   int16_t ZRateSensorTemp;
    // };
    // #pragma pack(pop)
    
    // #pragma pack(push,1)
    // struct ADCSCommissioning // Commissioning packet
    // {
    //     uint32_t CurrentUnixTime;
    //     int16_t FineEstimatedXAngularRate;
    //     int16_t FineEstimatedYAngularRate;
    //     int16_t FineEstimatedZAngularRate;
    //     int16_t EstimatedXAngularRate;
    //     int16_t EstimatedYAngularRate;
    //     int16_t EstimatedZAngularRate;
    //     int16_t EstimatedRollAngle;
    //     int16_t EstimatedPitchAngle;
    //     int16_t EstimatedYawAngle;
    //     int16_t Latitude;
    //     int16_t Longitude;
    //     uint16_t Altitude;
    //     int16_t MagX;
    //     int16_t MagY;
    //     int16_t MagZ;
    //     uint8_t CSS1;
    //     uint8_t CSS2;
    //     uint8_t CSS3;
    //     uint8_t CSS4;
    //     uint8_t CSS5;
    //     uint8_t CSS6;
    //     uint8_t CSS7;
    //     uint8_t CSS8;
    //     uint8_t CSS9;
    //     uint8_t CSS10;
    //     int16_t Cam2centroidX;
    //     int16_t Cam2centroidY;
    //     uint8_t Cam2Capturestatus;
    //     uint8_t Cam2Detectionresult;
    //     int16_t Cam1centroidX;
    //     int16_t Cam1centroidY;
    //     uint8_t Cam1Capturestatus;
    //     uint8_t Cam1Detectionresult;
    //     int16_t CommandedXWheelSpeed;
    //     int16_t CommandedYWheelSpeed;
    //     int16_t CommandedZWheelSpeed;
    //     int16_t InnovationVectorX;
    //     int16_t InnovationVectorY;
    //     int16_t InnovationVectorZ;
    //     int16_t IGRFModelledMagneticFieldX;
    //     int16_t IGRFModelledMagneticFieldY;
    //     int16_t IGRFModelledMagneticFieldZ;
    //     int16_t QuaternionErrorQ1;
    //     int16_t QuaternionErrorQ2;
    //     int16_t QuaternionErrorQ3;
    //     int16_t EstimatedXgyroBias;
    //     int16_t EstimatedYgyroBias;
    //     int16_t EstimatedZgyroBias;
    //     int16_t XWheelSpeed;
    //     int16_t YWheelSpeed;
    //     int16_t ZWheelSpeed;
    //     int16_t MagneticFieldX;
    //     int16_t MagneticFieldY;
    //     int16_t MagneticFieldZ;
    //     int16_t XAngularRate;
    //     int16_t YAngularRate;
    //     int16_t ZAngularRate;
    //     int16_t SunX;
    //     int16_t SunY;
    //     int16_t SunZ;
    //     int16_t NadirX;
    //     int16_t NadirY;
    //     int16_t NadirZ;
    //     uint16_t CubeControl3V3Current;
    //     uint16_t CubeControl5VCurrent;
    //     uint16_t CubeControlVbatCurrent;
    //     };
    // #pragma pack(pop)

    // adcs Service Class
    class AdcsService {
        private:
            std::shared_ptr<udp::Socket> socket_ptr;

            std::unordered_map<int, CubeSpace::TablesJson::CubeComputerStruct> command_formats;
            std::unordered_map<int, CubeSpace::TablesJson::CubeComputerStruct> telemetry_formats;

            std::shared_ptr<CubeSpaceDriver> driver;

            std::vector<char> string_response(std::string message);
            std::vector<char> error_response(std::string message);

            uint16_t health_packet_sequence_number = 0;
            uint16_t commissioning_packet_sequence_number = 0;
            
        public:
            // fill in later 
            AdcsService(std::shared_ptr<udp::Socket> s);
            void getADCShealthPacket();
            void getCommissioningPacket();
            // Process incoming packets.
            std::vector<char> processPacket(std::string command_string);
            std::vector<char> processPacket_telemetry(int id);
            std::vector<char> processPacket_telemetry_json(int id);
            std::vector<char> processPacket_command(int id, std::span<char> binary_data);
            std::vector<char> processPacket_command_json(int id, std::string json_data);
    };
}