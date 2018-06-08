//
// Created by qiu on 23/05/18.
//

#include "unique_label_step.h"

namespace patternmatching {

UniqueLabelStep::UniqueLabelStep()
    : currentStep(E_START), circularConstraintNumber(0), currentCircularConstraint(0) {}

void UniqueLabelStep::initStepCc(const int _circularConstraintNumber) {
  circularConstraintNumber = _circularConstraintNumber;
  currentCircularConstraint = 0;
}

bool UniqueLabelStep::getNextStep(const size_t lastStepEliminatedVerticles,
                                  Step *nextStep, std::string *nextStepName) {

  bool finished = false;

  switch (currentStep) {
    case E_START:
      // initialisation
      _setStepLcc(nextStep, nextStepName);
      break;
    case E_LCC:
      // Stay on same
      if (lastStepEliminatedVerticles > 0) {
      } else {

        // Change to CC or end
        if (currentCircularConstraint < circularConstraintNumber) {
          _setStepCc(nextStep, nextStepName);
        } else {
          finished = true;
          _setStepEnd(nextStep, nextStepName);
        }
      }
      break;
    case E_CC:currentCircularConstraint += 1;
      _setStepLcc(nextStep, nextStepName);
      break;
    case E_END:
    default:break;
  }

  return finished;

}

void UniqueLabelStep::_setStepLcc(Step *nextStep, std::string *nextStepName) {
  currentStep = E_LCC;
  *nextStep = E_LCC;
  *nextStepName = "LCC";
}
void UniqueLabelStep::_setStepCc(Step *nextStep, std::string *nextStepName) {
  currentStep = E_CC;
  *nextStep = E_CC;
  *nextStepName = "CC";
}
void UniqueLabelStep::_setStepEnd(Step *nextStep, std::string *nextStepName) {
  currentStep = E_END;
  *nextStep = E_END;
  *nextStepName = "END";
}

}