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

  bool EnabledV() {
    return this->m_process_status == GProcessStatus::PROCESSED;
  }

  std::tuple<bool, Vertex, Vertex> FindNearestSourceDestV(Coordinate src_coor,
                                                          Coordinate dst_coor);

  virtual bool InitStrategyV() = 0;
  virtual std::string DoQueryV(Vertex &&source, Vertex &&target) = 0;

  void SetResponseReq(GResponseReq response_req) {
    this->m_response_req = response_req;
  }
  void SetAlgorithmReq(GAlgorithmReq algorithm_req) {
    this->m_algorithm_req = algorithm_req;
  }

  GResponseReq GetResponseReq() { return this->m_response_req; }
  GAlgorithmReq GetAlgorithmReq() { return this->m_algorithm_req; }

protected:
  GProcessStatus m_process_status{0};

  GAlgorithmReq m_algorithm_req{GAlgorithmReq::BI_DIJKSTRA};
  GResponseReq m_response_req{GResponseReq::INHERIT_AND_SHORTEST_PATH_COORS};

  AdjacentList m_incoming_vertexs;
  AdjacentWeightList m_incoming_weights;
  AdjacentList m_outgoing_vertexs;
  AdjacentWeightList m_outgoing_weights;

  CoordinateList m_coordinates;

  bool ReadFileToAdjacentList();
  bool ReadFileToCoordinateList();

  virtual bool ReadGraphData() = 0;

  void AddEdge(Vertex &source, Vertex &target, Weight &weight);

private:
  void AddEdgeSerialize(VertexList &vertex_list, WeightList &weight_list,
                        Vertex &node, Weight &weight);
};

} // namespace graph

#endif // GRAPH_ALGO_H
