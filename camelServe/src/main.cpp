#include "fmt/core.h"
#include "util/fs.h"
#include "util/string.h"

#include "graph/def.h"


int main() {
  graph::SearchResult search_result{
      graph::GResultTypes::TOTAL_DIST_ONLY, {{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
  fmt::print("{}", search_result.ToString());
  return 0;
}
