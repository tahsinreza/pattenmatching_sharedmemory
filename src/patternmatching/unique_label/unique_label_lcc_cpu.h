//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_UNIQUE_LABEL_LCC_CPU_H
#define TOTEM_UNIQUE_LABEL_LCC_CPU_H

#include "unique_label_common_cpu.h"
#include <iostream>

namespace patternmatching {


template <class State>
class UniqueLabelLccCpu : public UniqueLabelCpuBase<State> {
 protected:
  typedef UniqueLabelCpuBase<State> BaseClass;
 public:
  __host__ error_t preprocessPatern(const graph_t &pattern);
  __host__ void printLocalConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t compute(const graph_t &graph, State *globalState) const;

 private:

  inline bool isMatch(const State &globalState, const vid_t vertexId) const;
  inline void removeMatch(State *globalState, const vid_t vertexId) const;

  std::unordered_map<weight_t, std::unordered_map<weight_t, size_t> > patternLocalConstraint;
};

}

#include "unique_label_lcc_cpu.tpp"

#endif //TOTEM_UNIQUE_LABEL_LCC_CPU_H
