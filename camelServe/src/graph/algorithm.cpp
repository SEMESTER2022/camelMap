#include "algorithm.h"

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

auto graph::Algorithm::FindNearestCoor(const auto &col,
                                       const Coordinate &coor) const {
  // clang-format off
  return \
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
}

bool graph::Algorithm::ReadFileToCoordinateList() {
  if (this->m_path_in_coor.empty()) {
    return false;
  }

  std::fstream infile(this->m_path_in_coor, std::ios_base::in);
  if (infile.is_open() == false) {
    return false;
  }

  int nodes, lat, lng;
  Vertex node;

  infile >> nodes;
  this->m_coordinates.resize(nodes);

  while (infile >> node >> lng >> lat) {
    if (node > nodes) {
      return false;
    }
    this->m_coordinates[--node] = {lng, lat};
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

std::optional<std::pair<Vertex, Vertex>>
graph::Algorithm::FindNearestSourceDestV(Coordinate src_coor,
                                         Coordinate dst_coor) const {

  MongoQueryf([](auto client) {
    auto col = (*client)[net::DB_CAMEL_MAP][net::COL_US_NEW_YORK_COOR];
    Vertex src{}, tgt{};

    auto doc = this->FindNearestCoor(col, src_coor);
    if (!doc.has_value()) {
      spdlog::info("Not found nearest source coor");
      return nullopt;
    }

    auto src_it = doc.find("node_id");
    if (src_it == doc.end()) {
      spdlog::error("An error occurred");
      return nullopt;
    }

    src = doc["node_id"].get_int64();

    doc = this->FindNearestCoor(col, dst_coor);
    if (!doc.has_value()) {
      spdlog::error("Not found nearest dest coor");
      return nullopt;
    }

    auto tgt_it = doc.find("node_id");
    if (src_it == doc.end()) {
      spdlog::error("An error occured");
      return nullopt;
    }

    tgt = doc["node_id"].get_int64();

    return {std::move(src), std::move(tgt)};
  });
}
