#ifndef GRAPH_ALGO_DIJKSTRA_H
#define GRAPH_ALGO_DIJKSTRA_H

#include "fmt/core.h"
#include "fmt/ranges.h"

#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class AlgoDijkstra : public Algorithm {
private:
protected:
  template <class BreakCondition, class UpdateBestDist>
  bool Process(AdjacentList &adj, VisitedList &visited, WeightList &distance,
               MinPriorityQueue &pq, BreakCondition break_condition,
               UpdateBestDist update_best_dist);

  std::string BiDijkstra(Vertex &&, Vertex &&);

public:
  AlgoDijkstra() : Algorithm() {}
  bool InitGraphV() override;
  bool EnabledV() override { return this->m_process_status; }

  std::string DoQueryV(Vertex &&, Vertex &&) override;
};

} // namespace graph

#endif // GRAPH_ALGO_DIJKSTRA_H