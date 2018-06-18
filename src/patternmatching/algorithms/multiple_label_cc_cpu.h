//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_CC_CPU_H
#define TOTEM_MULTIPLE_LABEL_CC_CPU_H

#include "multiple_label_common_cpu.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "multiple_label_circular_constraint.h"

namespace patternmatching {

template<class State>
class MultipleLabelCcCpu : public MultipleLabelCpuBase<State> {
 protected:
  typedef MultipleLabelCpuBase<State> BaseClass;
 public:
  void init(const graph_t &graph, const graph_t &pattern);

  __host__ error_t
  preprocessPatern(const graph_t &pattern);
  int getCircularConstraintNumber() const;

  __host__ void printCircularConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t
  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

  const std::vector <MultipleLabelCircularConstraint>& getCircularConstraintVector() const;

 private:

  void buildConstraintList(
      const graph_t &pattern,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &remainingLength,
      std::vector <vid_t> *historyVertexId,
      std::vector <weight_t> *historyVertexLabel,
      std::vector <MultipleLabelCircularConstraint> *constraintVector);

  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const MultipleLabelCircularConstraint &currentConstraint,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &startingPosition,
      const size_t &remainingLength);

  bool isInConstraintVector(const MultipleLabelCircularConstraint &constraint) const;

  inline bool isOmitted(const State &globalState, const vid_t vertexId) const;
  inline void makeOmitted(State *globalState, const vid_t vertexId) const;
  inline void removeMatch(State *globalState, const vid_t vertexId, const pvid_t patternVertexId) const;

  std::vector <std::unordered_map<vid_t, size_t>> sourceTraversalVector;
  std::vector <std::unordered_set<vid_t>> patternTraversalVector;
  std::vector <MultipleLabelCircularConstraint> circularConstraintVector;
  typename std::vector<MultipleLabelCircularConstraint>::const_iterator circularConstraintIterator;
};

}

#include "multiple_label_cc_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_CC_CPU_H
