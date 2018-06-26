//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_TEMPLATE_WALK_H
#define PROJECT_MULTIPLE_LABEL_TEMPLATE_WALK_H

#include "totem_graph.h"
#include <vector>
#include <iostream>

namespace patternmatching {

class Walk {
 public:
  enum MoveType {
    E_MOVE_BACK,
    E_CHECK,
    E_VERTEX_STORE,
    E_VERTEX_NO_STORE
  };
  Walk();
  void addMoveBack(const vid_t &vertexIndex, const size_t &previousPositionIt);
  void addCheck(const vid_t &vertexIndex, const size_t &previousPositionIt);
  void addVertex(const vid_t &vertexIndex);
  void computeStoredVertex();
  void print(std::ostream &ostream = std::cout) const;
 public:
  size_t length;
  size_t vertexLength;
  std::vector<MoveType> moveVector;
  std::vector<vid_t> vertexIndexVector;
  std::vector<size_t> moveBackIndexVector;
};
}

#endif //PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H
