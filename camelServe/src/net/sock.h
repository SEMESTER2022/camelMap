#ifndef NET_SOCK_H
#define NET_SOCK_H

#include <chrono>
#include <memory>
#include <string>

#ifdef WIN32
#include <winsock2.h>
#else
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace net {

// clang-format off
#ifndef WIN32
    typedef unsigned int SOCKET;
#   include <errno.h>
#   define WSAGetLastError() errno
#   define WSAEINVAL        EINVAL
#   define WSAEALREADY      EALREADY
#   define WSAEWOULDBLOCK   EWOULDBLOCK
#   define WSAEAGAIN        EAGAIN
#   define WSAEMSGSIZE      EMSGSIZE
#   define WSAEINTR         EINTR
#   define WSAEINPROGRESS   EINPROGRESS
#   define WSAEADDRINUSE    EADDRINUSE
#   define WSAENOTSOCK      EBADF
#   define INVALID_SOCKET   (SOCKET)(~0)
#   define SOCKET_ERROR     -1
#else
#   ifndef WSAEAGAIN
#       ifdef EAGAIN
#           define WSAEAGAIN EAGAIN
#       else
#           define WSAEAGAIN WSAEWOULDBLOCK
#       endif
#   endif
#endif

#ifdef WIN32
#   ifndef S_IRUSR
#       define S_IRUSR      0400
#       define S_IWUSR      0200
#   endif
#else
#   define MAX_PATH         1024
#endif
// clang-format on

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

  [[nodiscard]] int Read(void *buf, size_t len) const;
  [[nodiscard]] int Write(void *buf, size_t len) const;

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
