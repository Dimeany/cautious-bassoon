#include "cdh_health_packet.h"

// std::ostream& operator<<(std::ostream& os, cdh::HealthPacket const& hp){
//     os << "posX: " <<  hp.posX << "\n";
//     os << "posY: " <<  hp.posY << "\n";
//     os << "posZ: " <<  hp.posZ << "\n";
//     os << "velX: " <<  hp.velX << "\n";
//     os << "velY: " <<  hp.velY << "\n";
//     os << "velZ: " <<  hp.velZ << "\n";
//     os << "ADCS run mode: " <<  (unsigned int) hp.ADCS_run_mode << "\n";
//     os << "eps temp: " <<  hp.eps_temp << "\n";
//     os << "eps temp1: " <<  hp.eps_temp1 << "\n";
//     os << "eps temp0: " <<  hp.eps_temp0 << "\n";
//     std::string ch_on_string;
//     for (unsigned int i = 0; i < 12; i++){
//         ch_on_string += std::to_string((hp.ch_on & (1 << i))) + " ";
//     }
//     os << "dfl_on: " << ch_on_string << "\n";
//     std::string dfl_on_string;
//     for (unsigned int i = 0; i < 12; i++){
//         dfl_on_string += std::to_string((hp.dfl_on & (1 << i))) + " ";
//     }
//     os << "vcur: " << std::to_string(hp.tlm_vcur[0]) + " " +
//                                 std::to_string(hp.tlm_vcur[1]) + " " +
//                                 std::to_string(hp.tlm_vcur[2]) + " " +
//                                 std::to_string(hp.tlm_vcur[3]) + " " +
//                                 std::to_string(hp.tlm_vcur[4]) + " " +
//                                 std::to_string(hp.tlm_vcur[5]) + " " +
//                                 std::to_string(hp.tlm_vcur[6]) + " " +
//                                 std::to_string(hp.tlm_vcur[7]) + " " +
//                                 std::to_string(hp.tlm_vcur[8]) + " " +
//                                 std::to_string(hp.tlm_vcur[9]) + " " +
//                                 std::to_string(hp.tlm_vcur[10]) + " " +
//                                 std::to_string(hp.tlm_vcur[11]) + " "
//     << "\n";
//     os << "icur: " << std::to_string(hp.tlm_icur[0]) + " " +
//                                 std::to_string(hp.tlm_icur[1]) + " " +
//                                 std::to_string(hp.tlm_icur[2]) + " " +
//                                 std::to_string(hp.tlm_icur[3]) + " " +
//                                 std::to_string(hp.tlm_icur[4]) + " " +
//                                 std::to_string(hp.tlm_icur[5]) + " " +
//                                 std::to_string(hp.tlm_icur[6]) + " " +
//                                 std::to_string(hp.tlm_icur[7]) + " " +
//                                 std::to_string(hp.tlm_icur[8]) + " " +
//                                 std::to_string(hp.tlm_icur[9]) + " " +
//                                 std::to_string(hp.tlm_icur[10]) + " " +
//                                 std::to_string(hp.tlm_icur[11]) + " "
//     << "\n";
//     os << "pcur: " << std::to_string(hp.tlm_pcur[0]) + " " +
//                                 std::to_string(hp.tlm_pcur[1]) + " " +
//                                 std::to_string(hp.tlm_pcur[2]) + " " +
//                                 std::to_string(hp.tlm_pcur[3]) + " " +
//                                 std::to_string(hp.tlm_pcur[4]) + " " +
//                                 std::to_string(hp.tlm_pcur[5]) + " " +
//                                 std::to_string(hp.tlm_pcur[6]) + " " +
//                                 std::to_string(hp.tlm_pcur[7]) + " " +
//                                 std::to_string(hp.tlm_pcur[8]) + " " +
//                                 std::to_string(hp.tlm_pcur[9]) + " " +
//                                 std::to_string(hp.tlm_pcur[10]) + " " +
//                                 std::to_string(hp.tlm_pcur[11]) + " ";
//     return os;
// }
