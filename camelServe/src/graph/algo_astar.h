#ifndef GRAPH_ALGO_ASTAR_H
#define GRAPH_ALGO_ASTAR_H

#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class AlgoBiAstar : public Algorithm {
private:
public:
  AlgoBiAstar() {}

  bool InitGraphV() override;
  bool EnabledV() override;
  std::string DoQueryV(Vertex &&source, Vertex &&target) override {
    return this->m_search_result.ToJsonStr();
  }
};
} // namespace graph

#endif // GRAPH_ALGO_ASTAR_H