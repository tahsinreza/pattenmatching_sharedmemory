//
// Created by qiu on 21/06/18.
//

#ifndef PROJECT_GRAPH_STAT_H
#define PROJECT_GRAPH_STAT_H

#include "totem_graph.h"
#include <unordered_map>
#include <iostream>

class GraphStat {
 public:

  GraphStat& operator+=(const GraphStat& other);
  friend GraphStat operator+(GraphStat left,
                                        const GraphStat& right);

  void addVertex(const weight_t &vertexLabel);
  void addEdge(const weight_t &vertexFromLabel, const weight_t &vertexTpLabel);

  void computeStats();

  void print(std::ostream &ostream = std::cout) const;
 public:
  std::unordered_map<weight_t, std::unordered_map<weight_t, size_t> > edgeLabelTotalNumberMap;
  std::unordered_map<weight_t, size_t> vertexLabelTotalNumberMap;

  size_t vertexTotalNumber;
  std::unordered_map<weight_t, float> vertexLabelAverageNumberMap;

  size_t edgeTotalNumber;
  std::unordered_map<weight_t, std::unordered_map<weight_t, float> > edgeLabelAverageNumberMap;
  std::unordered_map<weight_t, size_t> edgeOutboundLabelTotalNumberMap;
  std::unordered_map<weight_t, float> edgeOutboundLabelAverageNumberMap;


};

#endif //PROJECT_GRAPH_STAT_H
