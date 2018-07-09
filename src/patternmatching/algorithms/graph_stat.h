//
// Created by qiu on 21/06/18.
//

#ifndef PROJECT_GRAPH_STAT_H
#define PROJECT_GRAPH_STAT_H

#include "totem_graph.h"
#include <unordered_map>
#include <map>
#include <iostream>
#include "common_utils.h"

class GraphStat {
 public:
  #if PRODUCTION == 1
  template<class T1, class T2>
  using MapType = std::unordered_map<T1, T2 >;
  #else
  template<class T1, class T2>
  using MapType = std::map<T1, T2 >;
  #endif
 public:
  GraphStat();
  GraphStat(const std::map<weight_t, std::map<weight_t, size_t> > &accumulationMaximumMap);

  GraphStat& operator+=(const GraphStat& other);
  friend GraphStat operator+(GraphStat left,
                                        const GraphStat& right);

  void addVertex(const weight_t &vertexLabel);
  void addEdge(const weight_t &vertexFromLabel, const weight_t &vertexTpLabel);
  void addAccumulation(const weight_t &vertexFromLabel, const std::map<weight_t, size_t> &vertexToMapNumber);

  void computeStats();

  void print(std::ostream &ostream = std::cout) const;
 public:
  MapType<weight_t, MapType<weight_t, size_t> > edgeLabelTotalNumberMap;
  MapType<weight_t, size_t> vertexLabelTotalNumberMap;
  MapType<weight_t, MapType<weight_t, MapType<size_t, size_t> > > vertexLabelWithAtLeastNeighborLabelMap;

  size_t vertexTotalNumber;
  MapType<weight_t, double> vertexLabelAverageNumberMap;

  size_t edgeTotalNumber;
  MapType<weight_t, std::unordered_map<weight_t, double> > edgeLabelAverageNumberMap;
  MapType<weight_t, size_t> edgeOutboundLabelTotalNumberMap;
  MapType<weight_t, double> edgeOutboundLabelAverageNumberMap;

  std::map<weight_t, std::map<weight_t, size_t> > accumulationMaximumMap;

};

#endif //PROJECT_GRAPH_STAT_H
