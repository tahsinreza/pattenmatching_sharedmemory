//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_enumeration_cpu.h"
#include "multiple_label_constraint_template.h"
#include <iostream>
#include <deque>
#include "utils.h"
#include <iostream>
#include <fstream>

namespace patternmatching {


template<class State>
void MultipleLabelEnumerationCpu<State>::writeEnumeration(
    const Walk &walk,
    const std::vector<vid_t> &historyIndexVector) const {
  auto fstream = Logger::get().getStream(Logger::E_OUTPUT_FILE_ENUMERATION_RESUTLS, omp_get_thread_num());
  for(int i=0;i<historyIndexVector.size();i++) {
    fstream << "("<<walk.historyIndexVector[i]<<","<<historyIndexVector[i]<<");";
  }
  fstream<< std::endl;
  fstream.close();
}

template<class State>
size_t MultipleLabelEnumerationCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const Walk &walk,
    std::vector<vid_t> &historyIndexVector,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentPositionInConstraint) {

  size_t enumerationNumber = 0;

  // Close the recursion
  if (currentPositionInConstraint == walk.length) {
    writeEnumeration(walk, historyIndexVector);
    return 1;
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
      enumerationNumber += checkConstraint(graph,
                                           globalState,
                                           walk,
                                           historyIndexVector,
                                           sourceVertexId,
                                           neighborVertexId,
                                           currentPositionInConstraint + 1);
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

      return checkConstraint(graph,
                             globalState,
                             walk,
                             historyIndexVector,
                             sourceVertexId,
                             currentVertexId,
                             currentPositionInConstraint + 1);

    }
  } else if (currentWalkMove == Walk::E_MOVE_BACK) {
    auto nextWalkMoveBackIndex = walk.moveBackIndexVector[currentPositionInConstraint];
    auto nextVertex = historyIndexVector[nextWalkMoveBackIndex];

    return checkConstraint(graph,
                           globalState,
                           walk,
                           historyIndexVector,
                           sourceVertexId,
                           nextVertex,
                           currentPositionInConstraint + 1);
  }

  return enumerationNumber;
}

template<class State>
__host__ void MultipleLabelEnumerationCpu<State>::resetState(State *globalState) {
}

template<class State>
__host__ AlgoResults
MultipleLabelEnumerationCpu<State>::compute(const graph_t &graph, State *globalState) {
  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            templateConstraint,
                            &MultipleLabelConstraintTemplate::print,
                            Logger::E_OUTPUT_FILE_LOG);

  PROGRESSION_INSERT_BEGIN()

  const auto &walkKey = templateConstraint.walkMap.cbegin();
  const auto &walkStartingVertex = walkKey->first;
  const auto &walk = walkKey->second;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "selectedWalk : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            walk,
                            &Walk::print,
                            Logger::E_OUTPUT_FILE_LOG);

  size_t enumerationNumber = 0;
  #pragma omp parallel
  {
    std::vector<vid_t> historyIndexVector;
    historyIndexVector.reserve(walk.length);

    #pragma omp for private(historyIndexVector) reduction(+:enumerationNumber)
    for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
      if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

      if (globalState->vertexPatternMatch[vertexId].isIn(walkStartingVertex)) {
        // Check cycle
        historyIndexVector.clear();
        historyIndexVector.push_back(vertexId);
        enumerationNumber += checkConstraint(graph,
                                             globalState,
                                             walk,
                                             historyIndexVector,
                                             vertexId,
                                             vertexId,
                                             1);

      }

      PROGRESSION_INSERT_LOOP()
    }
  }

  AlgoResults algoResults;
  algoResults.enumeration = enumerationNumber;

  return algoResults;
}

template<class State>
void MultipleLabelEnumerationCpu<State>::setConstraint(
    const MultipleLabelConstraintTemplate &templateConstraint_) {
  templateConstraint = templateConstraint_;
}
}