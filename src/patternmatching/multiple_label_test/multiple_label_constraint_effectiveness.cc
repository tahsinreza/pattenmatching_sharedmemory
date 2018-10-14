//
// Created by qiu on 21/06/18.
//

#include "multiple_label_constraint_effectiveness.h"
#include <cmath>

#define DEBUG_PRINT(X) std::cout << #X << " : " << X <<std::endl;

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

  weight_t labelFrom, labelTo, labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (int i = 0; i < constraint.length; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    for (int j = 0; j < constraint.length; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];

      if (j == 0) {
        findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        uniqueVertexNumber =
            averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        labelPrevious = constraint.vertexLabelVector[(i + j - 1) % constraint.length];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      edgeKnownNumber,
                                                      0);
        uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                uniqueVertexNumber,
                                                                labelFrom,
                                                                labelTo,
                                                                edgeKnownNumber,
                                                                0);

      }
    }
    // Closure
    labelTo = constraint.vertexLabelVector[(i + constraint.length) % constraint.length];
    findingProbability *= probaFindSameVertex(graphStat, uniqueVertexNumber, labelTo);

    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  for (int i = 0; i < constraint.length; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  for (int i = 0; i < constraint.length; i++) {
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    for (int j = 0; j < constraint.length - 1; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];
      cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

      if (j == 0) {
        traversedVertexNumber =
            averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        labelPrevious = constraint.vertexLabelVector[(i + j - 1) % constraint.length];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        if (edgeKnownNumber == 0) {
          traversedVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                     traversedVertexNumber,
                                                                     labelFrom,
                                                                     labelTo,
                                                                     0,
                                                                     0);
        } else {
          traversedVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
              / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
              + averageNumberOfUniqueVertexKnowing(graphStat,
                                                   traversedVertexNumber,
                                                   labelFrom,
                                                   labelTo,
                                                   0,
                                                   0);
        }
      }
    }
    // Closure
    labelFrom = constraint.vertexLabelVector[(i + constraint.length - 1) % constraint.length];
    cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[i]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(constraint.length - 1) / 2 * (findingProbabilityArray[i]);
    approximateCostEliminated += costIncrease;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;

}
void MultipleLabelConstraintEffectiveness::computeStrict(const graph_t &pattern,
                                                         const GraphStat &graphStat,
                                                         const MultipleLabelConstraintCircular &constraint) {
  clear();

  weight_t labelFrom, labelTo, labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (int i = 0; i < constraint.length; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    std::map<weight_t, size_t> visitedLabelNumber;
    for (int j = 0; j < constraint.length; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];

      if (j == 0) {
        findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        uniqueVertexNumber =
            averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        labelPrevious = constraint.vertexLabelVector[(i + j - 1) % constraint.length];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);
        size_t vertexKnownNumber =
            (visitedLabelNumber[labelTo] > edgeKnownNumber) ? visitedLabelNumber[labelTo] - edgeKnownNumber : 0;

        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      edgeKnownNumber,
                                                      vertexKnownNumber);
        uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                uniqueVertexNumber,
                                                                labelFrom,
                                                                labelTo,
                                                                edgeKnownNumber,
                                                                vertexKnownNumber);

      }
      visitedLabelNumber[labelFrom] += 1;
    }
    // Closure
    labelTo = constraint.vertexLabelVector[(i + constraint.length) % constraint.length];
    findingProbability *= probaFindSameVertex(graphStat, uniqueVertexNumber, labelTo);

    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  for (int i = 0; i < constraint.length; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  for (int i = 0; i < constraint.length; i++) {
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    std::map<weight_t, size_t> visitedLabelNumber;
    for (int j = 0; j < constraint.length - 1; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];
      cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

      if (j == 0) {
        traversedVertexNumber =
            averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        labelPrevious = constraint.vertexLabelVector[(i + j - 1) % constraint.length];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);
        size_t vertexKnownNumber =
            (visitedLabelNumber[labelTo] > edgeKnownNumber) ? visitedLabelNumber[labelTo] - edgeKnownNumber : 0;

        traversedVertexNumber = averageNumberOfVertexKnowing(graphStat,
                                                             traversedVertexNumber,
                                                             labelFrom,
                                                             labelTo,
                                                             edgeKnownNumber,
                                                             vertexKnownNumber);
      }
      visitedLabelNumber[labelFrom] += 1;
    }
    // Closure
    labelFrom = constraint.vertexLabelVector[(i + constraint.length - 1) % constraint.length];
    cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[i]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(constraint.length - 1) / 2 * (findingProbabilityArray[i]);
    approximateCostEliminated += costIncrease;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}
