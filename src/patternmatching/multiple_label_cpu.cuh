

#ifndef TOTEM_MULTIPLE_LABEL_CPU_CUH
#define TOTEM_MULTIPLE_LABEL_CPU_CUH

#include "patternmatching_cmd_line_option.h"
#include "multiple_label_step.h"
#include "totem.h"
#include "totem_mem.h"
#include "algorithms/multiple_label_common_cpu.cuh"
#include "algorithms/multiple_label_lcc_cpu.cuh"
#include "algorithms/multiple_label_cc_cpu.cuh"
#include "algorithms/multiple_label_pc_cpu.cuh"

namespace patternmatching {

class MultipleLabelCpu {
 public:
  typedef MultipleLabelGlobalStateInt StateType;
  typedef MultipleLabelLccCpu<StateType> LccType;
  typedef MultipleLabelCcCpu<StateType> CcType;
  typedef MultipleLabelPcCpu<StateType> PcType;
  int runPatternMatching();

  error_t allocate(CmdLineOption &cmdLineOption);
  error_t free();

 private:
  void initialiseTotem();
  void printActiveGraph(std::ostream &ostream=std::cout) const;
  void printActiveVertex(std::ostream &ostream=std::cout) const;
  void logResults(const int currentIteration, const bool logGraph) const;

  size_t currentStepVertexEliminated;
  std::string currentStepName;
  double currentStepTime;
  double totalStepTime;

  graph_t *graph;
  graph_t *pattern;
  totem_attr_t attributeCpu;
  MultipleLabelGlobalStateInt patternmatchingState;

  MultipleLabelStep algorithmStep;

  LccType lccCpu;
  CcType ccCpu;
  PcType pcCpu;
};

}

#endif