#include "algorithm.h"

#include <fstream>

#include "graph/def.h"

bool graph::Algorithm::ReadFileToCoordinateList() {
  if (this->m_path_in_coor.empty()) {
    return false;
  }

  std::fstream infile(this->m_path_in_coor, std::ios_base::in);
  if (infile.is_open() == false) {
    return false;
  }

  int nodes, lat, lng;
  Vertex node;

  infile >> nodes;
  this->m_coordinates.resize(nodes);

  while (infile >> node >> lng >> lat) {
    if (node > nodes) {
      return false;
    }
    this->m_coordinates[--node] = {lng, lat};
  }

  return true;
}

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
    this->AddEdge(source, target, weight);
  }

  return true;
}
