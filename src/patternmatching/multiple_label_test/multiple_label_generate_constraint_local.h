//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_LOCAL_H
#define TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_LOCAL_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_local.h"
#include <iostream>
#include <unordered_set>

namespace patternmatching {

class MultipleLabelGenerateConstraintLocal {
 public:
  void preprocessPatern(const graph_t &pattern);
  void print(std::ostream &ostream = std::cout) const;


  int getConstraintNumber() const;
  const std::vector<MultipleLabelConstraintLocal> &getConstraintVector() const;
 private:
  std::vector<MultipleLabelConstraintLocal > patternLocalConstraint;
};

}

#endif //TOTEM_MULTIPLE_LABEL_LCC0_CPU_H
