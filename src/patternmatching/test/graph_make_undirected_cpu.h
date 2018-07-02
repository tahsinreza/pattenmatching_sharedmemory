

#ifndef TOTEM_GRAPH_MAKE_UNDIRECTED_CPU_CUH
#define TOTEM_GRAPH_MAKE_UNDIRECTED_CPU_CUH

#include "cmd_line_option.h"
#include "totem_graph.h"
#include "make_undirected_cpu.h"

namespace patternmatching {

class GraphMakeUndirectedCpu {
 public:
  using MakeUndirectedType = MakeUndirectedCpu;


  int runMakeUndirected();

  error_t allocate(CmdLineOption &cmdLineOption);
  error_t free();

 private:
  void initialiseTotem();
  void printGraph(std::ostream &ostream=std::cout) const;

  double currentStepTime;
  double totalStepTime;

  graph_t *graph;
  graph_t *pattern;
  MakeUndirectedType::EdgeSetType edgeSet;
  MakeUndirectedType makeUndirectedCpu;
  std::string outputFileUndirected;
  std::string outputFileUndirectedPruned;

};

}

#endif