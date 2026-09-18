#ifndef COMMON_LIBRARY_INCLUDE_ADCS_STRUCT_DECODE_OPERATORS_H
#define COMMON_LIBRARY_INCLUDE_ADCS_STRUCT_DECODE_OPERATORS_H

#include "cube_space_types.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, CubeSpace::ADCSStateTelemetryFormat const& s);

#endif