//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_LOCAL_CONSTRAINT_H
#define PROJECT_MULTIPLE_LABEL_LOCAL_CONSTRAINT_H

#include "totem_graph.h"
#include "multiple_label_common.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "bitmap_fixed.h"


namespace patternmatching {

class MultipleLabelConstraintLocal {
 public:
  MultipleLabelConstraintLocal();
  MultipleLabelConstraintLocal(
      const pvid_t &originVertex,
      const weight_t &originLabel,
      const std::unordered_map<weight_t, size_t> &localConstraint,
      const FixedBitmapType &localConstraintNeighborVertex_);
  void print(std::ostream &ostream = std::cout) const;
 public:
  pvid_t originVertex;
  weight_t originLabel;
  std::unordered_map<weight_t, size_t> localConstraint;
  FixedBitmapType localConstraintNeighborVertex;
};
}

#endif //PROJECT_MULTIPLE_LABEL_LOCAL_CONSTRAINT_H
