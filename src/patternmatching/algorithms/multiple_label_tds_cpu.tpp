//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_tds_cpu.h"
#include "multiple_label_template_constraint.h"
#include <iostream>
#include <deque>
#include "utils.h"

namespace patternmatching {

template<class State>
void MultipleLabelTdsCpu<State>::init(const graph_t &graph, const graph_t &pattern) {
  patternTraversalVector.resize(pattern.vertex_count);
}

template<class State>
bool MultipleLabelTdsCpu<State>::isInConstraintVector(const TemplateConstraint &constraint) const {
  for (const auto &it : templateConstraintVector) {
    if (it == constraint) return true;
  }
  return false;
}

template<class State>
bool MultipleLabelTdsCpu<State>::haveCommonVector(const TemplateConstraint &constraint1,
                                                  const TemplateConstraint &constraint2) const {
  for (const auto &it : constraint1.vertexIndexGraph) {
    if (constraint2.vertexIndexGraph.count(it.first) > 0) {
      return true;
    }
  }
  return false;
}

template<class State>
bool MultipleLabelTdsCpu<State>::haveCommonEdge(const TemplateConstraint &constraint1,
                                                const TemplateConstraint &constraint2) const {
  for (const auto &it : constraint1.vertexIndexGraph) {
    if (constraint2.vertexIndexGraph.count(it.first) > 0) {
      for (const auto &subit : it.second) {
        for (const auto &vertexIndexIt : constraint2.vertexIndexGraph.at(it.first)) {
          if (vertexIndexIt == subit) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

template<class State>
__host__ void
MultipleLabelTdsCpu<State>::preprocessCircularConstraint(
    const std::vector<MultipleLabelCircularConstraint> &circularConstraintVector) {

  std::deque<TemplateConstraint> templateConstraintList;

  // process circular constraint
  for (const auto &it : circularConstraintVector) {
    TemplateConstraint constraint = TemplateConstraint(it);
    templateConstraintList.push_back(constraint);
  }

  auto templateConstraintIt1 = templateConstraintList.begin();
  while (templateConstraintIt1 != templateConstraintList.end()) {
    auto templateConstraintIt2 = templateConstraintIt1 + 1;
    bool haveMerged = false;
    while (templateConstraintIt2 != templateConstraintList.end()) {
      if (haveCommonEdge(*templateConstraintIt1, *templateConstraintIt2)) {
        auto mergedConstraint = TemplateConstraint(*templateConstraintIt1, *templateConstraintIt2);

        haveMerged = true;
        templateConstraintList.erase(templateConstraintIt1);
        templateConstraintList.erase(templateConstraintIt2);
        templateConstraintList.push_back(mergedConstraint);
        templateConstraintIt1 = templateConstraintList.begin();
        break;
      }
      if (!haveMerged) ++templateConstraintIt2;
    }
    if (!haveMerged) ++templateConstraintIt1;
  }

  for (const auto &it : templateConstraintList) {
    if (it.templateConstraintOrigin != TemplateConstraint::E_CC) {
      templateConstraintVector.push_back(it);
    }
  }
}

template<class State>
__host__ void
MultipleLabelTdsCpu<State>::preprocessPathConstraint(
    const std::vector<PathConstraint> &pathConstraintVector) {

  std::unordered_map<weight_t, std::deque<TemplateConstraint> > templateConstraintListLabelMap;

  // process path constraint
  for (const auto &it : pathConstraintVector) {
    TemplateConstraint constraint = TemplateConstraint(it);
    templateConstraintListLabelMap[it.initialLabel].push_back(constraint);
  }

  for (auto &it : templateConstraintListLabelMap) {
    auto &templateConstraintList = it.second;
    auto templateConstraintIt1 = templateConstraintList.begin();
    while (templateConstraintIt1 != templateConstraintList.end()) {
      auto templateConstraintIt2 = templateConstraintIt1 + 1;
      bool haveMerged = false;
      while (templateConstraintIt2 != templateConstraintList.end()) {
        if (haveCommonVector(*templateConstraintIt1, *templateConstraintIt2)) {
          auto mergedConstraint = TemplateConstraint(*templateConstraintIt1, *templateConstraintIt2);

          haveMerged = true;
          templateConstraintList.erase(templateConstraintIt1);
          templateConstraintList.erase(templateConstraintIt2);
          templateConstraintList.push_back(mergedConstraint);
          templateConstraintIt1 = templateConstraintList.begin();
          break;
        }
        if (!haveMerged) ++templateConstraintIt2;
      }
      if (!haveMerged) ++templateConstraintIt1;
    }
  }

  for (const auto &it : templateConstraintListLabelMap) {
    for (const auto &subit : it.second) {
      if (subit.templateConstraintOrigin != TemplateConstraint::E_PC) {
        templateConstraintVector.push_back(subit);
      }
    }
  }
}

template<class State>
__host__ void
MultipleLabelTdsCpu<State>::preprocessSubtemplateConstraint() {

  std::deque<TemplateConstraint> templateConstraintList;

  // process subtemplate constraint
  for (const auto &it : templateConstraintVector) {
    templateConstraintList.push_back(it);
  }

  size_t offset = 0;

  while (templateConstraintList.size() > 0 && (offset < templateConstraintList.size() - 1)) {
    int mergedNumber = 0;
    auto it = templateConstraintList.begin() + offset;
    auto currentTemplateConstraint = *it;
    ++it;
    while (it != templateConstraintList.end()) {
      if (haveCommonVector(currentTemplateConstraint, *it)) {
        currentTemplateConstraint = TemplateConstraint(currentTemplateConstraint, *it);

        templateConstraintList.erase(it);
        it = templateConstraintList.begin();
        mergedNumber++;
      } else {
        ++it;
      }
    }
    if (mergedNumber > 0) {
      offset = 0;
      if (!isInConstraintVector(currentTemplateConstraint)) {
        templateConstraintVector.push_back(currentTemplateConstraint);
      }
    } else {
      offset++;
    }
  }
}

template<class State>
__host__ error_t
MultipleLabelTdsCpu<State>::preprocessPatern(
    const graph_t &pattern,
    const std::vector<MultipleLabelCircularConstraint> &circularConstraintVector,
    const std::vector<PathConstraint> &pathConstraintVector) {

  preprocessCircularConstraint(circularConstraintVector);
  preprocessPathConstraint(pathConstraintVector);
  preprocessSubtemplateConstraint();

  for (auto &it : templateConstraintVector) {
    it.generateWalkMap();
  }

  templateConstraintIterator = templateConstraintVector.begin();
  return SUCCESS;
}

template<class State>
__host__ void MultipleLabelTdsCpu<State>::printTemplateConstraint(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << templateConstraintVector.size() << ". " << std::endl;
  for (const auto &it : templateConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    it.printWalk(ostream);
    ++currentConstraint;
  }
}

template<class State>
bool MultipleLabelTdsCpu<State>::checkConstraint(
    const graph_t &graph,
    State *globalState,
    const Walk &walk,
    std::unordered_map<vid_t, std::unordered_map<pvid_t, std::vector<TraversalHypothesis> > > &sourceTraversalMap,
    std::vector<vid_t> &historyIndexVector,
    TraversalHypothesis &traversalHypothesis,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentPositionInConstraint) {

  // Close the recursion
  if (currentPositionInConstraint == walk.length) return true;

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

      /*if (sourceTraversalMap.find(neighborVertexId) != sourceTraversalMap.end()
          && sourceTraversalMap[neighborVertexId].find(nextWalkVertexIndex) != sourceTraversalMap[neighborVertexId].end()
          && isInVector(sourceTraversalMap[neighborVertexId][nextWalkVertexIndex], traversalHypothesis))
        continue;*/

      if(currentWalkMove == Walk::E_VERTEX_STORE) {
        traversalHypothesis.push_back(neighborVertexId);
      }

      sourceTraversalMap[neighborVertexId][nextWalkVertexIndex].push_back(traversalHypothesis);
      historyIndexVector.push_back(neighborVertexId);
      if (checkConstraint(graph,
                          globalState,
                          walk,
                          sourceTraversalMap,
                          historyIndexVector,
                          traversalHypothesis,
                          sourceVertexId,
                          neighborVertexId,
                          currentPositionInConstraint + 1)) {
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

    if (checkConstraint(graph,
                        globalState,
                        walk,
                        sourceTraversalMap,
                        historyIndexVector,
                        traversalHypothesis,
                        sourceVertexId,
                        historyIndexVector[nextWalkMoveBackIndex],
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
  //resetState(globalState);
  Logger::get().log(Logger::E_LEVEL_DEBUG, "TDS Start : ", Logger::E_OUTPUT_DEBUG);

  const auto &currentConstraint = *templateConstraintIterator;

  Logger::get().log(Logger::E_LEVEL_DEBUG, "currentConstraint : ", Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG,
                            currentConstraint,
                            &TemplateConstraint::print,
                            Logger::E_OUTPUT_FILE_LOG);

  std::unordered_map<vid_t, std::unordered_map<pvid_t, std::vector<TraversalHypothesis> > > sourceTraversalMap;
  std::vector<vid_t> historyIndexVector;
  TraversalHypothesis traversalHypothesis;

  #pragma omp parallel for private(sourceTraversalMap, historyIndexVector, traversalHypothesis)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isVertexActive(*globalState, vertexId)) continue;

    size_t constraintIndex = 0;
    bool hasBeenModified = false;

    for (auto it = currentConstraint.walkMap.cbegin();
         it != currentConstraint.walkMap.cend();
         ++it, ++constraintIndex) {

      if (globalState->vertexPatternMatch[vertexId].isIn((*it).first)) {

        // Check cycle
        sourceTraversalMap.clear();
        historyIndexVector.clear();
        historyIndexVector.push_back(vertexId);
        traversalHypothesis.clear();
        if (!checkConstraint(graph,
                             globalState,
                             (*it).second,
                             sourceTraversalMap,
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
        Logger::get().log(Logger::E_LEVEL_DEBUG, ss.str());
        BaseClass::deactivateVertex(globalState, vertexId);*/
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

  return vertexEliminatedNumber;
}

template<class State>
int MultipleLabelTdsCpu<State>::getTemplateConstraintNumber() const {
  return templateConstraintVector.size();
}

}