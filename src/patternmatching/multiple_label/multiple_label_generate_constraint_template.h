//
// Created by qiu on 17/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_TEMPLATE_H
#define TOTEM_MULTIPLE_LABEL_GENERATE_CONSTRAINT_TEMPLATE_H

#include "multiple_label_common_cpu.h"
#include "multiple_label_constraint_template.h"
#include "multiple_label_constraint_circular.h"
#include "multiple_label_constraint_path.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

namespace patternmatching {

class MultipleLabelGenerateConstraintTemplate {
 public:
  void preprocessPatern(const graph_t &pattern,
                        const std::vector<MultipleLabelConstraintCircular> &circularConstraintVector,
                        const std::vector<MultipleLabelConstraintPath> &pathConstraintVector);

  void print(std::ostream &ostream = std::cout) const;

  int getConstraintNumber() const;
  const std::vector<MultipleLabelConstraintTemplate> &getConstraintVector() const;

 private:
  void preprocessCircularConstraint(const std::vector<MultipleLabelConstraintCircular> &circularConstraintVector);
  void preprocessPathConstraint(const std::vector<MultipleLabelConstraintPath> &pathConstraintVector);
  void preprocessSubtemplateConstraint();

  std::vector<MultipleLabelConstraintTemplate> templateConstraintVector;
};

}

#endif //TOTEM_MULTIPLE_LABEL_TDS_CPU_H
