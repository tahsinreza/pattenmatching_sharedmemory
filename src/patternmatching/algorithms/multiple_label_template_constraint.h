//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H
#define PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H

#include "totem_graph.h"
#include "multiple_label_circular_constraint.h"
#include "multiple_label_path_constraint.h"
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include "multiple_label_template_walk.h"
#include "multiple_label_template_edge.h"

namespace patternmatching {

class TemplateConstraint {
 public:
  enum TemplateConstraintOrigin {
    E_CC,
    E_PC,
    E_TDS_CC,
    E_TDS_PC,
    E_TDS_SUB,
    E_TDS_FULL
  };

  TemplateConstraint(const graph_t &pattern);
  TemplateConstraint(const MultipleLabelCircularConstraint &circularConstraint);
  TemplateConstraint(const PathConstraint &pathConstraint);
  TemplateConstraint(const TemplateConstraint &templateConstraint1, const TemplateConstraint &templateConstraint2);
  void generateWalkMap();

  void print(std::ostream &ostream = std::cout) const;
  void printWalk(std::ostream &ostream = std::cout) const;
 public:

  bool operator==(const TemplateConstraint &other) const;
  bool operator!=(const TemplateConstraint &other) const;
  void generateWalk(Walk &walk,
                    std::vector<vid_t> &historyIndex,
                    std::vector<UndirectedEdge> &historyEdgeIndex,
                    const vid_t &currentVertexIndex) const;
  size_t historyFindIndex(const std::vector<vid_t> &historyIndex, const vid_t &vertexIndex) const;
  size_t historyFindIndex(const std::vector<UndirectedEdge> &historyIndex,
                                              const vid_t &vertexFromIndex,
                                              const vid_t &vertexToIndex) const;
  TemplateConstraintOrigin templateConstraintOrigin;
  size_t length;
  weight_t initialLabel;
  std::unordered_map<vid_t, std::set<vid_t> > vertexIndexGraph;
  std::unordered_map<vid_t, Walk> walkMap;
};
}

#endif //PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H
