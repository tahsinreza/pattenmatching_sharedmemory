//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_LCC_CPU_H
#define TOTEM_MULTIPLE_LABEL_LCC_CPU_H

#include "totem_multiple_label_common_cpu.cuh"
#include <iostream>
#include <unordered_set>

namespace patternmatching {


template <class State>
class MultipleLabelLccCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  __host__ error_t preprocessPatern(const graph_t &pattern);
  __host__ void printLocalConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t compute(const graph_t &graph, State *globalState) const;

 private:
  class LocalConstraint {
   public:
    LocalConstraint();
    LocalConstraint(
        const pvid_t &originVertex,
        const weight_t &originLabel,
        const std::unordered_map<weight_t, size_t> &localConstraint,
        const std::unordered_set <vid_t> &localConstraintNeighborVertex_);
    void print(std::ostream &ostream = std::cout) const;
   public:
    pvid_t originVertex;
    weight_t originLabel;
    std::unordered_map<weight_t, size_t> localConstraint;
    std::unordered_set<vid_t> localConstraintNeighborVertex;
  };

  inline void removeMatch(State *globalState, const vid_t vertexId, const pvid_t patternVertexId) const;


  std::vector<LocalConstraint > patternLocalConstraint;

  //std::vector<std::unordered_map<weight_t, size_t> > patternLocalConstraint;
};

}

#include "totem_multiple_label_lcc_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_LCC_CPU_H
