#ifndef GRAPH_ALGO_H
#define GRAPH_ALGO_H

#include <optional>
#include <string>

#include "graph/def.h"

namespace graph {
class Algorithm {
protected:
  std::string m_algo_name;

public:
  Algorithm(std::string algo_name) : m_algo_name(std::move(algo_name)) {}
  ~Algorithm();

public:
  virtual std::string DoQueryV(Vertex &&source, Vertex &&dest) = 0;
  std::string GetAlgoName() { return this->m_algo_name; };
};
} // namespace graph

#endif // GRAPH_ALGO_H