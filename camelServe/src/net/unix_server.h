#ifndef NET_UNIX_SERVER_H
#define NET_UNIX_SERVER_H

#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "graph/def.h"
#include "net/sock.h"

namespace net {
#define SOCK_NAME "/tmp/camel.sock"
static constexpr uint32_t BACKLOG_SIZE = 50;

static constexpr size_t MAX_MSG_SIZE{65536};

class UnixServer {
private:
  struct sockaddr_un m_addr;

  std::shared_ptr<Sock> m_sock;
  std::mutex m_mutex;

public:
  UnixServer();

  ~UnixServer();

  [[nodiscard]] bool Init();

  [[nodiscard]] bool Shutdown();

  [[nodiscard]] std::shared_ptr<net::Sock> Accept() const;

  [[nodiscard]] bool HandleRequestAndSendReply(std::shared_ptr<Sock> sock);
};
} // namespace net

net::UnixServer &UnixServerInstance();

#endif // NET_UNIX_SERVER_H
