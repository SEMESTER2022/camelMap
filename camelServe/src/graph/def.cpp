#include "graph/def.h"

#include <string>

#include "fmt/core.h"
#include "fmt/ranges.h"

#include "spdlog/spdlog.h"

namespace graph {
std::string GSearchReply::ToJsonStr() {
  std::string out = fmt::format(
      R"(
            {{
              "is_success": {},
              "total_dist": {},
              "shortest_path_coors": {},
              "traverse_f_coors": {},
              "traverse_r_coors": {}
            }}
          )",
      this->m_is_success, this->m_total_dist, this->m_shortest_path_coors,
      this->m_traverse_f_coors, this->m_traverse_r_coors);
  return out;
}

} // namespace graph
