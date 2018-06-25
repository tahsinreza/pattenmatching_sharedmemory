//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_TDS_ENUMERATION_CPU_H
#define TOTEM_MULTIPLE_LABEL_TDS_ENUMERATION_CPU_H

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
class MultipleLabelTdsEnumerationCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
  using TraversalHypothesis = std::vector< vid_t >;
 public:
  void init(const graph_t &graph, const graph_t &pattern);

  __host__ error_t
  preprocessPatern(const graph_t &pattern);

  __host__ void printTemplateConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t
  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

 private:
  size_t checkConstraint(
      const graph_t &graph,
      State *globalState,
      const Walk &walk,
      std::vector<vid_t> &historyIndexVector,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentPositionInConstraint);


  TemplateConstraint templateConstraint;
};

}

#include "multiple_label_tds_enumeration_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
