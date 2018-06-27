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
GraphStat
GraphStatCpu<State, BaseClass>::compute(const graph_t &graph, State *globalState) const {
  GraphStat graphStat;
  std::cout << "Start Graph Stat " << std::endl;

  #pragma omp parallel
  {
    GraphStat privateGraphStat;

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
      }

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
