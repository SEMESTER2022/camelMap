#ifndef GRAPH_DEF_H
#define GRAPH_DEF_H

#include <array>
#include <queue>
#include <string>
#include <vector>

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

enum GReplyType : uint32_t {
  TOTAL_DIST_ONLY = 0, // for testing
  SHORTEST_COOR_ONLY,
  SHORTEST_AND_VISITTED_COOR,
};

enum GSearchAlgorithm : uint32_t {
  DIJKSTRA,
  BIDIJKSTRA,
  BIASTAR_LANDMARK,
};

struct SearchReply {
  GReplyType m_reply_type;
  bool m_is_success{false};
  Weight m_total_dist{0};
  CoordinateList m_shortest_coor_list;
  CoordinateList m_visitted_none_shortest_coor_list;

  std::string ToJsonStr();
};

} // namespace graph

#endif // GRAPH_DEF_H
