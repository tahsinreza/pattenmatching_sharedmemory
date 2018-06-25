//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_TDS_ENUMERATION_CPU_H
#define TOTEM_MULTIPLE_LABEL_TDS_ENUMERATION_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_template.h"
#include "multiple_label_constraint_circular.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

template<class State>
class MultipleLabelEnumerationCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
  using TraversalHypothesis = std::vector< vid_t >;
 public:
  __host__ size_t
  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

  void setConstraint(
      const MultipleLabelConstraintTemplate& templateConstraintVector);
 private:
  size_t checkConstraint(
      const graph_t &graph,
      State *globalState,
      const Walk &walk,
      std::vector<vid_t> &historyIndexVector,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentPositionInConstraint);

  MultipleLabelConstraintTemplate templateConstraint;
};

}

#include "multiple_label_enumeration_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
