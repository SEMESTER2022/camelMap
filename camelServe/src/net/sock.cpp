#include "net/sock.h"

#include <chrono>
#include <memory>
#include <string>

#include "util/time.h"

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

net::Sock::Sock(net::Sock &&other) {
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

net::SOCKET net::Sock::Release() {
  const SOCKET s = m_socket;
  m_socket = INVALID_SOCKET;
  return s;
}

void net::Sock::Reset() { CloseSocket(m_socket); }

ssize_t net::Sock::Recv(void *buf, size_t len, int flag) const {
  return recv(m_socket, static_cast<char *>(buf), len, flag);
}

ssize_t net::Sock::Send(void *buf, size_t len, int flag) const {
  return send(m_socket, static_cast<char *>(buf), len, flag);
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

  timeval timeout_struct = util::MillisToTimeval(timeout);

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

  return true;
}

void net::Sock::SendComplete(const std::string &data,
                             std::chrono::milliseconds timeout) const {
  const auto deadline = util::GetTime<std::chrono::milliseconds>() + timeout;
  size_t sent{0};

  for (;;) {
    ssize_t ret{Send(const_cast<char *>(data.data()) + sent, data.size() - sent,
                     MSG_NOSIGNAL)};
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

    const auto now = util::GetTime<std::chrono::milliseconds>();

    if (now >= deadline) {
      throw std::runtime_error(
          fmt::format("Send timeout (sent only {} of {} bytes before that)",
                      sent, data.size()));
    }

    const auto wait_time =
        std::min(deadline - now, std::chrono::milliseconds{MAX_WAIT_FOR_IO});
    (void)Wait(wait_time, SEND);
  }
}

std::string net::Sock::RecvUntilTerminator(uint8_t terminator,
                                           std::chrono::milliseconds timeout,
                                           size_t max_data) const {
  const auto deadline = util::GetTime<std::chrono::milliseconds>() + timeout;
  std::string data;
  bool terminator_found{false};

  for (;;) {
    if (data.size() >= max_data) {
      throw std::runtime_error(fmt::format(
          "Received too many bytes without a terminator ({})", data.size()));
    }

    char buf[512];

    const ssize_t peek_ret{
        Recv(buf, std::min(sizeof(buf), max_data - data.size()), MSG_PEEK)};

    switch (peek_ret) {
    case -1: {
      const int err{WSAGetLastError()};
      if (IOErrorIsPermanent(err)) {
        throw std::runtime_error(
            fmt::format("read(): {}", NetworkErrorString(err)));
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
            fmt::format("read() returned {} bytes on attempt to read {} bytes "
                        "but previos peek claimed {} bytes are available",
                        read_ret, try_len, peek_ret));
      }

      const size_t append_len{terminator_found ? try_len - 1 : try_len};

      data.append(buf, buf + append_len);
      if (terminator_found) {
        return data;
      }
    }

    const auto now = util::GetTime<std::chrono::milliseconds>();

    if (now >= deadline) {
      throw std::runtime_error(fmt::format(
          "Receive timeout (received {} bytes without terminator before that)",
          data.size()));
    }

    // Wait for a short while (or the socket to become ready for reading) before
    // retrying.
    const auto wait_time =
        std::min(deadline - now, std::chrono::milliseconds{MAX_WAIT_FOR_IO});
    (void)Wait(wait_time, RECV);
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

bool CloseSocket(net::SOCKET &hSocket) {
  if (hSocket == INVALID_SOCKET)
    return false;
#ifdef WIN32
  int ret = closesocket(hSocket);
#else
  int ret = close(hSocket);
#endif

  if (ret) {
    spdlog::warn("Socket close failed: {}. Error: {}\n", hSocket,
                 NetworkErrorString(WSAGetLastError()));
  }

  hSocket = INVALID_SOCKET;
  return ret != SOCKET_ERROR;
}
