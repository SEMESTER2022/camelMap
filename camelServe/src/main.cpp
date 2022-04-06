#include <memory>
#include <stdlib.h>

#include "graph/graph.h"
#include "net/mongo.h"
#include "net/unix_server.h"

#include "fmt/core.h"
#include "spdlog/spdlog.h"

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

int main(int argc, char *argv[]) {
  spdlog::info("Init application ....");
  bool ok = false;

  spdlog::info("Start graph ... ");
  ok = init_graph();
  if (ok == false) {
    spdlog::error("Init graph failed.");
    return EXIT_FAILURE;
  }
  spdlog::info("Start graph ok.");
  spdlog::info("Start mongo ... ");

  ok = init_mongo();
  if (ok == false) {
    spdlog::error("Init mongo failed.");
    return EXIT_FAILURE;
  }
  spdlog::info("Start mongo ok.");
  spdlog::info("Start unix_server ...");

  ok = init_unix_server();
  if (ok == false) {
    spdlog::error("Init unix_server failed");
    return EXIT_FAILURE;
  }
  spdlog::info("Start unix server ok.");

  while (true) {
  }

  return EXIT_SUCCESS;
}
