//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MAKE_UNDIRECTED_CPU_H
#define TOTEM_MAKE_UNDIRECTED_CPU_H

#include <iostream>
#include <unordered_set>
#include <set>
#include "graph_stat.h"
#include "edge.h"

namespace patternmatching {

class MakeUndirectedCpu {
 public:
  using EdgeSetType = std::set<DirectedEdge>;
  using EdgeUnorderedSetType = std::unordered_set<DirectedEdge, DirectedEdgeHash>;
 public:
  void compute(const graph_t &graph, EdgeSetType &edgeSet);
  void computeRemove(const graph_t &graph, EdgeSetType &edgeSet);

 private:
};

}


#endif //TOTEM_GRAPH_STAT_CPU_H
