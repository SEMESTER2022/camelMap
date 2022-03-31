#ifndef GRAPH_DEF_H
#define GRAPH_DEF_H

#include <array>
#include <queue>
#include <string>
#include <vector>

namespace graph {
using Vertex = uint32_t;
using Weight = uint64_t;
using Coordinate = std::array<float, 2>;
using VisitedList = std::vector<bool>;
using WeightList = std::vector<Weight>;
using PotentialWeightList = std::vector<double>;
using VertexList = std::vector<Vertex>;
using TargetList = std::vector<std::pair<Vertex, Weight>>;
using AdjacentList = std::vector<TargetList>;
using CoordinateList = std::vector<Coordinate>;

template <class WeightCompare>
using MinPriorityQueue = std::priority_queue<Vertex, VertexList, WeightCompare>;

const Weight kInfinite = std::numeric_limits<Weight>::max();

enum GProcessBiDijkstraStatus : uint32_t {
  NONE = 0,
  UNPROCESS,
  ONPROCESSING,
  FAILED,
  PROCESSED,
};

enum GResultTypes : uint32_t {
  TOTAL_DIST_ONLY = 0,
  SHORTEST_COOR,
  SHORTEST_AND_VISITTED_COOR,
};

struct SearchResult {
  GResultTypes m_result_type;
  bool m_is_success{false};
  Weight m_total_dist{0};
  CoordinateList m_shortest_coor_list;
  CoordinateList m_visitted_none_shortest_coor_list;

  std::string ToJsonStr();
};

} // namespace graph

#endif // GRAPH_DEF_H