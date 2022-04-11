#ifndef GRAPH_ALGO_ASTAR_H
#define GRAPH_ALGO_ASTAR_H

#include <optional>

#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class AlgoAstar : public Algorithm {
public:
  AlgoAstar() : Algorithm(){};
  bool InitStrategyV() override;

  std::string DoQueryV(Vertex &&source, Vertex &&target) override;

protected:
  bool ReadGraphData() override;

  PotentialWeight PotentialFunction(Vertex target, Vertex node);

  PotentialWeight BiAstarPotentialFunction_f(Vertex source, Vertex target,
                                             Vertex node);
  PotentialWeight BiAstarPotentialFunction_r(Vertex source, Vertex target,
                                             Vertex node);

  template <class BreakCondition, class UpdateBestDist, class MinPQ,
            class BackTrace, class CalculatePotentialWeight>
  bool Process(const AdjacentList &adj, VisitedList &visited,
               PotentialWeightList &distance, MinPQ &pq, BackTrace &back_trace,
               BreakCondition break_condition, UpdateBestDist update_best_dist,
               CalculatePotentialWeight calculate_potential_weight);

  std::string BiAstar(Vertex &&source, Vertex &&target);

private:
};
} // namespace graph

#endif // GRAPH_ALGO_ASTAR_H
