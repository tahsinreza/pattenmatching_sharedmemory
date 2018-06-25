//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_GRAPH_STAT_CPU_H
#define TOTEM_GRAPH_STAT_CPU_H

#include "multiple_label_common_cpu.h"
#include <iostream>
#include <unordered_set>
#include "graph_stat.h"

namespace patternmatching {


template <class State>
class GraphStatCpu : MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  void printGraphStat(std::ostream &ostream = std::cout) const;

  size_t compute(const graph_t &graph, State *globalState);

 private:
  GraphStat graphStat;
};

}

#include "graph_stat_cpu.tpp"

#endif //TOTEM_GRAPH_STAT_CPU_H
