//
// Created by qiu on 07/06/18.
//

#include "unique_label_circular_constraint.h"

namespace patternmatching {
UniqueLabelCircularConstraint::UniqueLabelCircularConstraint(
    const std::vector<vid_t> &historyVertexId,
    const std::vector<weight_t> &historyVertexLabel) {
  auto vertexIndexVectorCopy = historyVertexId;
  auto vertexLabelVectorCopy = historyVertexLabel;

  // remove the last vertex which is the same as the first one
  vertexIndexVectorCopy.pop_back();
  vertexLabelVectorCopy.pop_back();
  length = vertexIndexVectorCopy.size();

  // find minimum
  size_t minValue = vertexIndexVectorCopy[0];
  size_t minIndex = 0;
  size_t currentValue;
  for (int currentIndex = 1; currentIndex < length; currentIndex++) {
    currentValue = vertexIndexVectorCopy[currentIndex];
    if (currentValue < minValue) {
      minValue = currentValue;
      minIndex = currentIndex;
    }
  }

  // Create a full order
  bool reverse = false;
  if (minIndex > 0 && minIndex < length - 1) {
    reverse = vertexIndexVectorCopy[minIndex - 1] < vertexIndexVectorCopy[minIndex + 1];
  } else if (minIndex == 0) {
    reverse = vertexIndexVectorCopy[length - 1] < vertexIndexVectorCopy[1];
  } else {
    reverse = vertexIndexVectorCopy[length - 2] < vertexIndexVectorCopy[0];
  }

  // Fill in
  vertexIndexVector.resize(length);
  vertexLabelVector.resize(length);

  int copyPosition = minIndex;
  for (int currentPosition = 0; currentPosition < length; currentPosition++) {
    vertexIndexVector[currentPosition] = vertexIndexVectorCopy[copyPosition];
    vertexLabelVector[currentPosition] = vertexLabelVectorCopy[copyPosition];

    if (reverse) { --copyPosition; } else { ++copyPosition; }
    if (copyPosition == length) copyPosition = 0;
    if (copyPosition == -1) copyPosition = length - 1;
  }

}

bool UniqueLabelCircularConstraint::operator==(const UniqueLabelCircularConstraint &other) const {
  return (length == other.length) && (vertexIndexVector == other.vertexIndexVector);
}

void UniqueLabelCircularConstraint::print(std::ostream &ostream) const {
  ostream << "Vertex Index : ";
  for (const auto &it : vertexIndexVector) {
    ostream << it << " -> ";
  }
  ostream << vertexIndexVector[0] << std::endl;

  ostream << "Vertex Label : ";
  for (const auto &it : vertexLabelVector) {
    ostream << it << " -> ";
  }
  ostream << vertexLabelVector[0] << std::endl;
}
}