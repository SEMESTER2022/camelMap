#ifndef GRAPH_QUERY_H
#define GRAPH_QUERY_H

#include "util/def.h"

namespace graph {
class Query {
private:
  SearchResult m_search_result;
  Vertex m_source;
  Vertex m_dest;

public:
  Query(Vertex source, Vertex dest)
      : m_source(std::move(source), m_dest(dest)) {}

  void SetSearchResult(SearchResult &&result) {
    this->m_search_result = SearchResult{result};
  }
};
} // namespace graph

#endif // GRAPH_QUERY_H