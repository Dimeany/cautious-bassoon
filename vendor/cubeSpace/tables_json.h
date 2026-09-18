#ifndef TABLES_JSON_UTILS_H
#define TABLES_JSON_UTILS_H

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "overloaded.h"
#include "picojson.h"

namespace CubeSpace {
namespace TablesJson {

// Kinds of tables in JSON file
enum class TableKind {
  Struct,
  Enum,
  List,
};

// Identifies a table using its contents
TableKind
identify_table(picojson::object table);

// Data types found in FRM
enum class CubeComputerDataType {
  Uint,
  Int,
  Enum,
  Bool,
  Array,
  Float,
  Double,
};

// Field (for a Command Format or a Telemetry Format)
struct CubeComputerField {
  std::string name;
  std::string description;
  CubeComputerDataType type;
  size_t length_bits;
  size_t offset_bits;
};

// Command Format or Telemetry Format
struct CubeComputerStruct {
  std::string table;
  std::string description;
  std::optional<unsigned char> set_id;
  std::optional<unsigned char> get_id;
  size_t length_bytes;
  std::vector<CubeComputerField> fields;
};


// Convert from string value taken from FRM
CubeComputerDataType
parse_data_type(std::string str);

// Convert JSON object into field
CubeComputerField
read_field(picojson::object field);

// Convert tables into structs
std::vector<CubeComputerStruct>
read_structs(picojson::array tables);

// Values of fields
using CubeComputerFieldValue = std::variant<
  uint64_t,
  int64_t,
  std::string,
  float,
  double
>;

// "to string" function, for output
std::string
cube_computer_field_value_to_string(CubeComputerFieldValue const& value);

picojson::value
cube_computer_field_value_to_json(CubeComputerFieldValue const& value);


// Converts an array of values into a byte array
std::vector<char>
to_byte_array(std::vector<std::optional<CubeComputerFieldValue>> values, CubeComputerStruct const& struct_);

// Converts a byte array into an array of values
std::vector<CubeComputerFieldValue>
from_byte_array(std::vector<char> bytes, CubeComputerStruct const& struct_);

std::vector<CubeComputerStruct>
read_tables_json(std::string file_name);

} // namespace TablesJson
} // namespace CubeSpace


#endif
