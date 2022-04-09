#ifndef GRAPH_ALGO_H
#define GRAPH_ALGO_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "spdlog/spdlog.h"

#include "graph/def.h"

namespace graph {
class Algorithm {
private:
  void AddEdgeSerialize(TargetList &list, Vertex &node, Weight &weight);

protected:
  std::string m_path_in_coor;
  std::string m_path_in_dist;
  std::string m_path_in_time;
  std::string m_path_out_graph;

  uint32_t m_process_status{0};

  AdjacentList m_incoming_edges;
  AdjacentList m_outgoing_edges;
  CoordinateList m_coordinates;

  bool ReadFileToAdjacentList();
  bool ReadFileToCoordinateList();

  virtual bool ReadGraphData() = 0;

  void AddEdge(Vertex &source, Vertex &target, Weight &weight) {
    this->AddEdgeSerialize(this->m_outgoing_edges[source], target, weight);
    this->AddEdgeSerialize(this->m_incoming_edges[target], source, weight);
  }

public:
  Algorithm() {}
  ~Algorithm() = default;

public:
  void SetInFile(std::string in_coor, std::string in_dist,
                 std::string in_time) {
    this->m_path_in_coor = std::move(in_coor);
    this->m_path_in_dist = std::move(in_dist);
    this->m_path_in_time = std::move(in_time);
  }

  void SetOutFile(std::string out_graph) {
    this->m_path_out_graph = std::move(out_graph);
  }

  bool EnabledV() { return this->m_process_status; }

  std::tuple<bool, Vertex, Vertex> FindNearestSourceDestV(Coordinate src_coor,
                                                          Coordinate dst_coor);

  virtual bool InitStrategyV() = 0;
  virtual std::string DoQueryV(Vertex &&source, Vertex &&target) = 0;
};
} // namespace graph

#endif // GRAPH_ALGO_H
