//
// Created by qiu on 07/06/18.
//

#include "multiple_label_template_constraint.h"

namespace patternmatching {

TemplateConstraint::TemplateConstraint(const MultipleLabelCircularConstraint &circularConstraint)
    : templateConstraintOrigin(E_CC) {
  length = circularConstraint.length;
  auto edgeFromIt = circularConstraint.vertexIndexVector.cbegin();
  auto edgeToIt = circularConstraint.vertexIndexVector.cbegin() + 1;
  while (edgeToIt != circularConstraint.vertexIndexVector.cend()) {
    vertexIndexGraph[*edgeFromIt].insert(*edgeToIt);
    vertexIndexGraph[*edgeToIt].insert(*edgeFromIt);

    ++edgeFromIt;
    ++edgeToIt;
  }
  // CLose the loop
  edgeToIt = circularConstraint.vertexIndexVector.cbegin();
  vertexIndexGraph[*edgeFromIt].insert(*edgeToIt);
  vertexIndexGraph[*edgeToIt].insert(*edgeFromIt);
}

TemplateConstraint::TemplateConstraint(const PathConstraint &pathConstraint)
    : templateConstraintOrigin(E_PC) {
  length = pathConstraint.length;
  auto edgeFromIt = pathConstraint.vertexIndexVector.cbegin();
  auto edgeToIt = pathConstraint.vertexIndexVector.cbegin() + 1;
  while (edgeToIt != pathConstraint.vertexIndexVector.cend()) {
    vertexIndexGraph[*edgeFromIt].insert(*edgeToIt);
    vertexIndexGraph[*edgeToIt].insert(*edgeFromIt);

    ++edgeFromIt;
    ++edgeToIt;
  }
}

TemplateConstraint::TemplateConstraint(const TemplateConstraint &templateConstraint1,
                                       const TemplateConstraint &templateConstraint2) {
  if ((templateConstraint1.templateConstraintOrigin == E_CC || templateConstraint1.templateConstraintOrigin == E_TDS_CC)
      && (templateConstraint2.templateConstraintOrigin == E_CC
          || templateConstraint2.templateConstraintOrigin == E_TDS_CC)) {
    templateConstraintOrigin = E_TDS_CC;
  } else if (
      (templateConstraint1.templateConstraintOrigin == E_PC || templateConstraint1.templateConstraintOrigin == E_TDS_PC)
          && (templateConstraint2.templateConstraintOrigin == E_PC
              || templateConstraint2.templateConstraintOrigin == E_TDS_PC)) {
    templateConstraintOrigin = E_TDS_PC;
  } else {
    templateConstraintOrigin = E_TDS_SUB;
  }

  length = templateConstraint1.length;
  vertexIndexGraph = templateConstraint1.vertexIndexGraph;

  for (const auto &it : templateConstraint2.vertexIndexGraph) {
    if (vertexIndexGraph.count(it.first) == 0) ++length;
    for (const auto &subit : it.second) {
      vertexIndexGraph[it.first].insert(subit);
    }
  }
}

TemplateConstraint::TemplateConstraint(const graph_t &pattern) {
  templateConstraintOrigin = E_TDS_FULL;
/*
  length = templateConstraint1.length;
  vertexIndexGraph = templateConstraint1.vertexIndexGraph;

  for (const auto &it : templateConstraint2.vertexIndexGraph) {
    if (vertexIndexGraph.count(it.first) == 0) ++length;
    for (const auto &subit : it.second) {
      vertexIndexGraph[it.first].insert(subit);
    }
  }*/
}

size_t TemplateConstraint::historyFindIndex(const std::vector<vid_t> &historyIndex,
                                            const vid_t &vertexIndex) const {
  auto it = historyIndex.cbegin();
  size_t index = 0;
  for (; it != historyIndex.cend(); ++it, ++index) {
    if (*it == vertexIndex) return index;
  }
  return index;
}
size_t TemplateConstraint::historyFindIndex(const std::vector<UndirectedEdge> &historyIndex,
                                            const vid_t &vertexFromIndex,
                                            const vid_t &vertexToIndex) const {
  UndirectedEdge edge = UndirectedEdge(vertexFromIndex, vertexToIndex);
  auto it = historyIndex.cbegin();
  size_t index = 0;
  for (; it != historyIndex.cend(); ++it, ++index) {
    if (*it == edge) return index;
  }
  return index;
}

void TemplateConstraint::generateWalk(Walk &walk,
                                      std::vector<vid_t> &historyVertexIndex,
                                      std::vector<UndirectedEdge> &historyEdgeIndex,
                                      const vid_t &currentVertexIndex) const {
  bool first = true;
  auto currentWalkIndex = walk.length - 1;
  for (const auto &neighborVertex : vertexIndexGraph.at(currentVertexIndex)) {
    auto previousIndex = historyFindIndex(historyVertexIndex, neighborVertex);
    if (previousIndex != historyVertexIndex.size()) {
      if(historyFindIndex(historyEdgeIndex, currentVertexIndex, neighborVertex) != historyEdgeIndex.size()) continue;

      if (!first) {
        walk.addMoveBack(currentVertexIndex, currentWalkIndex);
      }
      historyEdgeIndex.emplace_back(currentVertexIndex, neighborVertex);
      walk.addCheck(neighborVertex, previousIndex);
    } else {
      if (!first) {
        walk.addMoveBack(currentVertexIndex, currentWalkIndex);
      }
      historyEdgeIndex.emplace_back(currentVertexIndex, neighborVertex);
      historyVertexIndex.push_back(neighborVertex);
      walk.addVertex(neighborVertex);
      generateWalk(walk, historyVertexIndex, historyEdgeIndex, neighborVertex);
    }
    first = false;
  }
}

void TemplateConstraint::generateWalkMap() {
  //std::cin;
  for (const auto &it : vertexIndexGraph) {
    Walk currentWalk;

    std::vector<vid_t> historyVertexIndex;
    std::vector<UndirectedEdge> historyEdgeIndex;
    historyVertexIndex.push_back(it.first);
    currentWalk.addVertex(it.first);
    generateWalk(currentWalk, historyVertexIndex, historyEdgeIndex, it.first);

    walkMap[it.first] = currentWalk;
    currentWalk.print();
  }
}

bool TemplateConstraint::operator==(const TemplateConstraint &other) const {
  return (length == other.length) && (vertexIndexGraph == other.vertexIndexGraph);
}

bool TemplateConstraint::operator!=(const TemplateConstraint &other) const {
  return (length != other.length) || (vertexIndexGraph != other.vertexIndexGraph);
}

void TemplateConstraint::print(std::ostream &ostream) const {
  ostream << "Vertex Index Graph [length : " << length << "] : ";
  for (const auto &it : vertexIndexGraph) {
    for (const auto &subit : vertexIndexGraph.at(it.first)) {
      ostream << " (" << it.first << "," << subit << ")";

    }
  }
  ostream << std::endl;
}

}