#include "graph/algo_astar.h"

#ifdef USE_ALGO_BIASTAR_LANDMARK

#include <fstream>
#include <map>

#include "config.h"
#include "spdlog/spdlog.h"
#include "util/fs.h"

std::string graph::AlgoBiAstarLandmark::DoQueryV(Vertex &&source,
                                                 Vertex &&target) {
  return "Response from Astar\n";
}

bool graph::AlgoBiAstarLandmark::InitStrategyV() {
  bool ok = this->ReadGraphData();
  this->m_process_status =
      ok ? GProcessStatus::PROCESSED : GProcessStatus::FAILED;

  spdlog::info("AStar {}", this->m_outgoing_vertexs.size());
  return ok;
}

void graph::AlgoBiAstarLandmark::AddPotentialEdge(
    Vertex source, Vertex target, PotentialWeight potential_weight) {
  if (this->m_preprocessed_outgoing_weights[source].empty()) {
    this->m_preprocessed_outgoing_weights[source].resize(
        this->m_outgoing_vertexs[source].size());
  }

  for (auto target_i = 0; target_i < this->m_outgoing_vertexs[source].size();
       ++target_i) {
    if (this->m_outgoing_vertexs[source][target_i] == target) {
      this->m_preprocessed_outgoing_weights[source][target_i] =
          potential_weight;
      break;
    }
  }

  for (auto source_i = 0; source_i < this->m_incoming_vertexs[target].size();
       ++source_i) {
    if (this->m_incoming_vertexs[target][source_i] == source) {
      this->m_preprocessed_incoming_weights[target][source_i] =
          potential_weight;
      break;
    }
  }
}

bool graph::AlgoBiAstarLandmark::ReadFileToLandMarkList() {
  if (CS_MAP_DATA_ASTAR_LANDMARKS_LOCATION == "") {
    return false;
  }

  std::fstream infile(CS_MAP_DATA_ASTAR_LANDMARKS_LOCATION, std::ios_base::in);
  if (infile.is_open() == false) {
    return false;
  }

  uint32_t nodes;
  Vertex node;

  infile >> nodes;
  this->m_landmarks.resize(nodes);

  while (infile >> node) {
    if (node > this->m_outgoing_vertexs.size()) {
      return false;
    }

    this->m_landmarks[--nodes] = node;
  }

  return true;
}

bool graph::AlgoBiAstarLandmark::ReadFileToPreprocessedAdjacentWeightList() {
  if (CS_MAP_DATA_ASTAR_OUTPUT_LOCATION == "") {
    this->m_process_status = GProcessStatus::FAILED;
    return false;
  }

  std::fstream infile(CS_MAP_DATA_ASTAR_OUTPUT_LOCATION, std::ios_base::in);
  if (infile.is_open() == false) {
    this->m_process_status = GProcessStatus::UNPROCESS;
    return false;
  }

  uint32_t nodes;
  PotentialWeight potential_weight;
  Vertex src, tgt;

  infile >> nodes;
  this->m_preprocessed_incoming_weights.resize(nodes);
  this->m_preprocessed_outgoing_weights.resize(nodes);

  while (infile >> src >> tgt >> potential_weight) {
    this->AddPotentialEdge(src, tgt, potential_weight);
  }

  this->m_process_status = GProcessStatus::PROCESSED;

  return true;
}

bool graph::AlgoBiAstarLandmark::ReadGraphData() {
  bool ok = false;
  if (this->m_outgoing_vertexs.size() == 0) {
    ok = this->ReadFileToAdjacentList();
    if (ok == false) {
      return false;
    }

    ok = this->ReadFileToCoordinateList();
    if (ok == false) {
      return false;
    }
  }

  ok = this->ReadFileToLandMarkList();
  if (ok == false) {
    return false;
  }

  (void)this->ReadFileToPreprocessedAdjacentWeightList();
  return true;
}

std::optional<Weight>
graph::AlgoBiAstarLandmark::PotentialFunction_f(Vertex landmark, Vertex target,
                                                Vertex node) {
  auto potential_it = this->m_map_potential_weights_f->find(landmark);
  if (potential_it == this->m_map_potential_weights_f.end()) {
    return std::nullopt;
  }

  const auto &[std::ignore, potential_weights] = *potential_it;
  return potential_weights[target] - potential_weights[node];
}

std::optional<Weight>
graph::AlgoBiAstarLandmark::PotentialFunction_r(Vertex landmark, Vertex target,
                                                Vertex node) {
  auto potential_it = this->m_map_potential_weights_r->find(landmark);
  if (potential_it == this->m_map_potential_weights_r.end()) {
    return std::nullopt;
  }

  const auto &[std::ignore, potential_weights] = *potential_it;
  return potential_weights[target] - potential_weights[node];
}

