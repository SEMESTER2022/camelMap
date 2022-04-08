#ifndef GRAPH_ALGO_ASTAR_H
#define GRAPH_ALGO_ASTAR_H

#include "graph/algo_dijkstra.h"
#include "graph/def.h"

namespace graph {
class AlgoAstar : public AlgoDijkstra {
private:
  PotentialWeightList m_potential_dist;

  double PotentialFunction();
  bool PreProcessBiDijkstraing();

public:
  AlgoAstar() {}

  bool InitStrategyV() override;
  std::string DoQueryV(Vertex &&source, Vertex &&target) override;
};
} // namespace graph

#endif // GRAPH_ALGO_ASTAR_H
