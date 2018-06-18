//
// Created by qiu on 23/05/18.
//

#ifndef TOTEM_MULTIPLE_LABEL_STATE_H
#define TOTEM_MULTIPLE_LABEL_STATE_H

#include <string>

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
    E_TDS
  };

  MultipleLabelStep();
  void initStepCc(const int circularConstraintNumber);
  void initStepPc(const int circularConstraintNumber);
  void initStepTds(const int circularConstraintNumber);

  bool getNextStep(const size_t lastStepEliminatedVerticles,
                     Step* nextStep, std::string *nextStepName);

 private:
  void setStepLcc0(Step* nextStep, std::string *nextStepName);
  void setStepLcc(Step* nextStep, std::string *nextStepName);
  void setStepCc(Step* nextStep, std::string *nextStepName);
  void setStepPc(Step* nextStep, std::string *nextStepName);
  void setStepTds(Step* nextStep, std::string *nextStepName);
  void setStepEnd(Step* nextStep, std::string *nextStepName);

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
