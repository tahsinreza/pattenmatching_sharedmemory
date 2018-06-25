//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_CIRCULAR_H
#define TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_CIRCULAR_H

#include "multiple_label_common_cpu.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "multiple_label_constraint_circular.h"
#include "utils.h"

namespace patternmatching {

class MultipleLabelGenerateConstraintCircular {
 public:

  void preprocessPatern(const graph_t &pattern);

  void print(std::ostream &ostream = std::cout) const;

  const std::vector <MultipleLabelConstraintCircular>& getConstraintVector() const;
  int getConstraintNumber() const;

 private:
  void buildConstraintList(
      const graph_t &pattern,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &remainingLength,
      FixedBitmapType &patternTraversalVector,
      std::vector <vid_t> &historyVertexId,
      std::vector <weight_t> &historyVertexLabel,
      std::vector <MultipleLabelConstraintCircular> &constraintVector);

  std::vector <MultipleLabelConstraintCircular> circularConstraintVector;
};

}

#endif //TOTEM_MULTIPLE_LABEL_CC_CPU_H
