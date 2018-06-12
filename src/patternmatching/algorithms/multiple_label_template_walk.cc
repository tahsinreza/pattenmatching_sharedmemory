//
// Created by qiu on 07/06/18.
//

#include "multiple_label_template_walk.h"

namespace patternmatching {


Walk::Walk()
: length(0), moveVector(), vertexIndexVector(), moveBackIndexVector()
{
}

void Walk::addMoveBack(const vid_t &vertexIndex, const size_t &previousPositionIt) {
  moveVector.push_back(E_MOVE_BACK);
  vertexIndexVector.push_back(vertexIndex);
  moveBackIndexVector.push_back(previousPositionIt);
  ++length;
}

void Walk::addCheck(const vid_t &vertexIndex, const size_t &previousPositionIt) {
  moveVector.push_back(E_CHECK);
  vertexIndexVector.push_back(vertexIndex);
  moveBackIndexVector.push_back(previousPositionIt);
  ++length;
}

void Walk::addVertex(const vid_t &vertexIndex) {
  moveVector.push_back(E_VERTEX);
  vertexIndexVector.push_back(vertexIndex);
  moveBackIndexVector.push_back(0);
  ++length;
}
void Walk::print(std::ostream &ostream) const {
  bool first = true;
  ostream << "Walk : ";
  for (int i = 0; i < length; i++) {
    if (first) first = false;
    else ostream << " -> ";
    ostream << "[" << moveVector[i] << "," << vertexIndexVector[i] << "," << moveBackIndexVector[i] << "]";
  }
  ostream << std::endl;
}

}