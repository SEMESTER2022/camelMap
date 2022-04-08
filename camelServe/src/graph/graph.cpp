#include "graph/graph.h"

graph::Graph &GraphInstance() {
  static graph::Graph *g_graph{new graph::Graph()};
  return *g_graph;
}
