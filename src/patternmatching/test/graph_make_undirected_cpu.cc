
#include <omp.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "totem.h"
#include "graph_make_undirected_cpu.h"
#include "logger.h"
#include "common_utils.h"

namespace patternmatching {

void GraphMakeUndirectedCpu::initialiseTotem() {
  // Configure OpenMP.
  omp_set_num_threads(omp_get_max_threads());
  omp_set_schedule(omp_sched_guided, 0);

}

error_t GraphMakeUndirectedCpu::allocate(CmdLineOption &cmdLineOption) {
  // Load graph
  Logger::get().log(Logger::E_LEVEL_INFO, "Loading graph");
  graph_initialize(cmdLineOption.getInputGraphFilePath().c_str(), 0, &graph);

  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize Totem");
  initialiseTotem();

  outputFileUndirected = cmdLineOption.getInputGraphFilePath() + ".undirected";
  outputFileUndirectedPruned = cmdLineOption.getInputGraphFilePath() + ".undirected-pruned";
  Logger::get().log(Logger::E_LEVEL_INFO, std::string("Output file : ") + outputFileUndirected);
  Logger::get().log(Logger::E_LEVEL_INFO, std::string("Output file : ") + outputFileUndirectedPruned);

  Logger::get().log(Logger::E_LEVEL_INFO, "End of initialisation");
  return SUCCESS;
}

error_t GraphMakeUndirectedCpu::free() {
  graph_finalize(graph);

  return SUCCESS;
}

int GraphMakeUndirectedCpu::runMakeUndirected() {
  totalStepTime = 0.;
/*
  Logger::get().log(Logger::E_LEVEL_INFO, "Start Undirected");
  {
    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    makeUndirectedCpu.compute(*graph, edgeSet);
    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    auto output = sprintfString("Running time %.4f/%.4f", currentStepTime, totalStepTime);
    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }

  Logger::get().log(Logger::E_LEVEL_INFO, "Print graph file Undirected");
  {
    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    std::ofstream fileStream(outputFileUndirected, std::ios::trunc | std::ios::out);
    printGraph(fileStream);
    fileStream.close();
    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    auto output = sprintfString("Running time %.4f/%.4f", currentStepTime, totalStepTime);
    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }
*/
  Logger::get().log(Logger::E_LEVEL_INFO, "Start Undirected");
  {
    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    makeUndirectedCpu.computeRemove(*graph, edgeSet);
    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    auto output = sprintfString("Running time %.4f/%.4f", currentStepTime, totalStepTime);
    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }

  Logger::get().log(Logger::E_LEVEL_INFO, "Print graph file Undirected");
  {
    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    std::ofstream fileStream(outputFileUndirectedPruned, std::ios::trunc | std::ios::out);
    printGraph(fileStream);
    fileStream.close();
    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    auto output = sprintfString("Running time %.4f/%.4f", currentStepTime, totalStepTime);
    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }

  return 0;
}

void GraphMakeUndirectedCpu::printGraph(std::ostream &ostream) const {
  ostream << "#Nodes: " << graph->vertex_count << std::endl;
  ostream << "#Edges: " << edgeSet.size() << std::endl;
  ostream << "#Undirected" << std::endl;
  for (const auto &it : edgeSet) {
    ostream << it.vertexFromId << " " << it.vertexToIt << std::endl;
  }
}

}