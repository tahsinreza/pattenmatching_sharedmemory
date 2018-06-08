//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_PC_CPU_H
#define TOTEM_MULTIPLE_LABEL_PC_CPU_H

#include "multiple_label_common_cpu.cuh"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

template<class State>
class MultipleLabelPcCpu : public MultipleLabelCpuBase<State> {
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

 private:
  class PathConstraint {
   public:
    PathConstraint(
        const std::vector <vid_t> &historyVertexId,
        const std::vector <weight_t> &historyVertexLabel);
    void print(std::ostream &ostream = std::cout) const;
   public:
    bool operator==(const PathConstraint &other) const;
    size_t length;
    weight_t initialLabel;
    std::vector <vid_t> vertexIndexVector;
    std::vector <weight_t> vertexLabelVector;
  };

  void buildConstraintList(
      const graph_t &pattern,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentLength,
      std::vector <vid_t> *historyVertexId,
      std::vector <weight_t> *historyVertexLabel,
      std::vector <MultipleLabelPcCpu<State>::PathConstraint> *constraintVector);

  bool checkConstraint(
      const graph_t &graph,
      State *globalState,
      const MultipleLabelPcCpu<State>::PathConstraint &currentConstraint,
      const bool &reverse,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &remainingLength);

  bool isInConstraintVector(const MultipleLabelPcCpu<State>::PathConstraint &constraint) const;

  inline bool isOmitted(const State &globalState, const vid_t vertexId) const;
  inline void makeOmitted(State *globalState, const vid_t vertexId) const;
  inline void removeMatch(State *globalState, const vid_t vertexId, const pvid_t patternVertexId) const;

  std::vector <std::unordered_map<vid_t, size_t>> sourceTraversalVector;
  std::vector <std::set<vid_t>> patternTraversalVector;
  std::vector <PathConstraint> pathConstraintVector;
  typename std::vector<PathConstraint>::const_iterator pathConstraintIterator;
};

}

#include "multiple_label_pc_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_PC_CPU_H
