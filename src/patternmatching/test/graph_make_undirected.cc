//
// Created by qiu on 17/05/18.
//

#include "cmd_line_option.h"
#include "logger.h"
#include "graph_make_undirected_cpu.h"

using namespace patternmatching;

int main(int argc, char **argv) {
  CmdLineOption cmdLineOption;
  CmdLineOption::ArgumentSet optionalArguments;
  optionalArguments.set(CmdLineOption::INPUT_PATTERN_DIRECTORY);
  cmdLineOption.parseCmdLine(argc, argv, optionalArguments);

  Logger::get().init(cmdLineOption.getOutputResultDirectory(), false);

  GraphMakeUndirectedCpu graphMakeUndirectedCpu;
  graphMakeUndirectedCpu.allocate(cmdLineOption);

  graphMakeUndirectedCpu.runMakeUndirected();

  graphMakeUndirectedCpu.free();

  return 0;
}
