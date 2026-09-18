#include <fcntl.h>
#include <iostream>

#include <gtest/gtest.h>
#include <picojson.h>

#include "command_sender.h"
#include "tables_json.h"

using namespace CubeSpace::TablesJson;

picojson::value
parse_string(std::string const& str) {
  std::string err;
  picojson::value v;
  picojson::parse(v, str.begin(), str.end(), &err);
  EXPECT_EQ(err, "");
  return v;
}

CubeComputerStruct
get_test_struct() {
  return CubeComputerStruct {
    "Table Test",
    "Test Format",
    201,
    211,
    27,
    std::vector<CubeComputerField> {
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Uint,
        4,
        0,
      },
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Int,
        8,
        8,
      },
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Float,
        32,
        16,
      },
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Enum,
        1,
        108,
      },
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Enum,
        1,
        109,
      },
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Array,
        40,
        112,
      },
      CubeComputerField {
        "Foo",
        "Foo Bar",
        CubeComputerDataType::Double,
        64,
        152,
      },
    },
  };
}

TEST(TablesJson, identify_table) {
  std::string command_json_str = R"END({
  "Properties": {
    "Description": "Perform a reset",
    "ID": "1",
    "Parameters Length (bytes)": "1"
  },
  "Rows": [
    {
      "Data Type": "UINT",
      "Description": "Magic number to make sure it is a valid reset command. Should equal 0x5A",
      "Length (bits)": "8",
      "Name": "Magic number",
      "Offset (bits)": "0"
    }
  ],
  "Table": "Table 10: Reset Command Format"
})END";

  std::string enum_json_str = R"END({
  "Properties": {},
  "Rows": [
    {
      "Description": "Internal Flash Program",
      "Name": "Internal Flash Program",
      "Numeric Value": "1"
    }
  ],
  "Table": "Table 63: BootSetProgramsList Enumeration Values"
})END";

  auto command_json = parse_string(command_json_str);
  auto enum_json = parse_string(enum_json_str);

  EXPECT_EQ(TableKind::Struct, identify_table(command_json.get<picojson::object>()));
  EXPECT_EQ(TableKind::Enum, identify_table(enum_json.get<picojson::object>()));
}

TEST(TablesJson, to_byte_array) {
  auto struct_ = get_test_struct();

  auto values = std::vector<std::optional<CubeComputerFieldValue>>(struct_.fields.size());
  values[0] = uint64_t {10};
  values[1] = int64_t {-2};
  values[2] = 15.9f;
  values[3] = 0;
  values[4] = 1;
  values[5] = "foo";
  values[6] = 3.2;

  auto byte_array = to_byte_array(values, struct_);
  EXPECT_EQ(27, byte_array.size());
  EXPECT_EQ(char(0x0A), byte_array[0]);
  EXPECT_EQ(char(0xFE), byte_array[1]);
  EXPECT_EQ(char(0x00), byte_array[6]);
  EXPECT_EQ(char(0x00), byte_array[7]);
  EXPECT_EQ(char(0x00), byte_array[8]);
  EXPECT_EQ(char(0x00), byte_array[9]);
  EXPECT_EQ(char(0x00), byte_array[10]);
  EXPECT_EQ(char(0x00), byte_array[11]);
  EXPECT_EQ(char(0x00), byte_array[12]);
  EXPECT_EQ(char(0x20), byte_array[13]);
  EXPECT_EQ(char(0x66), byte_array[14]);
  EXPECT_EQ(char(0x6f), byte_array[15]);
  EXPECT_EQ(char(0x6f), byte_array[16]);
  EXPECT_EQ(char(0x00), byte_array[17]);
  EXPECT_EQ(char(0x00), byte_array[18]);

  float output_float;
  memcpy(&output_float, &byte_array[2], sizeof(output_float));
  EXPECT_EQ(15.9f, output_float);

  double output_double;
  memcpy(&output_double, &byte_array[19], sizeof(output_double));
  EXPECT_EQ(3.2, output_double);
}


TEST(TablesJson, from_byte_array) {
  auto struct_ = get_test_struct();

  auto byte_array = std::vector<char>(28);
  byte_array[0] = char(0x0A);
  byte_array[1] = char(0xFE);
  byte_array[6] = char(0x00);
  byte_array[7] = char(0x00);
  byte_array[8] = char(0x00);
  byte_array[9] = char(0x00);
  byte_array[10] = char(0x00);
  byte_array[11] = char(0x00);
  byte_array[12] = char(0x00);
  byte_array[13] = char(0x20);
  byte_array[14] = char(0x66);
  byte_array[15] = char(0x6f);
  byte_array[16] = char(0x6f);
  byte_array[17] = char(0x00);
  byte_array[18] = char(0x00);

  float output_float = 15.9f;
  memcpy(&byte_array[2], &output_float, sizeof(output_float));

  double output_double = 3.2;
  memcpy(&byte_array[19], &output_double, sizeof(output_double));

  auto values = from_byte_array(byte_array, struct_);

  EXPECT_EQ(uint64_t {10}, std::get<uint64_t>(values[0]));
  EXPECT_EQ(int64_t {-2}, std::get<int64_t>(values[1]));
  EXPECT_EQ(output_float, std::get<float>(values[2]));
  EXPECT_EQ(0, std::get<int64_t>(values[3]));
  EXPECT_EQ(1, std::get<int64_t>(values[4]));
  EXPECT_EQ(std::string("foo\0\0", 5), std::get<std::string>(values[5]));
  EXPECT_EQ(output_double, std::get<double>(values[6]));
}

TEST(CommandSender, write_command) {
  EXPECT_EQ(0, mkfifo("test_fifo", S_IRUSR | S_IWUSR));
  int fd = open("test_fifo", O_RDWR);

  auto structs = read_tables_json("resources/tables.json");

  CommandSender sender {
    fd,
    structs,
    {},
    {},
  };

  // Write "Current UNIX Time" to FIFO
  sender.set_command("2");
  sender.set("0=10");
  sender.set("32=42");
  sender.send();

  // Read from FIFO
  uint8_t id;
  EXPECT_EQ(1, read(fd, &id, 1));
  EXPECT_EQ(2, id);

  uint32_t seconds;
  EXPECT_EQ(4, read(fd, &seconds, 4));
  EXPECT_EQ(10, seconds);

  uint16_t milliseconds;
  EXPECT_EQ(2, read(fd, &milliseconds, 2));
  EXPECT_EQ(42, milliseconds);

  unlink("test_fifo");
}

TEST(CommandSender, read_telemetry) {
  EXPECT_EQ(0, mkfifo("test_fifo", S_IRUSR | S_IWUSR));
  int fd = open("test_fifo", O_RDWR);

  auto structs = read_tables_json("resources/tables.json");

  CommandSender sender {
    fd,
    structs,
    {},
    {},
  };

  // Write I2C response to FIFO
  uint32_t seconds = 10;
  EXPECT_EQ(4, write(fd, &seconds, 4));

  uint32_t milliseconds = 42;
  EXPECT_EQ(2, write(fd, &milliseconds, 2));

  // Read "Current UNIX Time" from FIFO
  sender.set_telemetry("140");
  sender.read();

  // Expect command ID to be written to FIFO
  uint8_t id;
  EXPECT_EQ(1, read(fd, &id, 1));
  EXPECT_EQ(140, id);

  // Expect seconds and milliseconds to have been read from FIFO
  EXPECT_EQ(10, std::get<uint64_t>(*sender.field_values[0]));
  EXPECT_EQ(42, std::get<uint64_t>(*sender.field_values[1]));

  unlink("test_fifo");
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
