//
// Created by qiu on 07/06/18.
//

#include "multiple_label_constraint_template.h"
#include "common_utils.h"

namespace patternmatching {

MultipleLabelConstraintTemplate::MultipleLabelConstraintTemplate()
    : templateConstraintOrigin(E_EMPTY), length(0) {
}

MultipleLabelConstraintTemplate::MultipleLabelConstraintTemplate(const MultipleLabelConstraintCircular &circularConstraint)
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

MultipleLabelConstraintTemplate::MultipleLabelConstraintTemplate(const MultipleLabelConstraintPath &pathConstraint)
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

MultipleLabelConstraintTemplate::MultipleLabelConstraintTemplate(const MultipleLabelConstraintTemplate &templateConstraint1,
                                       const MultipleLabelConstraintTemplate &templateConstraint2) {
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

MultipleLabelConstraintTemplate::MultipleLabelConstraintTemplate(const graph_t &pattern) {
  templateConstraintOrigin = E_TDS_FULL;

  length = pattern.vertex_count;
  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    for (eid_t neighborEdgeId = pattern.vertices[vertexId];
         neighborEdgeId < pattern.vertices[vertexId + 1];
         neighborEdgeId++) {
      vid_t neighborVertexId = pattern.edges[neighborEdgeId];
      vertexIndexGraph[vertexId].insert(neighborVertexId);
      vertexIndexGraph[neighborVertexId].insert(vertexId);
    }
  }
}


bool MultipleLabelConstraintTemplate::haveCommonVector(const MultipleLabelConstraintTemplate &other) const {
  for (const auto &it : vertexIndexGraph) {
    if (other.vertexIndexGraph.count(it.first) > 0) {
      return true;
    }
  }
  return false;
}

bool MultipleLabelConstraintTemplate::haveCommonEdge(const MultipleLabelConstraintTemplate &other) const {
  for (const auto &it : vertexIndexGraph) {
    if (other.vertexIndexGraph.count(it.first) > 0) {
      for (const auto &subit : it.second) {
        for (const auto &vertexIndexIt : other.vertexIndexGraph.at(it.first)) {
          if (vertexIndexIt == subit) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

size_t MultipleLabelConstraintTemplate::historyFindIndex(const std::vector<vid_t> &historyIndex,
                                            const vid_t &vertexIndex) const {
  auto it = historyIndex.cbegin();
  size_t index = 0;
  for (; it != historyIndex.cend(); ++it, ++index) {
    if (*it == vertexIndex) return index;
  }
  return index;
}

void MultipleLabelConstraintTemplate::generateWalk(Walk &walk,
                                      std::vector<vid_t> &historyVertexIndex,
                                      std::vector<UndirectedEdge> &historyEdgeIndex,
                                      const vid_t &currentVertexIndex) const {
  bool isOn = true;
  auto currentWalkIndex = walk.vertexLength - 1;
  for (const auto &neighborVertex : vertexIndexGraph.at(currentVertexIndex)) {
    auto edge = UndirectedEdge(currentVertexIndex, neighborVertex);
    if(!isInVector(historyVertexIndex, neighborVertex)) {
      if (!isOn) {
        walk.addMoveBack(currentVertexIndex, currentWalkIndex);
      }

      historyEdgeIndex.push_back(edge);
      historyVertexIndex.push_back(neighborVertex);
      walk.addVertex(neighborVertex);
      generateWalk(walk, historyVertexIndex, historyEdgeIndex, neighborVertex);
      isOn = false;
    } else {
      if(isInVector(historyEdgeIndex,edge)) continue;

      if (!isOn) {
        walk.addMoveBack(currentVertexIndex, currentWalkIndex);
        isOn = true;
      }
      auto previousIndex = historyFindIndex(historyVertexIndex, neighborVertex);

      historyEdgeIndex.push_back(edge);
      walk.addCheck(neighborVertex, previousIndex);
    }
  }
}

void MultipleLabelConstraintTemplate::generateWalkMap() {
  for (const auto &it : vertexIndexGraph) {
    Walk currentWalk;

    std::vector<vid_t> historyVertexIndex;
    std::vector<UndirectedEdge> historyEdgeIndex;
    historyVertexIndex.push_back(it.first);
    currentWalk.addVertex(it.first);
    generateWalk(currentWalk, historyVertexIndex, historyEdgeIndex, it.first);
    currentWalk.computeStoredVertex();

    walkMap[it.first] = currentWalk;
  }
}

bool MultipleLabelConstraintTemplate::operator==(const MultipleLabelConstraintTemplate &other) const {
  return (length == other.length) && (vertexIndexGraph == other.vertexIndexGraph);
}

bool MultipleLabelConstraintTemplate::operator!=(const MultipleLabelConstraintTemplate &other) const {
  return (length != other.length) || (vertexIndexGraph != other.vertexIndexGraph);
}

void MultipleLabelConstraintTemplate::print(std::ostream &ostream) const {
  ostream << "Vertex Index Graph [length : " << length << "] : ";
  for (const auto &it : vertexIndexGraph) {
    for (const auto &subit : vertexIndexGraph.at(it.first)) {
      ostream << " (" << it.first << "," << subit << ")";

    }
  }
  ostream << std::endl;
}
void MultipleLabelConstraintTemplate::printWalk(std::ostream &ostream) const {
  ostream << "Vertex Index Walk : ";
  for (const auto &it : walkMap) {
    it.second.print(ostream);
  }
  ostream << std::endl;
}

}