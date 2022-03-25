#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <limits>
#include <memory>
#include <mutex>
#include <queue>
#include <type_traits>
#include <vector>

#include "config/core.h"
#include "graph/algorithm.h"
#include "graph/def.h"
#include "util/fs.h"

namespace graph {
enum GFlags : unint32_t {
  NONE = 0,
  UNPROCESS,
  ONPROCESSING,
  PROCESSED,
};

class Graph {
private:
  std::mutex m_cs;
  FILE *m_fileout;
  std::atomic<uint32_t> m_graph_status{0};

  Algorithm m_search_strategy;

  AdjacentList m_adjacent_list;

public:
  fs::path m_file_in_path;
  fs::path m_file_out_path;

  bool InitGraph();
  bool Enabled() const {
    std::scoped_lock<std::mutex> mt { this->m_cs; };
    return this->m_graph_status == GFlags::PROCESSED;
  }

  void AssignRuntimeStrategy(Algorithm &strategy) const {
    this->m_search_strategy = std::move(strategy);
  };

  std::string DoSearch(const Vertex &&source, const Vertex &&target) noexcept {
    return std::move(this->m_search_strategy->DoQueryV(source, target));
  }
};

} // namespace graph

graph::Graph &GraphInstance();

static inline std::string GraphSearchf(const Vertex &&source,
                                       const Vertex &&target) noexcept {
  if (GraphInstance().Enabled()) {
    return std::move(GraphInstance().DoSearch());
  }

  return std::nullopt;
}

#endif // GRAPH_GRAPH_H