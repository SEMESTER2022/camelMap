#include "graph/algorithm.h"

#include <cmath>
#include <fstream>

#include "graph/def.h"
#include "net/mongo.h"
#include "net/mongo_def.h"

#include "spdlog/spdlog.h"

void graph::Algorithm::AddEdgeSerialize(TargetList &list, Vertex &node,
                                        Weight &weight) {
  for (auto &[target, weight_target] : list) {
    if (target == node && weight_target > weight) {
      weight_target = weight;
      return;
    }
  }

  list.emplace_back(std::make_pair(node, weight));
}

bool graph::Algorithm::ReadFileToCoordinateList() {
  if (this->m_path_in_coor.empty()) {
    return false;
  }

  std::fstream infile(this->m_path_in_coor, std::ios_base::in);
  if (infile.is_open() == false) {
    return false;
  }

  uint nodes;
  int lat, lng;
  Vertex node;

  infile >> nodes;
  this->m_coordinates.resize(nodes);

  while (infile >> node >> lng >> lat) {
    if (node > nodes) {
      return false;
    }

    this->m_coordinates[--node] = {lng / pow(10, 6), lat / pow(10, 6)};
  }

  return true;
}

bool graph::Algorithm::ReadFileToAdjacentList() {
  if (this->m_path_in_dist.empty()) {
    return false;
  }

  std::fstream infile(this->m_path_in_dist, std::ios_base::in);
  if (infile.is_open() == false) {
    return false;
  }

  int nodes, edges;
  Vertex source, target;
  Weight weight;

  infile >> nodes >> edges;
  this->m_outgoing_edges.resize(nodes);
  this->m_incoming_edges.resize(nodes);

  while (infile >> source >> target >> weight) {
    --source;
    --target;
    this->AddEdge(source, target, weight);
  }

  return true;
}

std::tuple<bool, graph::Vertex, graph::Vertex>
graph::Algorithm::FindNearestSourceDestV(Coordinate src_coor,
                                         Coordinate dst_coor) {
  Vertex src{}, tgt{};
  bool ok = MongoQueryf([&](mongocxx::client& client) {
    mongocxx::collection col =
      client[DB_CAMEL_MAP][COL_US_NEW_YORK_COOR];

    static auto query = [&](auto coor) {
      // clang-format off
          return
            col.find_one(make_document(
              kvp("location", make_document(
                kvp("$near", make_document(
                  kvp("$geometry", make_document(
                    kvp("type", "Point"),
                    kvp("coordinates", make_array(
                      coor[0], coor[1]
                    ))
                  ))
                ))
              ))
            ));
      // clang-format on
    };

    auto doc = query(src_coor);
    if (!doc) {
      spdlog::info("Not found nearest source coor");
      return false;
    }

    auto view = doc->view();

    auto src_it = view.find("node_id");
    if (src_it == view.end()) {
      spdlog::error("An error occurred");
      return false;
    }

    src = static_cast<Vertex>(src_it->get_int64());

    doc = query(dst_coor);
    if (!doc) {
      spdlog::error("Not found nearest dest coor");
      return false;
    }

    view = doc->view();

    auto tgt_it = view.find("node_id");
    if (tgt_it == view.end()) {
      spdlog::error("An error occured");
      return false;
    }

    tgt = static_cast<Vertex>(tgt_it->get_int64());

    return true;
  });

  if (ok == true) {
    return {true, std::move(src), std::move(tgt)};
  }

  return {false, 0, 0};
}
