//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_lcc_cpu.h"
#include "common_utils.h"
#include <iostream>

namespace patternmatching {

template<class State>
void MultipleLabelLccCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ AlgoResults
MultipleLabelLccCpu<State>::compute(const graph_t &graph, State *globalState) const {
  AlgoResults algoResults;
  size_t edgeEliminatedNumber = 0;
  std::unordered_map<weight_t, size_t> currentLocalConstraint;
  currentLocalConstraint.reserve(10);

  #pragma omp parallel for reduction(+:edgeEliminatedNumber) firstprivate(currentLocalConstraint)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;
    if (!BaseClass::isVertexScheduled(*globalState, vertexId)) continue;

    weight_t currentLabel = graph.values[vertexId];

    // Build the set of possible neighbor matches
    FixedBitmapType neighborConstraintSet;
    for (const auto &it : globalState->vertexPatternMatch[vertexId]) {
      const auto &currentLocalConstraintTmp = patternLocalConstraint[it];
      for (const auto &neighborVertexId : currentLocalConstraintTmp.localConstraintNeighborVertex) {
        neighborConstraintSet.insert(neighborVertexId);
      }
    }

    // build local constraint
    for (const auto &it : currentLocalConstraint) {
      currentLocalConstraint[it.first] = 0;
    }

    for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      // Test if this vertex can be in the set of possible neighbor matches
      bool isNeighborVertexPossibleMatch = false;
      for (const auto &it :  globalState->vertexPatternMatch[neighborVertexId]) {
        if (neighborConstraintSet.isIn(it)) {
          isNeighborVertexPossibleMatch = true;
          break;
        }
      }

      // Remove edge if match is impossible
      if (!isNeighborVertexPossibleMatch) {
        ++edgeEliminatedNumber;
        BaseClass::deactivateEdge(globalState, neighborEdgeId);
        BaseClass::makeModifiedVertex(globalState, vertexId);
        continue;
      }

      // Else, count label
      weight_t neighborLabel = graph.values[neighborVertexId];
      if (currentLocalConstraint.find(neighborLabel) != currentLocalConstraint.end()) {
        ++(currentLocalConstraint[neighborLabel]);
      } else {
        currentLocalConstraint[neighborLabel] = 1;
      }
    }

    bool hasBeenModified = false;
    for (const auto & patternVertexIt : globalState->vertexPatternMatch[vertexId]) {
      const auto &testLocalConstraint = patternLocalConstraint[patternVertexIt];

      // test if currentLabel is the origin label for the current localConstraint
      if (currentLabel != testLocalConstraint.originLabel) {
        BaseClass::makeToUnmatch(globalState, vertexId, patternVertexIt);
        hasBeenModified = true;
        continue;
      }

      // test local constraint
      for (const auto &it : testLocalConstraint.localConstraint) {
        if (currentLocalConstraint[it.first] < it.second) {
          BaseClass::makeToUnmatch(globalState, vertexId, patternVertexIt);
          hasBeenModified = true;
          break;
        }
      }
    }
    if (hasBeenModified) {
      BaseClass::makeModifiedVertex(globalState, vertexId);
    }
  }

  size_t vertexEliminatedNumber = 0;
  size_t matchEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber, matchEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    if (BaseClass::isVertexModified(*globalState, vertexId)) {
      // The vertex was modified
      for (const auto &patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
        BaseClass::removeMatch(globalState, vertexId, patternIndex);
        matchEliminatedNumber++;
      }

      if (!BaseClass::isMatch(*globalState, vertexId)) {
        BaseClass::deactivateVertex(globalState, vertexId);
        vertexEliminatedNumber += 1;
      }

      BaseClass::scheduleVertex(globalState, vertexId);
      BaseClass::clearToUnmatch(globalState, vertexId);
    } else {
      // Schedule vertex close to the one modified
      bool scheduled = false;
      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        if (!BaseClass::isVertexModified(*globalState, neighborVertexId)) continue;

        BaseClass::scheduleVertex(globalState, vertexId);
        scheduled = true;
        break;
      }
      if (!scheduled) {
        BaseClass::unscheduleVertex(globalState, vertexId);
      }
    }
  }

  globalState->graphActiveVertexCount-=vertexEliminatedNumber;
  globalState->graphActiveEdgeCount-=edgeEliminatedNumber;

  algoResults.vertexEliminated=vertexEliminatedNumber;
  algoResults.edgeEliminated=edgeEliminatedNumber;
  algoResults.matchEliminated=matchEliminatedNumber;

  return algoResults;
}

template<class State>
void MultipleLabelLccCpu<State>::setConstraintVector(
    const std::vector <MultipleLabelConstraintLocal>& patternLocalConstraint_) {
  patternLocalConstraint=patternLocalConstraint_;
}

}
