//
// Created by qiu on 17/05/18.
//
#include "totem_util.h"
#include "multiple_label_generate_constraint_template.h"
#include "multiple_label_constraint_template.h"
#include <iostream>
#include <deque>
#include "common_utils.h"

namespace patternmatching {

void MultipleLabelGenerateConstraintTemplate::preprocessCircularConstraint(
    const std::vector<MultipleLabelConstraintCircular> &circularConstraintVector) {

  std::deque<MultipleLabelConstraintTemplate> templateConstraintList;

  // process circular constraint
  for (const auto &it : circularConstraintVector) {
    MultipleLabelConstraintTemplate constraint = MultipleLabelConstraintTemplate(it);
    templateConstraintList.push_back(constraint);
  }

  auto templateConstraintIt1 = templateConstraintList.begin();
  while (templateConstraintIt1 != templateConstraintList.end()) {
    auto templateConstraintIt2 = templateConstraintIt1 + 1;
    bool haveMerged = false;
    while (templateConstraintIt2 != templateConstraintList.end()) {
      if (templateConstraintIt1->haveCommonEdge(*templateConstraintIt2)) {
        auto mergedConstraint = MultipleLabelConstraintTemplate(*templateConstraintIt1, *templateConstraintIt2);

        haveMerged = true;
        templateConstraintList.erase(templateConstraintIt1);
        templateConstraintList.erase(templateConstraintIt2);
        templateConstraintList.push_back(mergedConstraint);
        templateConstraintIt1 = templateConstraintList.begin();
        break;
      }
      if (!haveMerged) ++templateConstraintIt2;
    }
    if (!haveMerged) ++templateConstraintIt1;
  }

  for (const auto &it : templateConstraintList) {
    if (it.templateConstraintOrigin != MultipleLabelConstraintTemplate::E_CC) {
      templateConstraintVector.push_back(it);
    }
  }
}

void MultipleLabelGenerateConstraintTemplate::preprocessPathConstraint(
    const std::vector<MultipleLabelConstraintPath> &pathConstraintVector) {

  std::unordered_map<weight_t, std::deque<MultipleLabelConstraintTemplate> > templateConstraintListLabelMap;

  // process path constraint
  for (const auto &it : pathConstraintVector) {
    MultipleLabelConstraintTemplate constraint = MultipleLabelConstraintTemplate(it);
    templateConstraintListLabelMap[it.initialLabel].push_back(constraint);
  }

  for (auto &it : templateConstraintListLabelMap) {
    auto &templateConstraintList = it.second;
    auto templateConstraintIt1 = templateConstraintList.begin();
    while (templateConstraintIt1 != templateConstraintList.end()) {
      auto templateConstraintIt2 = templateConstraintIt1 + 1;
      bool haveMerged = false;
      while (templateConstraintIt2 != templateConstraintList.end()) {
        if (templateConstraintIt1->haveCommonVector(*templateConstraintIt2)) {
          auto mergedConstraint = MultipleLabelConstraintTemplate(*templateConstraintIt1, *templateConstraintIt2);

          haveMerged = true;
          templateConstraintList.erase(templateConstraintIt1);
          templateConstraintList.erase(templateConstraintIt2);
          templateConstraintList.push_back(mergedConstraint);
          templateConstraintIt1 = templateConstraintList.begin();
          break;
        }
        if (!haveMerged) ++templateConstraintIt2;
      }
      if (!haveMerged) ++templateConstraintIt1;
    }
  }

  for (const auto &it : templateConstraintListLabelMap) {
    for (const auto &subit : it.second) {
      if (subit.templateConstraintOrigin != MultipleLabelConstraintTemplate::E_PC) {
        templateConstraintVector.push_back(subit);
      }
    }
  }
}

void MultipleLabelGenerateConstraintTemplate::preprocessSubtemplateConstraint() {

  std::deque<MultipleLabelConstraintTemplate> templateConstraintList;

  // process subtemplate constraint
  for (const auto &it : templateConstraintVector) {
    templateConstraintList.push_back(it);
  }

  size_t offset = 0;

  while (templateConstraintList.size() > 0 && (offset < templateConstraintList.size() - 1)) {
    int mergedNumber = 0;
    auto it = templateConstraintList.begin() + offset;
    auto currentTemplateConstraint = *it;
    ++it;
    while (it != templateConstraintList.end()) {
      if (currentTemplateConstraint.haveCommonVector(*it)) {
        currentTemplateConstraint = MultipleLabelConstraintTemplate(currentTemplateConstraint, *it);

        templateConstraintList.erase(it);
        it = templateConstraintList.begin();
        mergedNumber++;
      } else {
        ++it;
      }
    }
    if (mergedNumber > 0) {
      offset = 0;
      if (!isInVector(templateConstraintVector, currentTemplateConstraint)) {
        templateConstraintVector.push_back(currentTemplateConstraint);
      }
    } else {
      offset++;
    }
  }
}

void MultipleLabelGenerateConstraintTemplate::preprocessPatern(
    const graph_t &pattern,
    const std::vector<MultipleLabelConstraintCircular> &circularConstraintVector,
    const std::vector<MultipleLabelConstraintPath> &pathConstraintVector) {

  preprocessCircularConstraint(circularConstraintVector);
  preprocessPathConstraint(pathConstraintVector);
  preprocessSubtemplateConstraint();

  for (auto &it : templateConstraintVector) {
    it.generateWalkMap();
  }

}

void MultipleLabelGenerateConstraintTemplate::print(std::ostream &ostream) const {
  int currentConstraint = 0;

  ostream << "Constraint number =  " << templateConstraintVector.size() << ". " << std::endl;
  for (const auto &it : templateConstraintVector) {
    ostream << "=== Current constraint = " << currentConstraint << " ===" << std::endl;
    it.print(ostream);
    it.printWalk(ostream);
    ++currentConstraint;
  }
}

int MultipleLabelGenerateConstraintTemplate::getConstraintNumber() const {
  return templateConstraintVector.size();
}

const std::vector<MultipleLabelConstraintTemplate> &MultipleLabelGenerateConstraintTemplate::getConstraintVector() const {
  return templateConstraintVector;
}
}