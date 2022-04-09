#include "graph/algo_astar.h"

#include "spdlog/spdlog.h"

bool graph::AlgoAstar::PreProcessBiDijkstraing() { return true; }

std::string graph::AlgoAstar::DoQueryV(Vertex &&source, Vertex &&target) {
  return "Response from Astar\n";
}

bool graph::AlgoAstar::InitStrategyV() {
  bool ok = this->ReadGraphData();
  this->m_process_status =
      ok ? GProcessStatus::PROCESSED : GProcessStatus::FAILED;

  spdlog::info("AStar {}", this->m_outgoing_edges.size());
  return ok;
}
