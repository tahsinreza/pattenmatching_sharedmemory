//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_engine.cuh"
#include "totem_util.h"
#include "multiple_label_lcc_cpu.cuh"
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
__host__ size_t
MultipleLabelLccCpu<State>::compute(const graph_t &graph, State *globalState) const {

  size_t edgeEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:edgeEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    weight_t currentLabel = graph.values[vertexId];

    // Build the set of possible neighbor matches
    std::unordered_set <vid_t> neighborConstraintSet;
    for (const auto &it : globalState->vertexPatternMatch[vertexId]) {
      const auto &currentLocalConstraint = patternLocalConstraint[it];
      for (const auto &neighborVertexId : currentLocalConstraint.localConstraintNeighborVertex) {
        neighborConstraintSet.insert(neighborVertexId);
      }
    }

    // build local constraint
    std::unordered_map <weight_t, size_t> currentLocalConstraint;

    for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      // Test if this vertex can be in the set of possible neighbor matches
      bool isNeighborVertexPossibleMatch = false;
      for( const auto &it :  globalState->vertexPatternMatch[neighborVertexId]) {
        if(neighborConstraintSet.find(it) !=neighborConstraintSet.end()) {
          isNeighborVertexPossibleMatch=true;
          break;
        }
      }

      // Remove edge if match is impossible
      if(!isNeighborVertexPossibleMatch) {
        std::stringstream ss;
        ss << "Eliminated edge : " << neighborEdgeId;
        Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());
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
        removeMatch(globalState, vertexId, patternVertexId);
        continue;
      }

      // test local constraint
      for (const auto &it : testLocalConstraint.localConstraint) {
        if (currentLocalConstraint[it.first] < it.second) {
          removeMatch(globalState, vertexId, patternVertexId);
          break;
        }
      }
    }
  }

  size_t vertexEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    for(const auto& patternIndex : globalState->vertexPatternToUnmatchLcc[vertexId]) {
      BaseClass::removeMatch(globalState, vertexId, patternIndex);
    }

    if (!BaseClass::isMatch(*globalState, vertexId)) {
      std::stringstream ss;
      ss << "Eliminated vertex : " << vertexId;
      Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());

      BaseClass::deactivateVertex(globalState, vertexId);
      vertexEliminatedNumber += 1;
    }

    globalState->vertexPatternToUnmatchLcc[vertexId].clear();
  }

  std::cout << "Eliminated edges : " << edgeEliminatedNumber << std::endl;

  return vertexEliminatedNumber;
}

template<class State>
void MultipleLabelLccCpu<State>::removeMatch(State *globalState,
                                             const vid_t vertexId,
                                             const pvid_t patternVertexId) const {
  globalState->vertexPatternToUnmatchLcc[vertexId].insert(patternVertexId);
}

template<class State>
MultipleLabelLccCpu<State>::LocalConstraint::LocalConstraint()
    : originVertex(0), originLabel(0) {
  localConstraint.clear();
  localConstraintNeighborVertex.clear();
}

template<class State>
MultipleLabelLccCpu<State>::LocalConstraint::LocalConstraint(
    const pvid_t &originVertex_,
    const weight_t &originLabel_,
    const std::unordered_map <weight_t, size_t> &localConstraint_,
    const std::unordered_set <vid_t> &localConstraintNeighborVertex_)
    : originVertex(originVertex_), originLabel(originLabel_) {
  localConstraint = localConstraint_;
  localConstraintNeighborVertex = localConstraintNeighborVertex_;
}

template<class State>
void MultipleLabelLccCpu<State>::LocalConstraint::print(std::ostream &ostream) const {
  ostream << "Vertex " << originVertex << " : Label " << originLabel << std::endl;
  for (const auto &subit : localConstraint) {
    ostream << "\t Sublabel " << subit.first << " : " << subit.second << std::endl;
  }
  for (const auto &subit : localConstraintNeighborVertex) {
    ostream << "\t Neighbor " << subit << std::endl;
  }
}

}
