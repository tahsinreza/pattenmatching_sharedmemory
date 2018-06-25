//
// Created by qiu on 17/05/18.
//

#include "patternmatching_cmd_line_option.h"
#include "patternmatching_logger.h"
#include "graph_stat_effectiveness_cpu.h"

using namespace patternmatching;

int main(int argc, char **argv) {
  CmdLineOption cmdLineOption;
  cmdLineOption.parseCmdLine(argc, argv);

  Logger::get().init(cmdLineOption.getOutputResultDirectory());

  GraphStatEffectivenessCpu graphStatEffectivenessCpu;
  graphStatEffectivenessCpu.allocate(cmdLineOption);

  graphStatEffectivenessCpu.runPatternMatching();

  graphStatEffectivenessCpu.free();

  return 0;
}
