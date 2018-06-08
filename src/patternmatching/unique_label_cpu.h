

#ifndef TOTEM_UNIQUE_LABEL_CPU_CUH
#define TOTEM_UNIQUE_LABEL_CPU_CUH

#include "patternmatching_cmd_line_option.h"
#include "unique_label_step.h"
#include "totem.h"
#include "totem_mem.h"
#include "algorithms/unique_label_common_cpu.cuh"
#include "algorithms/unique_label_lcc_cpu.cuh"
#include "algorithms/unique_label_cc_cpu.cuh"

namespace patternmatching {

class UniqueLabelCpu {
 public:
  typedef UniqueLabelGlobalStateInt StateType;
  typedef UniqueLabelLccCpu<StateType> LccType;
  typedef UniqueLabelCcCpu<StateType> CcType;
  int runPatternMatching();

  error_t allocate(CmdLineOption &cmdLineOption);
  error_t free();

 private:
  void initialiseTotem();
  void printActiveGraph(std::ostream &ostream=std::cout) const;
  void logResults(const int currentIteration, const bool logGraph) const;

  size_t currentStepVertexEliminated;
  std::string currentStepName;
  double currentStepTime;
  double totalStepTime;

  graph_t *graph;
  graph_t *pattern;
  totem_attr_t attributeCpu;
  UniqueLabelGlobalStateInt patternmatchingState;

  UniqueLabelStep algorithmStep;

  LccType lccCpu;
  CcType ccCpu;
};

}

#endif