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
  bool Process(AdjacentList &, VisitedList &, WeightList &, MinPriorityQueue &,
               BreakCondition, UpdateBestDist);

  std::string Dijkstra(Vertex &&, Vertex &&);
  std::string BiDijkstra(Vertex &&, Vertex &&);
  bool ReadGraphRawDataFromFile();

public:
  AlgoDijkstra() : Algorithm() {}
  bool InitStrategyV() override;

  std::string DoQueryV(Vertex &&, Vertex &&) override;
};

} // namespace graph

#endif // GRAPH_ALGO_DIJKSTRA_H