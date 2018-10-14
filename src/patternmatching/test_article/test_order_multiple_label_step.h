//
// Created by qiu on 23/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_STATE_H
#define TOTEM_MULTIPLE_LABEL_STATE_H

#include <string>
#include "algo_results.h"

namespace patternmatching {

class MultipleLabelStep {

 public:
  enum Step {
    E_START,
    E_END,
    E_LCC0,
    E_LCC,
    E_CC,
    E_PC,
    E_TDS,
    E_ENUMERATION,
  };

  MultipleLabelStep();
  void initStepCc(const int circularConstraintNumber);
  void initStepPc(const int circularConstraintNumber);
  void initStepTds(const int circularConstraintNumber);

  bool getNextStep(const AlgoResults &algoResults,
                     Step* nextStep, std::string *nextStepName);

  void setStepLcc0(Step* nextStep, std::string *nextStepName);
  void setStepLcc(Step* nextStep, std::string *nextStepName);
  void setStepCc(Step* nextStep, std::string *nextStepName);
  void setStepPc(Step* nextStep, std::string *nextStepName);
  void setStepTds(Step* nextStep, std::string *nextStepName);
  void setStepEnumeration(Step* nextStep, std::string *nextStepName);
  void setStepEnd(Step* nextStep, std::string *nextStepName);
 private:
  Step currentStep;

  // cc state
  int circularConstraintNumber;
  int currentCircularConstraint;


  // pc state
  int pathConstraintNumber;
  int currentPathConstraint;

  // pc state
  int tdsConstraintNumber;
  int currentTdsConstraint;

};

}
#endif //TOTEM_MULTIPLE_LABEL_STATE_H