std::optional<PotentialWeight>
graph::AlgoBiAstarLandmark::BiAstarPotentialFunction_f(Vertex landmark,
                                                       Vertex target,
                                                       Vertex node) {
  std::optional potential_value_f =
      this->PotentialFunction_f(landmark, target, node);
  if (!potential_value_f.has_value()) {
    return std::nullopt;
  }

  std::optional potential_value_r =
      this->PotentialFunction_r(landmark, target, node);
  if (!potential_value_r.has_value()) {
    return std::nullopt;
  }

  return (static_cast<PotentialWeight>(potential_value_f - potential_value_r) /
          2);
}

std::optional<PotentialWeight>
graph::AlgoBiAstarLandmark::BiAstarPotentialFunction_r(Vertex landmark,
                                                       Vertex target,
                                                       Vertex node) {
  std::optional potential_weight_f =
      this->BiAstarPotentialFunction_f(landmark, target, node);

  if (!potential_weight_f.has_value()) {
    return std::nullopt;
  }
  return -(*potential_weight_f);
}

bool graph::AlgoBiAstarLandmark::PreprocessingAStarLandmark() {
  if (this->m_process_status == GProcessStatus::PROCESSED) {
    return true;
  }

  if (this->m_process_status == GProcessStatus::FAILED) {
    return false;
  }

  return true;
}

template <class BreakCondition, class UpdateBestDist, class MinPQ,
          class BackTrace, class CalculatePotentialWeight>
bool graph::AlgoBiAstarLandmark::Process(
    const AdjacentList &adj, visited &visited, PotentialWeightList &distance,
    MinPQ &pq, BackTrace &back_trace, BreakCondition break_condition,
    UpdateBestDist update_best_dist,
    CalculatePotentialWeight calculate_potential_weight) {
  if (pq.empty() || break_condition(pq.top())) {
    return false;
  }

  Vertex node = pq.top();
  pq.pop();

  if (visited[node]) {
    return this->Process(adj, visited, distance, pq, back_trace,
                         break_condition, update_best_dist);
  }

  for (const auto &neighbour : adj[node]) {
    if (!visited[neighbour]) {
      PotentialWeight neighbour_potential_weight =
          calculate_potential_weight(node, neighbour);
      if (PotentialWeight new_weight =
              distance[node] + neighbour_potential_weight) {
        distance[neighbour] = new_weight;
        back_trace[neighbour] = node;
        pq.emplace(neighbour);
      }

      update_best_dist(node, neighbour);
    }
  }

  visited[node] = true;
  return true;
}

std::string graph::AlgoBiAstarLandmark::BiAstarLandmark(Vertex source,
                                                        Vertex target) {
  GSearchReply search_reply{};
  spdlog::info("BiAstar landmark on source {} to target", source, target);
  auto num_nodes = this->m_outgoing_vertexs.size();

  if (source == target || source >= num_nodes || target >= num_nodes) {
    return search_reply.ToJsonStr();
  }

  std::map<Vertex, Vertex> back_trace{};
  PotentialWeightList dist(num_nodes, kInfinitePotentialWeight);
  VisitedList visited(num_nodes, false);
  auto compare_distance = [&](Vertex u, Vertex v) { return dist[u] > dist[v]; };
  MinPriorityQueue<decltype(compare_distance)> minPq(compare_distance);
  dist[source] = 0;
  minPq.emplace(source);

  std::map<Vertex, Vertex> back_traceR{};
  PotentialWeightList distR(num_nodes, kInfinitePotentialWeight);
  VisitedList visitedR(num_nodes, false);
  auto compare_distanceR = [&](Vertex u, Vertex v) {
    return distR[u] > distR[v];
  };
  MinPriorityQueue<decltype(compare_distanceR)> minPqR(compare_distanceR);
  distR[target] = 0;
  minPqR.emplace(target);

  PotentialWeight best_dist = kInfinitePotentialWeight;
  Vertex best_vertex = 0;

  while (this->Process(
      this->m_outgoing_vertexs, visited, dist, minPq, back_trace,
      [&](auto) {
        return minPqR.empty()
                   ? false
                   : dist[minPq.top()] + distR[minPqR.top()] >= best_dist;
      },
      [&](Vertex current, Vertex next) {
        if (visitedR[next]) {
          if (dist[next] + distR[next] < best_dist) {
            best_dist = dist[next] + distR[next];
            best_vertex = next;
          }
        }
      },
      [&](Vertex current, Vertex next) {
        m_outgoing_weights[current][next] - BiAstarPotentialFunction_f()
      }))
}

#endif