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
    E_LCC,
    E_CC,
    E_PC
  };

  MultipleLabelStep();
  void initStepCc(const int circularConstraintNumber);
  void initStepPc(const int circularConstraintNumber);

  bool getNextStep(const size_t lastStepEliminatedVerticles,
                     Step* nextStep, std::string *nextStepName);

 private:
  void setStepLcc(Step* nextStep, std::string *nextStepName);
  void setStepCc(Step* nextStep, std::string *nextStepName);
  void setStepPc(Step* nextStep, std::string *nextStepName);
  void setStepEnd(Step* nextStep, std::string *nextStepName);

  Step currentStep;

  // cc state
  int circularConstraintNumber;
  int currentCircularConstraint;


  // pc state
  int pathConstraintNumber;
  int currentPathConstraint;

};

}
#endif //TOTEM_MULTIPLE_LABEL_STATE_H