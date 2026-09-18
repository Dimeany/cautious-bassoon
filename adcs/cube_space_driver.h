#ifndef CDH_SUPERNOVA_ADCS_DRIVER_H
#define CDH_SUPERNOVA_ADCS_DRIVER_H

#include <cstring>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "cube_space_types.h"

// Additional struct not in FRM
struct FileDownloadChunk {
  uint16_t counter;
  char bytes[20];
};
template <> struct CubeSpace::MessageSize<FileDownloadChunk> { static constexpr size_t value = 22; };
template <> struct CubeSpace::GetID<FileDownloadChunk> { static constexpr int value = 119; };

struct AdcsException : public std::runtime_error {
  AdcsException(std::string const& what) : std::runtime_error(what) {};
};

struct CubeSpaceDriver {
  static constexpr uint8_t I2C_Address = 0x57;

  int i2c_file;

  CubeSpaceDriver(int i2c_file_)
    : i2c_file(i2c_file_) {}

  void read(uint8_t id_byte, std::span<char> result) const {
    auto bytes_written = ::write(i2c_file, reinterpret_cast<char const*>(&id_byte), 1);
    if (bytes_written != 1) {
      throw AdcsException("CubeSpaceDriver::read: write call returned " + std::to_string(bytes_written));
    }

    auto bytes_read = ::read(i2c_file, result.data(), result.size());
    if (bytes_read < 0 || bytes_read != static_cast<int>(result.size())) {
      throw AdcsException("CubeSpaceDriver::read: read call returned " + std::to_string(bytes_read));
    }
  }

  template<typename T>
  T read() const {
    uint8_t id_byte = CubeSpace::GetID_v<T>;

    T result {};
    read(id_byte, {reinterpret_cast<char*>(&result), CubeSpace::MessageSize_v<T>});
    return result;
  }

  void write(uint8_t id_byte, std::span<char const> command) const {
    auto write_vec = std::vector<char> {};
    write_vec.resize(command.size() + 1);
    write_vec[0] = id_byte;
    memcpy(write_vec.data() + 1, command.data(), command.size());

    auto bytes_written = ::write(i2c_file, write_vec.data(), write_vec.size());
    if (bytes_written != static_cast<int>(write_vec.size())) {
      throw AdcsException("CubeSpaceDriver::write: write call returned " + std::to_string(bytes_written));
    }
  }

  template<typename T>
  void write(T const& command) const {
    auto id_byte = CubeSpace::SetID_v<T>;

    write(id_byte, {reinterpret_cast<char const*>(&command), CubeSpace::MessageSize_v<T>});
  }
};

#endif
