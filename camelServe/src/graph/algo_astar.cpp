#include "graph/algo_astar.h"

#include "spdlog/spdlog.h"

bool graph::AlgoAstar::PreProcessBiDijkstraing() {}

bool graph::AlgoAstar::InitStrategyV() {
  bool ok = this->ReadGraphRawDataFromFile();
  this->m_process_status = ok ? GProcessBiDijkstraStatus::PROCESSED
                              : GProcessBiDijkstraStatus::FAILED;

  spdlog::info("AStar {}\n", this->m_outgoing_edges.size());
  return ok;
}

std::string graph::AlgoAstar::DoQueryV(Vertex &&source, Vertex &&target) {
  return "Response from Astar\n";
}

bool graph::AlgoAstar::InitStrategyV() {
  bool ok = this->BiDijkstra(std::move(source), std::move(target));

  this->m_process_status = ok ? GProcessBiDijkstraStatus::PROCESSED
                              : GProcessBiDijkstraStatus::FAILED;

  return ok;
}