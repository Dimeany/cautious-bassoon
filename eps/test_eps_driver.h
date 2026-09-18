#ifndef TEST_EPS_DRIVER_HEADER
#define TEST_EPS_DRIVER_HEADER

#include <iostream>
#include <chrono>
#include <stdlib.h>

#include "csp_driver.h"


namespace eps_test {
    struct CommandLineOptions {
        int test_count = 500;
        int attempt_count = 1;
        bool test_all_params = false;
        bool mission = 1; // 1 for impress 0 for exact
    };

    void usage();
    CommandLineOptions parse_cmd_opts(int argc, char *argv[]);

    void pull_all_parameters(int&, CommandLineOptions, bool mission);
    void pull_health(int&, CommandLineOptions);
}

#endif