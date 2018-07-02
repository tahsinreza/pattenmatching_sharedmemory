//
// Created by qiu on 21/06/18.
//

#ifndef PROJECT_ALGO_RESULTS_H
#define PROJECT_ALGO_RESULTS_H

#include "common_utils.h"
#include <iostream>

class AlgoResults {
 public:
  AlgoResults();
  void clear();
  bool isEmpty() const;
  void print(std::ostream &ostream = std::cout) const;
 public:
  size_t vertexEliminated;
  size_t edgeEliminated;
  size_t matchEliminated;
  size_t enumeration;
};

#endif //PROJECT_GRAPH_STAT_H
