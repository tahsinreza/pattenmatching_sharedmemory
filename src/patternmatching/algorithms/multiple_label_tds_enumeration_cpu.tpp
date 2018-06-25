//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_tds_enumeration_cpu.h"
#include "multiple_label_template_constraint.h"
#include <iostream>
#include <deque>
#include "utils.h"

namespace patternmatching {

template<class State>
void MultipleLabelTdsEnumerationCpu<State>::init(const graph_t &graph, const graph_t &pattern) {
}

template<class State>
__host__ error_t
MultipleLabelTdsEnumerationCpu<State>::preprocessPatern(
    const graph_t &pattern) {

  templateConstraint = TemplateConstraint(pattern);
  templateConstraint.generateWalkMap();

  return SUCCESS;
}

template<class State>
__host__ void MultipleLabelTdsEnumerationCpu<State>::printTemplateConstraint(std::ostream &ostream) const {
  ostream << "=== enumeration constraint ===" << std::endl;
  templateConstraint.print(ostream);
  templateConstraint.printWalk(ostream);
}

template<class State>
size_t MultipleLabelTdsEnumerationCpu<State>::checkConstraint(
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

    return checkConstraint(graph,
                           globalState,
                           walk,
                           historyIndexVector,
                           sourceVertexId,
                           historyIndexVector[nextWalkMoveBackIndex],
                           currentPositionInConstraint + 1);
  }
  return enumerationNumber;
}

template<class State>
__host__ void MultipleLabelTdsEnumerationCpu<State>::resetState(State *globalState) {
  globalState->resetModifiedList();
}

template<class State>
__host__ size_t
MultipleLabelTdsEnumerationCpu<State>::compute(const graph_t &graph, State *globalState) {
  //resetState(globalState);
  Logger::get().log(Logger::E_LEVEL_DEBUG, "Enumeration Start : ", Logger::E_OUTPUT_DEBUG);

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            templateConstraint,
                            &TemplateConstraint::print,
                            Logger::E_OUTPUT_FILE_LOG);

  size_t stepCompleted = 0;
  size_t stepMax = globalState->graphActiveVertexCount;
  size_t stepSize = 1000;

  const auto &walkKey = templateConstraint.walkMap.cbegin();
  const auto &walkStartingVertex = walkKey->first;
  const auto &walk = walkKey->second;

  std::vector<vid_t> historyIndexVector;

  size_t enumerationNumber=0;

  #pragma omp parallel for private(historyIndexVector) reduction(+:enumerationNumber)
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

    #pragma omp atomic
    ++stepCompleted;

    if (stepCompleted % stepSize == 0) {
      #pragma omp critical
      {
        std::cout << "Progression : " << stepCompleted << "/" << stepMax << std::endl;
      }
    }
  }

  return enumerationNumber;
}
}