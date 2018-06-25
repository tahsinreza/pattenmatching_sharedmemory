//
// Created by qiu on 24/06/18.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
#include <vector>

template<class T>
inline bool isInVector(const std::vector<T> &historyIndexVector, const T &vertex) {
  for(const auto& it : historyIndexVector) {
    if(it==vertex) return true;
  }
  return false;
}

#endif //PROJECT_UTILS_H
