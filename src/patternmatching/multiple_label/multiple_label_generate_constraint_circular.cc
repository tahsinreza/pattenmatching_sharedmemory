//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_generate_constraint_circular.h"
#include <iostream>
#include "common_utils.h"

namespace patternmatching {

void MultipleLabelGenerateConstraintCircular::buildConstraintList(
    const graph_t &pattern,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &remainingLength,
    FixedBitmapType &patternTraversalVector,
    std::vector<vid_t> &historyVertexId,
    std::vector<weight_t> &historyVertexLabel,
    std::vector<MultipleLabelConstraintCircular> &constraintVector) {

  // Close the loop
  if (remainingLength == 0) {
    if (sourceVertexId != currentVertexId) return;

    MultipleLabelConstraintCircular constraint = MultipleLabelConstraintCircular(historyVertexId, historyVertexLabel);

    if (!isInVector(circularConstraintVector, constraint)) constraintVector.push_back(constraint);

    return;
  }

  for (eid_t neighborEdgeId = pattern.vertices[currentVertexId]; neighborEdgeId < pattern.vertices[currentVertexId + 1];
       neighborEdgeId++) {
    vid_t neighborVertexId = pattern.edges[neighborEdgeId];
    weight_t neighborLabel = pattern.values[neighborVertexId];

    if (patternTraversalVector.isIn(neighborVertexId))
      continue;

    historyVertexId.push_back(neighborVertexId);
    historyVertexLabel.push_back(neighborLabel);
    patternTraversalVector.insert(neighborVertexId);

    buildConstraintList(pattern, sourceVertexId,
                        neighborVertexId, remainingLength - 1, patternTraversalVector,
                        historyVertexId, historyVertexLabel, constraintVector);

    patternTraversalVector.erase(neighborVertexId);
    historyVertexId.pop_back();
    historyVertexLabel.pop_back();

  }

}

void MultipleLabelGenerateConstraintCircular::preprocessPatern(const graph_t &pattern) {

  // for loop
  std::vector<vid_t> historyVertexId;
  std::vector<weight_t> historyVertexLabel;
  FixedBitmapType patternTraversalVector;

  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];

    historyVertexId.push_back(vertexId);
    historyVertexLabel.push_back(currentLabel);

    for (size_t currentCycleLength = 3; currentCycleLength <= pattern.vertex_count; currentCycleLength++) {
      patternTraversalVector.clear();
      buildConstraintList(pattern,
                          vertexId,
                          vertexId,
                          currentCycleLength,
                          patternTraversalVector,
                          historyVertexId,
                          historyVertexLabel,
                          circularConstraintVector);
    }

    historyVertexId.pop_back();
    historyVertexLabel.pop_back();
  }
}

void MultipleLabelGenerateConstraintCircular::print(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << circularConstraintVector.size() << ". " << std::endl;
  for (const auto &it : circularConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    ++currentConstraint;
  }
}

int MultipleLabelGenerateConstraintCircular::getConstraintNumber() const {
  return circularConstraintVector.size();
}

const std::vector<MultipleLabelConstraintCircular> &MultipleLabelGenerateConstraintCircular::getConstraintVector() const {
  return circularConstraintVector;
}

}