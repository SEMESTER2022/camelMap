#include "algo_dijkstra.h"

#include <fstream>
#include <list>
#include <map>
#include <string>

#include "spdlog/spdlog.h"

#include "graph/def.h"

template <class BreakCondition, class UpdateBestDist, class MinPQ,
          class BackTrace>
bool graph::AlgoDijkstra::Process(const AdjacentList &adj,
                                  const AdjacentWeightList &adjw,
                                  VisitedList &visited, WeightList &distance,
                                  MinPQ &pq, BackTrace &back_trace,
                                  BreakCondition break_condition,
                                  UpdateBestDist update_best_dist) {
  if (pq.empty() || break_condition(pq.top())) {
    return false;
  }

  Vertex node = pq.top();
  pq.pop();

  if (visited[node]) {
    return this->Process(adj, adjw, visited, distance, pq, back_trace,
                         break_condition, update_best_dist);
  }

  for (auto neighbour_i = 0; neighbour_i < adj[node].size(); ++neighbour_i) {
    if (Vertex neighbour = adj[node][neighbour_i]; !visited[neighbour]) {
      if (Weight new_weight = distance[node] + adjw[node][neighbour_i];
          new_weight < distance[neighbour]) {
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

std::string graph::AlgoDijkstra::Dijkstra(Vertex &&source, Vertex &&target) {
  // TODO: implement Dijkstra

  return "";
}

std::string graph::AlgoDijkstra::BiDijkstra(Vertex &&source, Vertex &&target) {
  SearchReply search_reply{};
  spdlog::info("Bidijkstra on source {} to target {}", source, target);
  auto num_nodes = this->m_outgoing_vertexs.size();

  if (source == target || source >= num_nodes || target >= num_nodes) {
    return search_reply.ToJsonStr();
  }

  std::map<Vertex, Vertex> back_trace{};
  WeightList dist(num_nodes, kInfiniteWeight);
  VisitedList visited(num_nodes, false);
  auto compare_distance = [&](Vertex u, Vertex v) { return dist[u] > dist[v]; };
  MinPriorityQueue<decltype(compare_distance)> minPq(compare_distance);
  dist[source] = 0;
  minPq.emplace(source);

  std::map<Vertex, Vertex> back_traceR{};
  WeightList distR(num_nodes, kInfiniteWeight);
  VisitedList visitedR(num_nodes, false);
  auto compare_distanceR = [&](Vertex u, Vertex v) {
    return distR[u] > distR[v];
  };

  MinPriorityQueue<decltype(compare_distanceR)> minPqR(compare_distanceR);
  distR[target] = 0;
  minPqR.emplace(target);

  Weight best_dist = kInfiniteWeight;
  Vertex best_vertex = 0;
  while (this->Process(this->m_outgoing_vertexs, this->m_outgoing_weights,
                       visited, dist, minPq, back_trace,
                       [&](auto) {
                         return minPqR.empty()
                                    ? false
                                    : dist[minPq.top()] + distR[minPqR.top()] >=
                                          best_dist;
                       },
                       [&](Vertex current, Vertex next) {
                         if (visitedR[next]) {
                           if (dist[next] + distR[next] < best_dist) {
                             best_dist = dist[next] + distR[next];
                             best_vertex = next;
                           }
                         }
                       }) &&
         this->Process(this->m_incoming_vertexs, this->m_incoming_weights,
                       visitedR, distR, minPqR, back_traceR,
                       [&](auto) {
                         return minPq.empty()
                                    ? false
                                    : dist[minPq.top()] + distR[minPqR.top()] >=
                                          best_dist;
                       },
                       [&](Vertex current, Vertex next) {
                         if (visited[next]) {
                           if (distR[next] + dist[next] < best_dist) {
                             best_dist = distR[next] + dist[next];
                             best_vertex = next;
                           }
                         }
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
      search_reply.m_shortest_coor_list.emplace_back(m_coordinates[vertex]);
    }

    search_reply.m_is_success = true;
    search_reply.m_total_dist = best_dist;
  }

  return search_reply.ToJsonStr();
}

bool graph::AlgoDijkstra::ReadGraphData() {
  bool ok = this->ReadFileToAdjacentList();

  if (ok == false) {
    return false;
  }

  ok = this->ReadFileToCoordinateList();

  return ok;
}

bool graph::AlgoDijkstra::InitStrategyV() {
  bool ok = this->ReadGraphData();
  this->m_process_status =
      ok ? GProcessStatus::PROCESSED : GProcessStatus::FAILED;

  spdlog::info("Init strategy dijkstra success with number of vertexs: {}",
               this->m_outgoing_vertexs.size());
  return ok;
}

std::string graph::AlgoDijkstra::DoQueryV(Vertex &&source, Vertex &&target) {
  return this->BiDijkstra(std::move(source), std::move(target));
}
