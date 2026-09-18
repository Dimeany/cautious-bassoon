#include <iostream>
#include <string>
#include <unistd.h>

#include "command_sender.h"

using namespace CubeSpace::TablesJson;

std::optional<CubeComputerStruct>
find_struct_by_set_id(std::vector<CubeComputerStruct> const& structs, int id) {
  for (auto const& struct_ : structs) {
    if (struct_.set_id == id) {
      return struct_;
    }
  }
  return {};
}
std::optional<CubeComputerStruct>
find_struct_by_get_id(std::vector<CubeComputerStruct> const& structs, int id) {
  for (auto const& struct_ : structs) {
    if (struct_.get_id == id) {
      return struct_;
    }
  }
  return {};
}


bool CommandSender::set_command(std::string str) {
  int id;
  try {
    id = std::stoi(str);
  }
  catch (std::exception const& ex) {
    std::cout << "Command id needs to be an integer" << std::endl;
    return false;
  }
  
  auto found_struct = find_struct_by_set_id(structs, id);
  if (!found_struct) {
    std::cout << "Command id not found" << std::endl;
    return false;
  }

  current_struct = *found_struct;
  field_values.clear();
  field_values.resize(current_struct.fields.size());

  return true;
}

bool CommandSender::set_telemetry(std::string str) {
  int id;
  try {
    id = std::stoi(str);
  }
  catch (std::exception const& ex) {
    std::cout << "Command id needs to be an integer" << std::endl;
    return false;
  }
  
  auto found_struct = find_struct_by_get_id(structs, id);
  if (!found_struct) {
    std::cout << "Command id not found" << std::endl;
    return false;
  }

  current_struct = *found_struct;
  field_values.clear();
  field_values.resize(current_struct.fields.size());

  return true;
}

void CommandSender::show() {
  std::cout << "Current struct: " << current_struct.table << std::endl;
  for (size_t i = 0; i < current_struct.fields.size() ; i++) {
    if (auto opt_field_value = field_values[i]) {
      auto const& field_value = *opt_field_value;
      std::cout << "Field " << current_struct.fields[i].name << " is set to: " << cube_computer_field_value_to_string(field_value) << std::endl;
    }
  }
}

picojson::value CommandSender::to_json() {
  picojson::object obj;
  for (size_t i = 0; i < current_struct.fields.size() ; i++) {
    if (auto opt_field_value = field_values[i]) {
      auto const& field_value = *opt_field_value;
      obj[current_struct.fields[i].name] = cube_computer_field_value_to_json(field_value);
    }
  }
  return picojson::value(obj);
}

std::optional<int> CommandSender::set(std::string str) {
  auto pos = str.find_first_of(" =");
  if (pos == std::string::npos) {
    std::cout << "must provide offset bits" << std::endl;
    return {};
  }
  size_t offset_bits;
  try {
    offset_bits = std::stoul(str.substr(0, pos));
  }
  catch (std::exception const& ex) {
    std::cout << "offset bits must be an integer" << std::endl;
    return {};
  }
  
  size_t field_index_to_set = 0;
  bool found = false;
  for (size_t i = 0; i < current_struct.fields.size(); i++) {
    if (offset_bits == current_struct.fields[i].offset_bits) {
      field_index_to_set = i;
      found = true;
      break;
    }
  }
  if (!found) {
    std::cout << "Field was not found by offset bits" << std::endl;
    return {};
  }
  
  auto field_value_str = str.substr(pos + 1);
  CubeComputerFieldValue field_value;
  switch (current_struct.fields[field_index_to_set].type) {
  case CubeComputerDataType::Uint:
    try {
      field_value = static_cast<uint64_t>(std::stoul(field_value_str));
    }
    catch (std::exception const& ex) {
      std::cout << "Could not parse as unsigned integer: " << field_value_str << std::endl;
      return {};
    }
    break;
  case CubeComputerDataType::Int:
  case CubeComputerDataType::Enum:
  case CubeComputerDataType::Bool:
    try {
      field_value = std::stol(field_value_str);
    }
    catch (std::exception const& ex) {
      std::cout << "Could not parse as integer: " << field_value_str << std::endl;
      return {};
    }
    break;
  case CubeComputerDataType::Array:
    field_value = field_value_str;
    break;
  case CubeComputerDataType::Float:
    try {
      field_value = std::stof(field_value_str);
    }
    catch (std::exception const& ex) {
      std::cout << "Could not parse as float: " << field_value_str << std::endl;
      return {};
    }
    break;
  case CubeComputerDataType::Double:
    try {
      field_value = std::stod(field_value_str);
    }
    catch (std::exception const& ex) {
      std::cout << "Could not parse as double: " << field_value_str << std::endl;
      return {};
    }
    break;
  }

  field_values[field_index_to_set] = field_value;
  return field_index_to_set;
}

void CommandSender::send() {
  if (!current_struct.set_id) {
    std::cout << "Command does not have a Set ID: " << current_struct.table << std::endl;
    return;
  }
  auto bytes = to_byte_array(field_values, current_struct);

  ::write(i2c_file, &*current_struct.set_id, 1);
  auto bytes_written = ::write(i2c_file, bytes.data(), bytes.size());
  if (bytes_written < 0 || static_cast<size_t>(bytes_written) != bytes.size()) {
    std::cout << "Error sending command: " << strerror(errno) << std::endl;
  }
}

void CommandSender::read() {
  if (!current_struct.get_id) {
    std::cout << "Command does not have a Get ID: " << current_struct.table << std::endl;
    return;
  }

  // Write one byte requesting telemetry
  auto bytes_written = ::write(i2c_file, &*current_struct.get_id, 1);
  if (bytes_written != 1) {
    std::cout << "Error sending telemetry request: " << strerror(errno) << std::endl;
    return;
  }

  // Read telemetry
  auto bytes = std::vector<char>(current_struct.length_bytes);

  auto bytes_read = ::read(i2c_file, bytes.data(), bytes.size());
  if (bytes_read < 0 || static_cast<size_t>(bytes_read) != bytes.size()) {
    std::cout << "Error reading telemetry frame: " << strerror(errno) << std::endl;
    return;
  }

  auto read_values = from_byte_array(bytes, current_struct);
  field_values.resize(read_values.size());
  for (size_t i = 0; i < read_values.size(); i++) {
    field_values[i] = read_values[i];
  }
}
