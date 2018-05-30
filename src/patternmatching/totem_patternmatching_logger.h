
#ifndef TOTEM_UNIQUE_LABEL_LOGGER_H
#define TOTEM_UNIQUE_LABEL_LOGGER_H

#include <string>
#include <sstream>

namespace patternmatching {

#define LOGGER_LOG_FUNCTION(LOG_LEVEL, FUNCTION, LOG_OUTPUT) \
if(Logger::isLogged(LOG_LEVEL)) {\
std::stringstream stringStream; \
FUNCTION(stringStream); \
Logger::get.log(LOG_LEVEL, stringStream.str(), LOG_OUTPUT); \
}

class Logger {
 public:
  enum LogLevel {
    E_LEVEL_RESULT=0,
    E_LEVEL_WARNING,
    E_LEVEL_INFO,
    E_LEVEL_DEBUG
  };
  enum LogOutput {
    E_OUTPUT_COUT=1<<0,
    E_OUTPUT_FILE_GRAPH=1<<1,
    E_OUTPUT_FILE_ITERATION_RESULTS=1<<2,
    E_OUTPUT_FILE_LOG=1<<3,
    E_OUTPUT_DEBUG=E_OUTPUT_COUT | E_OUTPUT_FILE_LOG
  };

  static Logger& get();
  Logger(const LogLevel&) = delete;
  Logger& operator=(const LogLevel&) = delete;

  void init(const std::string &resultDirectory);

  static bool isLogged(const LogLevel &logLevel);


  void log(const LogLevel &logLevel, const char *message, const LogOutput &logOutput = E_OUTPUT_DEBUG) const;
  void log(const LogLevel &logLevel, const std::string &message, const LogOutput &logOutput = E_OUTPUT_DEBUG) const;

  template<class Object>
  void logFunction(const LogLevel &logLevel,
                   const Object &object,
                   void (Object::* const functionHandle)(std::ostream &) const,
                   const LogOutput &logOutput = E_OUTPUT_DEBUG) const;

  void setCurrentIteration(const int &currentIteration);

 private:
  Logger();
  static Logger* instance;

  std::string resultDirectory;
  int currentIteration;

  static const std::string C_GRAPH_FILEPATTERN;
  static const std::string C_LOG_FILEPATTERN;
  static const std::string C_ITERATION_RESULTS_FILEPATTERN;
  static const int C_MIN_LOG_LEVEL;
};


template<class Object>
void Logger::logFunction(const LogLevel &logLevel,
                         const Object &object,
                         void (Object::* const functionHandle)(std::ostream &) const,
                         const LogOutput &logOutput) const {
  if (Logger::isLogged(logLevel)) {
    std::ostringstream stringStream;
    ((object).*(functionHandle))(stringStream);
    log(logLevel, stringStream.str(), logOutput);
  }
}

}

#endif