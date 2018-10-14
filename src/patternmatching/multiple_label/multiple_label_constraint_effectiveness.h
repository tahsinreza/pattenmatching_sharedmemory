//
// Created by qiu on 21/06/18.
//

#ifndef PROJECT_EFFECTIVENESS_H
#define PROJECT_EFFECTIVENESS_H

#include "common_utils.h"
#include "graph_stat.h"
#include "multiple_label_constraint_circular.h"
#include "multiple_label_constraint_path.h"
#include "multiple_label_constraint_template.h"
#include <iostream>

namespace patternmatching {

class MultipleLabelConstraintEffectiveness {
 public:
  MultipleLabelConstraintEffectiveness();

  void compute(const graph_t &pattern, const GraphStat &graphStat, const MultipleLabelConstraintCircular &constraint);
  void computeStrict(const graph_t &pattern,
                     const GraphStat &graphStat,
                     const MultipleLabelConstraintCircular &constraint);
  void computeBacktrack(const graph_t &pattern,
                        const GraphStat &graphStat,
                        const MultipleLabelConstraintCircular &constraint);


  void compute(const graph_t &pattern, const GraphStat &graphStat, const MultipleLabelConstraintPath &constraint);
  void computeStrict(const graph_t &pattern,
                     const GraphStat &graphStat,
                     const MultipleLabelConstraintPath &constraint);
  void computeBacktrack(const graph_t &pattern,
                        const GraphStat &graphStat,
                        const MultipleLabelConstraintPath &constraint);

  void compute(const graph_t &pattern, const GraphStat &graphStat, const MultipleLabelConstraintTemplate &constraint);
  void computeStrict(const graph_t &pattern,
                     const GraphStat &graphStat,
                     const MultipleLabelConstraintTemplate &constraint);
  void computeBacktrack(const graph_t &pattern,
                        const GraphStat &graphStat,
                        const MultipleLabelConstraintTemplate &constraint);

  void clear();
  void print(std::ostream &ostream = std::cout) const;

 public:
  double probaFindVertex(const GraphStat &graphStat,
                         const weight_t labelFrom,
                         const weight_t labelTo,
                         const size_t knowingEdgeNumber = 0,
                         const size_t knowingVertexNumber = 0) const;

  double probaFindSameVertex(const GraphStat &graphStat,
                             const double labelFromNumber,
                             const weight_t label) const;
  double probaFindDifferentVertex(const GraphStat &graphStat,
                            const double labelFromNumber,
                            const weight_t label) const;

  double probaFindOnePath(const GraphStat &graphStat,
                          const double labelFromNumber,
                          const weight_t labelFrom,
                          const weight_t labelTo) const;
  double probaFindOnePathKnowing(const GraphStat &graphStat,
                                 const double labelFromNumber,
                                 const weight_t labelFrom,
                                 const weight_t labelTo,
                                 const size_t knowingEdgeNumber = 0,
                                 const size_t knowingVertexNumber = 0) const;

  double averageNumberOfVertex(const GraphStat &graphStat,
                               const double labelFromNumber,
                               const weight_t labelFrom) const;
  double averageNumberOfVertex(const GraphStat &graphStat,
                               const double labelFromNumber,
                               const weight_t labelFrom,
                               const weight_t labelTo) const;
  double averageNumberOfVertexKnowing(const GraphStat &graphStat,
                                      const double labelFromNumber,
                                      const weight_t labelFrom,
                                      const weight_t labelTo,
                                      const size_t knowingEdgeNumber = 0,
                                      const size_t knowingVertexNumber = 0) const;
  double averageNumberOfUniqueVertex(const GraphStat &graphStat,
                                     const double labelFromNumber,
                                     const weight_t labelFrom,
                                     const weight_t labelTo) const;
  double averageNumberOfUniqueVertexKnowing(const GraphStat &graphStat,
                                            const double labelFromNumber,
                                            const weight_t labelFrom,
                                            const weight_t labelTo,
                                            const size_t knowingEdgeNumber = 0,
                                            const size_t knowingVertexNumber = 0) const;

  template<typename T>
  T averageDifferentVertexFromTries(const T vertexNumber, const T tries) const;
  template<typename T>
  T averageDifferentVertexFromTriesApproximate(const T vertexNumber, const T tries) const;
  template<typename T>
  T averageDifferentVertexFromTriesApproximateStrong(const T vertexNumber, const T tries) const;
  template<typename T>
  T probaDifferentVertexFromTries(const T vertexNumber, const T tries, const T uniqueVertex) const;
  template<typename T>
  T probaDifferentVertexFromTriesApproximate(const T vertexNumber, const T tries, const T uniqueVertex) const;

  template<typename T>
  T factorial(const T n) const;
  template<typename T>
  T stirling2(const T n, const T k) const;
  template<typename T>
  T ncr(const T n, const T k) const;

  double approximateMatchEliminated;
  double approximateCostEliminated;
  double approximateEffectiveness;
};
}

#include "multiple_label_constraint_effectiveness.tpp"

#endif //PROJECT_GRAPH_STAT_H
