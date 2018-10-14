//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_CIRCULAR_CONSTRAINT_H
#define PROJECT_MULTIPLE_LABEL_CIRCULAR_CONSTRAINT_H

#include "totem_graph.h"
#include <vector>
#include <iostream>

namespace patternmatching {

class MultipleLabelConstraintCircular {
 public:
  MultipleLabelConstraintCircular();
  MultipleLabelConstraintCircular(
      const std::vector <vid_t> &historyVertexId,
      const std::vector <weight_t> &historyVertexLabel);
  void print(std::ostream &ostream = std::cout) const;
 public:
  bool operator==(const MultipleLabelConstraintCircular &other) const;
  size_t length;
  std::vector <vid_t> vertexIndexVector;
  std::vector <weight_t> vertexLabelVector;
};
}

#endif //PROJECT_MULTIPLE_LABEL_CIRCULAR_CONSTRAINT_H
