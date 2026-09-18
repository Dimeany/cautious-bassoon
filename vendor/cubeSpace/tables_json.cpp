#include <fstream>

#include "overloaded.h"

#include "tables_json.h"

namespace CubeSpace {
namespace TablesJson {

TableKind
identify_table(picojson::object table) {
  auto properties = table["Properties"].get<picojson::object>();
  auto rows = table["Rows"].get<picojson::array>();
  auto row0 = std::optional<picojson::object> {};
  if (rows.size()) {
    row0 = rows[0].get<picojson::object>();
  }
  if (
    properties.count("Description") &&
    (
      properties.count("ID") ||
      properties.count("Set ID/Get ID")
    ) &&
    (
      properties.count("Parameters Length (bytes)") ||
      properties.count("Frame Length (bytes)")
    )
  ) {
    return TableKind::Struct;
  }
  if (
    row0 &&
    row0->count("Description") &&
    row0->count("Name") &&
    row0->count("Numeric Value")
  ) {
    return TableKind::Enum;
  }
  return TableKind::List;
}

CubeComputerDataType parse_data_type(std::string str) {
  if (str == "UINT") {
    return CubeComputerDataType::Uint;
  }
  else if (str == "INT") {
    return CubeComputerDataType::Int;
  }
  else if (str == "ENUM") {
    return CubeComputerDataType::Enum;
  }
  else if (str == "BOOL") {
    return CubeComputerDataType::Bool;
  }
  else if (str == "ARRAY") {
    return CubeComputerDataType::Array;
  }
  else if (str == "FLOAT") {
    return CubeComputerDataType::Float;
  }
  else if (str == "DOUBLE") {
    return CubeComputerDataType::Double;
  }
  else {
    std::cout << "Unrecognized data type: " << str << std::endl;
    exit(1);
  }
}

CubeComputerField
read_field(picojson::object field) {
  CubeComputerField result;

  result.name = field.at("Name").get<std::string>();
  result.description = field.at("Description").get<std::string>();
  result.type = parse_data_type(field.at("Data Type").get<std::string>());
  result.length_bits = std::stoi(field.at("Length (bits)").get<std::string>());
  result.offset_bits = std::stoi(field.at("Offset (bits)").get<std::string>());

  return result;
}

std::vector<CubeComputerStruct>
read_structs(picojson::array tables) {
  auto result = std::vector<CubeComputerStruct> {};

  for (auto const& table_value : tables) {
    auto const& table = table_value.get<picojson::object>();
    switch (identify_table(table)) {
    case TableKind::Struct: {
      CubeComputerStruct current;

      current.table = table.at("Table").get<std::string>();

      auto const& properties = table.at("Properties").get<picojson::object>();
      current.description = properties.at("Description").get<std::string>();

      if (properties.count("ID")) {
        auto id = std::stoi(properties.at("ID").get<std::string>());
        if (properties.count("Parameters Length (bytes)")) {
          current.set_id = id;
        }
        else {
          current.get_id = id;
        }
      }
      else {
        auto set_id_get_id = properties.at("Set ID/Get ID").get<std::string>();
        auto slash_pos = set_id_get_id.find("/");
        current.set_id = std::stoi(set_id_get_id.substr(0, slash_pos));
        current.get_id = std::stoi(set_id_get_id.substr(slash_pos + 1));
      }

      if (properties.count("Parameters Length (bytes)")) {
        current.length_bytes = std::stoi(properties.at("Parameters Length (bytes)").get<std::string>());
      }
      else {
        current.length_bytes = std::stoi(properties.at("Frame Length (bytes)").get<std::string>());
      }

      for (auto field : table.at("Rows").get<picojson::array>()) {
        current.fields.push_back(read_field(field.get<picojson::object>()));
      }

      result.push_back(std::move(current));
      break;
    }
    case TableKind::Enum:
      break;
    case TableKind::List:
      break;
    }
  }

  return result;
}

std::string
cube_computer_field_value_to_string(CubeComputerFieldValue const& value) {
  return std::visit(overloaded {
    [](std::string const& str) {
      return str;
    },
    [](auto const& value) {
      return std::to_string(value);
    },
  }, value);
}

picojson::value
cube_computer_field_value_to_json(CubeComputerFieldValue const& value) {
  return std::visit(overloaded {
    [](std::string const& str) {
      return picojson::value(str);
    },
    [](auto const& value) {
      return picojson::value(static_cast<double>(value));
    },
  }, value);
}

std::vector<char>
to_byte_array(std::vector<std::optional<CubeComputerFieldValue>> values, CubeComputerStruct const& struct_) {
  auto result = std::vector<char> {};
  result.resize(struct_.length_bytes);

  for (size_t i = 0; i < struct_.fields.size(); i++) {
    auto const& field = struct_.fields[i];
    auto const& value = values.at(i);

    if (!value) {
      continue;
    }

    auto current_field = std::vector<std::byte> {};

    switch (field.type) {
    case CubeComputerDataType::Uint: {
      auto uint_value = std::get<uint64_t>(*value);
      current_field.resize(sizeof(uint_value));
      memcpy(current_field.data(), &uint_value, sizeof(uint_value));
      break;
    }
    case CubeComputerDataType::Int:
    case CubeComputerDataType::Enum:
    case CubeComputerDataType::Bool: {
      auto int_value = std::get<int64_t>(*value);
      current_field.resize(sizeof(int_value));
      memcpy(current_field.data(), &int_value, sizeof(int_value));
      break;
    }
    case CubeComputerDataType::Array: {
      auto string_value = std::get<std::string>(*value);
      current_field.resize(field.length_bits / 8);
      if (field.length_bits % 8) {
        std::cout << "Unsupported: array length bits not a multiple of 8" << std::endl;
      }
      memcpy(current_field.data(), string_value.data(), std::min(current_field.size(), string_value.size()));
      break;
    }
    case CubeComputerDataType::Float: {
      auto float_value = std::get<float>(*value);
      current_field.resize(sizeof(float_value));
      memcpy(current_field.data(), &float_value, sizeof(float_value));
      break;
    }
    case CubeComputerDataType::Double: {
      auto double_value = std::get<double>(*value);
      current_field.resize(sizeof(double_value));
      memcpy(current_field.data(), &double_value, sizeof(double_value));
      break;
    }
    }

    size_t out_byte_offset = field.offset_bits / 8;
    size_t out_bit_offset = field.offset_bits % 8;
    if (field.length_bits < 8) {
      current_field[0] <<= (8 - field.length_bits);
      current_field[0] >>= (8 - field.length_bits - out_bit_offset);
      *(result.data() + out_byte_offset) |= static_cast<char>(current_field[0]);
    }
    else if (field.length_bits % 8 || out_bit_offset) {
      std::cerr << "Unhandled: length_bits >= 8 not a whole number of bytes on a byte boundary" << std::endl;
    }
    else {
      memcpy(result.data() + out_byte_offset, current_field.data(), field.length_bits / 8);
    }
  }

  return result;
}

std::vector<CubeComputerFieldValue>
from_byte_array(std::vector<char> bytes, CubeComputerStruct const& struct_) {
  auto result = std::vector<CubeComputerFieldValue> {};

  for (auto const& field : struct_.fields) {
    auto out_byte_offset = field.offset_bits / 8;
    auto out_bit_offset = field.offset_bits % 8;

    auto length_bytes = field.length_bits / 8;
    auto length_bits = field.length_bits % 8;
    if (length_bytes && (length_bits || out_bit_offset)) {
      std::cerr << "Unsupported: length bits >= 8 not byte-aligned" << std::endl;
    }
    if (length_bits) {
      length_bytes += 1;
    }

    auto current_field = std::vector<std::byte>(length_bytes);
    memcpy(current_field.data(), bytes.data() + out_byte_offset, length_bytes);
    current_field[0] <<= (8 - out_bit_offset - length_bits) % 8;
    current_field[0] >>= (8 - length_bits) % 8;

    switch (field.type) {
    case CubeComputerDataType::Uint: {
      uint64_t value = 0;
      memcpy(&value, current_field.data(), length_bytes);
      result.push_back(value);
      break;
    }
    case CubeComputerDataType::Int:
    case CubeComputerDataType::Enum:
    case CubeComputerDataType::Bool: {
      int64_t value = 0;
      memcpy(&value, current_field.data(), length_bytes);
      // Correct display of negative numbers
      if (!length_bits && length_bytes == 1) {
        if (value > std::numeric_limits<int8_t>::max()) {
          value = int8_t(value);
        }
      }
      else if (length_bytes == 2) {
        if (value > std::numeric_limits<int16_t>::max()) {
          value = int16_t(value);
        }
      }
      else if (length_bytes == 4) {
        if (value > std::numeric_limits<int32_t>::max()) {
          value = int32_t(value);
        }
      }
      result.push_back(value);
      break;
    }
    case CubeComputerDataType::Array: {
      std::string value(length_bytes, '\0');
      memcpy(value.data(), current_field.data(), length_bytes);
      result.push_back(value);
      break;
    }
    case CubeComputerDataType::Float: {
      float value = 0;
      memcpy(&value, current_field.data(), length_bytes);
      result.push_back(value);
      break;
    }
    case CubeComputerDataType::Double: {
      double value = 0;
      memcpy(&value, current_field.data(), length_bytes);
      result.push_back(value);
      break;
    }
    }
  }

  return result;
}

std::vector<CubeComputerStruct>
read_tables_json(std::string file_name) {
  std::ifstream file(file_name, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    std::cout << "Could not read file: " << file_name << std::endl;
  }

  std::string err;
  picojson::value v;
  picojson::parse(v, buffer.begin(), buffer.end(), &err);
  if (err.size()) {
    std::cout << err << std::endl;
    throw std::runtime_error("Error parsing tables.json");
  }

  return read_structs(v.get<picojson::array>());
}

} // namespace TablesJson
} // namespace CubeSpace
