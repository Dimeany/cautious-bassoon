#ifndef ENVIRONMENT_UTIL_H
#define ENVIRONMENT_UTIL_H

namespace util {
    char const* getenv_required(char const* var_name);
    int getenv_int(char const* var_name, int def);
}

#endif