void MultipleLabelConstraintEffectiveness::computeBacktrack(const graph_t &pattern,
                                                            const GraphStat &graphStat,
                                                            const MultipleLabelConstraintCircular &constraint) {
  clear();

  weight_t labelFrom, labelTo, labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (int i = 0; i < constraint.length; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    for (int j = 0; j < constraint.length; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];

      if (j == 0) {
        findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        uniqueVertexNumber =
            averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        labelPrevious = constraint.vertexLabelVector[(i + j - 1) % constraint.length];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        if (edgeKnownNumber == 0) {
          findingProbability *= probaFindOnePathKnowing(graphStat,
                                                        uniqueVertexNumber,
                                                        labelFrom,
                                                        labelTo,
                                                        0,
                                                        0);
          uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                  uniqueVertexNumber,
                                                                  labelFrom,
                                                                  labelTo,
                                                                  0,
                                                                  0);
        } else {
          uniqueVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
              / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
              + averageNumberOfUniqueVertexKnowing(graphStat,
                                                   uniqueVertexNumber,
                                                   labelFrom,
                                                   labelTo,
                                                   0,
                                                   0);
        }

      }
    }
    // Closure
    labelTo = constraint.vertexLabelVector[(i + constraint.length) % constraint.length];
    findingProbability *= probaFindSameVertex(graphStat, uniqueVertexNumber, labelTo);

    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  for (int i = 0; i < constraint.length; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  for (int i = 0; i < constraint.length; i++) {
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    for (int j = 0; j < constraint.length - 1; j++) {
      labelFrom = constraint.vertexLabelVector[(i + j) % constraint.length];
      labelTo = constraint.vertexLabelVector[(i + j + 1) % constraint.length];
      cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

      if (j == 0) {
        traversedVertexNumber =
            averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        labelPrevious = constraint.vertexLabelVector[(i + j - 1) % constraint.length];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        if (edgeKnownNumber == 0) {
          traversedVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                     traversedVertexNumber,
                                                                     labelFrom,
                                                                     labelTo,
                                                                     0,
                                                                     0);
        } else {
          traversedVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
              / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
              + averageNumberOfUniqueVertexKnowing(graphStat,
                                                   traversedVertexNumber,
                                                   labelFrom,
                                                   labelTo,
                                                   0,
                                                   0);
        }
      }
    }
    // Closure
    labelFrom = constraint.vertexLabelVector[(i + constraint.length - 1) % constraint.length];
    cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[i]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(constraint.length - 1) / 2 * (findingProbabilityArray[i]);
    approximateCostEliminated += costIncrease;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}

