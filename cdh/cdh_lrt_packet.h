#ifndef CDH_LRT_PACKET_HEADER
#define CDH_LRT_PACKET_HEADER

#include <cstdint>
#include "cdh_health_packet.h"
#include "../eps/eps_health.h"
#include "../gps/gps_health.h"
#include "DetectorMessages.hh"

namespace cdh {
    #pragma pack(push, 1)
    struct CDHLRTExact{
        cdh::CdhExactHealth cdh;

        udp::PacketHeader eps_header;
        eps::EpsPDUP4Health eps;

        udp::PacketHeader gps_header;
        gps::GpsHealth gps;

        udp::PacketHeader det_header;
        DetectorMessages::HealthPacket det;
    };
    struct CDHLRTImpress{
        cdh::CdhImpressHealth cdh;

        udp::PacketHeader eps_header;
        eps::EpsImpressHealth eps;

        udp::PacketHeader gps_header;
        gps::GpsHealth gps;

        udp::PacketHeader det_header;
        DetectorMessages::HealthPacket det;
    };

    #pragma pack(pop)
}
#endif