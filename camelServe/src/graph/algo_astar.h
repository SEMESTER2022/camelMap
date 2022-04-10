#ifndef GRAPH_ALGO_ASTAR_H
#define GRAPH_ALGO_ASTAR_H

#include <string>
#include <vector>

#include "graph/algo_dijkstra.h"
#include "graph/def.h"

namespace graph {
class AlgoAstar : public AlgoDijkstra {
private:
  std::vector<std::pair<Vertex, Coordinate>> m_landmark;
  PotentialWeightList m_potential_dist;

  double PotentialFunction();
  bool PreprocessingAStarLandmark();

public:
  AlgoAstar() {}

  bool InitStrategyV() override;
  std::string DoQueryV(Vertex &&source, Vertex &&target) override;
};
} // namespace graph

#endif // GRAPH_ALGO_ASTAR_H
