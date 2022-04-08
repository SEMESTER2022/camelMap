#include "algo_dijkstra.h"

#include <fstream>
#include <list>
#include <map>
#include <string>

#include "spdlog/spdlog.h"

#include "graph/def.h"

template <class BreakCondition, class UpdateBestDist, class MinPQ,
          class BackTrace>
bool graph::AlgoDijkstra::Process(AdjacentList &adj, VisitedList &visited,
                                  WeightList &distance, MinPQ &pq,
                                  BackTrace &back_trace,
                                  BreakCondition break_condition,
                                  UpdateBestDist update_best_dist) {
  if (pq.empty() || break_condition(pq.top())) {
    return false;
  }

  Vertex node = pq.top();
  pq.pop();

  if (visited[node]) {
    return this->Process(adj, visited, distance, pq, back_trace,
                         break_condition, update_best_dist);
  }

  for (const auto &[neighbour, weight] : adj[node]) {
    if (!visited[neighbour]) {
      if (Weight new_weight = distance[node] + weight;
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
  SearchResult search_result{};
  auto num_nodes = this->m_outgoing_edges.size();

  if (source == target || source >= num_nodes || target >= num_nodes) {
    return search_result.ToJsonStr();
  }

  std::map<Vertex, Vertex> back_trace{};
  WeightList dist(num_nodes, kInfinite);
  VisitedList visited(num_nodes, false);
  auto compare_distance = [&](Vertex u, Vertex v) { return dist[u] > dist[v]; };
  MinPriorityQueue<decltype(compare_distance)> minPq(compare_distance);
  dist[source] = 0;
  minPq.emplace(source);

  std::map<Vertex, Vertex> back_traceR{};
  WeightList distR(num_nodes, kInfinite);
  VisitedList visitedR(num_nodes, false);
  auto compare_distanceR = [&](Vertex u, Vertex v) {
    return distR[u] > dist[v];
  };
  MinPriorityQueue<decltype(compare_distanceR)> minPqR(compare_distanceR);
  distR[target] = 0;
  minPqR.emplace(target);

  Weight best_dist = kInfinite;
  Vertex best_vertex = 0;
  while (this->Process(this->m_outgoing_edges, visited, dist, minPq, back_trace,
                       [&](auto) {
                         return minPqR.empty()
                                    ? false
                                    : dist[minPq.top()] + distR[minPqR.top()] >=
                                          best_dist;
                       },
                       [&](Vertex current, Vertex next) {
                         if (visitedR[next]) {
                           if (dist[next] + distR[next] <= best_dist) {
                             best_dist = dist[next] + distR[next];
                             best_vertex = next;
                           }
                         }
                       }) &&
         this->Process(this->m_incoming_edges, visitedR, distR, minPqR,
                       back_traceR,
                       [&](auto) {
                         return minPq.empty()
                                    ? false
                                    : dist[minPq.top()] + distR[minPqR.top()] >=
                                          best_dist;
                       },
                       [&](Vertex current, Vertex next) {
                         if (visited[next]) {
                           if (distR[next] + dist[next] <= best_dist) {
                             best_dist = distR[next] + dist[next];
                             best_vertex = next;
                           }
                         }
                       })) {
  }

  if (!(minPq.empty() || minPqR.empty() || best_dist == kInfinite)) {
    spdlog::info("size for {}, size back {} and best_vertex is {}",
                 back_trace.size(), back_traceR.size(), best_vertex);
    std::list<Vertex> shortest_path{}, shortest_pathR{};
    auto trace_it = back_trace.find(best_vertex);
    while (trace_it != back_trace.end()) {
      shortest_path.insert(shortest_path.begin(), trace_it->second);
      trace_it = back_trace.find(trace_it->second);
    }

    auto trace_itR = back_traceR.find(best_vertex);
    while (trace_itR != back_traceR.end()) {
      shortest_pathR.emplace_back(trace_itR->second);
      trace_itR = back_traceR.find(trace_itR->second);
    }

    shortest_path.splice(shortest_path.begin(), shortest_pathR);
    for (const Vertex &vertex : shortest_path) {
      search_result.m_shortest_coor_list.emplace_back(m_coordinates[vertex]);
    }

    search_result.m_is_success = true;
    search_result.m_total_dist = best_dist;
  }

  return search_result.ToJsonStr();
}

bool graph::AlgoDijkstra::ReadGraphRawDataFromFile() {
  bool ok = this->ReadFileToAdjacentList();

  if (ok == false) {
    return false;
  }

  return this->ReadFileToCoordinateList();
}

bool graph::AlgoDijkstra::InitStrategyV() {
  bool ok = this->ReadGraphRawDataFromFile();
  this->m_process_status =
      ok ? GProcessStatus::PROCESSED : GProcessStatus::FAILED;

  spdlog::info("Init strategy dijkstra success with number of vertexs: {}",
               this->m_outgoing_edges.size());
  return ok;
}

std::string graph::AlgoDijkstra::DoQueryV(Vertex &&source, Vertex &&target) {
  return this->BiDijkstra(std::move(source), std::move(target));
}
