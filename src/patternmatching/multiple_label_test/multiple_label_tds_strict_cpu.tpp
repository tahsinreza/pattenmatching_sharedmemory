//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_tds_strict_cpu.h"
#include "multiple_label_constraint_template.h"
#include <iostream>
#include <deque>
#include "common_utils.h"

namespace patternmatching {

template<class State>
bool MultipleLabelTdsStrictCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const Walk &walk,
    std::vector<vid_t> &historyIndexVector,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentPositionInConstraint) {

  // Close the recursion
  if (currentPositionInConstraint == walk.length) {
    return true;
  }

  auto currentWalkMove = walk.moveVector[currentPositionInConstraint];

  if (currentWalkMove == Walk::E_VERTEX_NO_STORE || currentWalkMove == Walk::E_VERTEX_STORE) {
    auto nextWalkVertexIndex = walk.vertexIndexVector[currentPositionInConstraint];

    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if (!globalState->vertexPatternMatch[neighborVertexId].isIn(nextWalkVertexIndex))
        continue;

      if (isInVector(historyIndexVector, neighborVertexId)) continue;

      historyIndexVector.push_back(neighborVertexId);
      if (checkConstraint(graph,
                          globalState,
                          walk,
                          historyIndexVector,
                          sourceVertexId,
                          neighborVertexId,
                          currentPositionInConstraint + 1)) {
        BaseClass::makeAlreadyMatchedAtomic(globalState, neighborVertexId, nextWalkVertexIndex);
        return true;
      }
      historyIndexVector.pop_back();

    }
  } else if (currentWalkMove == Walk::E_CHECK) {
    auto nextWalkMoveBackIndex = walk.moveBackIndexVector[currentPositionInConstraint];
    auto checkVertexId = historyIndexVector[nextWalkMoveBackIndex];

    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if (neighborVertexId != checkVertexId) continue;

      if (checkConstraint(graph,
                          globalState,
                          walk,
                          historyIndexVector,
                          sourceVertexId,
                          currentVertexId,
                          currentPositionInConstraint + 1)) {
        return true;
      }

    }
  } else if (currentWalkMove == Walk::E_MOVE_BACK) {
    auto nextWalkMoveBackIndex = walk.moveBackIndexVector[currentPositionInConstraint];
    auto nextVertexId = historyIndexVector[nextWalkMoveBackIndex];

    if (checkConstraint(graph,
                        globalState,
                        walk,
                        historyIndexVector,
                        sourceVertexId,
                        nextVertexId,
                        currentPositionInConstraint + 1)) {
      return true;
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelTdsStrictCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ AlgoResults
MultipleLabelTdsStrictCpu<State>::compute(const graph_t &graph, State *globalState) {
  const auto &currentConstraint = *templateConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &MultipleLabelConstraintTemplate::print,
                            Logger::E_OUTPUT_FILE_LOG);

  PROGRESSION_INSERT_BEGIN()

  std::vector<vid_t> historyIndexVector;

  #pragma omp parallel for private(historyIndexVector)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    size_t constraintIndex = 0;
    bool hasBeenModified = false;

    for (auto it = currentConstraint.walkMap.cbegin();
         it != currentConstraint.walkMap.cend();
         ++it, ++constraintIndex) {

      if (globalState->vertexPatternMatch[vertexId].isIn((*it).first)) {
        if (!BaseClass::isAlreadyMatchedAtomic(*globalState, vertexId, (*it).first)) {

          // Check cycle
          historyIndexVector.clear();
          historyIndexVector.push_back(vertexId);
          if (!checkConstraint(graph,
                               globalState,
                               (*it).second,
                               historyIndexVector,
                               vertexId,
                               vertexId,
                               1)) {
            BaseClass::makeToUnmatch(globalState, vertexId, (*it).first);
            hasBeenModified = true;
          }
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

  #pragma omp parallel for reduction(+:vertexEliminatedNumber,matchEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    BaseClass::clearAlreadyMatched(globalState, vertexId);

    if (BaseClass::isVertexModified(*globalState, vertexId)) {
      for (const auto &patternIndex : globalState->vertexPatternToUnmatch[vertexId]) {
        BaseClass::removeMatch(globalState, vertexId, patternIndex);
        matchEliminatedNumber++;
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
      for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
           neighborEdgeId++) {
        if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
        vid_t neighborVertexId = graph.edges[neighborEdgeId];
        if (!BaseClass::isVertexModified(*globalState, neighborVertexId)) continue;

        BaseClass::scheduleVertex(globalState, vertexId);
      }
    }
  }
  templateConstraintIterator = templateConstraintVector.erase(templateConstraintIterator);

  globalState->graphActiveVertexCount-=vertexEliminatedNumber;
  AlgoResults algoResults;
  algoResults.vertexEliminated=vertexEliminatedNumber;
  algoResults.matchEliminated=matchEliminatedNumber;
  return algoResults;
}

template<class State>
void MultipleLabelTdsStrictCpu<State>::setConstraintVector(
    const std::vector <MultipleLabelConstraintTemplate>& templateConstraintVector_) {
  templateConstraintVector=templateConstraintVector_;
  templateConstraintIterator=templateConstraintVector.cbegin();
}

template<class State>
void MultipleLabelTdsStrictCpu<State>::setConstraintIterator(const size_t &index) {
  templateConstraintIterator = templateConstraintVector.cbegin()+index;
}
template<class State>
std::vector <MultipleLabelConstraintTemplate>* MultipleLabelTdsStrictCpu<State>::getConstraintVector() {
  return &templateConstraintVector;
}

}