void MultipleLabelConstraintEffectiveness::compute(const graph_t &pattern,
                                                   const GraphStat &graphStat,
                                                   const MultipleLabelConstraintPath &constraint) {
  clear();

  bool reverse;
  weight_t labelFrom, labelTo, labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  reverse = false;
  for (int i = 0; i <= 1; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    for (int j = 0; j < constraint.length - 1; j++) {
      auto curPos = (reverse) ? constraint.length - j - 1 : j;
      auto nextPos = (reverse) ? constraint.length - j - 2 : j + 1;
      labelFrom = constraint.vertexLabelVector[curPos];
      labelTo = constraint.vertexLabelVector[nextPos];

      if (j == 0) {
        findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        uniqueVertexNumber =
            averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        auto previousPos = (reverse) ? constraint.length - j : j - 1;
        labelPrevious = constraint.vertexLabelVector[previousPos];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      edgeKnownNumber,
                                                      0);
        uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                uniqueVertexNumber,
                                                                labelFrom,
                                                                labelTo,
                                                                edgeKnownNumber,
                                                                0);

      }
    }
    // Closure
    labelTo = constraint.vertexLabelVector[0];
    findingProbability *= probaFindDifferentVertex(graphStat, uniqueVertexNumber, labelTo);

    findingProbabilityArray.push_back(findingProbability);
    reverse = true;
  }

  // Compute number pruned
  for (int i = 0; i <= 1; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[0]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  reverse = false;
  for (int i = 0; i <= 1; i++) {
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    for (int j = 0; j < constraint.length - 2; j++) {
      auto curPos = (reverse) ? constraint.length - j - 1 : j;
      auto nextPos = (reverse) ? constraint.length - j - 2 : j + 1;
      labelFrom = constraint.vertexLabelVector[curPos];
      labelTo = constraint.vertexLabelVector[nextPos];
      cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

      if (j == 0) {
        traversedVertexNumber =
            averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        auto previousPos = (reverse) ? constraint.length - j : j - 1;
        labelPrevious = constraint.vertexLabelVector[previousPos];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        if (edgeKnownNumber == 0) {
          traversedVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                     traversedVertexNumber,
                                                                     labelFrom,
                                                                     labelTo,
                                                                     0,
                                                                     0);
        } else {
          traversedVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
              / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
              + averageNumberOfUniqueVertexKnowing(graphStat,
                                                   traversedVertexNumber,
                                                   labelFrom,
                                                   labelTo,
                                                   0,
                                                   0);
        }
      }
    }
    // Closure
    labelFrom = constraint.vertexLabelVector[(i + constraint.length - 1) % constraint.length];
    cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[i]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(constraint.length - 1) / 2 * (findingProbabilityArray[i]);
    approximateCostEliminated += costIncrease;
    reverse = true;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;

}
void MultipleLabelConstraintEffectiveness::computeStrict(const graph_t &pattern,
                                                         const GraphStat &graphStat,
                                                         const MultipleLabelConstraintPath &constraint) {
  clear();

  bool reverse;
  weight_t labelFrom, labelTo, labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  reverse = false;
  for (int i = 0; i <= 1; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    std::map<weight_t, size_t> visitedLabelNumber;
    for (int j = 0; j < constraint.length - 2; j++) {
      auto curPos = (reverse) ? constraint.length - j - 1 : j;
      auto nextPos = (reverse) ? constraint.length - j - 2 : j + 1;
      labelFrom = constraint.vertexLabelVector[curPos];
      labelTo = constraint.vertexLabelVector[nextPos];

      if (j == 0) {
        findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        uniqueVertexNumber =
            averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        auto previousPos = (reverse) ? constraint.length - j : j - 1;
        labelPrevious = constraint.vertexLabelVector[previousPos];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);
        size_t vertexKnownNumber =
            (visitedLabelNumber[labelTo] > edgeKnownNumber) ? visitedLabelNumber[labelTo] - edgeKnownNumber : 0;

        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      edgeKnownNumber,
                                                      vertexKnownNumber);
        uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                uniqueVertexNumber,
                                                                labelFrom,
                                                                labelTo,
                                                                edgeKnownNumber,
                                                                vertexKnownNumber);

      }
      visitedLabelNumber[labelFrom] += 1;
    }
    // Closure
    labelTo = constraint.vertexLabelVector[(i + constraint.length) % constraint.length];
    findingProbability *= probaFindDifferentVertex(graphStat, uniqueVertexNumber, labelTo);

    findingProbabilityArray.push_back(findingProbability);
    reverse = true;
  }

  // Compute number pruned
  for (int i = 0; i <= 1; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  reverse = false;
  for (int i = 0; i <= 1; i++) {
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    std::map<weight_t, size_t> visitedLabelNumber;
    for (int j = 0; j < constraint.length - 2; j++) {
      auto curPos = (reverse) ? constraint.length - j - 1 : j;
      auto nextPos = (reverse) ? constraint.length - j - 2 : j + 1;
      labelFrom = constraint.vertexLabelVector[curPos];
      labelTo = constraint.vertexLabelVector[nextPos];
      cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

      if (j == 0) {
        traversedVertexNumber =
            averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        auto previousPos = (reverse) ? constraint.length - j : j - 1;
        labelPrevious = constraint.vertexLabelVector[previousPos];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);
        size_t vertexKnownNumber =
            (visitedLabelNumber[labelTo] > edgeKnownNumber) ? visitedLabelNumber[labelTo] - edgeKnownNumber : 0;

        traversedVertexNumber = averageNumberOfVertexKnowing(graphStat,
                                                             traversedVertexNumber,
                                                             labelFrom,
                                                             labelTo,
                                                             edgeKnownNumber,
                                                             vertexKnownNumber);
      }
      visitedLabelNumber[labelFrom] += 1;
    }
    // Closure
    labelFrom = constraint.vertexLabelVector[(i + constraint.length - 1) % constraint.length];
    cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[i]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(constraint.length - 1) / 2 * (findingProbabilityArray[i]);
    approximateCostEliminated += costIncrease;
    reverse = true;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}
