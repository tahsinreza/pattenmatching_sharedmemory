//
// Created by qiu on 23/05/18.
//

#ifndef TOTEM_UNIQUE_LABEL_STATE_H
#define TOTEM_UNIQUE_LABEL_STATE_H

#include <string>

namespace patternmatching {

class UniqueLabelStep {

 public:
  enum Step {
    E_START,
    E_END,
    E_LCC,
    E_CC
  };

  UniqueLabelStep();
  void initStepCc(const int circularConstraintNumber);

  bool getNextStep(const size_t lastStepEliminatedVerticles,
                     Step* nextStep, std::string *nextStepName);

 private:
  void _setStepLcc(Step* nextStep, std::string *nextStepName);
  void _setStepCc(Step* nextStep, std::string *nextStepName);
  void _setStepEnd(Step* nextStep, std::string *nextStepName);

  Step currentStep;

  // cc state
  int circularConstraintNumber;
  int currentCircularConstraint;

};

}
#endif //TOTEM_UNIQUE_LABEL_STATE_H
