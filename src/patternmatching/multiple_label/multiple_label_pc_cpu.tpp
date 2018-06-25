//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_pc_cpu.h"
#include <iostream>
#include "utils.h"

namespace patternmatching {

template<class State>
bool MultipleLabelPcCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const MultipleLabelConstraintPath &currentConstraint,
    const bool &reverse,
    sourceTraversalMapType &sourceTraversalMap,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &remainingLength) {

  size_t currentPositionInConstraint = reverse ? remainingLength :
                                    (currentConstraint.length - remainingLength-1);

  size_t nextPositionInConstraint = reverse ? remainingLength - 1 :
                                    (currentConstraint.length - remainingLength);

  // Verify that we closed the cycle.
  if (remainingLength == 1) {
    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      const auto &nextConstraintVertexIndex = currentConstraint.vertexIndexVector[nextPositionInConstraint];
      if (globalState->vertexPatternMatch[neighborVertexId].find(nextConstraintVertexIndex)
          == globalState->vertexPatternMatch[neighborVertexId].end())
        continue;

      if (sourceVertexId != neighborVertexId) {
        BaseClass::makeAlreadyMatchedAtomic(globalState, neighborVertexId, nextConstraintVertexIndex);
        return true;
      }
    }
  } else {
    // Find neighbor
    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      const auto &nextConstraintVertexIndex = currentConstraint.vertexIndexVector[nextPositionInConstraint];
      if (!globalState->vertexPatternMatch[neighborVertexId].isIn(nextConstraintVertexIndex))
        continue;


      const auto &edge = (currentVertexId>neighborVertexId) ? std::make_pair(currentVertexId,neighborVertexId)
                                                            : std::make_pair(neighborVertexId,currentVertexId);

      if (sourceTraversalMap.find(edge)!=sourceTraversalMap.end()
          && sourceTraversalMap[edge].isIn(currentPositionInConstraint))  {
        continue;
      }

      sourceTraversalMap[edge].insert(nextConstraintVertexIndex);
      if (checkConstraint(graph, globalState, currentConstraint, reverse, sourceTraversalMap, sourceVertexId,
                          neighborVertexId, remainingLength - 1)) {
        return true;
      }
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelPcCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ size_t
MultipleLabelPcCpu<State>::compute(const graph_t &graph, State *globalState) {
  //resetState(globalState);

  const auto &currentConstraint = *pathConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &MultipleLabelConstraintPath::print,
                            Logger::E_OUTPUT_FILE_LOG);

  sourceTraversalMapType sourceTraversalMap;

  #pragma omp parallel for private(sourceTraversalMap)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;
    weight_t vertexLabel = graph.values[vertexId];
    if (vertexLabel != currentConstraint.initialLabel) continue;

    bool hasBeenModified = false;


    // Check first index
    size_t constraintPatternIndex = currentConstraint.vertexIndexVector[0];
    if (globalState->vertexPatternMatch[vertexId].isIn(constraintPatternIndex)) {
      if (!BaseClass::isAlreadyMatchedAtomic(*globalState, vertexId, constraintPatternIndex)) {
        // Check cycle
        sourceTraversalMap.clear();
        if (!checkConstraint(graph,
                             globalState,
                             currentConstraint,
                             false,
                             sourceTraversalMap,
                             vertexId,
                             vertexId,
                             currentConstraint.length - 1)) {
          BaseClass::makeToUnmatch(globalState, vertexId, constraintPatternIndex);
          hasBeenModified = true;
        }
      }
    }

    // Check last index
    constraintPatternIndex = currentConstraint.vertexIndexVector[currentConstraint.length - 1];
    if (globalState->vertexPatternMatch[vertexId].isIn(constraintPatternIndex)) {
      if (!BaseClass::isAlreadyMatchedAtomic(*globalState, vertexId, constraintPatternIndex)) {
        // Check cycle
        sourceTraversalMap.clear();
        if (!checkConstraint(graph,
                             globalState,
                             currentConstraint,
                             true,
                             sourceTraversalMap,
                             vertexId,
                             vertexId,
                             currentConstraint.length - 1)) {
          BaseClass::makeToUnmatch(globalState, vertexId, constraintPatternIndex);
          hasBeenModified = true;
        }
      }
    }

    if (hasBeenModified) {
      BaseClass::makeModifiedVertex(globalState, vertexId);
    }
  }

  size_t vertexEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    if (BaseClass::isVertexModified(*globalState, vertexId)) {
      for (const auto &patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
        BaseClass::removeMatch(globalState, vertexId, patternIndex);
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
      BaseClass::clearAlreadyMatched(globalState, vertexId);
    } else {
      // Schedule vertex close to the one modified
      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        if (!BaseClass::isVertexModified(*globalState, neighborVertexId)) continue;

        BaseClass::scheduleVertex(globalState, vertexId);
      }
    }
  }
  ++pathConstraintIterator;

  globalState->graphActiveVertexCount-=vertexEliminatedNumber;
  return vertexEliminatedNumber;
}

template<class State>
void MultipleLabelPcCpu<State>::setConstraintVector(
    const std::vector <MultipleLabelConstraintPath>& circularConstraintVector_) {
  pathConstraintVector=circularConstraintVector_;
  pathConstraintIterator=pathConstraintVector.cbegin();
}

template<class State>
void MultipleLabelPcCpu<State>::setConstraintIterator(const size_t &index) {
  pathConstraintIterator = pathConstraintVector.cbegin()+index;
}

}