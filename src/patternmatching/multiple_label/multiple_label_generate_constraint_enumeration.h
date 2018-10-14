//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_ENUMERATION_H
#define TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_ENUMERATION_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_template.h"
#include "multiple_label_constraint_circular.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

class MultipleLabelGenerateConstraintEnumeration {
 public:
  void preprocessPatern(const graph_t &pattern);
  void print(std::ostream &ostream = std::cout) const;

  const MultipleLabelConstraintTemplate& getConstraint() const;
  int getConstraintNumber() const;
 private:


  MultipleLabelConstraintTemplate templateConstraint;
};

}

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
