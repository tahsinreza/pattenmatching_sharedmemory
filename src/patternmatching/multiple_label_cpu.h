

#ifndef TOTEM_MULTIPLE_LABEL_CPU_CUH
#define TOTEM_MULTIPLE_LABEL_CPU_CUH

#include "patternmatching_cmd_line_option.h"
#include "multiple_label_step.h"
#include "totem_graph.h"
#include "algorithms/multiple_label_common_cpu.h"
#include "algorithms/multiple_label_lcc0_cpu.h"
#include "algorithms/multiple_label_lcc_cpu.h"
#include "algorithms/multiple_label_cc_cpu.h"
#include "algorithms/multiple_label_cc_backtrack_cpu.h"
#include "algorithms/multiple_label_cc_strict_cpu.h"
#include "algorithms/multiple_label_pc_cpu.h"
#include "algorithms/multiple_label_pc_backtrack_cpu.h"
#include "algorithms/multiple_label_pc_strict_cpu.h"
#include "algorithms/multiple_label_tds_cpu.h"
#include "algorithms/multiple_label_tds_backtrack_cpu.h"
#include "algorithms/multiple_label_tds_strict_cpu.h"

namespace patternmatching {

class MultipleLabelCpu {
 public:
  typedef MultipleLabelGlobalStateInt StateType;
  typedef MultipleLabelLcc0Cpu<StateType> Lcc0Type;
  typedef MultipleLabelLccCpu<StateType> LccType;
  //typedef MultipleLabelCcBacktrackCpu<StateType> CcType;
  //typedef MultipleLabelCcStrictCpu<StateType> CcType;
  typedef MultipleLabelCcCpu<StateType> CcType;
  typedef MultipleLabelPcCpu<StateType> PcType;
  //typedef MultipleLabelPcBacktrackCpu<StateType> PcType;
  //typedef MultipleLabelPcStrictCpu<StateType> PcType;
  typedef MultipleLabelTdsCpu<StateType> TdsType;
  //typedef MultipleLabelTdsBacktrackCpu<StateType> TdsType;
  //typedef MultipleLabelTdsStrictCpu<StateType> TdsType;
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

  Lcc0Type lcc0Cpu;
  LccType lccCpu;
  CcType ccCpu;
  PcType pcCpu;
  TdsType tdsCpu;
};

}

#endif