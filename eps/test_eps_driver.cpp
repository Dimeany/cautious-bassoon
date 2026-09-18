// test_eps_driver.cpp
// Created by Simeon Shaffar 5/16/2025
// This test is a compiled executable independend of the service that only includes the driver
// It is meant to be the baseline validation for serial communication without needing the service to be working

#include "test_eps_driver.h"


int main(int argc, char* argv[]) {
    auto opts = eps_test::parse_cmd_opts(argc, argv);

    std::cout << "Initializing CSP" << std::endl;
    start_csp();

    int number_of_pulls = 0;
    int number_of_fails = 0;
    long total_duration = 0;

    uint32_t _pdu_gndwdt = 0;

    for (int i = 0; i < opts.test_count; i++){
        std::cout << "attempt " << i << "\t";
        try {
            auto start = std::chrono::high_resolution_clock::now();

            if (opts.test_all_params) { // The entire stack makes the can line a little unrealiable, if getting fails then set attempt count to 2
                eps_test::pull_all_parameters(number_of_pulls, opts, opts.mission); 
                eps_test::pull_health(number_of_pulls, opts);
            }
            // For IMPRESS with just one parameter
            else if (opts.mission == IMPRESS) {
                bool ch_on_val[12] = {};
                int result = pull_parameter(impress_params_a[PCDU_CH_ON], ch_on_val);
                    if (result < 0) throw std::runtime_error("Serial communication issue pulling param: pcdu_ch_on");

                bool new_val = !ch_on_val[11];
                result = push_parameter(impress_params_a[PCDU_CH_ON], &new_val, 11);
                    if (result < 0) throw std::runtime_error("Serial communication issue pushing param: pcdu_ch_on");
            }
            // For EXACT with just one parameter
            else {
                if (pull_parameter(pdup4_params_a[P4_GNDWDT], &_pdu_gndwdt) < 0) 
                    throw std::runtime_error("Serial communication issue pulling param: pdu_gndwdt");
            }


            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            std::cout << "time: " << duration.count() << "\n";
            total_duration += duration.count();
        }
        catch (const std::exception& e){
            std::cerr << e.what() << "\n";
            ++number_of_fails;
        }
    }

    int num_of_successful_test = (opts.test_count - number_of_fails);

    std::cout << "TEST SUMMARY: \n";
    std::cout << "tests passed: " <<  num_of_successful_test << "/" << opts.test_count << "\n";
    std::cout << "success percentage: " << (static_cast<float>(num_of_successful_test) / static_cast<float>(opts.test_count)) * 100 << "%" << "\n";
    std::cout << "total pulls: " << number_of_pulls << "\n";
    std::cout << "average success time (us): " << total_duration/num_of_successful_test << "\n";
}


void eps_test::usage() {
    eps_test::CommandLineOptions opts = {};

    std::cerr << "EPS Service usage: " << std::endl;
    std::cerr << "   -h: Prints this help message" << std::endl;
    std::cerr << "   -t: Number of tests to run "
                            << " (default: " << opts.test_count << ")" << std::endl;
    std::cerr << "   -c: Attempt count for how many times the service will try before giving up on a command"
                            << " (default: " << opts.attempt_count << ")" << std::endl;
    std::cerr << "   -a: Test all parameters possible each iteration on top of the health packet"
                            << " (default: " << opts.test_all_params << ")" << std::endl;
    std::cerr << "   -e: EXACT - EPS service will run while pulling for EXACT telemetry"
                            << " (default: IMPRESS)" << std::endl;

    exit(1);
}

eps_test::CommandLineOptions eps_test::parse_cmd_opts(int argc, char *argv[]) {
    eps_test::CommandLineOptions options = {};

    int opt = 0;
    while ((opt = getopt(argc, argv, "ht:c:ae")) != -1) {
        switch (opt) {
            case 'h':
                eps_test::usage();
                break;
            case 't':
                options.test_count = std::stoi(optarg);
                break;
            case 'c':
                options.attempt_count = std::stoi(optarg);
                break;
            case 'a':
                options.test_all_params = true;
                break;
            case 'e':
                options.mission = 0; //0 for EXACT vs the default 1 for IMPRESS
                break;  
            default:
                eps_test::usage();
        }
    }

    return options;
}


void eps_test::pull_all_parameters(int& number_of_pulls, eps_test::CommandLineOptions opts, bool mission) {
    
    // FOR IMPRESS
    if (opts.mission == IMPRESS) {
        for(int i = 0; i < IMPRESS_PARAMS_COUNT; i++) {
            number_of_pulls++;

            const char* result;
            for(int i = 1; i <= opts.attempt_count; i++) {
                result = pull_parameter_string(impress_params_a[i]->name, mission, -1);
                    if (result != NULL) break;
            }

            if (result == NULL) 
                throw std::runtime_error("Serial connection error for parameter: " + std::string(impress_params_a[i]->name));
            if (std::string(result) == "")
                throw std::runtime_error("Invalid casting for parameter: " + std::string(impress_params_a[i]->name));
        }
    }
    // FOR EXACT
    else {
        for(int i = 0; i < PDUP4_PARAMS_COUNT; i++) {
            number_of_pulls++;

            const char* result;
            for(int i = 1; i <= opts.attempt_count; i++) {
                result = pull_parameter_string(pdup4_params_a[i]->name, mission, -1);
                    if (result != NULL) break;
            }

            if (result == NULL) 
                throw std::runtime_error("Serial connection error for parameter: " + std::string(impress_params_a[i]->name));
            if (std::string(result) == "")
                throw std::runtime_error("Invalid casting for parameter: " + std::string(impress_params_a[i]->name));
        }
    }
}

void eps_test::pull_health(int& number_of_pulls, eps_test::CommandLineOptions opts) {
    number_of_pulls++;
    int result;

    // FOR IMPRESS
    if (opts.mission == IMPRESS) {
        eps_pdup3_health_t hp = {};
        for(int i = 1; i <= opts.attempt_count; i++) {
            result = build_impress_health_queue(&hp); // Break out of this loop if it was successful
                if (result == 0) break;
        }

        if (result < 0 )  {
            std::cout << "\n" << hp;
            throw std::runtime_error("Serial connection error building health packet: " + std::to_string(result));
        }
    }

    // FOR EXACT
    else {
        eps_pdup4_health_t hp = {};
        for(int i = 1; i <= opts.attempt_count; i++) {
            result = build_exact_health_queue(&hp); // Break out of this loop if it was successful
                if (result == 0) break;
        }

        if (result < 0 )  {
            std::cout << "\n" << hp;
            throw std::runtime_error("Serial connection error building health packet: " + std::to_string(result));
        }
    }
}