void MultipleLabelConstraintEffectiveness::computeBacktrack(const graph_t &pattern,
                                                            const GraphStat &graphStat,
                                                            const MultipleLabelConstraintPath &constraint) {
  clear();

  bool reverse;
  weight_t labelFrom, labelTo, labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  reverse = false;
  for (int i = 0; i <= 1; i++) {
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    for (int j = 0; j < constraint.length - 2; j++) {
      auto curPos = (reverse) ? constraint.length - j - 1 : j;
      auto nextPos = (reverse) ? constraint.length - j - 2 : j + 1;
      labelFrom = constraint.vertexLabelVector[curPos];
      labelTo = constraint.vertexLabelVector[nextPos];

      if (j == 0) {
        findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        uniqueVertexNumber =
            averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        auto previousPos = (reverse) ? constraint.length - j : j - 1;
        labelPrevious = constraint.vertexLabelVector[previousPos];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        if (edgeKnownNumber == 0) {
          findingProbability *= probaFindOnePathKnowing(graphStat,
                                                        uniqueVertexNumber,
                                                        labelFrom,
                                                        labelTo,
                                                        0,
                                                        0);
          uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                  uniqueVertexNumber,
                                                                  labelFrom,
                                                                  labelTo,
                                                                  0,
                                                                  0);
        } else {
          uniqueVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
              / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
              + averageNumberOfUniqueVertexKnowing(graphStat,
                                                   uniqueVertexNumber,
                                                   labelFrom,
                                                   labelTo,
                                                   0,
                                                   0);
        }

      }
    }
    // Closure
    labelTo = constraint.vertexLabelVector[(i + constraint.length) % constraint.length];
    findingProbability *= probaFindDifferentVertex(graphStat, uniqueVertexNumber, labelTo);

    findingProbabilityArray.push_back(findingProbability);
    reverse = true;
  }

  // Compute number pruned
  for (int i = 0; i <= 1; i++) {
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[i]);
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  reverse = false;
  for (int i = 0; i <= 1; i++) {
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    for (int j = 0; j < constraint.length - 2; j++) {
      auto curPos = (reverse) ? constraint.length - j - 1 : j;
      auto nextPos = (reverse) ? constraint.length - j - 2 : j + 1;
      labelFrom = constraint.vertexLabelVector[curPos];
      labelTo = constraint.vertexLabelVector[nextPos];
      cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

      if (j == 0) {
        traversedVertexNumber =
            averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
      } else {
        auto previousPos = (reverse) ? constraint.length - j : j - 1;
        labelPrevious = constraint.vertexLabelVector[previousPos];
        auto edgeKnownNumber = static_cast<size_t>(labelPrevious == labelTo);

        if (edgeKnownNumber == 0) {
          traversedVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                     traversedVertexNumber,
                                                                     labelFrom,
                                                                     labelTo,
                                                                     0,
                                                                     0);
        } else {
          traversedVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
              / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
              + averageNumberOfUniqueVertexKnowing(graphStat,
                                                   traversedVertexNumber,
                                                   labelFrom,
                                                   labelTo,
                                                   0,
                                                   0);
        }
      }
    }
    // Closure
    labelFrom = constraint.vertexLabelVector[(i + constraint.length - 1) % constraint.length];
    cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(constraint.vertexLabelVector[i]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[i]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(constraint.length - 1) / 2 * (findingProbabilityArray[i]);
    approximateCostEliminated += costIncrease;
    reverse = true;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}

void MultipleLabelConstraintEffectiveness::compute(const graph_t &pattern,
                                                   const GraphStat &graphStat,
                                                   const MultipleLabelConstraintTemplate &constraint) {
  clear();

  weight_t labelFrom, labelTo;
  //weight_t labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    labelFrom = walk.vertexLabelVector[0];
    std::vector<double> uniqueVertexVector;
    uniqueVertexVector.push_back(uniqueVertexNumber);
    for (int j = 0; j < walk.length - 1; j++) {
      auto walkType = walk.moveVector[j + 1];
      if (walkType == Walk::E_VERTEX_STORE || walkType == Walk::E_VERTEX_NO_STORE) {
        labelTo = walk.vertexLabelVector[j + 1];

        if (j == 0) {
          findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
          uniqueVertexNumber =
              averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        } else {
          //labelPrevious = walk.vertexLabelVector[j - 1];
          auto edgeKnownNumber = 0; //static_cast<size_t>(labelPrevious == labelTo);

          findingProbability *= probaFindOnePathKnowing(graphStat,
                                                        uniqueVertexNumber,
                                                        labelFrom,
                                                        labelTo,
                                                        edgeKnownNumber,
                                                        0);
          uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                  uniqueVertexNumber,
                                                                  labelFrom,
                                                                  labelTo,
                                                                  edgeKnownNumber,
                                                                  0);

        }
        labelFrom = labelTo;
        uniqueVertexVector.push_back(uniqueVertexNumber);
      } else if (walkType == Walk::E_MOVE_BACK) {
        auto moveBackIndex = walk.moveBackIndexVector[j + 1];
        uniqueVertexNumber = uniqueVertexVector[moveBackIndex];
        labelFrom = walk.historyLabelVector[moveBackIndex];
      } else if (walkType == Walk::E_CHECK) {
        labelTo = walk.vertexLabelVector[j + 1];
        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      0,
                                                      0)
            * probaFindSameVertex(graphStat, uniqueVertexNumber, labelTo);
      }
    }
    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  size_t index = 0;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(walk.vertexLabelVector[0]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[index]);
    index++;
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  index = 0;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    labelFrom = walk.vertexLabelVector[0];
    std::vector<double> traversalVertexVector;
    traversalVertexVector.push_back(traversedVertexNumber);
    for (int j = 0; j < walk.length - 2; j++) {
      auto walkType = walk.moveVector[j + 1];
      if (walkType == Walk::E_VERTEX_STORE || walkType == Walk::E_VERTEX_NO_STORE) {
        labelTo = walk.vertexLabelVector[j + 1];
        cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

        if (j == 0) {
          traversedVertexNumber =
              averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
        } else {
          //labelPrevious = walk.vertexLabelVector[j - 1];
          auto edgeKnownNumber = 0; //static_cast<size_t>(labelPrevious == labelTo);

          if (edgeKnownNumber == 0) {
            traversedVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                       traversedVertexNumber,
                                                                       labelFrom,
                                                                       labelTo,
                                                                       0,
                                                                       0);
          } else {
            traversedVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
                / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
                + averageNumberOfUniqueVertexKnowing(graphStat,
                                                     traversedVertexNumber,
                                                     labelFrom,
                                                     labelTo,
                                                     0,
                                                     0);
          }
        }
        traversalVertexVector.push_back(traversedVertexNumber);
        labelFrom = labelTo;
      }else if (walkType == Walk::E_MOVE_BACK) {
        auto moveBackIndex = walk.moveBackIndexVector[j + 1];
        traversedVertexNumber = traversalVertexVector[moveBackIndex];
        labelFrom = walk.historyLabelVector[moveBackIndex];
      } else if (walkType == Walk::E_CHECK) {
        cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);
      }
    }
    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(walk.vertexLabelVector[0]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[index]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(walk.vertexLength - 1) / 2 * (findingProbabilityArray[index]);
    approximateCostEliminated += costIncrease;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;

}

