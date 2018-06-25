//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_LCC_CPU_H
#define TOTEM_MULTIPLE_LABEL_LCC_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_local_constraint.h"
#include <iostream>
#include <unordered_set>
#include "bitmap.h"

namespace patternmatching {


template <class State>
class MultipleLabelLccCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  __host__ error_t preprocessPatern(const graph_t &pattern);
  __host__ void printLocalConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t compute(const graph_t &graph, State *globalState) const;

  void resetState(State *globalState);

 private:

  std::vector<LocalConstraint > patternLocalConstraint;

  //std::vector<std::unordered_map<weight_t, size_t> > patternLocalConstraint;
};

}

#include "multiple_label_lcc_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_LCC_CPU_H
