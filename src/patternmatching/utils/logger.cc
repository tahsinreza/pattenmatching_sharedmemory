
#include "logger.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "common_utils.h"
#include <omp.h>

namespace patternmatching {

const std::string Logger::C_GRAPH_FILEPATTERN = "graph_%04d.totem";
const std::string Logger::C_VERTEX_FILEPATTERN = "vertex_%04d.totem";

const std::string Logger::C_LOG_FILEPATTERN = "log.totem";

const std::string Logger::C_ITERATION_RESULTS_FILEPATTERN = "results.totem";
const std::string Logger::C_ENUMERATION_RESULTS_FILEPATTERN = "enumeration_%04d.totem";
const std::string Logger::C_EFFECTIVENESS_RESULTS_FILEPATTERN = "effectiveness.totem";

#if LOGGER_LEVEL_NO_DEBUG == 1
  const int Logger::C_MIN_LOG_LEVEL = Logger::E_LEVEL_INFO;
#else
  const int Logger::C_MIN_LOG_LEVEL = Logger::E_LEVEL_DEBUG;
#endif

Logger *Logger::instance = nullptr;

Logger::Logger() {
  resultDirectory = "";
  currentIteration = -1;
}

Logger &Logger::get() {
  if (instance == nullptr) instance = new Logger;

  return *instance;
}

std::string Logger::getStreamName(const LogOutput &logOutput, const int param) const {
  if(logOutput == E_OUTPUT_FILE_ENUMERATION_RESUTLS) {
    char buffer[128];
    sprintf(buffer, C_ENUMERATION_RESULTS_FILEPATTERN.c_str(), param);
    return resultDirectory + buffer;
  }

  return std::string();
}

void Logger::init(const std::string &_resultDirectory, const bool &clearEnumeration) {
  if (_resultDirectory.back() == '/') {
    resultDirectory = _resultDirectory;
  } else {
    resultDirectory = _resultDirectory + "/";
  }
  currentIteration = 0;

  // Create directory if needed
  makeDirectoryStructure(resultDirectory);

  // clear log files
  std::ofstream iterationResultsFileStream
      (resultDirectory + C_ITERATION_RESULTS_FILEPATTERN, std::ofstream::out | std::ofstream::trunc);
  iterationResultsFileStream.close();
  std::ofstream effectivenessResultsFileStream
      (resultDirectory + C_EFFECTIVENESS_RESULTS_FILEPATTERN, std::ofstream::out | std::ofstream::trunc);
  effectivenessResultsFileStream.close();
  std::ofstream logFileStream(resultDirectory + C_LOG_FILEPATTERN, std::ofstream::out | std::ofstream::trunc);
  logFileStream.close();
  if(clearEnumeration) {
    for (int i = 0; i < omp_get_max_threads(); i++) {
      char buffer[128];
      sprintf(buffer, C_ENUMERATION_RESULTS_FILEPATTERN.c_str(), i);
      std::ofstream enumerationFileStream(resultDirectory + buffer, std::ofstream::out | std::ofstream::trunc);
      enumerationFileStream.close();
    }
  }
}

bool Logger::isLogged(const LogLevel &logLevel) {
  return logLevel <= C_MIN_LOG_LEVEL;
}

void Logger::log(const LogLevel &logLevel, const char *message, const LogOutput &logOutput) const {
  log(logLevel, std::string(message), logOutput);
}

void Logger::log(const LogLevel &logLevel, const std::string &message, const LogOutput &logOutput) const {
  if (!isLogged(logLevel)) return;

  if (logOutput & E_OUTPUT_COUT) {
    std::cout << message << std::endl;
  }

  if (logOutput & E_OUTPUT_FILE_GRAPH) {
    char buffer[128];
    sprintf(buffer, C_GRAPH_FILEPATTERN.c_str(), currentIteration);
    std::ofstream fileStream(resultDirectory + buffer, std::ofstream::out);
    fileStream << message << std::endl;
    fileStream.close();
  }

  if (logOutput & E_OUTPUT_FILE_VERTEX) {
    char buffer[128];
    sprintf(buffer, C_VERTEX_FILEPATTERN.c_str(), currentIteration);
    std::ofstream fileStream(resultDirectory + buffer, std::ofstream::out);
    fileStream << message << std::endl;
    fileStream.close();
  }

  if (logOutput & E_OUTPUT_FILE_ITERATION_RESULTS) {
    std::ofstream fileStream(resultDirectory + C_ITERATION_RESULTS_FILEPATTERN, std::ofstream::app);
    fileStream << message << std::endl;
    fileStream.close();
  }

  if (logOutput & E_OUTPUT_FILE_EFFECTIVENESS_RESUTLS) {
    std::ofstream fileStream(resultDirectory + C_EFFECTIVENESS_RESULTS_FILEPATTERN, std::ofstream::app);
    fileStream << message << std::endl;
    fileStream.close();
  }

  if (logOutput & E_OUTPUT_FILE_LOG) {
    std::ofstream fileStream(resultDirectory + C_LOG_FILEPATTERN, std::ofstream::app);
    #if __cplusplus>=201402L
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    fileStream << std::put_time(std::localtime(&in_time_t), "[%F %T] : ") << message << std::endl;
    #else
    fileStream << message << std::endl;
    #endif
    fileStream.close();
  }
}



void Logger::setCurrentIteration(const int &_currentIteration) {
  currentIteration = _currentIteration;
}

}