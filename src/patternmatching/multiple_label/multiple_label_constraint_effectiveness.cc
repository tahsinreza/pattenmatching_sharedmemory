//
// Created by qiu on 21/06/18.
//

#include "multiple_label_constraint_effectiveness.h"
#include <cmath>

namespace patternmatching {
MultipleLabelConstraintEffectiveness::MultipleLabelConstraintEffectiveness()
    : approximateMatchEliminated(0), approximateCostEliminated(0), approximateEffectiveness(0) {}

void MultipleLabelConstraintEffectiveness::clear() {
  approximateMatchEliminated = 0;
  approximateCostEliminated = 0;
  approximateEffectiveness = 0;
}

void MultipleLabelConstraintEffectiveness::compute(const graph_t &pattern,
                                                   const GraphStat &graphStat,
                                                   const MultipleLabelConstraintCircular &constraint) {
  clear();

//  computeNumberPruned(pattern, graphStat, constraint);

  auto version = 2;
  if (version == 1) {
    size_t possibleMatch = 0;
    for (int i = 0; i < constraint.length; i++) {
      possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
      double product = 1;
      for (int j = 0; j < constraint.length; j++) {
        product *= static_cast<double>(
            graphStat.edgeLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length])
                .at(constraint.vertexLabelVector[(i + j + 1) % constraint.length]))
            / graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length]);
      }

      approximateCostEliminated += possibleMatch * product;
    }
  } else if (version == 2) {
    size_t possibleMatch = 0;
    for (int i = 0; i < constraint.length; i++) {
      possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
      double traversalCost = 1;
      for (int j = 0; j < constraint.length; j++) {
        traversalCost *= static_cast<double>(
            graphStat.edgeLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length])
                .at(constraint.vertexLabelVector[(i + j + 1) % constraint.length]))
            / graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length]);
      }

      double pruningProbability = 1;
      double product2 = 1;
      for (int j = 0; j < constraint.length; j++) {
        auto proba =
            probaFindEdge(graphStat,
                          constraint.vertexLabelVector[(i + j) % constraint.length],
                          constraint.vertexLabelVector[(i + j + 1) % constraint.length])
                / graphStat.edgeOutboundLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j)
                    % constraint.length]);
        pruningProbability *= product2 * (1 - proba);
        product2 *= proba;
      }
      // Closure
      pruningProbability *= product2 / graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);

      approximateCostEliminated += possibleMatch * traversalCost * (1 - pruningProbability);
    }
  } else if (version == 3) {

    size_t possibleMatch = 0;
    for (int i = 0; i < constraint.length; i++) {
      possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
      double traversalCost = 1;
      for (int j = 0; j < constraint.length; j++) {
        traversalCost *= static_cast<double>(
            graphStat.edgeLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length])
                .at(constraint.vertexLabelVector[(i + j + 1) % constraint.length]))
            / graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length]);
      }

      double pruningProbability = 1;
      double product2 = 1;
      for (int j = 0; j < constraint.length; j++) {
        auto proba =
            probaFindEdge(graphStat,
                          constraint.vertexLabelVector[(i + j) % constraint.length],
                          constraint.vertexLabelVector[(i + j + 1) % constraint.length])
                / graphStat.edgeOutboundLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j)
                    % constraint.length]);
        pruningProbability *= product2 * (1 - proba);
        product2 *= proba;
      }

      double differenceProba = 1;
      for (int j = 1; j < constraint.length; j++) {
        differenceProba *= static_cast<double>(
            graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[j]) - 1) /
            graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[j]);
      }
      double
          sameProba = static_cast<double>(1) / graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
      pruningProbability *= product2 * sameProba * differenceProba;

      approximateCostEliminated += possibleMatch * traversalCost * (1 - pruningProbability);
    }
  } else if (version == 4) {

    size_t possibleMatch = 0;
    for (int i = 0; i < constraint.length; i++) {
      possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
      double traversalCost = 1;
      for (int j = constraint.length - 1; j >= 0; j--) {
        auto costTmp = static_cast<double>(
            graphStat.edgeLabelTotalNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length])
                .at(constraint.vertexLabelVector[(i + j + 1) % constraint.length]));
        traversalCost = traversalCost * costTmp +
            graphStat.edgeOutboundLabelAverageNumberMap.at(constraint.vertexLabelVector[(i + j) % constraint.length]);
      }

      double pruningProbability = 1;
      double product2 = 1;
      for (int j = 0; j < constraint.length; j++) {
        auto proba =
            probaFindEdge(graphStat,
                          constraint.vertexLabelVector[(i + j) % constraint.length],
                          constraint.vertexLabelVector[(i + j + 1) % constraint.length]);
        pruningProbability *= product2 * (1 - proba);
        product2 *= proba;
      }

      /*double differenceProba = 1;
      for (int j = 1; j < constraint.length; j++) {
        differenceProba *= static_cast<double>(
            graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[j]) - 1) /
            graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[j]);
      }*/
      double
          sameProba = static_cast<double>(1) / graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
      pruningProbability *= product2 * sameProba;

      approximateCostEliminated += possibleMatch * traversalCost * (1 - pruningProbability);
    }

  }

  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}
