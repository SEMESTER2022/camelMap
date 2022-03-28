#include "net/sock.h"

#include <memory>
#include <string>

#include "fmt/core.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

// clang-format off
#ifdef WIN32
#   include <codecvt>
#   include <locale>
#endif
// clang-format on

static inline bool IOErrorIsPermanent(int err) {
  return err != WSAEAGAIN && err != WSAEINTR && err != WSAEWOULDBLOCK &&
         err != WSAEINPROGRESS;
}

net::Sock::Sock() : m_socket(INVALID_SOCKET) {}

net::Sock::Sock(SOCKET s) : m_socket(s) {}

net::Sock(Sock &&other) {
  m_socket = other.m_socket;
  other.m_socket = INVALID_SOCKET;
}

net::Sock::~Sock() { Reset(); }

net::Sock &net::Sock::operator=(Sock &&other) {
  Reset();
  m_socket = other.m_socket;
  other.m_socket = INVALID_SOCKET;
  return *this;
}

net::SOCKET net::Sock::Get() const { return m_socket; }

SOCKET Sock::Release() {
  const SOCKET s = m_socket;
  m_socket = INVALID_SOCKET;
  return s;
}

void net::Sock::Reset() { CloseSocket(m_socket); }

ssize_t net::Sock::Send(const void *data, size_t len, int flags) const {
  return send(m_socket, static_cast<const char *>(data), len, flags);
}

ssize_t Sock::Recv(void *buf, size_t len, int flags) const {
  return recv(m_socket, static_cast<char *>(buf), len, flags);
}

int net::Sock::Connect(const sockaddr *addr, socklen_t addr_len) const {
  return connect(m_socket, addr, addr_len);
}

std::unique_ptr<Sock> net::Sock::Accept(sockaddr *addr,
                                        socklen_t *addr_len) const {
#ifdef WIN32
  static constexpr auto ERR = INVALID_SOCKET;
#else
  static constexpr auto ERR = SOCKET_ERROR;
#endif

  std::unique_ptr<Sock> sock;
  const auto socket = accept(m_socket, addr, addr_len);
  if (socket != ERR) {
    try {
      sock = std::make_unique<Sock>(socket);
    } catch (const std::exception &) {
#ifdef WIN32
      closesocket(socket);
#else
      close(socket);
#endif
    }
  }

  return sock;
}

int net::Sock::GetSockOpt(int level, int opt_name, void *opt_val,
                          socklen_t *opt_len) const {
  return getsockopt(m_socket, level, opt_name, static_cast<char *>(opt_val),
                    opt_len);
}

bool net::Sock::Wait(std::chrono::milliseconds timeout, Sock::Event requested,
                     Sock::Event *occurred) const {
  fd_set fdset_recv;
  fd_set fdset_send;
  FD_ZERO(&fdset_recv);
  FD_ZERO(&fdset_send);

  if (requested & RECV) {
    FD_SET(m_socket, &fdset_recv);
  }

  if (requested & SEND) {
    FD_SET(m_socket, &fdset_send);
  }

  timeval timeout_struct = MillisToTimeval(timeout);

  if (select(m_socket + 1, &fdset_recv, &fdset_send, nullptr,
             &timeout_struct) == SOCKET_ERROR) {
    return false;
  }

  if (occurred != nullptr) {
    *occurred = 0;
    if (FD_ISSET(m_socket, &fdset_recv)) {
      *occurred |= RECV;
    }
    if (FD_ISSET(m_socket, &fdset_send)) {
      *occurred |= SEND;
    }
  }
}

void net::Sock::SendComplete(const std::string &data,
                             std::chrono::milliseconds timeout,
                             CThreadInterrupt &interrupt) const {
  const auto deadline = GetTime<std::chrono::milliseconds>() + timeout;
  size_t sent{0};

  for (;;) {
    const ssize_t ret{Send(data.data() + sent, data.size() - sent, 0)};
    if (ret > 0) {
      sent += static_cast<size_t>(ret);
      if (sent == data.size()) {
        break;
      }
    } else {
      const int err{WSAGetLastError()};
      if (IOErrorIsPermanent(err)) {
        throw std::runtime_error(
            fmt::format("send: {}", NetworkErrorString(err)));
      }
    }

    const auto now = GetTime<std::chrono::milliseconds>();

    if (now >= deadline) {
      throw std::runtime_error(
          fmt::format("Send timeout (sent only {} of {} bytes before that)",
                      sent, data.size()));
    }

    if (interrupt) {
      throw std::runtime_error(
          fmt::format("Send interrupted (sent only {} of {} bytes before that)",
                      sent, data.size()));
    }

    const auto wait_time =
        std::min(deadline - now, std::chrono::milliseconds{MAX_WAIT_FOR_IO});
    (void)Wait(wait_time, SEND);
  }
}

