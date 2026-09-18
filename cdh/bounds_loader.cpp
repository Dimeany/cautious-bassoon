#include "bounds_loader.h"

#include <fstream>
#include <iostream>
#include <iterator>

std::optional<cdh::BoundsMap> loadBoundsFromJsonFile(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "Bounds file not found: " << path << std::endl;
        return std::nullopt;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    picojson::value v;
    std::string err = picojson::parse(v, content);
    if (!err.empty()) {
        std::cerr << "Failed to parse bounds JSON: " << err << std::endl;
        return std::nullopt;
    }

    if (!v.is<picojson::object>()) {
        std::cerr << "Bounds JSON root is not an object" << std::endl;
        return std::nullopt;
    }

    cdh::BoundsMap map;
    auto root = v.get<picojson::object>();
    for (auto const& kv : root) {
        const std::string subsystem = kv.first;
        const picojson::value& subsys_val = kv.second;
        if (!subsys_val.is<picojson::object>()) continue;
        auto subsys_obj = subsys_val.get<picojson::object>();
        for (auto const& field_kv : subsys_obj) {
            const std::string field = field_kv.first;
            const picojson::value& field_val = field_kv.second;
            if (!field_val.is<picojson::object>()) continue;
            auto field_obj = field_val.get<picojson::object>();

            auto it_min = field_obj.find("min");
            auto it_max = field_obj.find("max");
            if (it_min == field_obj.end() || it_max == field_obj.end()) {
                std::cerr << "Skipping bounds for " << subsystem << "." << field << " (missing min/max)" << std::endl;
                continue;
            }
            double min, max;
            try {
                min = it_min->second.get<double>();
                max = it_max->second.get<double>();
            } catch (const std::exception& e) {
                std::cerr << "Skipping bounds for " << subsystem << "." << field << " (min/max not numeric): " << e.what() << std::endl;
                continue;
            }
            if (!(min <= max)) {
                std::cerr << "Skipping bounds for " << subsystem << "." << field << " (min > max)" << std::endl;
                continue;
            }

            map[subsystem][field] = cdh::Bounds{min, max};
        }
    }

    return map;
}
