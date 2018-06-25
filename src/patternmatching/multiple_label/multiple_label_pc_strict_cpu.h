//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_PC_STRICT_CPU_H
#define TOTEM_MULTIPLE_LABEL_PC_STRICT_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

template<class State>
class MultipleLabelPcStrictCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  size_t  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

  void setConstraintVector(
      const std::vector <MultipleLabelConstraintPath>& pathConstraintVector);
  void setConstraintIterator(const size_t &index);

 private:

  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const MultipleLabelConstraintPath &currentConstraint,
      const bool &reverse,
      std::vector<vid_t> &historyIndexVector,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &remainingLength);

  std::vector <MultipleLabelConstraintPath> pathConstraintVector;
  typename std::vector<MultipleLabelConstraintPath>::const_iterator pathConstraintIterator;
};

}

#include "multiple_label_pc_strict_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_PC_CPU_H
