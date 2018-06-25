//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_LCC_CPU_H
#define TOTEM_MULTIPLE_LABEL_LCC_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_local.h"
#include <iostream>
#include <unordered_set>
#include "utils/bitmap_fixed.h"

namespace patternmatching {


template <class State>
class MultipleLabelLccCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  size_t compute(const graph_t &graph, State *globalState) const;

  void resetState(State *globalState);

  void setConstraintVector(
      const std::vector <MultipleLabelConstraintLocal>& patternLocalConstraint);
 private:

  std::vector<MultipleLabelConstraintLocal > patternLocalConstraint;
};

}

#include "multiple_label_lcc_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_LCC_CPU_H
