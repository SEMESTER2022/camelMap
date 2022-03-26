#include "fmt/core.h"

#include "graph/algo_dijkstra.h"
#include "graph/graph.h"

#include <memory>

#include "spdlog/spdlog.h"

int main() {
  graph::AlgoDijkstra algo = graph::AlgoDijkstra{};
  algo.SetInFile("/home/balebom/github/teamwork/camelMap/camelServe/data/"
                 "USA-road-d.NY.gr",
                 "/home/balebom/github/teamwork/camelMap/camelServe/data/"
                 "USA-road-d.NY.gr", "/home/balebom/github/teamwork/camelMap/camelServe/data/"
                 "USA-road-d.NY.gr");

  GraphInstance().AssignRuntimeStrategy(
      std::make_unique<graph::AlgoDijkstra>(algo));
  bool ok = GraphInstance().InitGraph();
  if (ok == false) {
    spdlog::info("Init graph failed");
  }

  spdlog::info("Result from 12 to 28 is {}", GraphInstance().DoSearch(12, 28));
  return 0;
}
