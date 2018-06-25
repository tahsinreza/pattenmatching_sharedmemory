//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_TDS_BACKTRACK_CPU_H
#define TOTEM_MULTIPLE_LABEL_TDS_BACKTRACK_CPU_H

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
class MultipleLabelTdsBacktrackCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
  using TraversalHypothesis = std::vector< vid_t >;
  using SourceTraversalMapType = std::unordered_map< vid_t, std::unordered_map<pvid_t, std::vector<TraversalHypothesis > > >;
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
      SourceTraversalMapType &sourceTraversalMap,
      std::vector<vid_t> &historyIndexVector,
      TraversalHypothesis &traversalHypothesis,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentPositionInConstraint);

  bool isInConstraintVector(const TemplateConstraint &constraint) const;

  std::vector <std::set<vid_t>> patternTraversalVector;
  std::vector <TemplateConstraint> templateConstraintVector;
  typename std::vector<TemplateConstraint>::const_iterator templateConstraintIterator;
};

}

#include "multiple_label_tds_backtrack_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
