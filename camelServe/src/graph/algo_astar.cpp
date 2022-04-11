#include "graph/algo_astar.h"

#include <cmath>
#include <list>
#include <map>

bool graph::AlgoAstar::InitStrategyV() {
  bool ok = this->ReadGraphData();
  this->m_process_status =
      ok ? GProcessStatus::PROCESSED : GProcessStatus::FAILED;

  spdlog::info("Init strategy astar success with number of vertexes {}",
               this->m_outgoing_vertexs.size());
  return ok;
}

std::string graph::AlgoAstar::DoQueryV(Vertex &&source, Vertex &&target) {
  return this->BiAstar(std::move(source), std::move(target));
}

bool graph::AlgoAstar::ReadGraphData() {
  bool ok = this->ReadFileToAdjacentList();

  if (ok == false) {
    return false;
  }

  ok = this->ReadFileToCoordinateList();

  return ok;
}

graph::PotentialWeight graph::AlgoAstar::PotentialFunction(Vertex target,
                                                           Vertex node) {
  const auto &[target_x, target_y] = this->m_coordinates[target];
  const auto &[node_x, node_y] = this->m_coordinates[node];

  return std::hypot(static_cast<PotentialWeight>(target_x - node_x),
                    static_cast<PotentialWeight>(target_y - node_y));
}

graph::PotentialWeight
graph::AlgoAstar::BiAstarPotentialFunction_f(Vertex source, Vertex target,
                                             Vertex node) {
  auto pi_f = this->PotentialFunction(target, node);
  auto pi_r = this->PotentialFunction(source, node);

  return (pi_f - pi_r) / 2;
}

graph::PotentialWeight
graph::AlgoAstar::BiAstarPotentialFunction_r(Vertex source, Vertex target,
                                             Vertex node) {
  return -this->BiAstarPotentialFunction_f(source, target, node);
}

template <class BreakCondition, class UpdateBestDist, class MinPQ,
          class BackTrace, class CalculatePotentialWeight,
          class TraceTraversedCoor>
bool graph::AlgoAstar::Process(
    const AdjacentList &adj, VisitedList &visited,
    PotentialWeightList &distance, MinPQ &pq, BackTrace &back_trace,
    BreakCondition break_condition, UpdateBestDist update_best_dist,
    CalculatePotentialWeight calculate_potential_weight,
    TraceTraversedCoor trace_traversed_coor) {
  if (pq.empty() || break_condition(pq.top())) {
    return false;
  }

  Vertex node = pq.top();
  pq.pop();

  if (visited[node]) {
    return this->Process(adj, visited, distance, pq, back_trace,
                         break_condition, update_best_dist,
                         calculate_potential_weight, trace_traversed_coor);
  }

  for (auto neighbour_i = 0; neighbour_i < adj[node].size(); ++neighbour_i) {
    if (Vertex neighbour = adj[node][neighbour_i]; !visited[neighbour]) {
      PotentialWeight neighbour_potential_weight =
          calculate_potential_weight(node, neighbour_i);
      if (Weight new_weight = distance[node] + neighbour_potential_weight;
          new_weight < distance[neighbour]) {
        distance[neighbour] = new_weight;
        back_trace[neighbour] = node;
        pq.emplace(neighbour);
      }

      update_best_dist(node, neighbour);
    }
  }

  visited[node] = true;
  trace_traversed_coor(node);
  return true;
}

std::string graph::AlgoAstar::BiAstar(Vertex &&source, Vertex &&target) {
  GSearchReply search_reply{};
  spdlog::info("BiAstar on source {} to target {} with response type {}",
               source, target, this->m_response_req);
  auto num_nodes = this->m_outgoing_vertexs.size();

  if (source == target || source >= num_nodes || target >= num_nodes) {
    return search_reply.ToJsonStr();
  }

  std::map<Vertex, Vertex> back_trace{};
  CoordinateList traversed_coors{};
  PotentialWeightList dist(num_nodes, kInfinitePotentialWeight);
  VisitedList visited(num_nodes, false);
  auto compare_distance = [&](Vertex u, Vertex v) { return dist[u] > dist[v]; };
  MinPriorityQueue<decltype(compare_distance)> minPq(compare_distance);
  dist[source] = 0;
  minPq.emplace(source);

  std::map<Vertex, Vertex> back_traceR{};
  CoordinateList traversed_coorsR{};
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

  while (
      this->Process(
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
          [&](Vertex current, auto next_i) {
            return static_cast<PotentialWeight>(
                       this->m_outgoing_weights[current][next_i]) -
                   this->BiAstarPotentialFunction_f(source, target, current) +
                   this->BiAstarPotentialFunction_f(
                       source, target,
                       this->m_outgoing_vertexs[current][next_i]);
          },
          [&](Vertex node) {
            traversed_coors.emplace_back(this->m_coordinates[node]);
          }) &&
      this->Process(
          this->m_incoming_vertexs, visitedR, distR, minPqR, back_traceR,
          [&](auto) {
            return minPq.empty()
                       ? false
                       : dist[minPq.top()] + distR[minPqR.top()] >= best_dist;
          },
          [&](Vertex current, Vertex next) {
            if (visited[next]) {
              if (distR[next] + dist[next] < best_dist) {
                best_dist = distR[next] + dist[next];
                best_vertex = next;
              }
            }
          },
          [&](Vertex current, auto next_i) {
            return static_cast<PotentialWeight>(
                       this->m_incoming_weights[current][next_i]) -
                   this->BiAstarPotentialFunction_r(target, source, current) +
                   this->BiAstarPotentialFunction_r(
                       target, source,
                       this->m_incoming_vertexs[current][next_i]);
          },
          [&](Vertex node) {
            traversed_coorsR.emplace_back(this->m_coordinates[node]);
          })) {
  }

  if (best_dist < kInfinitePotentialWeight) {
    std::list<Vertex> shortest_path{}, shortest_pathR{};
    auto trace_it = back_trace.find(best_vertex);
    while (trace_it != back_trace.end()) {
      shortest_path.insert(shortest_path.begin(), trace_it->second);
      trace_it = back_trace.find(trace_it->second);
    }

    shortest_path.emplace_back(best_vertex);

    auto trace_itR = back_traceR.find(best_vertex);
    while (trace_itR != back_traceR.end()) {
      shortest_pathR.emplace_back(trace_itR->second);
      trace_itR = back_traceR.find(trace_itR->second);
    }

    Weight actual_best_dist = 0;
    shortest_path.splice(shortest_path.end(), shortest_pathR);
    std::optional<Vertex> prev = std::nullopt;
    for (const Vertex &vertex : shortest_path) {
      if (prev.has_value()) {
        actual_best_dist += this->m_incoming_weights[*prev][vertex];
      }
      prev = vertex;

      if (this->m_response_req >=
          GResponseReq::INHERIT_AND_SHORTEST_PATH_COORS) {
        search_reply.m_shortest_path_coors.emplace_back(m_coordinates[vertex]);
      }
    }

    if (this->m_response_req >= GResponseReq::INHERIT_AND_TRAVERSE_COORS) {
      search_reply.m_traverse_f_coors = std::move(traversed_coors);
      search_reply.m_traverse_r_coors = std::move(traversed_coorsR);
    }

    search_reply.m_is_success = true;
    search_reply.m_total_dist = actual_best_dist;
  }

  return search_reply.ToJsonStr();
}
