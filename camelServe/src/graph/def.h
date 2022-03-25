#ifndef GRAPH_DEF_H
#define GRAPH_DEF_H

#include <array>
#include <string>
#include <vector>

namespace graph {
using Vertex = uint32_t;
using Dist = float;
using Coordinate = std::array<float, 2>;
using AdjacentList = std::vector<std::vector<std::pair<Vertex, Dist>>>;
using CoordinateList = std::vector<Coordinate>;
const Dist kInfinite = std::numeric_limits<Dist>::max();

enum GResultTypes : uint32_t {
  TOTAL_DIST_ONLY = 0,
  SHORTEST_COOR,
  SHORTEST_AND_VISITTED_COOR,
};

struct SearchResult {
  GResultTypes m_result_type;
  CoordinateList m_shortest_coor_list;
  CoordinateList m_visitted_none_shortest_coor_list;

  std::string ToString();
};

} // namespace graph

#endif // GRAPH_DEF_H