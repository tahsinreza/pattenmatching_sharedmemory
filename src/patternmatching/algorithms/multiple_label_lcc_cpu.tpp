//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_lcc_cpu.h"
#include <iostream>

namespace patternmatching {

template<class State>
__host__ error_t
MultipleLabelLccCpu<State>::preprocessPatern(const graph_t &pattern) {
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
__host__ void MultipleLabelLccCpu<State>::printLocalConstraint(std::ostream &ostream) const {
  for (const auto &it : patternLocalConstraint) {
    it.print(ostream);
  }
}

template<class State>
void MultipleLabelLccCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ size_t
MultipleLabelLccCpu<State>::compute(const graph_t &graph, State *globalState) const {

  size_t edgeEliminatedNumber = 0;
  std::cout << "Start LCC " << std::endl;
  std::unordered_map<weight_t, size_t> currentLocalConstraint;
  currentLocalConstraint.reserve(10);

  #pragma omp parallel for reduction(+:edgeEliminatedNumber) private(currentLocalConstraint)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;
    if (!BaseClass::isVertexScheduled(*globalState, vertexId)) continue;

    weight_t currentLabel = graph.values[vertexId];

    // Build the set of possible neighbor matches
    FixedBitmapType neighborConstraintSet;
    for (const auto &it : globalState->vertexPatternMatch[vertexId]) {
      const auto &currentLocalConstraint = patternLocalConstraint[it];
      for (const auto &neighborVertexId : currentLocalConstraint.localConstraintNeighborVertex) {
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
    auto patternVertexIterator = globalState->vertexPatternMatch[vertexId].cbegin();
    while (patternVertexIterator != globalState->vertexPatternMatch[vertexId].cend()) {
      // Avoid removing the current iterator that would be invalidated
      const auto patternVertexId = *(patternVertexIterator++);

      const auto &testLocalConstraint = patternLocalConstraint[patternVertexId];

      // test if currentLabel is the origin label for the current localConstraint
      if (currentLabel != testLocalConstraint.originLabel) {
        BaseClass::makeToUnmatch(globalState, vertexId, patternVertexId);
        hasBeenModified = true;
        continue;
      }

      // test local constraint
      for (const auto &it : testLocalConstraint.localConstraint) {
        if (currentLocalConstraint[it.first] < it.second) {
          BaseClass::makeToUnmatch(globalState, vertexId, patternVertexId);
          hasBeenModified = true;
          break;
        }
      }
    }
    if (hasBeenModified) {
      BaseClass::makeModifiedVertex(globalState, vertexId);
    }
  }

  //std::cout << "Mid LCC " << std::endl;
  size_t vertexEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    if (BaseClass::isVertexModified(*globalState, vertexId)) {
      // The vertex was modified
      for (const auto &patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
        BaseClass::removeMatch(globalState, vertexId, patternIndex);
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
  //std::cout << "End LCC " << std::endl;
  std::cout << "Eliminated edges : " << edgeEliminatedNumber << std::endl;

  return vertexEliminatedNumber;
}

}
