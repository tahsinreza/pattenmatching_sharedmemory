//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_PATH_H
#define TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_PATH_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

class MultipleLabelGenerateConstraintPath {
 public:
  void preprocessPatern(const graph_t &pattern);
  void print(std::ostream &ostream = std::cout) const;

  const std::vector <MultipleLabelConstraintPath>& getConstraintVector() const;
  int getConstraintNumber() const;

 private:

  void buildConstraintList(
      const graph_t &pattern,
      const vid_t &sourceVertexId,
      const vid_t &currentVertexId,
      const size_t &currentLength,
      FixedBitmapType &patternTraversalVector,
      std::vector <vid_t> &historyVertexId,
      std::vector <weight_t> &historyVertexLabel,
      std::vector <MultipleLabelConstraintPath> &constraintVector);

  std::vector <MultipleLabelConstraintPath> pathConstraintVector;
};

}

#endif //TOTEM_MULTIPLE_LABEL_PC_CPU_H
