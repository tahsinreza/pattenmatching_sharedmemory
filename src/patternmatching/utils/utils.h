//
// Created by qiu on 24/06/18.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
#include <vector>
#include <iostream>

#define PRODUCTION 0

#if PRODUCTION == 1
  #define PROGRESSION_INSERT 0
  #define LOGGER_LEVEL_NO_DEBUG 1
#else
  #define PROGRESSION_INSERT 0
  #define LOGGER_LEVEL_NO_DEBUG 0
#endif

#if PROGRESSION_INSERT == 1
#define PROGRESSION_INSERT_BEGIN()\
  size_t stepCompleted_=0;\
  size_t stepMax_=globalState->graphActiveVertexCount;\
  size_t stepSize_=stepMax_/100+1;

#define PROGRESSION_INSERT_LOOP()\
  progressionLoop(stepCompleted_, stepSize_, stepMax_);

#else
  #define PROGRESSION_INSERT_BEGIN() ;
  #define PROGRESSION_INSERT_LOOP() ;
#endif

inline void progressionLoop(size_t &stepCompleted_, size_t &stepSize_, size_t &stepMax_) {
  #pragma omp atomic
  ++stepCompleted_;

  if (stepCompleted_ % stepSize_ == 0) {
    #pragma omp critical
    {
      std::cout << "Progression : " << stepCompleted_ << "/" << stepMax_ << std::endl;
    }
  }

}

struct PairHash {
 public:
  template<typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const {
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};

template<class T>
inline bool isInVector(const std::vector<T> &historyIndexVector, const T &vertex) {
  for (const auto &it : historyIndexVector) {
    if (it == vertex) return true;
  }
  return false;
}

#endif //PROJECT_UTILS_H
