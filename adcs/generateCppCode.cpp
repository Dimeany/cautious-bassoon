#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

#include "tables_json.h"

using namespace CubeSpace::TablesJson;

// Generates C++ code from tables.json

std::vector<char>
read_file(std::string file_name) {
  std::ifstream file(file_name, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    std::cout << "Could not read file: " << file_name << std::endl;
  }
  return buffer;
}

std::string
name_to_cpp_name(std::string name) {
  auto pos = name.find("");
  while ((pos = name.find_first_of(" -./()")) != std::string::npos) {
    name.erase(pos, 1);
  }
  if (name.find_first_of("0123456789") == 0) {
    name = "Number" + name;
  }
  return name;
}

std::string
table_to_type_name(std::string table_name) {
  auto pos = table_name.find(": ");
  table_name = table_name.substr(pos + 2);
  return name_to_cpp_name(table_name);
}

std::string data_type_to_cpp_type(picojson::object const& row, int &expect_offset_bits, int &unused_count) {
  std::stringstream result;

  auto name = row.at("Name").get<std::string>();
  auto data_type = row.at("Data Type").get<std::string>();
  auto description = row.at("Description").get<std::string>();
  auto length_bits = std::stoi(row.at("Length (bits)").get<std::string>());
  auto offset_bits = std::stoi(row.at("Offset (bits)").get<std::string>());

  if (expect_offset_bits != offset_bits) {
    if (offset_bits % 8 != 0) {
      std::cout << "Offset bits not an even number of bytes!" << std::endl;
      exit(1);
    }
    auto diff = (offset_bits - expect_offset_bits) / 8;
    if (diff) {
      result << "  char unused_" << unused_count++ << "[" << diff << "];\n";
    }
    else {
      result << "  char : 0;\n";
    }
  }
  expect_offset_bits = offset_bits + length_bits;

  std::optional<bool> need_bitfield;

  result << "  ";

  if (data_type == "UINT") {
    if (length_bits <= 8) {
      result << "uint8_t";
      need_bitfield = length_bits != 8;
    }
    else if (length_bits <= 16) {
      result << "uint16_t";
      need_bitfield = length_bits != 16;
    }
    else if (length_bits <= 32) {
      result << "uint32_t";
      need_bitfield = length_bits != 32;
    }
    else if (length_bits <= 64) {
      result << "uint32_t";
      need_bitfield = length_bits != 64;
    }
    else {
      std::cout << "UINT length too long" << std::endl;
      exit(1);
    }
  }
  else if (data_type == "INT") {
    if (length_bits <= 8) {
      result << "int8_t";
      need_bitfield = length_bits != 8;
    }
    else if (length_bits <= 16) {
      result << "int16_t";
      need_bitfield = length_bits != 16;
    }
    else if (length_bits <= 32) {
      result << "int32_t";
      need_bitfield = length_bits != 32;
    }
    else if (length_bits <= 64) {
      result << "int32_t";
      need_bitfield = length_bits != 64;
    }
    else {
      std::cout << "INT length too long" << std::endl;
      exit(1);
    }
  }
  else if (data_type == "ENUM") {
    std::string table_string = "Table ";
    std::string enum_values_string = "Enumeration Values";

    auto enum_type_name = description;

    auto pos = enum_type_name.find("Table ");
    enum_type_name = enum_type_name.substr(pos);
    pos = enum_type_name.find(enum_values_string);
    enum_type_name = enum_type_name.substr(0, pos + enum_values_string.size());

    result << table_to_type_name(enum_type_name);

    need_bitfield = true;
  }
  else if (data_type == "BOOL") {
    result << "bool";
    need_bitfield = true;
  }
  else if (data_type == "ARRAY") {
    // array added later
    result << "char";
    need_bitfield = false;
  }
  else if (data_type == "FLOAT") {
    if (length_bits != 32) {
      std::cout << "Invalid float length" << std::endl;
      exit(1);
    }
    result << "float";
    need_bitfield = false;
  }
  else if (data_type == "DOUBLE") {
    if (length_bits != 64) {
      std::cout << "Invalid double length" << std::endl;
      exit(1);
    }
    result << "double";
    need_bitfield = false;
  }
  else {
    std::cout << "Data type not supported: " << data_type << std::endl;
    exit(1);
  }

  if (!need_bitfield) {
    std::cout << result.str() << std::endl;
    std::cout << "need_bitfield not set!" << std::endl;
    exit(1);
  }

  result << " " << name_to_cpp_name(name);

  if (*need_bitfield || (length_bits % 8 != 0)) {
    result << " : " << length_bits;
  }
  if (data_type == "ARRAY") {
    if (length_bits % 8 != 0) {
      std::cout << "Array not a whole number of bytes" << std::endl;
      exit(1);
    }
    result << "[" << length_bits / 8 << "]";
  }
  result << ";";

  return result.str();
}

