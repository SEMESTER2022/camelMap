#ifndef GRAPH__ALGO_BIASTAR_LANDMARK_H
#define GRAPH__ALGO_BIASTAR_LANDMARK_H

#include "config.h"

#ifdef USE_ALGO_BIASTAR_LANDMARK

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "graph/algorithm.h"
#include "graph/def.h"

namespace graph {
class AlgoBiAstarLandmark : public AlgoDijkstra {
public:
  AlgoBiAstarLandmark() {}

  bool InitStrategyV() override;
  std::string DoQueryV(Vertex &&source, Vertex &&target) override;

private:
  VertexList m_landmarks;

  std::map<Vertex, WeightList> m_map_potential_weights_f;
  std::map<Vertex, WeightList> m_map_potential_weights_r;

  AdjacentWeightList m_preprocessed_outgoing_weights;
  AdjacentWeightList m_preprocessed_incoming_weights;

  void AddPotentialEdge(Vertex source, Vertex target,
                        PotentialWeight potential_weight);

  bool ReadFileToLandMarkList();
  bool ReadFileToPreprocessedAdjacentWeightList();
  bool ReadGraphData() override;

  bool PreprocessingAStarLandmark();

  inline std::optional<Weight> PotentialFunction_f(Vertex landmark,
                                                   Vertex target, Vertex node);

  inline std::optional<Weight> PotentialFunction_r(Vertex landmark,
                                                   Vertex target, Vertex node);

  inline std::optional<PotentialWeight>
  BiAstarPotentialFunction_f(Vertex landmark, Vertex target, Vertex node);

  inline std::optional<PotentialWeight>
  BiAstarPotentialFunction_r(Vertex landmark, Vertex target, Vertex node);

  template <class BreakCondition, class UpdateBestDist, class MinPQ,
            class BackTrace, class CalculatePotentialWeight>
  bool Process(const AdjacentList &adj, visited &visited,
               PotentialWeightList &distance, MinPQ &pq, BackTrace &back_trace,
               BreakCondition break_condition, UpdateBestDist update_best_dist,
               CalculatePotentialWeight calculate_potential_weight);

  std::string BiAstarLandmark(Vertex source, Vertex target);
};
} // namespace graph

#endif // USE_BIASTAR_LANDMARK

#endif // GRAPH__ALGO_BIASTAR_LANDMARK_H
