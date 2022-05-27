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

  return std::hypot(static_cast<PotentialWeight>((target_x - node_x) * 1000000),
                    static_cast<PotentialWeight>((target_y - node_y) * 1000000));
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
    const AdjacentList &adj, const AdjacentWeightList& adjw, VisitedList &visited,
    WeightList &distance, PotentialWeightList& potential_list, MinPQ &pq, BackTrace &back_trace,
    BreakCondition break_condition, UpdateBestDist update_best_dist,
    CalculatePotentialWeight calculate_potential_weight,
    TraceTraversedCoor trace_traversed_coor) {
  if (pq.empty() || break_condition(pq.top())) {
    return false;
  }

  Vertex node = pq.top();
  pq.pop();

  if (visited[node]) {
    return this->Process(adj, adjw, visited, distance, potential_list, pq, back_trace,
                         break_condition, update_best_dist,
                         calculate_potential_weight, trace_traversed_coor);
  }

  for (auto neighbour_i = 0; neighbour_i < adj[node].size(); ++neighbour_i) {
    if (Vertex neighbour = adj[node][neighbour_i]; !visited[neighbour]) {
      Weight new_weight = distance[node] + adjw[node][neighbour_i];
      if (new_weight < distance[neighbour]) {
        distance[neighbour] = new_weight;
        back_trace[neighbour] = node;
      }

      potential_list[neighbour] = calculate_potential_weight(neighbour);
      pq.emplace(neighbour);

      update_best_dist(neighbour);
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
  WeightList dist(num_nodes, kInfiniteWeight);
  VisitedList visited(num_nodes, false);
  PotentialWeightList potential_list(num_nodes, kInfinitePotentialWeight);
  auto compare_distance = [&](Vertex u, Vertex v) { return static_cast<PotentialWeight>(dist[u]) + potential_list[u] > static_cast<PotentialWeight>(dist[v]) + potential_list[v]; };
  MinPriorityQueue<decltype(compare_distance)> minPq(compare_distance);
  potential_list[source] = BiAstarPotentialFunction_f(source, target, source);
  dist[source] = 0;
  minPq.emplace(source);

  std::map<Vertex, Vertex> back_traceR{};
  CoordinateList traversed_coorsR{};
  WeightList distR(num_nodes, kInfiniteWeight);
  VisitedList visitedR(num_nodes, false);
  PotentialWeightList potential_listR(num_nodes, kInfinitePotentialWeight);
  auto compare_distanceR = [&](Vertex u, Vertex v) { return static_cast<PotentialWeight>(distR[u]) + potential_listR[u] > static_cast<PotentialWeight>(distR[v]) + potential_listR[v]; };
  MinPriorityQueue<decltype(compare_distanceR)> minPqR(compare_distanceR);
  potential_listR[target] = BiAstarPotentialFunction_r(target, source, target);
  distR[target] = 0;
  minPqR.emplace(target);

  Weight best_dist = kInfiniteWeight;
  Vertex best_vertex = 0;

  while (
      this->Process(
          this->m_outgoing_vertexs, this->m_outgoing_weights, visited, dist, potential_list, minPq, back_trace,
          [&](auto) {
            return minPqR.empty()
                       ? false
                       : dist[minPq.top()] + distR[minPqR.top()] >= best_dist;
          },
          [&](Vertex next) {
            if (visitedR[next]) {
              if (dist[next] + distR[next] < best_dist) {
                best_dist = dist[next] + distR[next];
                best_vertex = next;
              }
            }
          },
          [&](Vertex next) {
            return this->BiAstarPotentialFunction_f(source, target, next);
          },
          [&](Vertex node) {
            traversed_coors.emplace_back(this->m_coordinates[node]);
          }) &&
      this->Process(
          this->m_incoming_vertexs, this->m_incoming_weights, visitedR, distR, potential_listR, minPqR, back_traceR,
          [&](auto) {
            return minPq.empty()
                       ? false
                       : dist[minPq.top()] + distR[minPqR.top()] >= best_dist;
          },
          [&](Vertex next) {
            if (visited[next]) {
              if (distR[next] + dist[next] < best_dist) {
                best_dist = distR[next] + dist[next];
                best_vertex = next;
              }
            }
          },
          [&](Vertex next) {
            return this->BiAstarPotentialFunction_r(target, source, next);
          },
          [&](Vertex node) {
            traversed_coorsR.emplace_back(this->m_coordinates[node]);
          })) {
  }

  if (best_dist < kInfiniteWeight) {
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

    shortest_path.splice(shortest_path.end(), shortest_pathR);
    for (const Vertex &vertex : shortest_path) {
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
    search_reply.m_total_dist = best_dist;
  }

  return search_reply.ToJsonStr();
}
