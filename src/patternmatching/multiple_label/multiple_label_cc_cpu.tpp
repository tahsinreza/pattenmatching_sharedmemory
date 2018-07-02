//
// Created by qiu on 17/05/18.
//
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_cc_cpu.h"
#include <iostream>
#include "common_utils.h"

namespace patternmatching {

template<class State>
bool MultipleLabelCcCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const MultipleLabelConstraintCircular &currentConstraint,
    sourceTraversalMapType &sourceTraversalMap,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &startingPosition,
    const size_t &remainingLength) {

  // Verify that we closed the cycle.
  if (remainingLength == 1) {
    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if (sourceVertexId == neighborVertexId) {
        return true;
      }
    }
  } else {
    // Find next position in constraint
    size_t currentPositionInConstraint = (startingPosition + currentConstraint.length - (remainingLength))
        % currentConstraint.vertexLabelVector.size();
    size_t nextPositionInConstraint = (startingPosition + currentConstraint.length - (remainingLength - 1))
        % currentConstraint.vertexLabelVector.size();

    const auto nextConstraintVertexIndex = currentConstraint.vertexIndexVector[nextPositionInConstraint];

    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      const vid_t &neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if (!globalState->vertexPatternMatch[neighborVertexId].isIn(nextConstraintVertexIndex))
        continue;

      const auto &edge = (currentVertexId > neighborVertexId) ? std::make_pair(currentVertexId, neighborVertexId)
                                                              : std::make_pair(neighborVertexId, currentVertexId);

      if (sourceTraversalMap.find(edge) != sourceTraversalMap.end()
          && sourceTraversalMap[edge].isIn(currentPositionInConstraint)) {
        continue;
      }

      sourceTraversalMap[edge].insert(nextConstraintVertexIndex);
      if (checkConstraint(graph, globalState, currentConstraint, sourceTraversalMap, sourceVertexId,
                          neighborVertexId, startingPosition, remainingLength - 1)) {
        BaseClass::makeAlreadyMatchedAtomic(globalState, neighborVertexId, nextConstraintVertexIndex);
        return true;
      }
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelCcCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ AlgoResults
MultipleLabelCcCpu<State>::compute(
    const graph_t &graph, State
*globalState) {
  const auto &currentConstraint = *circularConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &MultipleLabelConstraintCircular::print,
                            Logger::E_OUTPUT_FILE_LOG);

  PROGRESSION_INSERT_BEGIN()

  sourceTraversalMapType sourceTraversalMap;
  sourceTraversalMap.reserve(1000);

  #pragma omp parallel for private(sourceTraversalMap) //schedule(dynamic, 1000)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    size_t startingPositionInConstraint = 0;
    size_t constraintIndex = 0;
    bool hasBeenModified = false;

    for (auto it = currentConstraint.vertexIndexVector.cbegin();
         it != currentConstraint.vertexIndexVector.cend();
         ++it, ++constraintIndex) {

      if (globalState->vertexPatternMatch[vertexId].isIn(*it)) {
        if (BaseClass::isAlreadyMatchedAtomic(*globalState, vertexId, *it)) continue;

        startingPositionInConstraint = constraintIndex;
        // Check cycle
        sourceTraversalMap.clear();
        if (!checkConstraint(graph,
                             globalState,
                             currentConstraint,
                             sourceTraversalMap,
                             vertexId,
                             vertexId,
                             startingPositionInConstraint,
                             currentConstraint.length)) {
          BaseClass::makeToUnmatch(globalState, vertexId, *it);
          hasBeenModified = true;
        }
      }
    }

    if (hasBeenModified) {
      BaseClass::makeModifiedVertex(globalState, vertexId);
    }
    PROGRESSION_INSERT_LOOP()
  }

  size_t vertexEliminatedNumber = 0;
  size_t matchEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber, matchEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    BaseClass::clearAlreadyMatched(globalState, vertexId);

    if (BaseClass::isVertexModified(*globalState, vertexId)) {
      for (const auto &patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
        BaseClass::removeMatch(globalState, vertexId, patternIndex);
        matchEliminatedNumber += 1;
      }

      if (!BaseClass::isMatch(*globalState, vertexId)) {
        /*std::stringstream ss;
        ss << "Removed Vertex : " << vertexId << std::endl;
        Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());*/
        BaseClass::deactivateVertex(globalState, vertexId);
        vertexEliminatedNumber += 1;
      }

      BaseClass::scheduleVertex(globalState, vertexId);
      BaseClass::clearToUnmatch(globalState, vertexId);
    } else {
      // Schedule vertex close to the one modified
      bool hasBeenScheduled = false;
      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        if (!BaseClass::isVertexModified(*globalState, neighborVertexId)) continue;

        BaseClass::scheduleVertex(globalState, vertexId);
        hasBeenScheduled = true;
        break;
      }
      if (!hasBeenScheduled) {
        BaseClass::unscheduleVertex(globalState, vertexId);
      }
    }

  }

  globalState->graphActiveVertexCount -= vertexEliminatedNumber;
  AlgoResults algoResults;
  algoResults.vertexEliminated = vertexEliminatedNumber;
  algoResults.matchEliminated = matchEliminatedNumber;
  ++circularConstraintIterator;

  return algoResults;
}

template<class State>
void MultipleLabelCcCpu<State>::setConstraintVector(
    const std::vector<MultipleLabelConstraintCircular> &circularConstraintVector_) {
  circularConstraintVector = circularConstraintVector_;
  circularConstraintIterator = circularConstraintVector.cbegin();
}

template<class State>
void MultipleLabelCcCpu<State>::setConstraintIterator(const size_t &index) {
  circularConstraintIterator = circularConstraintVector.cbegin() + index;
}
}