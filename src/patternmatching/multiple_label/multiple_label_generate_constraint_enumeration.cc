//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_constraint_template.h"
#include "multiple_label_generate_constraint_enumeration.h"
#include <iostream>
#include <deque>
#include "common_utils.h"

namespace patternmatching {

void MultipleLabelGenerateConstraintEnumeration::preprocessPatern(
    const graph_t &pattern) {

  templateConstraint = MultipleLabelConstraintTemplate(pattern);
  templateConstraint.generateWalkMap();
}

void MultipleLabelGenerateConstraintEnumeration::print(std::ostream &ostream) const {
  ostream << "=== enumeration constraint ===" << std::endl;
  templateConstraint.print(ostream);
  templateConstraint.printWalk(ostream);
}

int MultipleLabelGenerateConstraintEnumeration::getConstraintNumber() const {
  return 1;
}

const MultipleLabelConstraintTemplate &MultipleLabelGenerateConstraintEnumeration::getConstraint() const {
  return templateConstraint;
}

}