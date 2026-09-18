#ifndef SMALLSAT_ADCS_COMMAND_SENDER_H
#define SMALLSAT_ADCS_COMMAND_SENDER_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "tables_json.h"

struct CommandSender {
  const int i2c_file;
  const std::vector<CubeSpace::TablesJson::CubeComputerStruct> structs;

  CubeSpace::TablesJson::CubeComputerStruct current_struct;
  std::vector<std::optional<CubeSpace::TablesJson::CubeComputerFieldValue>> field_values;

  bool set_command(std::string str);
  bool set_telemetry(std::string str);
  void show();
  picojson::value to_json();
  std::optional<int> set(std::string str);
  void send();
  void read();
};


#endif
