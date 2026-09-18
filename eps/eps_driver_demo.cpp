// eps_driver_demo.cpp
// This file exists to show an example of how to use our implemenation of the driver
// It is also the test bed for development of the final driver

#include <iostream>
#include <string>
#include <chrono>

#include "csp_driver.h"

using namespace std::chrono;
using namespace eps;


void spam_health() {
    int counter = 0;

    while(true) {
        auto start = high_resolution_clock::now();

        // uint64_t energy;
        // int result = pull_parameter(eps_params_a[PCDU_ENERGY], &energy);
        // printf("\npcdu_energy: %ld, result: %d\n", energy, result);

        // uint16_t pcdu_icur;
        // result = pull_parameter(eps_params_a[PCDU_I_CUR], &pcdu_icur);
        // printf("\npcdu_icur[0]: %d, result: %d\n", pcdu_icur, result);

        eps_pdup3_health_t hp = {};
        build_impress_health_queue(&hp);

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end-start);

        std::cout << "\nattempt: " << counter++ << "\tduration: " << duration << std::endl;
        std::cout << hp << std::endl;

    }
}

void spam_single() {
    int counter = 0;
    param_t* param = impress_params_a[PCDU_GNDWDT];

    uint32_t bucket = 0;

    while(true) {
        auto start = high_resolution_clock::now();

        pull_parameter(param, &bucket);

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end-start);


        std::cout << "attempt: " << counter++ << "\t" << param->name << ": " << bucket << "\tduration: " << duration <<  std::endl;
    }
}

//int argc, char* argv[]
int main() {
    start_csp();

    csp_scan();

    uint32_t _pdu_gndwdt = 86000;
    param_t* pdu_gndwdt = pdup4_params_a[P4_GNDWDT];

    int counter = 0;
    int result = 0;

    push_parameter(pdu_gndwdt, &_pdu_gndwdt, INDEX_ALL);

    while (true) {
        _pdu_gndwdt = 0;
        
        result = pull_parameter(pdu_gndwdt, &_pdu_gndwdt);

        if (result == 0 && _pdu_gndwdt < 86000) { 
            _pdu_gndwdt = 86005;
            result += push_parameter(pdu_gndwdt, &_pdu_gndwdt, INDEX_ALL);
        } 
                
        std::cout << "Attempt: " << counter++ << ", result: " << result << ", value: " << _pdu_gndwdt << std::endl;
    }

    return 0;
}