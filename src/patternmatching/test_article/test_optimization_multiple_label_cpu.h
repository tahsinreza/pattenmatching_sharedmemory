

#ifndef TOTEM_MULTIPLE_LABEL_CPU_CUH
#define TOTEM_MULTIPLE_LABEL_CPU_CUH

#include "cmd_line_option.h"
#include "test_optimization_multiple_label_step.h"
#include "totem_graph.h"
#include "multiple_label_common_cpu.h"
#include "multiple_label_generate_constraint_circular.h"
#include "multiple_label_generate_constraint_enumeration.h"
#include "multiple_label_generate_constraint_local.h"
#include "multiple_label_generate_constraint_path.h"
#include "multiple_label_generate_constraint_template.h"
#include "multiple_label_lcc0_cpu.h"
#include "multiple_label_lcc_cpu.h"
#include "multiple_label_cc_backtrack_cpu.h"
#include "multiple_label_cc_strict_cpu.h"
#include "multiple_label_pc_backtrack_cpu.h"
#include "multiple_label_pc_strict_cpu.h"
#include "multiple_label_tds_backtrack_cpu.h"
#include "multiple_label_tds_strict_cpu.h"
#include "multiple_label_enumeration_cpu.h"
#include "undirected_cpu.h"
#include "algo_results.h"

namespace patternmatching {

class MultipleLabelCpu {
 public:
  typedef MultipleLabelGlobalStateInt StateType;

  typedef MultipleLabelLcc0Cpu<StateType> Lcc0Type;
  typedef MultipleLabelLccCpu<StateType> LccType;

  #ifdef OPTIMIZATION_USE_WORK_AGGREGATION
  typedef MultipleLabelCcBacktrackCpu<StateType> CcType;
  typedef MultipleLabelPcBacktrackCpu<StateType> PcType;
  typedef MultipleLabelTdsBacktrackCpu<StateType> TdsType;
  #else
  typedef MultipleLabelCcStrictCpu<StateType> CcType;
  typedef MultipleLabelPcStrictCpu<StateType> PcType;
  typedef MultipleLabelTdsStrictCpu<StateType> TdsType;
  #endif

  typedef MultipleLabelEnumerationCpu<StateType> EnumerationType;
  int runPatternMatching();

  error_t allocate(CmdLineOption &cmdLineOption);
  error_t free();

 private:
  void initialiseTotem();
  void printActiveGraph(std::ostream &ostream=std::cout) const;
  void printActiveVertex(std::ostream &ostream=std::cout) const;
  void logResults(const int currentIteration, const bool logGraph) const;

  AlgoResults algoResults;
  std::string currentStepName;
  double currentStepTime;
  double totalStepTime;

  graph_t *graph;
  graph_t *pattern;
  totem_attr_t attributeCpu;
  MultipleLabelGlobalStateInt patternmatchingState;

  MultipleLabelGenerateConstraintLocal generateLocal;
  MultipleLabelGenerateConstraintEnumeration generateEnumeration;
  MultipleLabelGenerateConstraintPath generatePath;
  MultipleLabelGenerateConstraintCircular generateCircular;
  MultipleLabelGenerateConstraintTemplate generateTemplate;

  MultipleLabelStep algorithmStep;
  MultipleLabelStep::Step currentStep;

  Lcc0Type lcc0Cpu;
  LccType lccCpu;
  CcType ccCpu;
  PcType pcCpu;
  TdsType tdsCpu;

  EnumerationType enumerationCpu;

};

}

#endif