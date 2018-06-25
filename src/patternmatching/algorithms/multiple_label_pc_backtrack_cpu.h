//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_PC_BACKTRACK_CPU_H
#define TOTEM_MULTIPLE_LABEL_PC_BACKTRACK_CPU_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_path_constraint.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

template<class State>
class MultipleLabelPcBacktrackCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  void init(const graph_t &graph, const graph_t &pattern);

  __host__ error_t
  preprocessPatern(const graph_t &pattern);
  int getPathConstraintNumber() const;

  __host__ void printPathConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t
  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

  const std::vector <PathConstraint>& getPathConstraintVector() const;

 private:

  void buildConstraintList(
      const graph_t &pattern,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentLength,
      std::vector <vid_t> *historyVertexId,
      std::vector <weight_t> *historyVertexLabel,
      std::vector <PathConstraint> *constraintVector);

  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const PathConstraint &currentConstraint,
      const bool &reverse,
      std::unordered_map<vid_t, FixedBitmapType> &sourceTraversalMap,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &remainingLength);

  bool isInConstraintVector(const PathConstraint &constraint) const;

  std::vector <std::set<vid_t>> patternTraversalVector;
  std::vector <PathConstraint> pathConstraintVector;
  typename std::vector<PathConstraint>::const_iterator pathConstraintIterator;
};

}

#include "multiple_label_pc_backtrack_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_PC_CPU_H
