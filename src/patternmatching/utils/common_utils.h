//
// Created by qiu on 24/06/18.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include "totem_graph.h"

#define PRODUCTION 1

#if PRODUCTION == 1
  #define PROGRESSION_INSERT 0
  #define LOGGER_LEVEL_NO_DEBUG 1
  #define ENUMERATION_BINARY_FORMAT 1
#else
  #define PROGRESSION_INSERT 1
  #define LOGGER_LEVEL_NO_DEBUG 0
  #define ENUMERATION_BINARY_FORMAT 0
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

inline std::vector<std::string> splitString(const std::string &input, const char &delim) {
  std::stringstream ss(input);
  std::string item;
  std::vector<std::string> output;
  while (std::getline(ss, item, delim)) {
    output.push_back(item);
  }
  return output;
}
inline std::vector<std::string> splitStringIncremental(const std::string &input, const char &delim) {
  std::stringstream ss(input);
  std::string item;
  std::string current;
  std::vector<std::string> output;
  bool first=true;
  while (std::getline(ss, item, delim)) {
    if(!item.empty() || first) {
      current += item + "/";
      output.push_back(current);
      first=false;
    }
  }
  return output;
}

inline bool isDirectory(const std::string &path) {
  struct stat st = {0};
  if(stat(path.c_str(), &st) == 0) {
    return static_cast<bool>(S_ISDIR(st.st_mode));
  }
  return false;
}

inline bool isFile(const std::string &path) {
  struct stat st = {0};
  if(stat(path.c_str(), &st) == 0) {
    return static_cast<bool>(S_ISREG(st.st_mode));
  }
  return false;
}

inline void makeDirectory(const std::string &path) {
  mkdir(path.c_str(), ACCESSPERMS);
}

inline void makeDirectoryStructure(std::string &path) {
  auto directoryVector=splitStringIncremental(path, '/');
  for(const auto &it : directoryVector) {
    if(!isDirectory(it)) makeDirectory(it);
  }
}

template<typename... Args>
std::string sprintfString(const char* format, Args&&... args) {
  auto size = std::snprintf(nullptr, 0, format, args...);
  std::string output(size + 1, 'a');
  std::sprintf(&output[0], format, args...);
  output.resize(size);
  return output;
}

error_t parseVertexFile(FILE *file_handler, graph_t *graph);

#endif //PROJECT_UTILS_H
