#include "graph/def.h"

#include <string>

#include "fmt/core.h"
#include "fmt/ranges.h"

namespace graph {
struct GReplyTypeDef {
  GReplyType type;
  std::string name;
};

// clang-format off
const GReplyTypeDef ResultTypeDefs[] =
{
    {GReplyType::TOTAL_DIST_ONLY, "total distance only"},
    {GReplyType::SHORTEST_COOR_ONLY, "shortest path coordinate"},
    {GReplyType::SHORTEST_AND_VISITTED_COOR, "shortest path and visitted coordinate"}
};
// clang-format on

std::string SearchReply::ToJsonStr() {
  std::string out =
      fmt::format(R"(
            {{
              "type": "{}",
              "is_success": {},
              "total_dist": {},
              "shortest_coordinate": {},
              "visited_none_shortest_coordinate": {}
            }}
          )",
                  ResultTypeDefs[this->m_reply_type].name, this->m_is_success,
                  this->m_total_dist, this->m_shortest_coor_list,
                  this->m_visitted_none_shortest_coor_list);

  return out;
}

} // namespace graph