void MultipleLabelConstraintEffectiveness::computeStrict(const graph_t &pattern,
                                                         const GraphStat &graphStat,
                                                         const MultipleLabelConstraintTemplate &constraint) {
  clear();

  weight_t labelFrom, labelTo;
  //weight_t labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    labelFrom = walk.vertexLabelVector[0];

    std::map<weight_t, size_t> visitedLabelNumber;
    std::vector<double> uniqueVertexVector;
    uniqueVertexVector.push_back(uniqueVertexNumber);
    for (int j = 0; j < walk.length - 2; j++) {
      auto walkType = walk.moveVector[j + 1];

      if (walkType == Walk::E_VERTEX_STORE || walkType == Walk::E_VERTEX_NO_STORE) {
        labelTo = walk.vertexLabelVector[j + 1];
        if (j == 0) {
          findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
          uniqueVertexNumber =
              averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        } else {
          //labelPrevious = walk.vertexLabelVector[j - 1];
          auto edgeKnownNumber = 0; //static_cast<size_t>(labelPrevious == labelTo);
          size_t vertexKnownNumber =
              (visitedLabelNumber[labelTo] > edgeKnownNumber) ? visitedLabelNumber[labelTo] - edgeKnownNumber : 0;

          findingProbability *= probaFindOnePathKnowing(graphStat,
                                                        uniqueVertexNumber,
                                                        labelFrom,
                                                        labelTo,
                                                        edgeKnownNumber,
                                                        vertexKnownNumber);
          uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                  uniqueVertexNumber,
                                                                  labelFrom,
                                                                  labelTo,
                                                                  edgeKnownNumber,
                                                                  vertexKnownNumber);

        }
        labelFrom = labelTo;
        uniqueVertexVector.push_back(uniqueVertexNumber);
        visitedLabelNumber[labelFrom] += 1;
      } else if (walkType == Walk::E_MOVE_BACK) {
        auto moveBackIndex = walk.moveBackIndexVector[j + 1];
        uniqueVertexNumber = uniqueVertexVector[moveBackIndex];
        labelFrom = walk.historyLabelVector[moveBackIndex];
      } else if (walkType == Walk::E_CHECK) {
        labelTo = walk.vertexLabelVector[j + 1];
        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      0,
                                                      0)
            * probaFindSameVertex(graphStat, uniqueVertexNumber, labelTo);
      }
    }
    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  size_t index = 0;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(walk.vertexLabelVector[0]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[index]);
    index++;
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  index = 0;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    labelFrom = walk.vertexLabelVector[0];
    std::map<weight_t, size_t> visitedLabelNumber;
    std::vector<double> traversalVertexVector;
    traversalVertexVector.push_back(traversedVertexNumber);
    for (int j = 0; j < walk.length - 2; j++) {
      auto walkType = walk.moveVector[j + 1];
      if (walkType == Walk::E_VERTEX_STORE || walkType == Walk::E_VERTEX_NO_STORE) {
        labelTo = walk.vertexLabelVector[j + 1];
        cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

        if (j == 0) {
          traversedVertexNumber =
              averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
        } else {
          //labelPrevious = walk.vertexLabelVector[j - 1];
          auto edgeKnownNumber = 0; //static_cast<size_t>(labelPrevious == labelTo);
          size_t vertexKnownNumber =
              (visitedLabelNumber[labelTo] > edgeKnownNumber) ? visitedLabelNumber[labelTo] - edgeKnownNumber : 0;

          traversedVertexNumber = averageNumberOfVertexKnowing(graphStat,
                                                               traversedVertexNumber,
                                                               labelFrom,
                                                               labelTo,
                                                               edgeKnownNumber,
                                                               vertexKnownNumber);
        }
        visitedLabelNumber[labelFrom] += 1;
        traversalVertexVector.push_back(traversedVertexNumber);
        labelFrom = labelTo;
      } else if (walkType == Walk::E_MOVE_BACK) {
        auto moveBackIndex = walk.moveBackIndexVector[j + 1];
        traversedVertexNumber = traversalVertexVector[moveBackIndex];
        labelFrom = walk.historyLabelVector[moveBackIndex];
      } else if (walkType == Walk::E_CHECK) {
        cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);
      }
    }

    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(walk.vertexLabelVector[0]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[index]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(walk.vertexLength - 1) / 2 * (findingProbabilityArray[index]);
    approximateCostEliminated += costIncrease;
    index++;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}

