//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_lcc0_cpu.h"
#include <iostream>

namespace patternmatching {

template<class State>
__host__ error_t
MultipleLabelLcc0Cpu<State>::preprocessPatern(const graph_t &pattern) {
  patternLocalConstraint.resize(pattern.vertex_count);
  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];

    auto &currentLocalConstraint = patternLocalConstraint[vertexId];
    currentLocalConstraint.originVertex = vertexId;
    currentLocalConstraint.originLabel = currentLabel;

    // build local constraint map
    for (eid_t patternEdgeId = pattern.vertices[vertexId]; patternEdgeId < pattern.vertices[vertexId + 1];
         patternEdgeId++) {
      vid_t neighborVertexId = pattern.edges[patternEdgeId];
      weight_t neighborLabel = pattern.values[neighborVertexId];

      currentLocalConstraint.localConstraintNeighborVertex.insert(neighborVertexId);
      if (currentLocalConstraint.localConstraint.find(neighborLabel) != currentLocalConstraint.localConstraint.end()) {
        ++(currentLocalConstraint.localConstraint)[neighborLabel];
      } else {
        currentLocalConstraint.localConstraint[neighborLabel] = 1;
      }
    }
  }

  return SUCCESS;
}

template<class State>
__host__ void MultipleLabelLcc0Cpu<State>::printLocalConstraint(std::ostream &ostream) const {
  for (const auto &it : patternLocalConstraint) {
    it.print(ostream);
  }
}

template<class State>
__host__ size_t
MultipleLabelLcc0Cpu<State>::compute(const graph_t &graph, State *globalState) const {

  size_t edgeEliminatedNumber = 0;
  std::cout << "Start LCC 0 " << std::endl;
  std::unordered_map <weight_t, size_t> currentLocalConstraint;
  currentLocalConstraint.reserve(10);

  #pragma omp parallel for reduction(+:edgeEliminatedNumber) private(currentLocalConstraint)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    weight_t currentLabel = graph.values[vertexId];

    // Build the set of possible neighbor label matches
    std::unordered_set <weight_t> neighborConstraintSet;
    for (const auto &it : globalState->vertexPatternMatch[vertexId]) {
      const auto &currentLocalConstraint = patternLocalConstraint[it];
      if(currentLabel!=currentLocalConstraint.originLabel) continue;
      for (const auto &neighborLabelIt : currentLocalConstraint.localConstraint) {
        neighborConstraintSet.insert(neighborLabelIt.first);
      }
    }

    if(neighborConstraintSet.empty()) {
      for (const auto &it : globalState->vertexPatternMatch[vertexId]) {
        BaseClass::makeToUnmatch(globalState, vertexId, it);
      }
      continue;
    }
    // build local constraint
    for(const auto& it : currentLocalConstraint) {
      currentLocalConstraint[it.first]=0;
    }

    for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      // Remove edge if match is impossible
      if(neighborConstraintSet.find(graph.values[neighborVertexId])==neighborConstraintSet.end()) {
        ++edgeEliminatedNumber;
        BaseClass::deactivateEdge(globalState, neighborEdgeId);
        continue;
      }

      // Else, count label
      weight_t neighborLabel = graph.values[neighborVertexId];
      if (currentLocalConstraint.find(neighborLabel) != currentLocalConstraint.end()) {
        ++(currentLocalConstraint)[neighborLabel];
      } else {
        currentLocalConstraint[neighborLabel] = 1;
      }
    }

    auto patternVertexIterator = globalState->vertexPatternMatch[vertexId].cbegin();
    while (patternVertexIterator != globalState->vertexPatternMatch[vertexId].cend()) {
      // Avoid removing the current iterator that would be invalidated
      const auto patternVertexId = *(patternVertexIterator++);

      const auto &testLocalConstraint = patternLocalConstraint[patternVertexId];

      // test if currentLabel is the origin label for the current localConstraint
      if (currentLabel != testLocalConstraint.originLabel) {
        BaseClass::makeToUnmatch(globalState, vertexId, patternVertexId);
        continue;
      }

      // test local constraint
      for (const auto &it : testLocalConstraint.localConstraint) {
        if (currentLocalConstraint[it.first] < it.second) {
          BaseClass::makeToUnmatch(globalState, vertexId, patternVertexId);
          break;
        }
      }
    }
  }

  std::cout << "Mid LCC 0 " << std::endl;
  size_t vertexEliminatedNumber = 0;
  std::cout << "Eliminated edges : " << edgeEliminatedNumber << std::endl;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    for(const auto& patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
      BaseClass::removeMatch(globalState, vertexId, patternIndex);
    }

    if (!BaseClass::isMatch(*globalState, vertexId)) {
      /*std::stringstream ss;
      ss << "Eliminated vertex : " << vertexId;
      Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());*/

      BaseClass::deactivateVertex(globalState, vertexId);
      vertexEliminatedNumber += 1;
    }

    BaseClass::clearToUnmatch(globalState, vertexId);
  }

  std::cout << "End LCC 0 " << std::endl;
  std::cout << "Eliminated edges : " << edgeEliminatedNumber << std::endl;

  return vertexEliminatedNumber;
}


}
