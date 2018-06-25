//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_TDS_CPU_H
#define TOTEM_MULTIPLE_LABEL_TDS_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_template_constraint.h"
#include "multiple_label_circular_constraint.h"
#include "multiple_label_path_constraint.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

template<class State>
class MultipleLabelTdsCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  void init(const graph_t &graph, const graph_t &pattern);

  __host__ error_t
  preprocessPatern(const graph_t &pattern,
                   const std::vector<MultipleLabelCircularConstraint> &circularConstraintVector,
                   const std::vector<PathConstraint> &pathConstraintVector);
  int getTemplateConstraintNumber() const;

  __host__ void printTemplateConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t
  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

 private:
  bool haveCommonEdge(const TemplateConstraint& constraint1, const TemplateConstraint& constraint2) const;
  bool haveCommonVector(const TemplateConstraint& constraint1, const TemplateConstraint& constraint2) const;
  void preprocessCircularConstraint(
      const std::vector<MultipleLabelCircularConstraint> &circularConstraintVector);
  void preprocessPathConstraint(
      const std::vector<PathConstraint> &pathConstraintVector);
  void preprocessSubtemplateConstraint();

  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const Walk &walk,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      std::vector<vid_t> &historyIndexVector,
      const size_t &currentPositionInConstraint);

  bool isInConstraintVector(const TemplateConstraint &constraint) const;
  bool isInHistoryIndexVector(const std::vector<vid_t> &historyIndexVector, const vid_t &vertex) const;

  std::vector <std::set<vid_t>> patternTraversalVector;
  std::vector <TemplateConstraint> templateConstraintVector;
  typename std::vector<TemplateConstraint>::const_iterator templateConstraintIterator;
};

}

#include "multiple_label_tds_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
