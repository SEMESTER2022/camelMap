#include "algo_dijkstra.h"

#include <fstream>
#include <string>

#include "spdlog/spdlog.h"

#include "graph/def.h"

template <class BreakCondition, class UpdateBestDist>
bool graph::AlgoDijkstra::Process(AdjacentList &adj, VisitedList &visited,
                                  WeightList &distance, MinPriorityQueue &pq,
                                  BreakCondition break_condition,
                                  UpdateBestDist update_best_dist) {
  if (pq.empty() || break_condition(pq.top())) {
    return false;
  }

  Vertex node = pq.top();
  pq.pop();

  if (visited[node]) {
    return this->Process(adj, visited, distance, pq, break_condition,
                         update_best_dist);
  }

  for (const auto &[neighbour, weight] : adj[node]) {
    if (!visited[neighbour]) {
      if (Weight new_weight = distance[node] + weight;
          new_weight < distance[neighbour]) {
        distance[neighbour] = new_weight;
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

  WeightList dist(num_nodes, kInfinite);
  VisitedList visited(num_nodes, false);
  auto compare_distance = [&](Vertex u, Vertex v) { return dist[u] > dist[v]; };
  MinPriorityQueue<decltype(compare_distance)> minPq(compare_distance);
  dist[source] = 0;
  minPq.emplace(source);

  WeightList distR(num_nodes, kInfinite);
  VisitedList visitedR(num_nodes, false);
  auto compare_distanceR = [&](Vertex u, Vertex v) {
    return distR[u] > dist[v];
  };
  MinPriorityQueue<decltype(compare_distanceR)> minPqR(compare_distanceR);
  distR[target] = 0;
  minPqR.emplace(target);

  Weight best_dist = kInfinite;
  while (this->Process(this->m_outgoing_edges, visited, dist, minPq,
                       [&](auto) {
                         return minPqR.empty()
                                    ? false
                                    : dist[minPq.top()] + distR[minPqR.top()] >=
                                          best_dist;
                       },
                       [&](Vertex current, Vertex next) {
                         if (visitedR[next]) {
                           best_dist =
                               std::min(best_dist, dist[next] + distR[next]);
                         }
                       }) &&
         this->Process(this->m_incoming_edges, visitedR, distR, minPqR,
                       [&](auto) {
                         return minPq.empty()
                                    ? false
                                    : dist[minPq.top()] + distR[minPqR.top()] >=
                                          best_dist;
                       },
                       [&](Vertex current, Vertex next) {
                         if (visited[next]) {
                           best_dist =
                               std::min(best_dist, distR[next] + dist[next]);
                         }
                       })) {
  }

  if (!(minPq.empty() || minPqR.empty() || best_dist == kInfinite)) {
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
  return std::move(this->ReadGraphRawDataFromFile());
}

std::string graph::AlgoDijkstra::DoQueryV(Vertex &&source, Vertex &&target) {
  bool ok = this->BiDijkstra(std::move(source), std::move(target));

  this->m_process_status = ok ? GProcessBiDijkstraStatus::PROCESSED
                              : GProcessBiDijkstraStatus::FAILED;

  return ok;
}
