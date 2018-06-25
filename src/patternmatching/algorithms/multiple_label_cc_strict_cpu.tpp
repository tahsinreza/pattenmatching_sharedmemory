//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_cc_strict_cpu.h"
#include <iostream>
#include "utils.h"

namespace patternmatching {

template<class State>
void MultipleLabelCcStrictCpu<State>::init(const graph_t &graph, const graph_t &pattern) {
  patternTraversalVector.resize(pattern.vertex_count);
}

template<class State>
bool MultipleLabelCcStrictCpu<State>::isInConstraintVector(const MultipleLabelCircularConstraint &constraint) const {
  for (const auto &it : circularConstraintVector) {
    if (it == constraint) return true;
  }
  return false;
}

template<class State>
__host__ void MultipleLabelCcStrictCpu<State>::buildConstraintList(
    const graph_t &pattern,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &remainingLength,
    std::vector<vid_t> *historyVertexId,
    std::vector<weight_t> *historyVertexLabel,
    std::vector<MultipleLabelCircularConstraint> *constraintVector) {

  // Close the loop
  if (remainingLength == 0) {
    if (sourceVertexId != currentVertexId) return;

    MultipleLabelCircularConstraint constraint = MultipleLabelCircularConstraint(*historyVertexId, *historyVertexLabel);

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
MultipleLabelCcStrictCpu<State>::preprocessPatern(const graph_t &pattern) {
  // for loop
  std::vector<vid_t> historyVertexId;
  std::vector<weight_t> historyVertexLabel;

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
__host__ void MultipleLabelCcStrictCpu<State>::printCircularConstraint(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << circularConstraintVector.size() << ". " << std::endl;
  for (const auto &it : circularConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    ++currentConstraint;
  }
}

template<class State>
bool MultipleLabelCcStrictCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const MultipleLabelCircularConstraint &currentConstraint,
    std::vector<vid_t> &historyIndexVector,
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
    size_t nextPositionInConstraint = (startingPosition + currentConstraint.length - (remainingLength - 1))
        % currentConstraint.vertexLabelVector.size();

    const auto &nextConstraintVertexIndex = currentConstraint.vertexIndexVector[nextPositionInConstraint];

    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      const vid_t &neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if (!globalState->vertexPatternMatch[neighborVertexId].isIn(nextConstraintVertexIndex))
        continue;

      if (isInVector(historyIndexVector, neighborVertexId)) {
        continue;
      }
      historyIndexVector.push_back(neighborVertexId);
      if (checkConstraint(graph, globalState, currentConstraint, historyIndexVector, sourceVertexId,
                          neighborVertexId, startingPosition, remainingLength - 1)) {
        BaseClass::makeAlreadyMatchedAtomic(globalState, neighborVertexId, nextConstraintVertexIndex);
        return true;
      }
      historyIndexVector.pop_back();
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelCcStrictCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ size_t
MultipleLabelCcStrictCpu<State>::compute(
    const graph_t &graph, State
*globalState) {
  //resetState(globalState);
  //std::cout << "Start CC " << std::endl;

  const auto &currentConstraint = *circularConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &MultipleLabelCircularConstraint::print,
                            Logger::E_OUTPUT_FILE_LOG);

  std::vector<vid_t> historyIndexVector;

  #pragma omp parallel for private(historyIndexVector)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    size_t startingPositionInConstraint = 0;
    size_t constraintIndex = 0;
    bool hasBeenModified = false;

    for (auto it = currentConstraint.vertexIndexVector.cbegin();
         it != currentConstraint.vertexIndexVector.cend();
         ++it, ++constraintIndex) {

      if (globalState->vertexPatternMatch[vertexId].isIn(*it)) {
        if (BaseClass::isAlreadyMatchedAtomic(*globalState, vertexId, *it)) {
          hasBeenModified = true;
          continue;
        }

        startingPositionInConstraint = constraintIndex;
        // Check cycle
        historyIndexVector.clear();
        historyIndexVector.push_back(vertexId);

        if (!checkConstraint(graph,
                             globalState,
                             currentConstraint,
                             historyIndexVector,
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
  }


  //std::cout << "Mid CC " << std::endl;

  size_t vertexEliminatedNumber = 0;
  size_t matchEliminatedNumber = 0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber, matchEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

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
      BaseClass::clearAlreadyMatched(globalState, vertexId);
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

  globalState->graphActiveVertexCount-=vertexEliminatedNumber;
  ++circularConstraintIterator;

  std::cout << "Match eliminated : " << matchEliminatedNumber << std::endl;
  //std::cout << "End CC " << std::endl;
  return vertexEliminatedNumber;
}

template<class State>
int MultipleLabelCcStrictCpu<State>::getCircularConstraintNumber() const {
  return circularConstraintVector.size();
}

template<class State>
const std::vector<MultipleLabelCircularConstraint> &MultipleLabelCcStrictCpu<State>::getCircularConstraintVector() const {
  return circularConstraintVector;
}

template<class State>
void MultipleLabelCcStrictCpu<State>::setCircularConstraintIterator(
    const typename std::vector<MultipleLabelCircularConstraint>::const_iterator &it) {
  circularConstraintIterator = it;
}
}