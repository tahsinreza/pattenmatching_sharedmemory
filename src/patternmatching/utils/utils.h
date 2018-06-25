//
// Created by qiu on 24/06/18.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
#include <vector>

struct PairHash {
 public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};


template<class T>
inline bool isInVector(const std::vector<T> &historyIndexVector, const T &vertex) {
  for(const auto& it : historyIndexVector) {
    if(it==vertex) return true;
  }
  return false;
}

#endif //PROJECT_UTILS_H
