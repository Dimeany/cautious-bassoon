#ifndef CDH_BOUNDS_LOADER_H
#define CDH_BOUNDS_LOADER_H

#include <optional>
#include <string>
#include "cdh_cvt.h"
// picojson lives under src/vendor/cubeSpace in this repo; include via relative path
#include "../vendor/cubeSpace/picojson.h"

// Loads bounds from a JSON file. Returns nullopt on error or missing file.
std::optional<cdh::BoundsMap> loadBoundsFromJsonFile(const std::string& path);

#endif // CDH_BOUNDS_LOADER_H
