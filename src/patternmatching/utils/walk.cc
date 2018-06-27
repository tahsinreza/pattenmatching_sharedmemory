//
// Created by qiu on 07/06/18.
//

#include "walk.h"
#include "utils.h"

namespace patternmatching {


Walk::Walk()
: length(0), vertexLength(0), moveVector(), vertexIndexVector(), moveBackIndexVector(), historyIndexVector()
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
  moveVector.push_back(E_VERTEX_NO_STORE);
  vertexIndexVector.push_back(vertexIndex);
  moveBackIndexVector.push_back(0);
  ++length;
  historyIndexVector.push_back(vertexIndex);
  ++vertexLength;
}

void Walk::computeStoredVertex() {
  std::vector<vid_t> storedVertexId;
  for (size_t i = 0; i < length; i++) {
    if(moveVector[i]==E_VERTEX_NO_STORE) {
      bool isInRemainingVector=false;
      for (size_t j = i+1; j < length; j++) {
        if(vertexIndexVector[j]==vertexIndexVector[i] && moveVector[j]==E_CHECK) {
          isInRemainingVector=true;
          break;
        }
      }
      if(isInRemainingVector && !isInVector(storedVertexId, vertexIndexVector[i])) {
        moveVector[i]=E_VERTEX_STORE;
        storedVertexId.push_back(vertexIndexVector[i]);
      }
    }
  }
}
void Walk::print(std::ostream &ostream) const {
  bool first = true;
  ostream << "Walk : ";
  for (int i = 0; i < length; i++) {
    if (first) first = false;
    else ostream << " -> ";
    std::string move;
    if(moveVector[i]==E_CHECK) {
      move="CK";
    } else if(moveVector[i]==E_MOVE_BACK) {
      move="MB";
    } else if(moveVector[i]==E_VERTEX_STORE) {
      move="VS";
    } else if(moveVector[i]==E_VERTEX_NO_STORE) {
      move="VN";
    }
    ostream << "[" << move << "," << vertexIndexVector[i] << "," << moveBackIndexVector[i] << "]";
  }
  ostream << std::endl;
}

}