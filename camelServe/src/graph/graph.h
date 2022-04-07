#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <type_traits>
#include <vector>

#include "config.h"
#include "graph/algo_dijkstra.h"
#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class Graph {
private:
  std::unique_ptr<Algorithm> m_search_strategy;

public:
  bool Init() {
    // default bidijkstra algorithm
    this->m_search_strategy = std::make_unique<AlgoDijkstra>();
    this->m_search_strategy->SetInFile(CS_MAP_DATA_COOR_LOCATION,
                                       CS_MAP_DATA_DIST_LOCATION,
                                       CS_MAP_DATA_TIME_LOCATION);

    return this->m_search_strategy->InitStrategyV();
  }

  bool Shutdown() { return true; }

  bool Enabled() const {
    assert(this->m_search_strategy != nullptr);
    return this->m_search_strategy->EnabledV();
  }

  void AssignRuntimeStrategy(std::unique_ptr<Algorithm> search_strategy) {
    this->m_search_strategy = std::move(search_strategy);
  };

  std::string DoSearch(Vertex &&source, Vertex &&target) {
    assert(this->m_search_strategy != nullptr);
    return this->m_search_strategy->DoQueryV(std::move(source),
                                             std::move(target));
  }

  std::tuple<bool, Vertex, Vertex>
  FindNearestSourceDest(Coordinate &&src_coor, Coordinate &&dst_coor) const {
    return this->m_search_strategy->FindNearestSourceDestV(
        std::forward<Coordinate>(src_coor), std::forward<Coordinate>(dst_coor));
  }
};

} // namespace graph

graph::Graph &GraphInstance();

static inline std::optional<std::string>
GraphSearchf(graph::Vertex &&source, graph::Vertex &&target) noexcept {
  if (GraphInstance().Enabled()) {
    return GraphInstance().DoSearch(std::move(source), std::move(target));
  }

  return std::nullopt;
}

static inline std::tuple<bool, graph::Vertex, graph::Vertex>
GraphFindNearestSourceDestf(graph::Coordinate &&src_coor,
                            graph::Coordinate &&dst_coor) noexcept {
  if (GraphInstance().Enabled()) {
    return GraphInstance().FindNearestSourceDest(std::move(src_coor),
                                                 std::move(dst_coor));
  }

  return {false, 0, 0};
}

#endif // GRAPH_GRAPH_H