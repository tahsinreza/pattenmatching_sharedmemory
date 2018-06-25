//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_CC_BACKTRACK_CPU_H
#define TOTEM_MULTIPLE_LABEL_CC_BACKTRACK_CPU_H

#include "multiple_label_common_cpu.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "multiple_label_constraint_circular.h"

namespace patternmatching {

template<class State>
class MultipleLabelCcBacktrackCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  size_t  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

  void setConstraintVector(const std::vector <MultipleLabelConstraintCircular>& circularConstraintVector);
  void setConstraintIterator(const size_t &index);

 private:
  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const MultipleLabelConstraintCircular &currentConstraint,
      std::unordered_map<vid_t, FixedBitmapType> &sourceTraversalMap,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &startingPosition,
      const size_t &remainingLength);

  std::vector <MultipleLabelConstraintCircular> circularConstraintVector;
  typename std::vector<MultipleLabelConstraintCircular>::const_iterator circularConstraintIterator;
};

}

#include "multiple_label_cc_backtrack_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_CC_CPU_H
