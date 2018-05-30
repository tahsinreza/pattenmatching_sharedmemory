//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_engine.cuh"
#include "totem_util.h"
#include "totem_unique_label_cc_cpu.cuh"
#include <iostream>

namespace patternmatching {

template<class State>
void UniqueLabelCcCpu<State>::init(const graph_t &graph, const graph_t &pattern) {
  sourceTraversalVector.resize(graph.vertex_count);
  patternTraversalVector.resize(pattern.vertex_count);
}

template<class State>
bool UniqueLabelCcCpu<State>::isInConstraintVector(const UniqueLabelCcCpu<State>::CircularConstraint &constraint) const {
  for (const auto &it : circularConstraintVector) {
    if (it == constraint) return true;
  }
  return false;
}

template<class State>
__host__ void UniqueLabelCcCpu<State>::buildConstraintList(
    const graph_t &pattern,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &remainingLength,
    std::vector <vid_t> *historyVertexId,
    std::vector <weight_t> *historyVertexLabel,
    std::vector <UniqueLabelCcCpu<State>::CircularConstraint> *constraintVector) {

  // Close the loop
  if (remainingLength == 0) {
    if (sourceVertexId != currentVertexId) return;

    CircularConstraint constraint = CircularConstraint(*historyVertexId, *historyVertexLabel);

    if (!isInConstraintVector(constraint)) constraintVector->push_back(constraint);

    return;
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
                        neighborVertexId, remainingLength - 1,
                        historyVertexId, historyVertexLabel, constraintVector);

    patternTraversalVector[sourceVertexId].erase(neighborVertexId);
    historyVertexId->pop_back();
    historyVertexLabel->pop_back();

  }

}

template<class State>
__host__ error_t
UniqueLabelCcCpu<State>::preprocessPatern(const graph_t &pattern) {
  // for loop
  std::vector <vid_t> historyVertexId;
  std::vector <weight_t> historyVertexLabel;

  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];

    historyVertexId.push_back(vertexId);
    historyVertexLabel.push_back(currentLabel);

    for (size_t currentCycleLength = 3; currentCycleLength <= pattern.vertex_count; currentCycleLength++) {
      patternTraversalVector[vertexId].clear();
      buildConstraintList(pattern,
                          vertexId,
                          vertexId,
                          currentCycleLength,
                          &historyVertexId,
                          &historyVertexLabel,
                          &circularConstraintVector);
    }

    historyVertexId.pop_back();
    historyVertexLabel.pop_back();
  }

  circularConstraintIterator = circularConstraintVector.begin();

  return SUCCESS;
}

template<class State>
__host__ void UniqueLabelCcCpu<State>::printCircularConstraint(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << circularConstraintVector.size() << ". " << std::endl;
  for (const auto &it : circularConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    ++currentConstraint;
  }
}

template<class State>
bool UniqueLabelCcCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const UniqueLabelCcCpu<State>::CircularConstraint &currentConstraint,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &startingPosition,
    const size_t &remainingLength) {

  // Verify that we closed the cycle.
  if (remainingLength == 0) {
    return sourceVertexId == currentVertexId;
  }

  // Find neighbor
  size_t nextPositionInConstraint = (startingPosition + currentConstraint.length - (remainingLength - 1))
      % currentConstraint.vertexLabelVector.size();

  for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
       neighborEdgeId++) {
    vid_t neighborVertexId = graph.edges[neighborEdgeId];

    if (!BaseClass::isActive(*globalState, neighborVertexId)) continue;

    weight_t neighborLabel = graph.values[neighborVertexId];
    if (neighborLabel != currentConstraint.vertexLabelVector[nextPositionInConstraint]) continue;

    if (sourceTraversalVector[sourceVertexId].find(neighborVertexId)
        != sourceTraversalVector[sourceVertexId].end()
        && sourceTraversalVector[sourceVertexId][neighborVertexId] >= remainingLength)
      continue;

    sourceTraversalVector[sourceVertexId][neighborVertexId] = remainingLength;
    if (checkConstraint(graph, globalState, currentConstraint, sourceVertexId,
                        neighborVertexId, startingPosition, remainingLength - 1)) {
      makeMatchAtomic(globalState, currentVertexId);
      return true;
    }

  }
  return false;
}

template<class State>
__host__ void UniqueLabelCcCpu<State>::resetState(State *globalState) {
  globalState->resetPatternMatchCc();
  for (auto &it : sourceTraversalVector) {
    it.clear();
  }
}

