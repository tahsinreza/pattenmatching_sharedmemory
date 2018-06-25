

#ifndef TOTEM_GRAPH_EFFECTIVENESS_CPU_CUH
#define TOTEM_GRAPH_EFFECTIVENESS_CPU_CUH

#include "patternmatching_cmd_line_option.h"
#include "multiple_label_step.h"
#include "totem_graph.h"
#include "multiple_label_common_cpu.h"
#include "multiple_label_lcc0_cpu.h"
#include "multiple_label_lcc_cpu.h"
#include "multiple_label_cc_cpu.h"
#include "multiple_label_cc_backtrack_cpu.h"
#include "multiple_label_cc_strict_cpu.h"
#include "multiple_label_pc_cpu.h"
#include "multiple_label_pc_backtrack_cpu.h"
#include "multiple_label_pc_strict_cpu.h"
#include "multiple_label_tds_cpu.h"
#include "multiple_label_tds_backtrack_cpu.h"
#include "multiple_label_tds_strict_cpu.h"
#include "graph_stat_cpu.h"

namespace patternmatching {

class GraphStatEffectivenessCpu {
 public:
  typedef MultipleLabelGlobalStateInt StateType;
  typedef MultipleLabelLcc0Cpu<StateType> Lcc0Type;
  typedef MultipleLabelLccCpu<StateType> LccType;
  typedef MultipleLabelCcCpu<StateType> CcType;
  typedef MultipleLabelPcCpu<StateType> PcType;
  typedef MultipleLabelTdsCpu<StateType> TdsType;
  typedef GraphStatCpu<StateType, MultipleLabelCpuBase<StateType>> GraphStatType;
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
  MultipleLabelGlobalStateInt patternMatchingStateTemporary;

  Lcc0Type lcc0Cpu;
  LccType lccCpu;
  CcType ccCpu;
  PcType pcCpu;
  TdsType tdsCpu;
  GraphStatType graphStatCpu;
};

}

#endif