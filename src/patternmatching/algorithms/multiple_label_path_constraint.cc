//
// Created by qiu on 07/06/18.
//

#include "multiple_label_path_constraint.h"

namespace patternmatching {

PathConstraint::PathConstraint(
    const std::vector<vid_t> &historyVertexId,
    const std::vector<weight_t> &historyVertexLabel) {
  initialLabel = historyVertexLabel[0];
  length = historyVertexId.size();

  // Create a full order
  bool reverse = historyVertexId[length - 1] > historyVertexId[0];

  // Fill in
  vertexIndexVector.resize(length);
  vertexLabelVector.resize(length);
  if (reverse) {
    for (int currentPosition = 0; currentPosition < length; currentPosition++) {
      vertexIndexVector[currentPosition] = historyVertexId[length - 1 - currentPosition];
      vertexLabelVector[currentPosition] = historyVertexLabel[length - 1 - currentPosition];
    }
  } else {
    vertexIndexVector = historyVertexId;
    vertexLabelVector = historyVertexLabel;
  }
}

bool PathConstraint::operator==(const PathConstraint &other) const {
  return (initialLabel == other.initialLabel) && (length == other.length)
      && (vertexIndexVector == other.vertexIndexVector);
}

void PathConstraint::print(std::ostream &ostream) const {
  bool first = true;
  ostream << "Vertex Index : ";
  for (const auto &it : vertexIndexVector) {
    if (first) {
      first = false;
      ostream << it;

    } else {
      ostream << " -> " << it;
    }
  }
  ostream << std::endl;

  first = true;
  ostream << "Vertex Label : ";
  for (const auto &it : vertexLabelVector) {
    if (first) {
      first = false;
      ostream << it;

    } else {
      ostream << " -> " << it;
    }
  }
  ostream << std::endl;
}
}