std::string
convert_struct(picojson::object const& table) {
  std::stringstream result;

  auto properties = table.at("Properties").get<picojson::object>();
  auto type_name = table_to_type_name(table.at("Table").get<std::string>());
  result << "struct " << type_name << " {\n";

  int offset_bits = 0;
  int unused_count = 0;
  for (auto const& row_value : table.at("Rows").get<picojson::array>()) {
    auto const& row = row_value.get<picojson::object>();
    result << data_type_to_cpp_type(row, offset_bits, unused_count) << "\n";
  }

  std::optional<int> expect_size;
  if (properties.count("Parameters Length (bytes)")) {
    expect_size = std::stoi(properties.at("Parameters Length (bytes)").get<std::string>());
  }
  else if (properties.count("Frame Length (bytes)")) {
    expect_size = std::stoi(properties.at("Frame Length (bytes)").get<std::string>());
  }

  if (!expect_size) {
    std::cout << "Could not find struct size!" << std::endl;
    exit(1);
  }

  // Some tables have member lists that add up to less than their
  // specified sizes, e.g. Table 35: SRAM Latchup counters Telemetry
  // Format
  int actual_size = offset_bits / 8;
  if (offset_bits % 8) {
    actual_size++;
  }
  if (actual_size < *expect_size) {
    result << "  char unused_" << unused_count++ << "[" << *expect_size - actual_size << "];\n";
  }

  result << "};\n";

  // Print static assert.
  int assert_size = *expect_size;
  if (assert_size == 0) {
    assert_size = 1;
  }
  result << "static_assert(sizeof(" << type_name << ") == " << assert_size <<  ");\n";

  // Instantiate templates.
  result << "template <> struct MessageSize<" << type_name << "> { static constexpr size_t value = " << *expect_size << "; };\n";
  if (properties.count("ID")) {
    if (properties.count("Parameters Length (bytes)")) {
      result << "template <> struct SetID<" << type_name << "> { static constexpr int value = " << properties.at("ID").get<std::string>() << "; };\n";
    }
    else if (properties.count("Frame Length (bytes)")) {
      result << "template <> struct GetID<" << type_name << "> { static constexpr int value = " << properties.at("ID").get<std::string>() << "; };\n";
    }
  }
  else if (properties.count("Set ID/Get ID")) {
    auto set_id_get_id = properties.at("Set ID/Get ID").get<std::string>();
    auto slash_pos = set_id_get_id.find("/");
    auto set_id = set_id_get_id.substr(0, slash_pos);
    auto get_id = set_id_get_id.substr(slash_pos + 1);

    result << "template <> struct SetID<" << type_name << "> { static constexpr int value = " << set_id << "; };\n";
    result << "template <> struct GetID<" << type_name << "> { static constexpr int value = " << get_id << "; };\n";
  }

  return result.str();
}

std::string
convert_enum(picojson::object const& table) {
  std::stringstream result;

  result << "enum class " << table_to_type_name(table.at("Table").get<std::string>()) << " : uint8_t {\n";

  int prev_value = -1;
  int not_used_count = 0;
  for (auto const& row_value : table.at("Rows").get<picojson::array>()) {
    auto const& row = row_value.get<picojson::object>();
    auto numeric_value = std::stoi(row.at("Numeric Value").get<std::string>());

    auto cpp_name = name_to_cpp_name(row.at("Name").get<std::string>());
    if (cpp_name == "NotUsed") {
      cpp_name += std::to_string(not_used_count++);
    }

    result << "  " << cpp_name;

    if (numeric_value != prev_value + 1) {
      result << " = " << numeric_value;
    }

    prev_value = numeric_value;

    result << ",\n";
  }

  result << "};\n";

  return result.str();
}

int main() {
  auto bytes = read_file("tables.json");
  std::string err;
  picojson::value v;
  picojson::parse(v, bytes.begin(), bytes.end(), &err);
  if (err.size()) {
    std::cout << err << std::endl;
    return 0;
  }

  auto arr = v.get<picojson::array>();

  std::cout << "#pragma once\n";
  std::cout << "\n";

  std::cout << "#include <cstddef>\n";
  std::cout << "#include <cstdint>\n";
  std::cout << "\n";

  std::cout << "#pragma pack(push, 1)" << std::endl;
  std::cout << "\n";

  std::cout << "namespace CubeSpace {\n";

  std::cout << "template<typename T> struct MessageSize;\n";
  std::cout << "template<typename T> struct GetID;\n";
  std::cout << "template<typename T> struct SetID;\n";
  std::cout << "\n";

  std::cout << "template<typename T> constexpr size_t MessageSize_v = MessageSize<T>::value;\n";
  std::cout << "template<typename T> constexpr int GetID_v = GetID<T>::value;\n";
  std::cout << "template<typename T> constexpr int SetID_v = SetID<T>::value;\n";
  std::cout << "\n";

  for (size_t i = 0; i < arr.size(); i++) {
    auto const& table_value = arr[i];
    auto const& table = table_value.get<picojson::object>();

    if (identify_table(table) == TableKind::Enum) {
      std::cout << convert_enum(table) << "\n";
    }
  }
  for (size_t i = 0; i < arr.size(); i++) {
    auto const& table_value = arr[i];
    auto const& table = table_value.get<picojson::object>();

    if (identify_table(table) == TableKind::Struct) {
      std::cout << convert_struct(table) << "\n";
    }
  }

  std::cout << "} // namespace CubeSpace\n";
  std::cout << "\n";

  std::cout << "#pragma pack(pop)\n";

  std::cout << std::endl;
}
