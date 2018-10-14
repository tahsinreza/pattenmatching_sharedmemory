//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_TDS_BACKTRACK_CPU_H
#define TOTEM_MULTIPLE_LABEL_TDS_BACKTRACK_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_template.h"
#include "multiple_label_constraint_circular.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>
#include "algo_results.h"

namespace patternmatching {

template<class State>
class MultipleLabelTdsBacktrackCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
  using TraversalHypothesis = std::vector< vid_t >;
  using SourceTraversalMapType = std::unordered_map< vid_t, std::unordered_map<pvid_t, std::vector<TraversalHypothesis > > >;
 public:
  AlgoResults  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

  void setConstraintVector(
      const std::vector <MultipleLabelConstraintTemplate>& templateConstraintVector);
  void setConstraintIterator(const size_t &index);
  std::vector <MultipleLabelConstraintTemplate>* getConstraintVector();
 private:
  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const Walk &walk,
      SourceTraversalMapType &sourceTraversalMap,
      std::vector<vid_t> &historyIndexVector,
      TraversalHypothesis &traversalHypothesis,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentPositionInConstraint);

  std::vector <MultipleLabelConstraintTemplate> templateConstraintVector;
  typename std::vector<MultipleLabelConstraintTemplate>::const_iterator templateConstraintIterator;
};

}

#include "multiple_label_tds_backtrack_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
