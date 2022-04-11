#ifndef GRAPH_REQUEST_H
#define GRAPH_REQUEST_H

#include <optional>
#include <string>

namespace graph {
class Request {
private:
  std::string m_query;

public:
  Request(std::string query) : m_query(std::move(query)) {}
  ~Request() = default;

  std::optional<std::string> Handle();
};
} // namespace graph

#endif // GRAPH_REQUEST_H
