//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_UNIQUE_LABEL_CC_CPU_H
#define TOTEM_UNIQUE_LABEL_CC_CPU_H

#include "unique_label_common_cpu.h"
#include "unique_label_circular_constraint.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>


namespace patternmatching {

template<class State>
class UniqueLabelCcCpu : public UniqueLabelCpuBase<State> {
 protected:
  typedef UniqueLabelCpuBase<State> BaseClass;
 public:
  void init(const graph_t &graph, const graph_t &pattern);

  __host__ error_t
  preprocessPatern(const graph_t &pattern);
  int getCircularConstraintNumber() const;

  __host__ void printCircularConstraint(std::ostream &ostream = std::cout) const;

  __host__ size_t
  compute(const graph_t &graph, State *globalState);
  void resetState(State *globalState);

 private:

  void buildConstraintList(
      const graph_t &pattern,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &remainingLength,
      std::vector <vid_t> *historyVertexId,
      std::vector <weight_t> *historyVertexLabel,
      std::vector <CircularConstraint> *constraintVector);

  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const CircularConstraint &currentConstraint,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &startingPosition,
      const size_t &remainingLength);

  bool isInConstraintVector(const CircularConstraint &constraint) const;

  inline bool isOmitted(const State &globalState, const vid_t vertexId) const;
  inline void makeOmitted(State *globalState, const vid_t vertexId) const;
  inline bool isMatch(const State &globalState, const vid_t vertexId) const;
  inline bool isMatchAtomic(const State &globalState, const vid_t vertexId) const;
  inline void makeMatch(State *globalState, const vid_t vertexId) const;
  inline void makeMatchAtomic(State *globalState, const vid_t vertexId) const;

  std::vector <std::unordered_map<vid_t, size_t>> sourceTraversalVector;
  std::vector <std::set<vid_t>> patternTraversalVector;
  std::vector <CircularConstraint> circularConstraintVector;
  typename std::vector<CircularConstraint>::const_iterator circularConstraintIterator;
};

}

#include "unique_label_cc_cpu.tpp"

#endif //TOTEM_UNIQUE_LABEL_CC_CPU_H
