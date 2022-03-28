#include "fmt/core.h"

#include "graph/algo_astar.h"
#include "graph/algo_dijkstra.h"
#include "graph/graph.h"

#include <memory>

#include "spdlog/spdlog.h"

int main() {
  graph::AlgoDijkstra algo = graph::AlgoDijkstra{};
  algo.SetInFile("/home/balebom/github/teamwork/camelMap/camelServe/data/"
                 "USA-road-d.NY.gr",
                 "/home/balebom/github/teamwork/camelMap/camelServe/data/"
                 "USA-road-d.NY.gr",
                 "/home/balebom/github/teamwork/camelMap/camelServe/data/"
                 "USA-road-d.NY.gr");

  GraphInstance().AssignRuntimeStrategy(
      std::make_unique<graph::AlgoDijkstra>(std::move(algo)));
  bool ok = GraphInstance().InitStrategy();
  if (ok == false) {
    spdlog::info("Init graph failed");
    return 0;
  }
  spdlog::info("Result from 12 to 308 is {}",
               GraphInstance().DoSearch(12, 308));

  graph::AlgoAstar algo_astar = graph::AlgoAstar{};
  algo_astar.SetInFile("/home/balebom/github/teamwork/camelMap/camelServe/data/"
                       "USA-road-d.NY.gr",
                       "/home/balebom/github/teamwork/camelMap/camelServe/data/"
                       "USA-road-d.NY.gr",
                       "/home/balebom/github/teamwork/camelMap/camelServe/data/"
                       "USA-road-d.NY.gr");

  GraphInstance().AssignRuntimeStrategy(
      std::make_unique<graph::AlgoAstar>(std::move(algo_astar)));

  ok = GraphInstance().InitStrategy();
  if (ok == false) {
    spdlog::info("Init graph failed again");
    return 0;
  }

  spdlog::info("Result from 12 to 28 is {}", GraphInstance().DoSearch(12, 28));

  spdlog::info("Hehe {}", algo.DoQueryV(17, 32));
  return 0;
}