template<class State>
__host__ size_t
UniqueLabelCcCpu<State>::compute(const graph_t &graph, State *globalState) {

  resetState(globalState);

  const auto& currentConstraint = *circularConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &CircularConstraint::print,
                            Logger::E_OUTPUT_FILE_LOG);

  #pragma omp parallel for
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isActive(*globalState, vertexId)) continue;
    if (isMatchAtomic(*globalState, vertexId)) continue;

    // Find current label in current constraint
    weight_t currentLabel = graph.values[vertexId];

    bool constraintFound = false;
    size_t startingPositionInConstraint = 0;
    size_t constraintIndex = 0;
    for (auto it = currentConstraint.vertexLabelVector.cbegin();
         it != currentConstraint.vertexLabelVector.cend();
         ++it, ++constraintIndex) {
      if (*it == currentLabel) {
        constraintFound = true;
        startingPositionInConstraint = constraintIndex;
        break;
      }
    }
    if (!constraintFound) {
      makeOmitted(globalState, vertexId);
      continue;
    }

    // Check cycle
    checkConstraint(graph,
                    globalState,
                    currentConstraint,
                    vertexId,
                    vertexId,
                    startingPositionInConstraint,
                    currentConstraint.length);

  }

  ++circularConstraintIterator;

  size_t vertexEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isActive(*globalState, vertexId)) continue;
    if (isOmitted(*globalState, vertexId)) continue;

    if (!isMatch(*globalState, vertexId)) {
      std::stringstream ss;
      ss << "Removed Vertex : " << vertexId << std::endl;
      Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());
      BaseClass::deactivate(globalState, vertexId);
      vertexEliminatedNumber += 1;
    }
  }

  return vertexEliminatedNumber;
}

template<class State>
int UniqueLabelCcCpu<State>::getCircularConstraintNumber() const {
  return circularConstraintVector.size();
}

template<class State>
bool UniqueLabelCcCpu<State>::isOmitted(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexPatternOmittedCc[vertexId] == true;
}

template<class State>
void UniqueLabelCcCpu<State>::makeOmitted(State *globalState, const vid_t vertexId) const {
  globalState->vertexPatternOmittedCc[vertexId] = true;
}

template<class State>
bool UniqueLabelCcCpu<State>::isMatch(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexPatternMatchCc[vertexId] == true;
}

template<class State>
bool UniqueLabelCcCpu<State>::isMatchAtomic(const State &globalState, const vid_t vertexId) const {
  auto address = &(globalState.vertexPatternMatchCc[vertexId]);
  uint8_t value;
  #pragma omp atomic read
  value = *address;
  return value == true;
}

template<class State>
void UniqueLabelCcCpu<State>::makeMatch(State *globalState, const vid_t vertexId) const {
  globalState->vertexPatternMatchCc[vertexId] = true;
}
template<class State>
void UniqueLabelCcCpu<State>::makeMatchAtomic(State *globalState, const vid_t vertexId) const {
  auto address = &(globalState->vertexPatternMatchCc[vertexId]);
  #pragma omp atomic write
  *address = true;
}

template<class State>
UniqueLabelCcCpu<State>::CircularConstraint::CircularConstraint(
    const std::vector <vid_t> &historyVertexId,
    const std::vector <weight_t> &historyVertexLabel) {
  auto vertexIndexVectorCopy = historyVertexId;
  auto vertexLabelVectorCopy = historyVertexLabel;

  // remove the last vertex which is the same as the first one
  vertexIndexVectorCopy.pop_back();
  vertexLabelVectorCopy.pop_back();
  length = vertexIndexVectorCopy.size();

  // find minimum
  size_t minValue = vertexIndexVectorCopy[0];
  size_t minIndex = 0;
  size_t currentValue;
  for (int currentIndex = 1; currentIndex < length; currentIndex++) {
    currentValue = vertexIndexVectorCopy[currentIndex];
    if (currentValue < minValue) {
      minValue = currentValue;
      minIndex = currentIndex;
    }
  }

  // Create a full order
  bool reverse = false;
  if (minIndex > 0 && minIndex < length - 1) {
    reverse = vertexIndexVectorCopy[minIndex - 1] < vertexIndexVectorCopy[minIndex + 1];
  } else if (minIndex == 0) {
    reverse = vertexIndexVectorCopy[length - 1] < vertexIndexVectorCopy[1];
  } else {
    reverse = vertexIndexVectorCopy[length - 2] < vertexIndexVectorCopy[0];
  }

  // Fill in
  vertexIndexVector.resize(length);
  vertexLabelVector.resize(length);

  int copyPosition = minIndex;
  for (int currentPosition = 0; currentPosition < length; currentPosition++) {
    vertexIndexVector[currentPosition] = vertexIndexVectorCopy[copyPosition];
    vertexLabelVector[currentPosition] = vertexLabelVectorCopy[copyPosition];

    if (reverse) { --copyPosition; } else { ++copyPosition; }
    if (copyPosition == length) copyPosition = 0;
    if (copyPosition == -1) copyPosition = length - 1;
  }

}

template<class State>
bool UniqueLabelCcCpu<State>::CircularConstraint::operator==(const UniqueLabelCcCpu<State>::CircularConstraint &other) const {
  return (length == other.length) && (vertexIndexVector == other.vertexIndexVector);
}

template<class State>
void UniqueLabelCcCpu<State>::CircularConstraint::print(std::ostream &ostream) const {
  ostream << "Vertex Index : ";
  for (const auto &it : vertexIndexVector) {
    ostream << it << " -> ";
  }
  ostream << vertexIndexVector[0] << std::endl;

  ostream << "Vertex Label : ";
  for (const auto &it : vertexLabelVector) {
    ostream << it << " -> ";
  }
  ostream << vertexLabelVector[0] << std::endl;
}
}