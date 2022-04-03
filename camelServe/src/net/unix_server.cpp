#include <algorithm>
#include <string.h>
#include <string>
#include <vector>

#include "fmt/core.h"
#include "fmt/format.h"
#include "graph/graph.h"
#include "net/sock.h"
#include "net/unix_server.h"
#include "spdlog/spdlog.h"

net::UnixServer::UnixServer() {}

bool net::UnixServer::Listen() {
  int sock_ret = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (sock_ret == -1) {
    spdlog::info("Connection socket failed");
    return false;
  }

  m_sock = std::make_unique<Sock>(static_cast<unsigned int>(sock_ret));

  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sun_family = AF_UNIX;

  strncpy(m_addr.sun_path, SOCK_NAME, sizeof(m_addr.sun_path) - 1);

  int ret =
      bind(m_sock->Get(), reinterpret_cast<const struct sockaddr *>(&m_addr),
           sizeof(m_addr));
  if (ret == -1) {
    spdlog::info("Bind socket failed");
    return false;
  }

  ret = listen(m_sock->Get(), BACKLOG_SIZE);
  if (ret == -1) {
    spdlog::info("Listen socket failed");
    return false;
  }

  return true;
}

std::unique_ptr<net::Sock> net::UnixServer::Accept() const {
  if (m_sock == nullptr) {
    spdlog::info("m_sock is emptied");
    return nullptr;
  }

#ifdef WIN32
  static constexpr auto ERR = INVALID_SOCKET;
#else
  static constexpr auto ERR = SOCKET_ERROR;
#endif

  std::unique_ptr<Sock> sock;
  int socket = accept(m_sock->Get(), nullptr, nullptr);
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

struct Reply {
  bool success{true};
  std::string msg{""};

  std::string ToStr() {
    return fmt::format(R"(
                  {{
                    "success": {},
                    "msg": {}
                  }}
                )",
                       success, msg);
  }
};

bool net::UnixServer::HandleRequestAndSendReply(
    std::unique_ptr<Sock> sock) const {
  std::string query{""};
  Reply reply{};
  auto send_response = [&]() {
    try {
      spdlog::info("Response: {}", query);
      sock->SendComplete(std::move(query), MAX_WAIT_FOR_IO);
      return true;
    } catch (const std::runtime_error &e) {
      spdlog::error("{}", NetworkErrorString(WSAGetLastError()));
      return false;
    }
  };

  try {
    query = sock->RecvUntilTerminator('\n', MAX_WAIT_FOR_IO, MAX_MSG_SIZE);
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

  std::vector<std::string> coors;
  size_t last_pos = 0, pos = query.find(';');
  while (pos != std::string::npos) {
    coors.emplace_back(std::string(query.c_str() + last_pos, pos - last_pos));
    last_pos = pos + 1;
    pos = query.find(';', pos + 1);
  }

  if (coors.size() != 4) {
    reply = Reply{false, "Coordinate is not valid"};
    return send_response();
  }

  auto &&[ok, source, target] =
      GraphFindNearestSourceDestf({std::stod(coors[0]), std::stod(coors[1])},
                                  {std::stod(coors[2]), std::stod(coors[3])});

  if (!ok) {
    reply = Reply{false, "An error occured, please try later"};
    return send_response();
  }

  auto search_result = GraphSearchf(std::move(source), std::move(target));
  if (!search_result.has_value()) {
    reply = Reply{false, "An error occured, please try later"};
    return send_response();
  }

  reply = Reply{true, std::move(*search_result)};
  return send_response();
}

net::UnixServer &UnixServerInstance() {
  static net::UnixServer *g_unix_server{new net::UnixServer()};
  return *g_unix_server;
}
