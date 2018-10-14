//
// Created by qiu on 17/05/18.
//

#include "cmd_line_option.h"
#include "logger.h"
#include "test_optimization_multiple_label_cpu.h"

using namespace patternmatching;

int main(int argc, char **argv) {
  CmdLineOption cmdLineOption;
  cmdLineOption.parseCmdLine(argc, argv);

  Logger::get().init(cmdLineOption.getOutputResultDirectory(), false);

  #ifdef OPTIMIZATION_USE_WORK_AGGREGATION
    Logger::get().log(Logger::E_LEVEL_INFO, "Run with work aggregation");
  #endif
  #ifdef OPTIMIZATION_USE_EARLY_TERMINATION
  Logger::get().log(Logger::E_LEVEL_INFO, "Run with early termination");
  #endif
  #ifdef OPTIMIZATION_USE_MULTIPLE_VALIDATION
  Logger::get().log(Logger::E_LEVEL_INFO, "Run with multiple validation");
  #endif


  MultipleLabelCpu multipleLabelCpu;
  multipleLabelCpu.allocate(cmdLineOption);

  multipleLabelCpu.runPatternMatching();

  multipleLabelCpu.free();

  return 0;
}
