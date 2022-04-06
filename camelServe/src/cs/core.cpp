#include "cs/core.h"

#include "spdlog/spdlog.h"

#include "graph/graph.h"
#include "net/mongo.h"
#include "net/unix_server.h"

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

bool cs::AppInit() {
    spdlog::info("hello from share lib");

    spdlog::info("Init application ....");
    bool ok = false;

    spdlog::info("Start graph ... ");
    ok = init_graph();
    if (ok == false) {
        spdlog::error("Init graph failed.");
        return false;
    }
    spdlog::info("Start graph ok.");
    spdlog::info("Start mongo ... ");

    ok = init_mongo();
    if (ok == false) {
        spdlog::error("Init mongo failed.");
        return false;
    }
    spdlog::info("Start mongo ok.");
    spdlog::info("Start unix_server ...");

    ok = init_unix_server();
    if (ok == false) {
        spdlog::error("Init unix_server failed");
        return false;
    }

    spdlog::info("Start unix server ok.");

    return true;
}

