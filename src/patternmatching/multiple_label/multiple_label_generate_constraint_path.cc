//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_generate_constraint_path.h"
#include <iostream>
#include "common_utils.h"

namespace patternmatching {

void MultipleLabelGenerateConstraintPath::buildConstraintList(
    const graph_t &pattern,
    const vid_t &sourceVertexId,
    const vid_t &currentVertexId,
    const size_t &currentLength,
    FixedBitmapType &patternTraversalVector,
    std::vector<vid_t> &historyVertexId,
    std::vector<weight_t> &historyVertexLabel,
    std::vector<MultipleLabelConstraintPath> &constraintVector) {

  // Close the loop
  if (currentLength >= 2) {
    weight_t sourceLabel = pattern.values[sourceVertexId];
    weight_t currentLabel = pattern.values[currentVertexId];
    if (sourceLabel == currentLabel && sourceVertexId != currentVertexId) {

      MultipleLabelConstraintPath constraint = MultipleLabelConstraintPath(historyVertexId, historyVertexLabel);

      if (!isInVector(constraintVector, constraint)) constraintVector.push_back(constraint);

      return;
    }
  }

  for (eid_t neighborEdgeId = pattern.vertices[currentVertexId]; neighborEdgeId < pattern.vertices[currentVertexId + 1];
       neighborEdgeId++) {
    vid_t neighborVertexId = pattern.edges[neighborEdgeId];
    weight_t neighborLabel = pattern.values[neighborVertexId];

    if (patternTraversalVector.isIn(neighborVertexId)) continue;

    historyVertexId.push_back(neighborVertexId);
    historyVertexLabel.push_back(neighborLabel);
    patternTraversalVector.insert(neighborVertexId);

    buildConstraintList(pattern, sourceVertexId,
                        neighborVertexId, currentLength + 1, patternTraversalVector,
                        historyVertexId, historyVertexLabel, constraintVector);

    patternTraversalVector.erase(neighborVertexId);
    historyVertexId.pop_back();
    historyVertexLabel.pop_back();
  }
}

void MultipleLabelGenerateConstraintPath::preprocessPatern(const graph_t &pattern) {
  // for loop
  std::vector<vid_t> historyVertexId;
  std::vector<weight_t> historyVertexLabel;
  FixedBitmapType patternTraversalVector;

  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];

    historyVertexId.push_back(vertexId);
    historyVertexLabel.push_back(currentLabel);

    patternTraversalVector.clear();
    patternTraversalVector.insert(vertexId);
    buildConstraintList(pattern,
                        vertexId,
                        vertexId,
                        0,
                        patternTraversalVector,
                        historyVertexId,
                        historyVertexLabel,
                        pathConstraintVector);

    historyVertexId.pop_back();
    historyVertexLabel.pop_back();
  }
}

void MultipleLabelGenerateConstraintPath::print(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << pathConstraintVector.size() << ". " << std::endl;
  for (const auto &it : pathConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    ++currentConstraint;
  }
}

int MultipleLabelGenerateConstraintPath::getConstraintNumber() const {
  return pathConstraintVector.size();
}

const std::vector<MultipleLabelConstraintPath> &MultipleLabelGenerateConstraintPath::getConstraintVector() const {
  return pathConstraintVector;
}

}