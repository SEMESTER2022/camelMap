#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <type_traits>
#include <vector>

#include "config/core.h"
#include "graph/algo_dijkstra.h"
#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class Graph {
private:
  std::unique_ptr<Algorithm> m_search_strategy;

public:
  bool InitGraph() {
    assert(this->m_search_strategy != nullptr);
    return this->m_search_strategy->InitGraphV();
  }

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

#endif // GRAPH_GRAPH_H