#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "config.h"
#include "graph/algo_astar.h"
#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class Graph {
private:
  std::unique_ptr<Algorithm> m_search_strategy;

public:
  inline GAlgorithmReq GetAlgorithmReq() {
    return this->m_search_strategy->GetAlgorithmReq();
  }

  inline GResponseReq GetResponseReq() {
    return this->m_search_strategy->GetResponseReq();
  }

  inline void SetAlgorithmReq(GAlgorithmReq algorithm_req) {
    this->m_search_strategy->SetAlgorithmReq(algorithm_req);
  }

  inline void SetResponseReq(GResponseReq response_req) {
    this->m_search_strategy->SetResponseReq(response_req);
  }

  bool Init() {
    // default bidijkstra algorithm
    bool ok = this->AssignRuntimeStrategy(std::make_unique<AlgoAstar>());
    if (ok) {
      this->SetAlgorithmReq(GAlgorithmReq::BI_ASTAR);
      this->SetResponseReq(GResponseReq::INHERIT_AND_SHORTEST_PATH_COORS);
    }

    return ok;
  }

  bool Shutdown() { return true; }

  bool Enabled() const {
    assert(this->m_search_strategy != nullptr);
    return this->m_search_strategy->EnabledV();
  }

  bool AssignRuntimeStrategy(std::unique_ptr<Algorithm> search_strategy) {
    this->m_search_strategy = std::move(search_strategy);
    return this->m_search_strategy->InitStrategyV();
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