void MultipleLabelConstraintEffectiveness::computeBacktrack(const graph_t &pattern,
                                                            const GraphStat &graphStat,
                                                            const MultipleLabelConstraintTemplate &constraint) {
  clear();

  weight_t labelFrom, labelTo;
  //weight_t labelPrevious;

  // Compute finding probability array
  std::vector<double> findingProbabilityArray;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    double findingProbability = 1.;
    double uniqueVertexNumber = 1;
    labelFrom = walk.vertexLabelVector[0];
    std::vector<double> uniqueVertexVector;
    uniqueVertexVector.push_back(uniqueVertexNumber);
    for (int j = 0; j < walk.length - 2; j++) {
      auto walkType = walk.moveVector[j + 1];
      if (walkType == Walk::E_VERTEX_STORE || walkType == Walk::E_VERTEX_NO_STORE) {
        labelTo = walk.vertexLabelVector[j + 1];

        if (j == 0) {
          findingProbability *= probaFindOnePathKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
          uniqueVertexNumber =
              averageNumberOfUniqueVertexKnowing(graphStat, uniqueVertexNumber, labelFrom, labelTo, 0, 0);
        } else {
          //labelPrevious = walk.vertexLabelVector[j - 1];
          auto edgeKnownNumber = 0; //static_cast<size_t>(labelPrevious == labelTo);

          if (edgeKnownNumber == 0) {
            findingProbability *= probaFindOnePathKnowing(graphStat,
                                                          uniqueVertexNumber,
                                                          labelFrom,
                                                          labelTo,
                                                          0,
                                                          0);
            uniqueVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                    uniqueVertexNumber,
                                                                    labelFrom,
                                                                    labelTo,
                                                                    0,
                                                                    0);
          } else {
            uniqueVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
                / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
                + averageNumberOfUniqueVertexKnowing(graphStat,
                                                     uniqueVertexNumber,
                                                     labelFrom,
                                                     labelTo,
                                                     0,
                                                     0);
          }
        }
        labelFrom = labelTo;
        uniqueVertexVector.push_back(uniqueVertexNumber);
      } else if (walkType == Walk::E_MOVE_BACK) {
        auto moveBackIndex = walk.moveBackIndexVector[j + 1];
        uniqueVertexNumber = uniqueVertexVector[moveBackIndex];
        labelFrom = walk.historyLabelVector[moveBackIndex];
      } else if (walkType == Walk::E_CHECK) {
        labelTo = walk.vertexLabelVector[j + 1];
        findingProbability *= probaFindOnePathKnowing(graphStat,
                                                      uniqueVertexNumber,
                                                      labelFrom,
                                                      labelTo,
                                                      0,
                                                      0)
            * probaFindSameVertex(graphStat, uniqueVertexNumber, labelTo);
      }
    }
    findingProbabilityArray.push_back(findingProbability);
  }

  // Compute number pruned
  size_t index = 0;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(walk.vertexLabelVector[0]);
    approximateMatchEliminated += possibleMatch * (1 - findingProbabilityArray[index]);
    index++;
  }

  // Compute cost
  auto threadNumber = omp_get_max_threads();
  index = 0;
  for (const auto &it : constraint.walkMap) {
    const Walk walk = it.second;
    double traversedVertexNumber = 1;
    double cost = traversedVertexNumber;
    labelFrom = walk.vertexLabelVector[0];
    std::vector<double> traversalVertexVector;
    traversalVertexVector.push_back(traversedVertexNumber);
    for (int j = 0; j < walk.length - 2; j++) {
      auto walkType = walk.moveVector[j + 1];
      if (walkType == Walk::E_VERTEX_STORE || walkType == Walk::E_VERTEX_NO_STORE) {
        labelTo = walk.vertexLabelVector[j + 1];
        cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);

        if (j == 0) {
          traversedVertexNumber =
              averageNumberOfVertexKnowing(graphStat, traversedVertexNumber, labelFrom, labelTo, 0, 0);
        } else {
          //labelPrevious = walk.vertexLabelVector[j - 1];
          auto edgeKnownNumber = 0; //static_cast<size_t>(labelPrevious == labelTo);

          if (edgeKnownNumber == 0) {
            traversedVertexNumber = averageNumberOfUniqueVertexKnowing(graphStat,
                                                                       traversedVertexNumber,
                                                                       labelFrom,
                                                                       labelTo,
                                                                       0,
                                                                       0);
          } else {
            traversedVertexNumber = 1 - static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
                / graphStat.vertexLabelTotalNumberMap.at(labelFrom)
                + averageNumberOfUniqueVertexKnowing(graphStat,
                                                     traversedVertexNumber,
                                                     labelFrom,
                                                     labelTo,
                                                     0,
                                                     0);
          }
        }
        traversalVertexVector.push_back(traversedVertexNumber);
        labelFrom = labelTo;
      }else if (walkType == Walk::E_MOVE_BACK) {
        auto moveBackIndex = walk.moveBackIndexVector[j + 1];
        traversedVertexNumber = traversalVertexVector[moveBackIndex];
        labelFrom = walk.historyLabelVector[moveBackIndex];
      } else if (walkType == Walk::E_CHECK) {
        cost += averageNumberOfVertex(graphStat, traversedVertexNumber, labelFrom);
      }
    }
    cost += traversedVertexNumber;

    size_t possibleMatch = graphStat.vertexLabelTotalNumberMap.at(walk.vertexLabelVector[0]);
    double costIncrease = possibleMatch * cost * (1 - findingProbabilityArray[index]);
    costIncrease /= threadNumber;
    costIncrease /= 1 + static_cast<double>(walk.vertexLength - 1) / 2 * (findingProbabilityArray[index]);
    approximateCostEliminated += costIncrease;
    index++;
  }

  // Compute effectiveness
  approximateEffectiveness = approximateMatchEliminated / approximateCostEliminated;
}

