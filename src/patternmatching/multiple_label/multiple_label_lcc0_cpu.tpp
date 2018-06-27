//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_lcc0_cpu.h"
#include "utils.h"
#include <iostream>

namespace patternmatching {

template<class State>
__host__ AlgoResults
MultipleLabelLcc0Cpu<State>::compute(const graph_t &graph, State *globalState) const {
  AlgoResults algoResults;
  size_t edgeEliminatedNumber = 0;
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

    for (const auto &patternVertexIt : globalState->vertexPatternMatch[vertexId]) {
      const auto &testLocalConstraint = patternLocalConstraint[patternVertexIt];

      // test if currentLabel is the origin label for the current localConstraint
      if (currentLabel != testLocalConstraint.originLabel) {
        BaseClass::makeToUnmatch(globalState, vertexId, patternVertexIt);
        continue;
      }

      // test local constraint
      for (const auto &it : testLocalConstraint.localConstraint) {
        if (currentLocalConstraint[it.first] < it.second) {
          BaseClass::makeToUnmatch(globalState, vertexId, patternVertexIt);
          break;
        }
      }
    }
  }

  size_t vertexEliminatedNumber = 0;
  size_t matchEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber, matchEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    for(const auto& patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
      matchEliminatedNumber++;
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

  globalState->graphActiveVertexCount-=vertexEliminatedNumber;
  globalState->graphActiveEdgeCount-=edgeEliminatedNumber;

  algoResults.vertexEliminated=vertexEliminatedNumber;
  algoResults.edgeEliminated=edgeEliminatedNumber;
  algoResults.matchEliminated=matchEliminatedNumber;

  return algoResults;
}

template<class State>
void MultipleLabelLcc0Cpu<State>::setConstraintVector(
    const std::vector <MultipleLabelConstraintLocal>& patternLocalConstraint_) {
  patternLocalConstraint=patternLocalConstraint_;
}


}
