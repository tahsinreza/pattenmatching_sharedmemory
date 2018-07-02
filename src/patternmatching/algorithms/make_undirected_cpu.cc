//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "make_undirected_cpu.h"
#include <iostream>
#include <fstream>

namespace patternmatching {

void
MakeUndirectedCpu::compute(const graph_t &graph, EdgeSetType &edgeSet) {
  edgeSet.clear();
  #pragma omp parallel
  {
    EdgeSetType edgeSetPrivate;

    #pragma omp for nowait
    for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {

      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        edgeSetPrivate.insert(DirectedEdge(vertexId, neighborVertexId));
        edgeSetPrivate.insert(DirectedEdge(neighborVertexId, vertexId));
      }
    }

    #pragma omp critical
    {
      for (const auto &it : edgeSetPrivate) {
        edgeSet.insert(it);
      }
    };
  }
}

void
MakeUndirectedCpu::computeRemove(const graph_t &graph, EdgeSetType &edgeSet) {
  EdgeSetType edgeSetTmp;
  edgeSet.clear();

  #pragma omp parallel
  {
    EdgeSetType edgeSetPrivate;

    #pragma omp for nowait
    for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {

      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        edgeSetPrivate.insert(DirectedEdge(vertexId, neighborVertexId));
      }
    }

    #pragma omp critical
    {
      for (const auto &it : edgeSetPrivate) {
        edgeSetTmp.insert(it);
      }
    };
  }

  auto edge = DirectedEdge();
  for (const auto &it : edgeSetTmp) {
    edge.vertexToIt = it.vertexFromId;
    edge.vertexFromId = it.vertexToIt;

    if (edgeSetTmp.find(edge) != edgeSetTmp.end()) {
      edgeSet.insert(edge);
    }
  }

}

}
