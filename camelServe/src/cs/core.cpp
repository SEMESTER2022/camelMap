#include "cs/core.h"

#include <csignal>
#include <queue>
#include <vector>

#include "spdlog/spdlog.h"

#include "config.h"
#include "graph/graph.h"
#include "net/mongo.h"
#include "net/unix_server.h"
#include "util/threadpool.h"

enum APP_SERVICE : uint32_t {
  GRAPH = 0,
  MONGO,
  UNIX_SERVER,
};

bool init_graph() {
  // TODO
  // set strategy to astar
  return GraphInstance().Init();
}

bool init_mongo() {
  // TODO
  return MongoInstance().Init();
}

bool init_unix_server() {
  // TODO
  return UnixServerInstance().Init();
}

std::priority_queue<APP_SERVICE, std::vector<APP_SERVICE>> service_queue;

bool shutdownf() {
  while (!service_queue.empty()) {
    auto service = service_queue.top();
    bool ok{false};
    service_queue.pop();
    switch (service) {
      // TODO: Shutdown services
    case APP_SERVICE::GRAPH:
      ok = GraphInstance().Shutdown();
      if (!ok) {
        spdlog::error("Shut down service graph failed");
      } else {
        spdlog::info("Shut down service graph ok.");
      }
      break;
    case APP_SERVICE::MONGO:
      ok = MongoInstance().Shutdown();
      if (!ok) {
        spdlog::error("Shut down service mongo failed");
      } else {
        spdlog::info("Shut down service mongo ok");
      }
      break;
    case APP_SERVICE::UNIX_SERVER:
      ok = UnixServerInstance().Shutdown();
      if (!ok) {
        spdlog::error("Shut down service unix server failed");
      } else {
        spdlog::info("Shut down service unix server ok");
      }
      break;
    default:
      break;
    }
  }

  return true;
}

bool cs::init() {
  spdlog::set_pattern("[%H:%M:%S %z] [%^-%L-%$] [thread %t] %v");

  spdlog::info("Init application ....");
  bool ok = false;

  spdlog::info("Start graph ... ");
  ok = init_graph();
  if (ok == false) {
    spdlog::error("Init graph failed.");
    ok = shutdownf();
    return false;
  }
  service_queue.push(APP_SERVICE::GRAPH);
  spdlog::info("Start graph ok.");
  spdlog::info("Start mongo ... ");

  ok = init_mongo();
  if (ok == false) {
    spdlog::error("Init mongo failed.");
    return false;
  }
  service_queue.push(APP_SERVICE::MONGO);
  spdlog::info("Start mongo ok.");
  spdlog::info("Start unix_server ...");

  ok = init_unix_server();
  if (ok == false) {
    spdlog::error("Init unix_server failed");
    return false;
  }
  service_queue.push(APP_SERVICE::UNIX_SERVER);
  spdlog::info("Start unix server ok.");

  return true;
}

bool restart_service() {
  bool ok = shutdownf();
  if (!ok) {
    spdlog::error("Failed to restart service (shutdownf)");
    return false;
  }

  ok = cs::init();
  if (!ok) {
    spdlog::error("Failed to restart service (init)");
    return false;
  }

  return ok;
}

void signal_handler(int signal_num) {
  spdlog::info("Receive interrupt signal, server is shutting down");
  (void)shutdownf();
  exit(signal_num);
}

void cs::mainloop() {
  signal(SIGINT, signal_handler);

  ThreadPool request_pool(4);
  while (true) {
    auto data_sock = UnixServerInstance().Accept();
    if (data_sock->Get() == -1) {
      spdlog::error("Accept socket error");
      bool ok = restart_service();
      if (ok == false) {
        spdlog::info(
            "An error occured, please try again. Preparing for shutting down");
        break;
      }
    }

    std::future<bool> future_ret =
        request_pool.enqueue([data_sock = std::move(data_sock)] {
          spdlog::info("Handle request ... for socket {} ", data_sock->Get());
          bool result =
              UnixServerInstance().HandleRequestAndSendReply(data_sock);
          return result;
        });

    if (!future_ret.get()) {
      spdlog::info("send data failed");
    } else {
      spdlog::info("Success handle for request");
    }
  }
  return;
}

void cs::shutdown() {
  bool ok = shutdownf();
  if (!ok) {
    spdlog::error("Shutdown service failed!");
  } else {
    spdlog::info("Shutdown service sucessfully! Good bye.");
  }
  return;
}
