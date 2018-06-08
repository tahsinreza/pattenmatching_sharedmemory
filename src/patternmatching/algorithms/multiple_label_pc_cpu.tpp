//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_pc_cpu.h"
#include <iostream>

namespace patternmatching {

template<class State>
void MultipleLabelPcCpu<State>::init(const graph_t &graph, const graph_t &pattern) {
  sourceTraversalVector.resize(graph.vertex_count);
  patternTraversalVector.resize(pattern.vertex_count);
}

template<class State>
bool MultipleLabelPcCpu<State>::isInConstraintVector(const PathConstraint &constraint) const {
  for (const auto &it : pathConstraintVector) {
    if (it == constraint) return true;
  }
  return false;
}

template<class State>
__host__ void MultipleLabelPcCpu<State>::buildConstraintList(
    const graph_t &pattern,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentLength,
    std::vector <vid_t> *historyVertexId,
    std::vector <weight_t> *historyVertexLabel,
    std::vector <PathConstraint> *constraintVector) {

  // Close the loop
  if (currentLength >= 2 ) {
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
MultipleLabelPcCpu<State>::preprocessPatern(const graph_t &pattern) {
  // for loop
  std::vector <vid_t> historyVertexId;
  std::vector <weight_t> historyVertexLabel;

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
__host__ void MultipleLabelPcCpu<State>::printPathConstraint(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << pathConstraintVector.size() << ". " << std::endl;
  for (const auto &it : pathConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    ++currentConstraint;
  }
}

template<class State>
bool MultipleLabelPcCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const PathConstraint &currentConstraint,
    const bool &reverse,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &remainingLength) {


  if(sourceVertexId==14) {DEBUG_PRINT(reverse)}
  if(sourceVertexId==14) {DEBUG_PRINT(remainingLength)}
  if(sourceVertexId==14) {DEBUG_PRINT(currentVertexId)}

  size_t nextPositionInConstraint = reverse ? remainingLength - 1:
                                    (currentConstraint.length - remainingLength);

  if(sourceVertexId==14) {DEBUG_PRINT(nextPositionInConstraint)}

  // Verify that we closed the cycle.
  if (remainingLength == 1) {
    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if(sourceVertexId==14) {DEBUG_PRINT(neighborVertexId)}
      if(sourceVertexId==14) {DEBUG_PRINT(currentConstraint.vertexIndexVector[nextPositionInConstraint])}
      if(sourceVertexId==14) {DEBUG_PRINT_SET(globalState->vertexPatternMatch[neighborVertexId])}

      if (globalState->vertexPatternMatch[neighborVertexId].find(currentConstraint.vertexIndexVector[nextPositionInConstraint])
          == globalState->vertexPatternMatch[neighborVertexId].end())
        continue;
      if(sourceVertexId==14) {DEBUG_PRINT(neighborVertexId)}

      if (sourceTraversalVector[sourceVertexId].find(neighborVertexId)
          != sourceTraversalVector[sourceVertexId].end()
          && sourceTraversalVector[sourceVertexId][neighborVertexId] >= remainingLength)
        continue;
      if(sourceVertexId==14) {DEBUG_PRINT(neighborVertexId)}

      if (sourceVertexId != neighborVertexId) {
        if(sourceVertexId==14) {DEBUG_PRINT(sourceVertexId)}
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

      if (globalState->vertexPatternMatch[neighborVertexId].find(currentConstraint.vertexIndexVector[nextPositionInConstraint])
          == globalState->vertexPatternMatch[neighborVertexId].end())
        continue;

      //weight_t neighborLabel = graph.values[neighborVertexId];
      //if (neighborLabel != currentConstraint.vertexLabelVector[nextPositionInConstraint]) continue;

      if (sourceTraversalVector[sourceVertexId].find(neighborVertexId)
          != sourceTraversalVector[sourceVertexId].end()
          && sourceTraversalVector[sourceVertexId][neighborVertexId] >= remainingLength)
        continue;

      sourceTraversalVector[sourceVertexId][neighborVertexId] = remainingLength;
      if (checkConstraint(graph, globalState, currentConstraint, reverse, sourceVertexId,
                          neighborVertexId, remainingLength - 1)) {
        return true;
      }
      sourceTraversalVector[sourceVertexId].erase(neighborVertexId);
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelPcCpu<State>::resetState(State *globalState) {
  globalState->resetPatternMatchPc();
  for (auto &it : sourceTraversalVector) {
    it.clear();
  }
}

template<class State>
__host__ size_t
MultipleLabelPcCpu<State>::compute(const graph_t &graph, State *globalState) {
  //resetState(globalState);

  const auto &currentConstraint = *pathConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &PathConstraint::print,
                            Logger::E_OUTPUT_FILE_LOG);

  //#pragma omp parallel for
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;
    weight_t vertexLabel = graph.values[vertexId];
    if (vertexLabel != currentConstraint.initialLabel) continue;

    // Check first index
    size_t constraintPatternIndex=currentConstraint.vertexIndexVector[0];
    if(globalState->vertexPatternMatch[vertexId].find(constraintPatternIndex) != globalState->vertexPatternMatch[vertexId].end()) {
      // Check cycle
      sourceTraversalVector[vertexId].clear();
      sourceTraversalVector[vertexId][vertexId]=currentConstraint.length;
      if (!checkConstraint(graph,
                           globalState,
                           currentConstraint,
                           false,
                           vertexId,
                           vertexId,
                           currentConstraint.length-1)) {
        removeMatch(globalState, vertexId, constraintPatternIndex);
      }
    }

    // Check last index
    constraintPatternIndex=currentConstraint.vertexIndexVector[currentConstraint.length-1];
    if(globalState->vertexPatternMatch[vertexId].find(constraintPatternIndex) != globalState->vertexPatternMatch[vertexId].end()) {
      // Check cycle
      sourceTraversalVector[vertexId].clear();
      sourceTraversalVector[vertexId][vertexId]=currentConstraint.length;
      if (!checkConstraint(graph,
                           globalState,
                           currentConstraint,
                           true,
                           vertexId,
                           vertexId,
                           currentConstraint.length-1)) {
        removeMatch(globalState, vertexId, constraintPatternIndex);
      }
    }
  }

  size_t vertexEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;
    weight_t vertexLabel = graph.values[vertexId];
    if (vertexLabel != currentConstraint.initialLabel) continue;

    for (const auto &patternIndex : globalState->vertexPatternToUnmatchPc[vertexId]) {
      BaseClass::removeMatch(globalState, vertexId, patternIndex);
    }

    if (!BaseClass::isMatch(*globalState, vertexId)) {
      std::stringstream ss;
      ss << "Removed Vertex : " << vertexId << std::endl;
      Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());
      BaseClass::deactivateVertex(globalState, vertexId);
      vertexEliminatedNumber += 1;
    }
  }
  ++pathConstraintIterator;

  return vertexEliminatedNumber;
}

template<class State>
int MultipleLabelPcCpu<State>::getPathConstraintNumber() const {
  return pathConstraintVector.size();
}

template<class State>
void MultipleLabelPcCpu<State>::removeMatch(State *globalState,
                                            const vid_t vertexId,
                                            const pvid_t patternVertexId) const {
  globalState->vertexPatternToUnmatchPc[vertexId].insert(patternVertexId);
}

}