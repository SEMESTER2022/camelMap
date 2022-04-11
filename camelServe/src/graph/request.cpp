#include "graph/request.h"

#include <optional>
#include <vector>

#include "graph/algo_astar.h"
#include "graph/algo_dijkstra.h"
#include "graph/graph.h"

std::optional<std::string> graph::Request::Handle() {
  std::vector<std::string> coors;
  size_t last_pos = 0, pos = this->m_query.find(';');
  while (pos != std::string::npos) {
    coors.emplace_back(
        std::string(this->m_query.c_str() + last_pos, pos - last_pos));
    last_pos = pos + 1;
    pos = this->m_query.find(';', pos + 1);
  }

  if (coors.size() < 4) {
    return std::nullopt;
  }

  GResponseReq response_req{-1};
  GAlgorithmReq algorithm_req{-1};
  double coor_src_x, coor_src_y, coor_target_x, coor_target_y;

  try {
    coor_src_x = stod(coors[0]);
    coor_src_y = stod(coors[1]);
    coor_target_x = stod(coors[2]);
    coor_target_y = stod(coors[3]);

    if (coors.size() >= 5) {
      response_req = static_cast<GResponseReq>(std::stoi(coors[4]));
    }

    if (coors.size() >= 6) {
      algorithm_req = static_cast<GAlgorithmReq>(std::stoi(coors[5]));
    }
  } catch (...) {
    spdlog::error("parse data failed");
    return std::nullopt;
  }

  GAlgorithmReq current_algorithm_req = GraphInstance().GetAlgorithmReq();
  GResponseReq current_response_req = GraphInstance().GetResponseReq();
  std::optional<std::string> search_reply;

  if (algorithm_req != GAlgorithmReq::NO_ALGORITHM_REQ &&
      current_algorithm_req != algorithm_req) {
    switch (algorithm_req) {
    case GAlgorithmReq::BI_ASTAR:
      GraphInstance().AssignRuntimeStrategy(std::make_unique<AlgoAstar>());
      GraphInstance().SetAlgorithmReq(GAlgorithmReq::BI_ASTAR);
      break;
    case GAlgorithmReq::DIJKSTRA:
      [[fallthrough]]; // unsupported yet
    case GAlgorithmReq::ASTAR:
      [[fallthrough]]; // unsupported yet
    case GAlgorithmReq::BI_ASTAR_LANDMARK:
      search_reply = std::nullopt;
      spdlog::error(
          "Trying to set search strategy to unsupported Algorithm failed");
      return search_reply; // unsupported yet
    case GAlgorithmReq::BI_DIJKSTRA:
      [[fallthrough]];
    default:
      GraphInstance().AssignRuntimeStrategy(std::make_unique<AlgoDijkstra>());
      GraphInstance().SetAlgorithmReq(GAlgorithmReq::BI_DIJKSTRA);
      break;
    }
  }

  if (response_req != GResponseReq::NO_RESPONSE_REQ &&
      response_req != current_response_req) {
    GraphInstance().SetResponseReq(response_req);
  }

  auto &&[ok, source, target] = GraphFindNearestSourceDestf(
      {coor_src_x, coor_src_y}, {coor_target_x, coor_target_y});

  if (!ok) {
    spdlog::error("Find nearest source dest failed");
    return std::nullopt;
  }

  spdlog::info("Find neareast source dest success");
  search_reply = GraphSearchf(std::move(source), std::move(target));
  return search_reply;
}
