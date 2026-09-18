#include "environment_util.h"

#include <cstdlib>
#include <iostream>

namespace util {
    char const* getenv_required(char const* var_name) {
        const char* env_value = std::getenv(var_name);
        if (!env_value) {
            std::cerr << "Environment variable " << var_name << " is required but not set." << std::endl;
            std::exit(1);
        }
        return env_value;
    }

    int getenv_int(char const* var_name, int def)
    {
        const char* env_value = std::getenv(var_name);
        if (!env_value) { return def; }

        try
        {
            int int_value = std::stoi(env_value);
            return int_value;
        }
        catch (...)
        {
            return def;
        }
    }
} // namespace util