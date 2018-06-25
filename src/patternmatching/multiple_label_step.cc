//
// Created by qiu on 23/05/18.
//

#include "multiple_label_step.h"

namespace patternmatching {

MultipleLabelStep::MultipleLabelStep()
    : currentStep(E_START),
      circularConstraintNumber(0), currentCircularConstraint(0),
      pathConstraintNumber(0), currentPathConstraint(0),
      tdsConstraintNumber(0), currentTdsConstraint(0) {}

void MultipleLabelStep::initStepCc(const int circularConstraintNumber_) {
  circularConstraintNumber = circularConstraintNumber_;
  currentCircularConstraint = 0;
}

void MultipleLabelStep::initStepPc(const int pathConstraintNumber_) {
  pathConstraintNumber = pathConstraintNumber_;
  currentPathConstraint = 0;
}

void MultipleLabelStep::initStepTds(const int tdsConstraintNumber_) {
  tdsConstraintNumber = tdsConstraintNumber_;
  currentTdsConstraint = 0;
}

bool MultipleLabelStep::getNextStep(const size_t lastStepEliminatedVerticles,
                                    Step *nextStep, std::string *nextStepName) {

  bool finished = false;

  switch (currentStep) {
    case E_START:
      // initialisation
      setStepLcc0(nextStep, nextStepName);
      break;
    case E_LCC0:
      // Start
      setStepLcc(nextStep, nextStepName);
      break;
    case E_LCC:
      // Stay on same
      if (lastStepEliminatedVerticles > 0) {
      } else {
        // Change to CC, PC, or end
        if (currentCircularConstraint < circularConstraintNumber && 1) {
          setStepCc(nextStep, nextStepName);
        } else if (currentPathConstraint < pathConstraintNumber && 1) {
          setStepPc(nextStep, nextStepName);
        } else if (currentTdsConstraint < tdsConstraintNumber && 1) {
          setStepTds(nextStep, nextStepName);
        } else {
          setStepEnumeration(nextStep, nextStepName);
        }
      }
      break;
    case E_CC:
      currentCircularConstraint += 1;
      setStepLcc(nextStep, nextStepName);
      break;
    case E_PC:
      currentPathConstraint += 1;
      setStepLcc(nextStep, nextStepName);
      break;
    case E_TDS:
      currentTdsConstraint += 1;
      setStepLcc(nextStep, nextStepName);
      break;
    case E_ENUMERATION:
      finished = true;
      setStepEnd(nextStep, nextStepName);
      break;
    case E_END:
    default:break;
  }

  return finished;

}

void MultipleLabelStep::setStepLcc0(Step *nextStep, std::string *nextStepName) {
  currentStep = E_LCC0;
  *nextStep = E_LCC0;
  *nextStepName = "LCC0";
}
void MultipleLabelStep::setStepLcc(Step *nextStep, std::string *nextStepName) {
  currentStep = E_LCC;
  *nextStep = E_LCC;
  *nextStepName = "LCC";
}
void MultipleLabelStep::setStepCc(Step *nextStep, std::string *nextStepName) {
  currentStep = E_CC;
  *nextStep = E_CC;
  *nextStepName = "CC";
}

void MultipleLabelStep::setStepPc(Step *nextStep, std::string *nextStepName) {
  currentStep = E_PC;
  *nextStep = E_PC;
  *nextStepName = "PC";
}
void MultipleLabelStep::setStepTds(Step *nextStep, std::string *nextStepName) {
  currentStep = E_TDS;
  *nextStep = E_TDS;
  *nextStepName = "TDS";
}
void MultipleLabelStep::setStepEnumeration(Step *nextStep, std::string *nextStepName) {
  currentStep = E_ENUMERATION;
  *nextStep = E_ENUMERATION;
  *nextStepName = "ENUMERATION";
}
void MultipleLabelStep::setStepEnd(Step *nextStep, std::string *nextStepName) {
  currentStep = E_END;
  *nextStep = E_END;
  *nextStepName = "END";
}

}