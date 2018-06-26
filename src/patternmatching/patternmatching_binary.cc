//
// Created by qiu on 17/05/18.
//

#include "cmd_line_option.h"
#include "unique_label_cpu.h"
#include "multiple_label_cpu.h"
#include "logger.h"

using namespace patternmatching;


int main(int argc, char** argv) {
  CmdLineOption cmdLineOption;
  cmdLineOption.parseCmdLine(argc, argv);

  Logger::get().init(cmdLineOption.getOutputResultDirectory());


  if(0) {
    UniqueLabelCpu uniqueLabelCpu;
    uniqueLabelCpu.allocate(cmdLineOption);

    uniqueLabelCpu.runPatternMatching();

    uniqueLabelCpu.free();
  }
  else {
    MultipleLabelCpu multipleLabelCpu;
    multipleLabelCpu.allocate(cmdLineOption);

    multipleLabelCpu.runPatternMatching();

    multipleLabelCpu.free();

  }

  return 0;
}
