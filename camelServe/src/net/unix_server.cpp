#include <algorithm>
#include <string.h>
#include <string>
#include <string_view>
#include <vector>

#include "fmt/core.h"
#include "fmt/format.h"
#include "graph/graph.h"
#include "net/sock.h"
#include "net/unix_server.h"
#include "spdlog/spdlog.h"

net::UnixServer::UnixServer() {}

bool net::UnixServer::Listen() {
  int connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (connection_socket == -1) {
    spdlog::info("Connection socket failed");
    return false;
  }

  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sun_family = AF_UNIX;

  strncpy(m_addr.sun_path, SOCK_NAME, sizeof(m_addr.sun_path) - 1);

  int ret =
      bind(connection_socket, (const struct sockaddr *)&m_addr, sizeof(m_addr));
  if (ret == -1) {
    spdlog::info("Bind socket failed");
    return false;
  }

  ret = listen(connection_socket, BACKLOG_SIZE);
  if (ret == -1) {
    spdlog::info("Listen socket failed");
    return false;
  }

  return true;
}

std::unique_ptr<Sock> net::UnixServer::Accept() const {
#ifdef WIN32
  static constexpr auto ERR = INVALID_SOCKET;
#else
  static constexpr auto ERR = SOCKET_ERROR;
#endif

  std::unique_ptr<Sock> sock;
  const auto socket = accept(m_socket, NULL, NULL);
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
  std::string msg{""};
  bool success{true};

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
  try {
    query = sock->RecvUntilTerminator('\n', MAX_WAIT_FOR_IO, MAX_MSG_SIZE);
  } catch (const std::runtime_error &e) {
    spdlog::error("Receive data: {}", e.what());
    reply = Reply{false, "Some thing went wrong, please try later"};
    goto send_response;
  }

  if (query->empty()) {
    spdlog::info("Query is emptied");
    reply = Reply{false, "Query is emptied"};
    goto send_response;
  }

  std::vector<std::string_view> coors;
  size_t last_pos = 0, pos = query.find(';');
  while (pos != std::string::npos) {
    semicolons.emplace_back(
        std::string_view(query.c_str(0 + last_pos, pos - last_pos)));
    last_pos = pos + 1;
    pos = query.find(';', pos + 1);
  }

  if (coors.size() != 4) {
    reply = Reply{false, "Coordinate is not valid"};
    goto send_response;
  }

  // source lng, source lat, target lng, target lat
  // ToDo: Replace with structure binding [source, target]
  auto nearest_result =
      GraphFindNearestSourceDestf({std::stof(coors[0]), std::stof(coors[1])},
                                  {std::stof(coors[2]), std::stof(coors[3])});

  if (!nearest_result.has_value()) {
    reply = Reply{false, "An error occured, please try later"};
    goto send_response;
  }

  auto search_result = GraphSearchf(result->first, result->second);
  if (!search_result.has_value()) {
    reply = Reply{false, "An error occured, please try later"};
    goto send_response;
  }

  reply = {true, std::move(search_result)};

send_response:
  try {
    sock->SendComplete();
  } catch (const std::runtime_error &e) {
    spdlog::error("{}", NetworkErrorString());
    reply = Reply{false, "An error occured, please try later"};
    return false;
  }

  return true;
}

net::UnixServer &UnixServerInstance() {
  static net::UnixServer *g_unix_server{new net::UnixServer()};
}
