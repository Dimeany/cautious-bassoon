#ifndef SOCKET_EXCEPTION_HEADER
#define SOCKET_EXCEPTION_HEADER

#include <stdexcept>

namespace udp {

class SocketException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

} // namespace udp

#endif