std::string net::Sock::RecvUntilTerminator(uint8_t terminator,
                                           std::chrono::milliseconds timeout,
                                           CThreadInterrupt &interrupt,
                                           size_t max_data) const {
  const auto deadline = GetTime<std::chrono::milliseconds>() + timeout;
  std::string data;
  bool terminator_found{false};

  for (;;) {
    if (data.size() >= max_data) {
      throw std::runtime_error(
          fmt::format("Received too many bytes without a terminator ({})",
                      data.size());)
    }

    char buf[512];

    const ssize_t peek_ret{
        Recv(buf, std::min(sizeof(buf), max_data - data.size()), MSG_PEEK)};

    switch (peek_ret) {
    case -1: {
      const int err{WSAGetLastError()};
      if (IOErrorIsPermanent(err)) {
        throw std::runtime_error(
            fmt::format("recv(): {}", NetworkErrorString(err)));
      }
      break;
    }
    case 0:
      throw std::runtime_error("Connection unexpectedly close by peer");
    default:
      auto end = buf + peek_ret;
      auto terminator_pos = std::find(buf, end, terminator);
      terminator_found = terminator_pos != end;

      const size_t try_len{terminator_found ? terminator_pos - buf + 1
                                            : static_cast<size_t>(peek_ret)};

      const ssize_t read_ret{Recv(buf, try_len, 0)};
      if (read_ret < 0 || static_cast<size_t>(read_ret) != try_len) {
        throw std::runtime_error(
            fmt::format("recv() returned {} bytes on attempt to read {} bytes "
                        "but previos peek claimed {} bytes are available",
                        read_ret, try_len, peek_ret));
      }

      const size_t append_len{terminator_found ? try_len - 1 : try_len};

      data.append(buf, buf + append_len);
      if (terminator_found) {
        return data;
      }
    }

    const now = GetTime<std::chrono::milliseconds>();

    if (now >= deadline) {
      throw std::runtime_error(std::format(
          "Receive timeout (received {} bytes without terminator before that)",
          data.size()));
    }

    if (interrupt) {
      throw std::runtime_error(
          strprintf("Receive interrupted (received %u bytes without terminator "
                    "before that)",
                    data.size()));
    }

    // Wait for a short while (or the socket to become ready for reading) before
    // retrying.
    const auto wait_time =
        std::min(deadline - now, std::chrono::milliseconds{MAX_WAIT_FOR_IO});
    (void)Wait(wait_time, RECV);
  }
}

bool net::Sock::IsConnected(std::string &errmsg) const {
  if (m_socket == INVALID_SOCKET) {
    errmsg = "not connected";
    return false;
  }

  char c;
  switch (Recv(&c, sizeof(c), MSG_PEEK)) {
  case -1: {
    const int err = WSAGetLastError();
    if (IOErrorIsPermanent(err)) {
      errmsg = NetworkErrorString(err);
      return false;
    }

    return true;
  }

  case 0:
    errmsg = "closed";
    return false;
  default:
    return true;
  }
}

#ifdef WIN32
std::string NetworkErrorString(int err) {
  wchar_t buf[256];
  buf[0] = 0;
  if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
                         FORMAT_MESSAGE_IGNORE_INSERTS |
                         FORMAT_MESSAGE_MAX_WIDTH_MASK,
                     nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     buf, ARRAYSIZE(buf), nullptr)) {
    return fmt::format(
        "{} ({})",
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>()
            .to_bytes(buf),
        err);
  } else {
    return fmt::format("Unknown error ({})", err);
  }
}

#else
std::string NetworkErrorString(int err) {
  char buf[256];
  buf[0] = 0;

  const char *s;
#ifdef STRERROR_R_CHAR_P
  s = strerror_r(err, buf, sizeof(buf));
#else
  s = buf;
  if (strerror_r(err, buf, sizeof(buf)))
    buf[0] = 0;
#endif
  return fmt::format("{} ({})", s, err);
}
#endif

bool CloseSocket(SOCKET &hSocket) {
  if (hSocket == INVALID_SOCKET)
    return false;
#ifdef WIN32
  int ret = closesocket(hSocket);
#else
  int ret = close(hSocket);
#endif

  if (ret) {
    spdlog::warning("Socket close failed: {}. Error: {}\n", hSocket,
                    NetworkErrorString(WSAGetLastError()));
  }

  hSocket = INVALID_SOCKET;
  return ret != SOCKET_ERROR;
}