double MultipleLabelConstraintEffectiveness::probaFindVertex(
    const GraphStat &graphStat, const weight_t labelFrom, const weight_t labelTo,
    const size_t knowingEdgeNumber,
    const size_t knowingVertexNumber) const {
  double vertexFullSetSize=graphStat.vertexLabelTotalNumberMap.at(labelFrom);
  double vertexSetSize = (knowingEdgeNumber>1) ? graphStat.vertexLabelWithAtLeastNeighborLabelMap.at(labelFrom).at(labelTo)
      .at(knowingEdgeNumber):vertexFullSetSize;
  auto probaDifferent = pow((vertexFullSetSize-knowingEdgeNumber-knowingVertexNumber)/(vertexFullSetSize-knowingEdgeNumber),1);
  /*for (auto i = 0; i < knowingVertexNumber; i++) {
    probaDifferent *= static_cast<double>(vertexSetSize - i - 1) / vertexSetSize;
  }*/
  return static_cast<double>(graphStat.vertexLabelWithAtLeastNeighborLabelMap.at(labelFrom).at(labelTo)
      .at(knowingEdgeNumber + 1))
      / vertexSetSize * probaDifferent;
}
double MultipleLabelConstraintEffectiveness::probaFindOnePath(const GraphStat &graphStat,
                                                              const double labelFromNumber,
                                                              const weight_t labelFrom,
                                                              const weight_t labelTo) const {
  return probaFindOnePathKnowing(graphStat, labelFromNumber, labelFrom, labelTo, 0, 0);
}
double MultipleLabelConstraintEffectiveness::probaFindOnePathKnowing(const GraphStat &graphStat,
                                                                     const double labelFromNumber,
                                                                     const weight_t labelFrom,
                                                                     const weight_t labelTo,
                                                                     const size_t knowingEdgeNumber,
                                                                     const size_t knowingVertexNumber) const {
  return 1 - pow(1 - probaFindVertex(graphStat, labelFrom, labelTo, knowingEdgeNumber, knowingVertexNumber),
                 labelFromNumber);
}

