#ifndef GRAPH_ALGO_H
#define GRAPH_ALGO_H

#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "spdlog/spdlog.h"

#include "graph/def.h"

namespace graph {
class Algorithm {
private:
  void add_edge_serialize(TargetList &list, Vertex& node, Weight& weight) {
    for (auto &[target, weight_target] : list) {
      if (target == node && weight_target > weight) {
        weight_target = weight;
        return;
      }
    }

    list.emplace_back(std::make_pair(node, weight));
  }

  void add_edge(Vertex& source, Vertex& target, Weight& weight) {
    this->add_edge_serialize(this->m_outgoing_edges[source], target, weight);
    this->add_edge_serialize(this->m_incoming_edges[target], source, weight);
  }

protected:
  std::string m_path_in_coor;
  std::string m_path_in_dist;
  std::string m_path_in_time;
  std::string m_path_out_graph;

  uint32_t m_process_status{0};

  SearchResult m_search_result;

  AdjacentList m_incoming_edges;
  AdjacentList m_outgoing_edges;

  bool ReadFileToAdjacentList();

public:
  Algorithm() : m_search_result(SearchResult{}) {}
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

  virtual bool InitGraphV() = 0;
  virtual bool EnabledV() = 0;

  virtual std::string DoQueryV(Vertex &&source, Vertex &&target) = 0;
};
} // namespace graph

#endif // GRAPH_ALGO_H