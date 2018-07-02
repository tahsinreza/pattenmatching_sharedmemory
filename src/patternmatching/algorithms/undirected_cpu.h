//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_UNDIRECTED_CPU_H
#define TOTEM_UNDIRECTED_CPU_H

#include "multiple_label_common_cpu.h"
#include <iostream>
#include <unordered_set>
#include "graph_stat.h"

namespace patternmatching {


template <class State, class BaseClass>
class UndirectedCpu : BaseClass {
 public:
  size_t compute(const graph_t &graph, State *globalState);

 private:
};

}

#include "undirected_cpu.tpp"

#endif //TOTEM_GRAPH_STAT_CPU_H
