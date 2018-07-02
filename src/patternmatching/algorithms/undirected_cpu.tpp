//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "undirected_cpu.h"
#include <iostream>
#include <fstream>

namespace patternmatching {

template<class State, class BaseClass>
size_t
UndirectedCpu<State, BaseClass>::compute(const graph_t &graph, State *globalState) {
  size_t directedEdge = 0;

  #pragma omp parallel for reduction(+:directedEdge)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
         neighborEdgeId++) {
      bool hasBeenFound = false;
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      for (eid_t neighborEdgeId2 = graph.vertices[neighborVertexId]; neighborEdgeId2 < graph.vertices[neighborVertexId + 1];
           neighborEdgeId2++) {
        if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId2)) continue;
        vid_t neighborVertexId2 = graph.edges[neighborEdgeId2];
        if (neighborVertexId2 == vertexId) {
          hasBeenFound = true;
          break;
        }
      }

      if (!hasBeenFound) {
        /*#pragma omp critical
        {
          std::cout << "Edge not found : "<< neighborVertexId << "," << vertexId <<std::endl;
        }*/

        directedEdge++;
      }
    }
  }

  return directedEdge;
}

}
