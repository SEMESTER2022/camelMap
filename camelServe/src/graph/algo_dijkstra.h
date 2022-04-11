#ifndef GRAPH_ALGO_DIJKSTRA_H
#define GRAPH_ALGO_DIJKSTRA_H

#include "fmt/core.h"
#include "fmt/ranges.h"

#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class AlgoDijkstra : public Algorithm {
protected:
  template <class BreakCondition, class UpdateBestDist, class MinPQ,
            class BackTrace, class TraceTraversedCoor>
  bool Process(const AdjacentList &adj, const AdjacentWeightList &adjw,
               VisitedList &visited, WeightList &distance, MinPQ &pq,
               BackTrace &back_trace, BreakCondition break_condition,
               UpdateBestDist update_best_dist,
               TraceTraversedCoor trace_traversed_coor);

  std::string Dijkstra(Vertex &&source, Vertex &&target);
  std::string BiDijkstra(Vertex &&source, Vertex &&target);

  bool ReadGraphData() override;

public:
  AlgoDijkstra() : Algorithm() {}
  bool InitStrategyV() override;

  std::string DoQueryV(Vertex &&source, Vertex &&target) override;

private:
};

} // namespace graph

#endif // GRAPH_ALGO_DIJKSTRA_H
