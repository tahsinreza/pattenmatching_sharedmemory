//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "graph_stat_cpu.h"
#include <iostream>
#include <fstream>

namespace patternmatching {

template<class State, class BaseClass>
void
GraphStatCpu<State, BaseClass>::preprocessPatern(const graph_t &pattern) {
  accumulationMaximumMap.clear();
  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t vertexLabel=pattern.values[vertexId];
    std::map<weight_t, size_t> currentAccumulation;
    for (eid_t neighborEdgeId = pattern.vertices[vertexId]; neighborEdgeId < pattern.vertices[vertexId + 1];
         neighborEdgeId++) {
      vid_t neighborVertexId = pattern.edges[neighborEdgeId];
      weight_t neighborLabel=pattern.values[neighborVertexId];
      currentAccumulation[neighborLabel]+=1;
    }
    for(const auto &it : currentAccumulation) {
      if(accumulationMaximumMap[vertexLabel][it.first]<currentAccumulation[it.first]) {
        accumulationMaximumMap[vertexLabel][it.first]=currentAccumulation[it.first];
      }
    }
  }

  for(const auto &it : accumulationMaximumMap) {
    for(const auto &it2 : it.second) {
      std::cout << it.first <<"\t"<<it2.first <<"\t"<<it2.second <<"\t"<<std::endl;
    }
  }
}

template<class State, class BaseClass>
GraphStat
GraphStatCpu<State, BaseClass>::compute(const graph_t &graph, State *globalState) const {
  GraphStat graphStat;
  std::cout << "Start Graph Stat " << std::endl;


  #pragma omp parallel
  {
    GraphStat privateGraphStat(accumulationMaximumMap);
    std::map<weight_t, size_t> privateAccumulation;

    #pragma omp for nowait
    for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
      if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

      weight_t currentLabel = graph.values[vertexId];
      privateGraphStat.addVertex(currentLabel);

      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;
        weight_t neighborLabel = graph.values[neighborVertexId];

        privateGraphStat.addEdge(currentLabel, neighborLabel);
        privateAccumulation[neighborLabel]+=1;
      }
      privateGraphStat.addAccumulation(currentLabel, privateAccumulation);
    }

    //Reduction
    #pragma omp critical
    {
      graphStat+=privateGraphStat;
    }
  }

  graphStat.computeStats();

  return graphStat;
}

}
