#ifndef GRAPH_DEF_H
#define GRAPH_DEF_H

#include <array>
#include <queue>
#include <string>
#include <vector>
#include <limits>

namespace graph {
using Vertex = uint32_t;
using Weight = uint64_t;
using PotentialWeight = double;
using Coordinate = std::array<double, 2>;
using VisitedList = std::vector<bool>;
using WeightList = std::vector<Weight>;
using PotentialWeightList = std::vector<PotentialWeight>;
using VertexList = std::vector<Vertex>;
using AdjacentList = std::vector<VertexList>;
using AdjacentWeightList = std::vector<WeightList>;
using CoordinateList = std::vector<Coordinate>;

template <class WeightCompare>
using MinPriorityQueue = std::priority_queue<Vertex, VertexList, WeightCompare>;

const Weight kInfiniteWeight = std::numeric_limits<Weight>::max();
const PotentialWeight kInfinitePotentialWeight =
    std::numeric_limits<PotentialWeight>::max();

enum GProcessStatus : uint32_t {
  UNPROCESS = 0,
  FAILED,
  PROCESSED,
};

enum GResponseReq : int32_t {
  NO_RESPONSE_REQ = -1,
  TOTAL_DIST_ONLY = 0,
  INHERIT_AND_SHORTEST_PATH_COORS,
  INHERIT_AND_TRAVERSE_COORS,
};

enum GAlgorithmReq : int32_t {
  NO_ALGORITHM_REQ = -1,
  BI_DIJKSTRA = 0,
  BI_ASTAR,
  DIJKSTRA,
  ASTAR,
  BI_ASTAR_LANDMARK,
};

struct GSearchReply {
  bool m_is_success{false};
  Weight m_total_dist{0};
  CoordinateList m_shortest_path_coors{};
  CoordinateList m_traverse_f_coors{};
  CoordinateList m_traverse_r_coors{};

  std::string ToJsonStr();
};

} // namespace graph

#endif // GRAPH_DEF_H
