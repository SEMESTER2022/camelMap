#ifndef NET_SOCK_H
#define NET_SOCK_H

#include "util/compat.h"

namespace net {

using namespace std::chrono_literals;

static constexpr auto MAX_WAIT_FOR_IO = 1s;

class Sock {
protected:
  SOCKET m_socket;

public:
  Sock();

  explicit Sock(SOCKET s);

  Sock(const Sock &) = delete;
  Sock(Sock &&other);

  ~Sock();

  Sock &operator=(const Sock &) = delete;
  Sock &operator=(Sock &&other);

  [[nodiscard]] SOCKET Get() const;

  SOCKET Release();
  void Reset();

  [[nodiscard]] ssize_t Recv(void *buf, size_t len, int flag) const;
  [[nodiscard]] ssize_t Send(void *buf, size_t len, int flag) const;

  using Event = uint8_t;

  static constexpr Event RECV = 0b01;

  static constexpr Event SEND = 0b10;

  [[nodiscard]] bool Wait(std::chrono::milliseconds timeout, Event requested,
                          Event *occurred = nullptr) const;

  void SendComplete(const std::string &data,
                    std::chrono::milliseconds timeout) const;

  [[nodiscard]] std::string
  RecvUntilTerminator(uint8_t terminator, std::chrono::milliseconds timeout,
                      size_t max_data) const;

  [[nodiscard]] bool IsConnected(std::string &errmsg) const;
};

} // namespace net

std::string NetworkErrorString(int err);

bool CloseSocket(SOCKET &hSocket);

#endif // NET_SOCK_H
