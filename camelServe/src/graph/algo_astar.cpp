#include "graph/algo_astar.h"

#include "spdlog/spdlog.h"
#include "util/fs.h"

bool graph::AlgoAstar::PreprocessingAStarLandmark() {
  bool ok = util::file_exist(m_path_out_graph);
  if (ok) {
    return ok;
  }
}

std::string graph::AlgoAstar::DoQueryV(Vertex &&source, Vertex &&target) {
  return "Response from Astar\n";
}

bool graph::AlgoAstar::InitStrategyV() {
  bool ok = this->ReadGraphData();
  this->m_process_status =
      ok ? GProcessStatus::PROCESSED : GProcessStatus::FAILED;

  spdlog::info("AStar {}", this->m_outgoing_vertexs.size());
  return ok;
}
