#ifndef GRAPH_ALGO_H
#define GRAPH_ALGO_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

#include "graph/def.h"

namespace graph {

class Algorithm {

public:
  Algorithm() {}
  ~Algorithm() = default;

  bool EnabledV() { return this->m_process_status; }

  std::tuple<bool, Vertex, Vertex> FindNearestSourceDestV(Coordinate src_coor,
                                                          Coordinate dst_coor);

  virtual bool InitStrategyV() = 0;
  virtual std::string DoQueryV(Vertex &&source, Vertex &&target) = 0;

protected:
  uint32_t m_process_status{0};

  AdjacentList m_incoming_vertexs;
  AdjacentWeightList m_incoming_weights;
  AdjacentList m_outgoing_vertexs;
  AdjacentWeightList m_outgoing_weights;

  CoordinateList m_coordinates;

  bool ReadFileToAdjacentList();
  bool ReadFileToCoordinateList();

  virtual bool ReadGraphData() = 0;

  void AddEdge(Vertex &source, Vertex &target, Weight &weight) {
    this->AddEdgeSerialize(this->m_outgoing_vertexs[source],
                           this->m_outgoing_weights[source], target, weight);
    this->AddEdgeSerialize(this->m_incoming_vertexs[target],
                           this->m_incoming_weights[target], source, weight);
  }

private:
  void AddEdgeSerialize(VertexList &vertex_list, WeightList &weight_list,
                        Vertex &node, Weight &weight);
};

} // namespace graph

#endif // GRAPH_ALGO_H
