#include <ostream>
#include "eps_health.h"

std::ostream& operator<<(std::ostream& os, const eps::EpsImpressHealth hp) {
    os << "-< EPS IMPRESS HEALTH >- \n";
    os << "pcdu_temp: " << hp.pcdu_temp << "\n";
    os << "pcdu_vbatt: " << hp.pcdu_vbatt << "\n";

    std::string ch_on_string;
    for (unsigned int i = 0; i < 12; i++){
        ch_on_string += std::to_string(hp.pcdu_ch_on[i]) + " ";
    }
    os << "pcdu_ch_on: " << ch_on_string << "\n";

    std::string vcur_string;
    for (unsigned int i = 0; i < 12; i++){
        vcur_string += std::to_string(hp.pcdu_vcur[i]) + " ";
    }
    os << "pcdu_vcur: " << vcur_string << "\n";

    std::string icur_string;
    for (unsigned int i = 0; i < 12; i++){
        icur_string += std::to_string(hp.pcdu_icur[i]) + " ";
    }
    os << "pcdu_icur: " << icur_string << "\n\n";


    os << "batt_heater_on: " << hp.batt_heater_on << "\n";
    os << "batt_balance_cell: " << hp.batt_balance_cell << "\n";
    os << "batt_temp: " << hp.batt_temp << "\n";
    os << "batt_vin: " << hp.batt_vin << "\n";
    os << "batt_vout: " << hp.batt_vout << "\n";
    os << "batt_iin: " << hp.batt_iin << "\n";
    os << "batt_iout: " << hp.batt_iout << "\n\n";

    os << "mppt_temp: " << hp.mppt_temp << "\n";

    std::string mppt_iin_string;
    for (unsigned int i = 0; i < 7; i++){
        mppt_iin_string += std::to_string(hp.mppt_iin[i]) + " ";
    }
    os << "mppt_iin: " << mppt_iin_string << "\n";

    std::string mppt_vin_string;
    for (unsigned int i = 0; i < 7; i++){
        mppt_vin_string += std::to_string(hp.mppt_vin[i]) + " ";
    }
    os << "mppt_iin: " << mppt_vin_string << "\n";


    os << "-< END EPS HEALTH >-\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, const eps::EpsPDUP4Health hp) {
    os << "PDU-P4 Health\n";
    os << "mcu_temp:\t" << hp.pdu_mcu_temp << " Cx100\n";

    os << "ch_on:\t\t[";
    for (int i = 0; i < 8; i++) 
    {
        os << std::to_string(hp.pdu_ch_on[i]); 
        if (i < 7) { os << " "; }
    }
    os << "]\n";

    os << "ch_voltage:\t[";
    for (unsigned int i = 0; i < 8; i++){
        os << std::to_string(hp.pdu_ch_voltage[i]);
        if (i < 7) { os << " "; }
    }
    os << "] mV\n";

    os << "ch_current:\t[";
    for (unsigned int i = 0; i < 8; i++){
        os << std::to_string(hp.pdu_ch_current[i]);
        if (i < 7) { os << " "; }
    }
    os << "] mA\n";
    
    os << "ch_temp:\t[";
    for (unsigned int i = 0; i < 8; i++){
        os << std::to_string(hp.pdu_ch_temp[i]);
        if (i < 7) { os << " "; }
    }
    os << "] Cx100\n";
    
    os << "bus_voltage:\t" << hp.pdu_bus_voltage << " mV\n";

    return os;
}