#include <fcntl.h>
#include <getopt.h>
#include <iostream>
#include <linux/i2c-dev.h>
#include <memory>
#include <optional>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include "overloaded.h"

#include "command_sender.h"
#include "tables_json.h"

using namespace CubeSpace::TablesJson;


void usage() {
  std::cout <<
R"END(Usage:
    SendCubeComputerCommand [-d /dev/i2c-N]

        Runs SendCubeComputerCommand interactively.

    SendCubeComputerCommand [-d /dev/i2c-N] \
        --comand CommandId \
        --set Offset1=Value1 \
        --set Offset2=Value2

        Sends a single command.

    SendCubeComputerCommand [-d /dev/i2c-N] --telemetry TelemetryId

        Makes a single telemetry request.

Flags:
    -d /dev/i2c-N

        Set the I2C device.  Default device is /dev/i2c-0.

    --command CommandId

        Send a command by ID.  Commands are given in the Firmware
        Reference Manual.

    --set Offset=Value

        Set the command field at the given offset to the specified
        value.  Fields that are not set are zeroed.

    --telemetry TelemetryId

        Make a telemetry request by ID.  Telemetry requests are given
        in the Firmware Reference Manual.

    -h

        Display this message.
)END" << std::endl;
  exit(1);
}

struct Options {
  std::string i2c_device = "/dev/i2c-0";
  std::string command;
  std::vector<std::string> set;
  std::string telemetry;
  bool no_i2c = false;
};

Options get_options(int argc, char **argv) {
  Options options;

  static struct option long_options[] = {
    { "command",   required_argument, 0, 1000 },
    { "set",       required_argument, 0, 1001 },
    { "telemetry", required_argument, 0, 2000 },
    { "no-i2c",    no_argument,       0, 3000 },
    { 0,           0,                 0, 0 },
  };

  int opt = 0;
  while ((opt = getopt_long(argc, argv, "hd:", long_options, 0)) != -1) {
    switch (opt) {
    case 'h':
      usage();
      break;
    case 'd':
      options.i2c_device = optarg;
      break;
    case 1000:
      options.command = optarg;
      break;
    case 1001:
      options.set.push_back(optarg);
      break;
    case 2000:
      options.telemetry = optarg;
      break;
    case 3000:
      options.no_i2c = true;
      break;
    }
  }

  return options;
}

// Returns command without initial substring if it has the initial
// substring, otherwise returns the empty optional
std::optional<std::string>
match_string(std::string initial, std::string command) {
  auto command_prefix = command.substr(0, initial.size());
  std::transform(command_prefix.begin(), command_prefix.end(), command_prefix.begin(), [](unsigned char c){ return std::tolower(c); });
  if (initial == command_prefix) {
    return command.substr(initial.size());
  }
  return {};
}

void interact_loop(CommandSender command_sender) {
  std::string command;
  while (std::getline(std::cin, command)) {
    if (match_string("help", command)) {
      std::cout <<
        "command [set_id]\n"
        "Select a command by its ID\n"
        "\n"
        "telemetry [get_id]\n"
        "Select a telemetry request by its ID\n"
        "\n"
        "set [offset] [value]\n"
        "Set a field to a certain value by its bit offset\n"
        "\n"
        "show\n"
        "Show the current command and the fields that are set\n"
        "\n"
        "send\n"
        "Send the current command over I2C\n"
        "\n"
        "read\n"
        "Read a CubeComputer frame over I2C\n"
        << std::endl;
    }
    else if (auto rest = match_string("command ", command)) {
      if (command_sender.set_command(*rest)) {
        std::cout << "Command has been set to: " << command_sender.current_struct.table << std::endl;
      }
    }
    else if (auto rest = match_string("telemetry ", command)) {
      if (command_sender.set_telemetry(*rest)) {
        std::cout << "Telemetry has been set to: " << command_sender.current_struct.table << std::endl;
      }
    }
    else if (auto rest = match_string("show", command)) {
      command_sender.show();
    }
    else if (auto rest = match_string("set ", command)) {
      if (auto field_index = command_sender.set(*rest)) {
        std::cout
          << "Field " << command_sender.current_struct.fields[*field_index].name
          // Assumes set() gives the optional `command_sender.field_values[*field_index]` a value.
          << " was set to: " << cube_computer_field_value_to_string(*command_sender.field_values[*field_index])
          << std::endl;
      }
    }
    else if (match_string("send", command)) {
      command_sender.send();
    }
    else if (match_string("read", command)) {
      command_sender.read();
    }
    else {
      std::cout << "Unrecognized command: " << command << ".  For the list of commands, type: help" << std::endl;
    }
  }
}

int main(int argc, char **argv) {
  auto options = get_options(argc, argv);

  int i2c_file = open(options.i2c_device.c_str(), O_RDWR);
  if (i2c_file < 0) {
    std::cout << "Could not open " << options.i2c_device << ": " << strerror(errno) << std::endl;
    return 1;
  }
  if (!options.no_i2c) {
    if (ioctl(i2c_file, I2C_SLAVE, 0x57) < 0) {
      std::cout << "Could not set I2C_SLAVE on " << options.i2c_device << ": " << strerror(errno) << std::endl;
      return 1;
    }
  }

  auto structs = read_tables_json("resources/tables.json");

  auto command_sender = CommandSender {
    i2c_file,
    structs,
    {},
    {},
  };

  if (options.command != "") {
    if (!command_sender.set_command(options.command)) {
      return 1;
    }
    for (auto const& set_str : options.set) {
      if (!command_sender.set(set_str)) {
        return 1;
      }
    }
    std::cout << command_sender.to_json() << std::endl;
    command_sender.send();
  }
  else if (options.telemetry != "") {
    if (!command_sender.set_telemetry(options.telemetry)) {
      return 1;
    }
    command_sender.read();
    std::cout << command_sender.to_json() << std::endl;
  }
  else {
    interact_loop(command_sender);
  }
}
