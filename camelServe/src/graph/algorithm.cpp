#include "algorithm.h"

#include <fstream>

#include "graph/def.h"

bool graph::Algorithm::ReadFileToAdjacentList() {
  if (this->m_path_in_dist.empty()) {
    return false;
  }

  std::fstream infile(this->m_path_in_dist, std::ios_base::in);
  if (infile.is_open() == false) {
    return false;
  }

  int nodes, edges;
  Vertex source, target;
  Weight weight;

  infile >> nodes >> edges;
  this->m_outgoing_edges.resize(nodes);
  this->m_incoming_edges.resize(nodes);

  while (infile >> source >> target >> weight) {
    --source;
    --target;
    this->add_edge(source, target, weight);
  }

  return true;
}
