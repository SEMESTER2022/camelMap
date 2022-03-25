#include "graph/def.h"

#include <string>

#include "fmt/core.h"
#include "fmt/ranges.h"

namespace graph {
struct GResultTypeDef {
  GResultTypes type;
  std::string name;
};

// clang-format off
const GResultTypeDef ResultTypeDefs[] =
{
    {GResultTypes::TOTAL_DIST_ONLY, "total distance only"},
    {GResultTypes::SHORTEST_COOR, "shortest path coordinate"},
    {GResultTypes::SHORTEST_AND_VISITTED_COOR, "shortest path and visitted coordinate"}
};
// clang-format on

std::string SearchResult::ToString() {
  std::string out = fmt::format(R"(
            {{"type": {},
            "shortest_coordinate": {},
            "visited_none_shortest_coordinate": {}}}

    )",
                                ResultTypeDefs[this->m_result_type].name,
                                this->m_shortest_coor_list,
                                this->m_visitted_none_shortest_coor_list);

  return out;
}

} // namespace graph