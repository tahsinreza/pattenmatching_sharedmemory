//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_tds_cpu.h"
#include "multiple_label_constraint_template.h"
#include <iostream>
#include <deque>
#include "utils.h"

namespace patternmatching {

template<class State>
bool MultipleLabelTdsCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const Walk &walk,
    SourceTraversalMapType &sourceTraversalMap,
    SourceTraversalEdgeMapType &sourceTraversalEdgeMap,
    std::vector<vid_t> &historyIndexVector,
    TraversalHypothesis &traversalHypothesis,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentPositionInConstraint) {

  // Close the recursion
  if (currentPositionInConstraint == walk.length) return true;

  auto currentWalkMove = walk.moveVector[currentPositionInConstraint];

  if (currentWalkMove == Walk::E_VERTEX_NO_STORE || currentWalkMove == Walk::E_VERTEX_STORE) {
    auto currentWalkVertexIndex = walk.vertexIndexVector[currentPositionInConstraint-1];
    auto nextWalkVertexIndex = walk.vertexIndexVector[currentPositionInConstraint];
    for (eid_t neighborEdgeId = graph.vertices[currentVertexId]; neighborEdgeId < graph.vertices[currentVertexId + 1];
         neighborEdgeId++) {
      if (!BaseClass::isEdgeActive(*globalState, neighborEdgeId)) continue;
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if (!BaseClass::isVertexActive(*globalState, neighborVertexId)) continue;

      if (!globalState->vertexPatternMatch[neighborVertexId].isIn(nextWalkVertexIndex))
        continue;

      //if (isInVector(historyIndexVector, neighborVertexId)) continue;

      const auto &edge = (currentVertexId>neighborVertexId) ? std::make_pair(currentVertexId,neighborVertexId)
                                                            : std::make_pair(neighborVertexId,currentVertexId);

      if (sourceTraversalEdgeMap.find(edge)!=sourceTraversalEdgeMap.end()
          && sourceTraversalEdgeMap[edge].isIn(currentWalkVertexIndex))  {
        continue;
      }

      if (sourceTraversalMap.find(neighborVertexId) != sourceTraversalMap.end()
          && sourceTraversalMap[neighborVertexId].find(nextWalkVertexIndex) != sourceTraversalMap[neighborVertexId].end()
          && isInVector(sourceTraversalMap[neighborVertexId][nextWalkVertexIndex], traversalHypothesis))
        continue;

      if(currentWalkMove == Walk::E_VERTEX_STORE) {
        traversalHypothesis.push_back(neighborVertexId);
      }

      sourceTraversalMap[neighborVertexId][nextWalkVertexIndex].push_back(traversalHypothesis);
      historyIndexVector.push_back(neighborVertexId);
      sourceTraversalEdgeMap[edge].insert(nextWalkVertexIndex);
      if (checkConstraint(graph,
                          globalState,
                          walk,
                          sourceTraversalMap,
                          sourceTraversalEdgeMap,
                          historyIndexVector,
                          traversalHypothesis,
                          sourceVertexId,
                          neighborVertexId,
                          currentPositionInConstraint + 1)) {
        BaseClass::makeAlreadyMatchedAtomic(globalState, neighborVertexId, nextWalkVertexIndex);
        return true;
      }
      historyIndexVector.pop_back();

      if(currentWalkMove == Walk::E_VERTEX_STORE) {
        traversalHypothesis.pop_back();
      }
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
                          sourceTraversalMap,
                          sourceTraversalEdgeMap,
                          historyIndexVector,
                          traversalHypothesis,
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
                        sourceTraversalMap,
                        sourceTraversalEdgeMap,
                        historyIndexVector,
                        traversalHypothesis,
                        sourceVertexId,
                        nextVertexId,
                        currentPositionInConstraint + 1)) {
      return true;
    }
  }
  return false;
}

template<class State>
__host__ void MultipleLabelTdsCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ size_t
MultipleLabelTdsCpu<State>::compute(const graph_t &graph, State *globalState) {
  const auto &currentConstraint = *templateConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &MultipleLabelConstraintTemplate::print,
                            Logger::E_OUTPUT_FILE_LOG);

  PROGRESSION_INSERT_BEGIN()

  SourceTraversalMapType sourceTraversalMap;
  SourceTraversalEdgeMapType sourceTraversalEdgeMap;
  std::vector<vid_t> historyIndexVector;
  TraversalHypothesis traversalHypothesis;

  #pragma omp parallel for private(sourceTraversalMap, sourceTraversalEdgeMap, historyIndexVector, traversalHypothesis)
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
          sourceTraversalMap.clear();
          sourceTraversalEdgeMap.clear();
          historyIndexVector.clear();
          historyIndexVector.push_back(vertexId);
          traversalHypothesis.clear();
          if (!checkConstraint(graph,
                               globalState,
                               (*it).second,
                               sourceTraversalMap,
                               sourceTraversalEdgeMap,
                               historyIndexVector,
                               traversalHypothesis,
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
  };

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

  globalState->graphActiveVertexCount-=vertexEliminatedNumber;
  return vertexEliminatedNumber;
}

template<class State>
void MultipleLabelTdsCpu<State>::setConstraintVector(
    const std::vector <MultipleLabelConstraintTemplate>& templateConstraintVector_) {
  templateConstraintVector=templateConstraintVector_;
  templateConstraintIterator=templateConstraintVector.cbegin();
}

template<class State>
void MultipleLabelTdsCpu<State>::setConstraintIterator(const size_t &index) {
  templateConstraintIterator = templateConstraintVector.cbegin()+index;
}

}