void MultipleLabelConstraintEffectiveness::computeStrict(const graph_t &pattern,
                                                         const GraphStat &graphStat,
                                                         const MultipleLabelConstraintCircular &constraint) {
  clear();

  weight_t labelFrom, labelTo;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (int i = 0; i < constraint.length; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    for (int j = 0; j < constraint.length; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];

      findingProbability *= probaFindOnePath(graphStat, uniqueVertexNumber, labelFrom, labelTo);
      uniqueVertexNumber = averageNumberOfUniqueVertex(graphStat, uniqueVertexNumber, labelFrom, labelTo, j == 0);
    }
    // Closure
    labelTo = constraint.vertexLabelVector[(i + constraint.length) % constraint.length];
    findingProbability *= probaFindOneVertex(graphStat, uniqueVertexNumber, labelTo);
    findingProbabilityArray.push_back(findingProbability);
  }
/*
  for(const auto &it : findingProbabilityArray) {
    std::cout << "findingProbabilityArray : "<< it << std::endl;
  }*/

  // Compute number pruned
  for (int i = 0; i < constraint.length; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  for (int i = 0; i < constraint.length; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double uniqueVertexNumber = 1;
    double cost = uniqueVertexNumber;
    for (int j = 0; j < constraint.length; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];
      uniqueVertexNumber = averageNumberOfUniqueVertex(graphStat, uniqueVertexNumber, labelFrom, labelTo, 1);
      cost *= uniqueVertexNumber;
    }

    auto costIncrease = possibleMatch * cost * (findingProbabilityArray[i]);
    costIncrease/=threadNumber;
    costIncrease/=1+ static_cast<double>(constraint.length-1)/2 * findingProbabilityArray[i];
    approximateCostEliminated += costIncrease;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}
void MultipleLabelConstraintEffectiveness::computeBacktrack(const graph_t &pattern,
                                                            const GraphStat &graphStat,
                                                            const MultipleLabelConstraintCircular &constraint) {
  clear();

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (int i = 0; i < constraint.length; i++) {
    double findingProbability = 1.;
    for (int j = 0; j < constraint.length; j++) {
      findingProbability *= probaFindVertex(graphStat,
                                            constraint.vertexLabelVector[(i + j) % constraint.length],
                                            constraint.vertexLabelVector[(i + j + 1) % constraint.length]);
    }
    // Closure
    findingProbability *= probaFindDifferentVertex(graphStat,
                                                   constraint.vertexLabelVector[(i + constraint.length)
                                                       % constraint.length],
                                                   constraint.vertexLabelVector[(i + constraint.length + 1)
                                                       % constraint.length]);
    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  for (int i = 0; i < constraint.length; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;

}

double MultipleLabelConstraintEffectiveness::probaFindEdge(
    const GraphStat &graphStat, const weight_t &labelFrom, const weight_t &labelTo) const {
  return static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo)) /
      graphStat.edgeOutboundLabelTotalNumberMap.at(labelFrom);
}

double MultipleLabelConstraintEffectiveness::probaFindVertex(
    const GraphStat &graphStat, const weight_t &labelFrom, const weight_t &labelTo) const {
  return static_cast<double>(graphStat.vertexLabelWithAtLeastNeighborLabelMap.at(labelFrom).at(labelTo).at(1))
      / graphStat.vertexLabelTotalNumberMap.at(labelFrom);
}
double MultipleLabelConstraintEffectiveness::probaFindDifferentVertex(
    const GraphStat &graphStat, const weight_t &labelFrom, const weight_t &labelTo) const {
  return 1 - pow(static_cast<double>(graphStat.vertexLabelTotalNumberMap.at(labelTo) - 1)
                     / graphStat.vertexLabelTotalNumberMap.at(labelTo),
                 graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo)
                     / graphStat.vertexLabelTotalNumberMap.at(labelFrom));
}

double MultipleLabelConstraintEffectiveness::probaFindOnePath(const GraphStat &graphStat,
                                                              const double labelFromNumber,
                                                              const weight_t labelFrom,
                                                              const weight_t labelTo) const {
  return 1 - pow(1 - probaFindVertex(graphStat, labelFrom, labelTo), labelFromNumber);
}
double MultipleLabelConstraintEffectiveness::probaFindOneVertex(const GraphStat &graphStat,
                                                                const double labelFromNumber,
                                                                const weight_t label) const {
  auto setSize = static_cast<double>(graphStat.vertexLabelTotalNumberMap.at(label));
  return 1 - pow((setSize - 1) / setSize, labelFromNumber);
}

double MultipleLabelConstraintEffectiveness::averageNumberOfUniqueVertex(const GraphStat &graphStat,
                                                                         const double labelFromNumber,
                                                                         const weight_t labelFrom,
                                                                         const weight_t labelTo,
                                                                         const bool forceUnique) const {
  auto maximumVertexToNumber = static_cast<double>(graphStat.vertexLabelTotalNumberMap.at(labelTo));
  auto tries = labelFromNumber * static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
      / maximumVertexToNumber;
  if (forceUnique) return tries;
  return averageDifferentVertexFromTriesApproximateStrong(maximumVertexToNumber, tries);
}

void MultipleLabelConstraintEffectiveness::print(std::ostream &ostream) const {
  ostream << "MultipleLabelConstraintEffectiveness:" << std::endl;
  ostream << "\tapproximateMatchEliminated : " << approximateMatchEliminated << std::endl;
  ostream << "\tapproximateCostEliminated : " << approximateCostEliminated << std::endl;
  ostream << "\tapproximateEffectiveness : " << approximateEffectiveness << std::endl;
  ostream << std::endl;
}

}