double MultipleLabelConstraintEffectiveness::probaFindSameVertex(const GraphStat &graphStat,
                                                                 const double labelFromNumber,
                                                                 const weight_t label) const {
  auto vertexSetSize = static_cast<double>(graphStat.vertexLabelTotalNumberMap.at(label));
  return 1 - pow((vertexSetSize - 1) / vertexSetSize, labelFromNumber);
}

double MultipleLabelConstraintEffectiveness::probaFindDifferentVertex(const GraphStat &graphStat,
                                                                      const double labelFromNumber,
                                                                      const weight_t label) const {
  auto vertexSetSize = static_cast<double>(graphStat.vertexLabelTotalNumberMap.at(label));
  return 1 - pow((1.) / vertexSetSize, labelFromNumber);
}

double MultipleLabelConstraintEffectiveness::averageNumberOfVertex(const GraphStat &graphStat,
                                                                   const double labelFromNumber,
                                                                   const weight_t labelFrom) const {
  auto tries = labelFromNumber * static_cast<double>(graphStat.edgeOutboundLabelTotalNumberMap.at(labelFrom))
      / graphStat.vertexLabelTotalNumberMap.at(labelFrom);
  return tries;
}

double MultipleLabelConstraintEffectiveness::averageNumberOfVertex(const GraphStat &graphStat,
                                                                   const double labelFromNumber,
                                                                   const weight_t labelFrom,
                                                                   const weight_t labelTo) const {
  return averageNumberOfVertexKnowing(graphStat, labelFromNumber, labelFrom, labelTo, 0, 0);
}

double MultipleLabelConstraintEffectiveness::averageNumberOfVertexKnowing(const GraphStat &graphStat,
                                                                          const double labelFromNumber,
                                                                          const weight_t labelFrom,
                                                                          const weight_t labelTo,
                                                                          const size_t knowingEdgeNumber,
                                                                          const size_t knowingVertexNumber) const {
  auto probaDifferent = 1;
  auto vertexSetSize = graphStat.vertexLabelTotalNumberMap.at(labelFrom);
  for (auto i = 0; i < knowingVertexNumber; i++) {
    probaDifferent *= static_cast<double>(vertexSetSize - i - 1) / vertexSetSize;
  }
  double tries;
  if (knowingEdgeNumber == 0) {
    tries = labelFromNumber * static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
        / graphStat.vertexLabelTotalNumberMap.at(labelFrom);
  } else {
    tries = labelFromNumber * static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
        / graphStat.vertexLabelWithAtLeastNeighborLabelMap.at(labelFrom).at(labelTo).at(knowingEdgeNumber);
  }
  return tries * probaDifferent;
}

double MultipleLabelConstraintEffectiveness::averageNumberOfUniqueVertex(const GraphStat &graphStat,
                                                                         const double labelFromNumber,
                                                                         const weight_t labelFrom,
                                                                         const weight_t labelTo) const {
  return averageNumberOfUniqueVertexKnowing(graphStat, labelFromNumber, labelFrom, labelTo,
                                            0, 0);
}
double MultipleLabelConstraintEffectiveness::averageNumberOfUniqueVertexKnowing(const GraphStat &graphStat,
                                                                                const double labelFromNumber,
                                                                                const weight_t labelFrom,
                                                                                const weight_t labelTo,
                                                                                const size_t knowingEdgeNumber,
                                                                                const size_t knowingVertexNumber) const {
  auto probaDifferent = 1;
  auto vertexSetSize = graphStat.vertexLabelTotalNumberMap.at(labelFrom);
  for (auto i = 0; i < knowingVertexNumber; i++) {
    probaDifferent *= static_cast<double>(vertexSetSize - i - 1) / vertexSetSize;
  }
  double tries;
  if (knowingEdgeNumber == 0) {
    tries = labelFromNumber * static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
        / graphStat.vertexLabelTotalNumberMap.at(labelFrom);
  } else {
    tries = labelFromNumber * static_cast<double>(graphStat.edgeLabelTotalNumberMap.at(labelFrom).at(labelTo))
        / graphStat.vertexLabelWithAtLeastNeighborLabelMap.at(labelFrom).at(labelTo).at(knowingEdgeNumber);
  }
  auto maximumVertexToNumber = static_cast<double>(graphStat.vertexLabelTotalNumberMap.at(labelTo));
  return averageDifferentVertexFromTriesApproximateStrong(maximumVertexToNumber, tries) * probaDifferent;
}

void MultipleLabelConstraintEffectiveness::print(std::ostream &ostream) const {
  ostream << "MultipleLabelConstraintEffectiveness:" << std::endl;
  ostream << "\tapproximateMatchEliminated : " << approximateMatchEliminated << std::endl;
  ostream << "\tapproximateCostEliminated : " << approximateCostEliminated << std::endl;
  ostream << "\tapproximateEffectiveness : " << approximateEffectiveness << std::endl;
  ostream << std::endl;
}

}