#include <string.h>
#include <string>

#include "graph/request.h"
#include "net/sock.h"
#include "net/unix_server.h"

#include "fmt/core.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

net::UnixServer::UnixServer() {}

bool net::UnixServer::Init() {
  int sock_ret = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock_ret == -1) {
    spdlog::info("Connection socket failed");
    return false;
  }

  m_sock = std::make_shared<Sock>(static_cast<unsigned int>(sock_ret));
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sun_family = AF_UNIX;

  strncpy(m_addr.sun_path, SOCK_NAME, sizeof(m_addr.sun_path) - 1);

  int ret = bind(sock_ret, reinterpret_cast<const struct sockaddr *>(&m_addr),
                 sizeof(m_addr));
  if (ret == -1) {
    spdlog::error("Bind socket failed {}",
                  NetworkErrorString(WSAGetLastError()));
    return false;
  }

  ret = listen(m_sock->Get(), BACKLOG_SIZE);
  if (ret == -1) {
    spdlog::info("Listen socket failed");
    return false;
  }

  return true;
}

bool net::UnixServer::Shutdown() {
  SOCKET sock = this->m_sock->Get();
  bool ok = CloseSocket(sock);
  if (!ok) {
    spdlog::error("Close connection socket failed");
    return false;
  }

  unlink(SOCK_NAME);
  return true;
}

net::UnixServer::~UnixServer() {}

std::shared_ptr<net::Sock> net::UnixServer::Accept() const {
  if (m_sock == nullptr) {
    spdlog::info("m_sock is emptied");
    return nullptr;
  }

#ifdef WIN32
  static constexpr auto ERR = INVALID_SOCKET;
#else
  static constexpr auto ERR = SOCKET_ERROR;
#endif

  std::shared_ptr<Sock> sock;
  int socket = accept(m_sock->Get(), nullptr, nullptr);
  if (socket != ERR) {
    try {
      sock = std::make_shared<Sock>(socket);
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

struct Reply {
  bool is_success{true};
  std::string msg{""};

  std::string ToStr() {
    return fmt::format(R"(
                  {{
                    "is_success": {},
                    "msg": {}
                  }})",
                       is_success, msg);
  }
};

bool net::UnixServer::HandleRequestAndSendReply(std::shared_ptr<Sock> sock) {
  spdlog::info("start handler");
  std::string query{""};
  Reply reply{};
  auto send_response = [&]() {
    try {
      sock->SendComplete(std::move(reply.ToStr()), MAX_WAIT_FOR_IO);
      spdlog::info("reply: {}", reply.ToStr());
      return true;
    } catch (const std::runtime_error &e) {
      spdlog::error("{}", NetworkErrorString(WSAGetLastError()));
      return false;
    }
  };

  try {
    query = sock->RecvUntilTerminator('\n', MAX_WAIT_FOR_IO, MAX_MSG_SIZE);
    spdlog::info("Server receive query: {}", query);
  } catch (const std::runtime_error &e) {
    spdlog::error("Receive data: {}", e.what());
    reply = Reply{false, "Some thing went wrong, please try later"};
    return send_response();
  }

  if (query.empty()) {
    spdlog::info("Query is emptied");
    reply = Reply{false, "Query is emptied"};
    return send_response();
  }

  graph::Request request{query};
  std::optional<std::string> result = request.Handle();

  if (result.has_value()) {
    reply = Reply{true, *result};
  } else {
    reply = Reply{false, "Handle request failed"};
  }

  return send_response();
}

net::UnixServer &UnixServerInstance() {
  static net::UnixServer *g_unix_server{new net::UnixServer()};
  return *g_unix_server;
}
