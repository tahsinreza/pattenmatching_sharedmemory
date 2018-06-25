//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_pc_backtrack_cpu.h"
#include <iostream>
#include "utils.h"

namespace patternmatching {

template<class State>
void MultipleLabelPcBacktrackCpu<State>::init(const graph_t &graph, const graph_t &pattern) {
  patternTraversalVector.resize(pattern.vertex_count);
}

template<class State>
bool MultipleLabelPcBacktrackCpu<State>::isInConstraintVector(const PathConstraint &constraint) const {
  for (const auto &it : pathConstraintVector) {
    if (it == constraint) return true;
  }
  return false;
}

template<class State>
__host__ void MultipleLabelPcBacktrackCpu<State>::buildConstraintList(
    const graph_t &pattern,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentLength,
    std::vector<vid_t> *historyVertexId,
    std::vector<weight_t> *historyVertexLabel,
    std::vector<PathConstraint> *constraintVector) {

  // Close the loop
  if (currentLength >= 2) {
    weight_t sourceLabel = pattern.values[sourceVertexId];
    weight_t currentLabel = pattern.values[currentVertexId];
    if (sourceLabel == currentLabel && sourceVertexId != currentVertexId) {

      PathConstraint constraint = PathConstraint(*historyVertexId, *historyVertexLabel);

      if (!isInConstraintVector(constraint)) constraintVector->push_back(constraint);

      return;
    }
  }

  for (eid_t neighborEdgeId = pattern.vertices[currentVertexId]; neighborEdgeId < pattern.vertices[currentVertexId + 1];
       neighborEdgeId++) {
    vid_t neighborVertexId = pattern.edges[neighborEdgeId];
    weight_t neighborLabel = pattern.values[neighborVertexId];

    if (patternTraversalVector[sourceVertexId].find(neighborVertexId)
        != patternTraversalVector[sourceVertexId].end())
      continue;

    historyVertexId->push_back(neighborVertexId);
    historyVertexLabel->push_back(neighborLabel);
    patternTraversalVector[sourceVertexId].insert(neighborVertexId);

    buildConstraintList(pattern, sourceVertexId,
                        neighborVertexId, currentLength + 1,
                        historyVertexId, historyVertexLabel, constraintVector);

    patternTraversalVector[sourceVertexId].erase(neighborVertexId);
    historyVertexId->pop_back();
    historyVertexLabel->pop_back();

  }

}

template<class State>
__host__ error_t
MultipleLabelPcBacktrackCpu<State>::preprocessPatern(const graph_t &pattern) {
  // for loop
  std::vector<vid_t> historyVertexId;
  std::vector<weight_t> historyVertexLabel;

  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];

    historyVertexId.push_back(vertexId);
    historyVertexLabel.push_back(currentLabel);

    patternTraversalVector[vertexId].clear();
    patternTraversalVector[vertexId].insert(vertexId);
    buildConstraintList(pattern,
                        vertexId,
                        vertexId,
                        0,
                        &historyVertexId,
                        &historyVertexLabel,
                        &pathConstraintVector);

    historyVertexId.pop_back();
    historyVertexLabel.pop_back();
  }

  pathConstraintIterator = pathConstraintVector.begin();

  return SUCCESS;
}

template<class State>
__host__ void MultipleLabelPcBacktrackCpu<State>::printPathConstraint(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << pathConstraintVector.size() << ". " << std::endl;
  for (const auto &it : pathConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    ++currentConstraint;
  }
}

template<class State>
bool MultipleLabelPcBacktrackCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const PathConstraint &currentConstraint,
    const bool &reverse,
    std::unordered_map<vid_t, FixedBitmapType> &sourceTraversalMap,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &remainingLength) {

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

      if (sourceTraversalMap.find(neighborVertexId) != sourceTraversalMap.end()
          && sourceTraversalMap[neighborVertexId].isIn(nextConstraintVertexIndex))
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

      if (sourceTraversalMap.find(neighborVertexId) != sourceTraversalMap.end()
          && sourceTraversalMap[neighborVertexId].isIn(nextConstraintVertexIndex))
        continue;

      sourceTraversalMap[neighborVertexId].insert(nextConstraintVertexIndex);
      if (checkConstraint(graph, globalState, currentConstraint, reverse, sourceTraversalMap, sourceVertexId,
                          neighborVertexId, remainingLength - 1)) {
        return true;
      }
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelPcBacktrackCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ size_t
MultipleLabelPcBacktrackCpu<State>::compute(const graph_t &graph, State *globalState) {
  //resetState(globalState);

  const auto &currentConstraint = *pathConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &PathConstraint::print,
                            Logger::E_OUTPUT_FILE_LOG);

  std::unordered_map<vid_t, FixedBitmapType> sourceTraversalMap;

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
int MultipleLabelPcBacktrackCpu<State>::getPathConstraintNumber() const {
  return pathConstraintVector.size();
}

template<class State>
const std::vector<PathConstraint> &MultipleLabelPcBacktrackCpu<State>::getPathConstraintVector() const {
  return pathConstraintVector;
}

}