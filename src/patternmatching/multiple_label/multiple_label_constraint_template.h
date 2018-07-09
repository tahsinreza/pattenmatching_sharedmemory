//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H
#define PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H

#include "totem_graph.h"
#include "multiple_label_constraint_circular.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include "walk.h"
#include "edge.h"

namespace patternmatching {

class MultipleLabelConstraintTemplate {
 public:
  enum TemplateConstraintOrigin {
    E_EMPTY,
    E_CC,
    E_PC,
    E_TDS_CC,
    E_TDS_PC,
    E_TDS_SUB,
    E_TDS_FULL
  };

  MultipleLabelConstraintTemplate();
  MultipleLabelConstraintTemplate(const graph_t &pattern);
  MultipleLabelConstraintTemplate(const MultipleLabelConstraintCircular &circularConstraint);
  MultipleLabelConstraintTemplate(const MultipleLabelConstraintPath &pathConstraint);
  MultipleLabelConstraintTemplate(const MultipleLabelConstraintTemplate &templateConstraint1,
                                  const MultipleLabelConstraintTemplate &templateConstraint2);
  void generateWalkMap();

  void print(std::ostream &ostream = std::cout) const;
  void printWalk(std::ostream &ostream = std::cout) const;
 public:

  bool operator==(const MultipleLabelConstraintTemplate &other) const;
  bool operator!=(const MultipleLabelConstraintTemplate &other) const;

  bool haveCommonEdge(const MultipleLabelConstraintTemplate &other) const;
  bool haveCommonVector(const MultipleLabelConstraintTemplate &other) const;
  void generateWalk(Walk &walk,
                    std::vector<vid_t> &historyIndex,
                    std::vector<UndirectedEdge> &historyEdgeIndex,
                    const vid_t &currentVertexIndex) const;
  size_t historyFindIndex(const std::vector<vid_t> &historyIndex, const vid_t &vertexIndex) const;
  TemplateConstraintOrigin templateConstraintOrigin;
  size_t length;
  std::unordered_map<vid_t, std::set<vid_t> > vertexIndexGraph;
  std::unordered_map<vid_t, Walk> walkMap;
};
}

#endif //PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H
