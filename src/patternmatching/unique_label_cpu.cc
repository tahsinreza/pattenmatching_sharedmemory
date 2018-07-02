
#include <omp.h>
#include <iostream>
#include <iomanip>
#include "unique_label_cpu.h"
#include "logger.h"
#include "common_utils.h"

namespace patternmatching {

void UniqueLabelCpu::initialiseTotem() {

  // Configure OpenMP.
  omp_set_num_threads(omp_get_max_threads());
  omp_set_schedule(omp_sched_guided, 0);

}

error_t UniqueLabelCpu::allocate(CmdLineOption &cmdLineOption) {
  // Load graph
  Logger::get().log(Logger::E_LEVEL_INFO, "Loading graph");
  if(isFile(cmdLineOption.getInputGraphBinFilePath())) {
    graph_initialize(cmdLineOption.getInputGraphBinFilePath().c_str(), 0, &graph);
  } else {
    graph_initialize(cmdLineOption.getInputGraphFilePath().c_str(), 0, &graph);

    if (!cmdLineOption.getInputVertexMetadataFilePath().empty()) {
      FILE *file_handler = fopen(cmdLineOption.getInputVertexMetadataFilePath().c_str(), "r");
      if (file_handler != NULL) parseVertexFile(file_handler, graph);
    }
    Logger::get().log(Logger::E_LEVEL_INFO, std::string("Saved graph as : ")+ cmdLineOption.getInputGraphBinFilePath());
    graph_store_binary(graph, cmdLineOption.getInputGraphBinFilePath().c_str());
  }

  // Load pattern
  Logger::get().log(Logger::E_LEVEL_INFO, "Loading pattern");
  if(isFile(cmdLineOption.getInputPatternDirectory() + "pattern_edge.totem")) {
    graph_initialize((cmdLineOption.getInputPatternDirectory() + "pattern_edge.totem").c_str(), 0, &pattern);
  } else {
    graph_initialize((cmdLineOption.getInputPatternDirectory() + "pattern_edge").c_str(), 0, &pattern);
  }

  if (!cmdLineOption.getInputVertexMetadataFilePath().empty()) {
    FILE *file_handler = fopen((cmdLineOption.getInputPatternDirectory() + "pattern_vertex_data").c_str(), "r");
    if (file_handler != NULL) parseVertexFile(file_handler, pattern);
  }

  patternmatchingState.allocate(graph->vertex_count);

  // Initialise unique_label
  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize LCC, CC, and Step unique_label");

  lccCpu.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO, lccCpu, &LccType::printLocalConstraint, Logger::E_OUTPUT_FILE_LOG);

  ccCpu.init(*graph, *pattern);
  ccCpu.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO, ccCpu, &CcType::printCircularConstraint, Logger::E_OUTPUT_FILE_LOG);

  algorithmStep.initStepCc(ccCpu.getCircularConstraintNumber());

  Logger::get().log(Logger::E_LEVEL_INFO, "End of initialisation");
  return SUCCESS;
}

error_t UniqueLabelCpu::free() {
  #if 0
  totem_finalize();
    #endif
  patternmatchingState.free();
  graph_finalize(graph);

  return SUCCESS;
}

int UniqueLabelCpu::runPatternMatching() {
  // run step
  bool finished = 0;
  UniqueLabelStep::Step currentStep;
  totalStepTime = 0.;

  Logger::get().log(Logger::E_LEVEL_INFO, "Start run");

  int currentIteration = 0;
  algorithmStep.getNextStep(currentStepVertexEliminated, &currentStep, &currentStepName);
  while (!finished && currentIteration < 100) {
  // reinitialise
    switch (currentStep) {
      case UniqueLabelStep::E_CC :
        ccCpu.resetState(&patternmatchingState);
        break;
      default:break;
    }

    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);

    switch (currentStep) {
      case UniqueLabelStep::E_LCC :
        currentStepVertexEliminated = lccCpu.compute(*graph, &patternmatchingState);
        break;
      case UniqueLabelStep::E_CC :
        currentStepVertexEliminated = ccCpu.compute(*graph, &patternmatchingState);
        break;
      default:break;
    }

    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;

    logResults(currentIteration, false);

    finished = algorithmStep.getNextStep(currentStepVertexEliminated, &currentStep, &currentStepName);
    currentIteration++;
  }

  //printActiveGraph(stringStream);
  Logger::get().log(Logger::E_LEVEL_INFO, "End run");
  Logger::get().log(Logger::E_LEVEL_INFO, "Saving pruned graph");
  logResults(currentIteration, true);

  return 0;
}

void UniqueLabelCpu::logResults(const int currentIteration, const bool logGraph) const {
  Logger::get().setCurrentIteration(currentIteration);
  // Print Result file
  if (currentIteration == 0) {
    std::string resultHeader = "Iteration; Step; Step time; Cumulative time; Eliminated vertex";
    Logger::get().log(Logger::E_LEVEL_RESULT, resultHeader, Logger::E_OUTPUT_FILE_ITERATION_RESULTS);
  }

  // Print result
  {
    auto format = "%04d; %s; %.4f ; %.4f ; %lu";
    auto size = std::snprintf(nullptr, 0, format,
                              currentIteration, currentStepName.c_str(), currentStepTime,
                              totalStepTime, currentStepVertexEliminated);
    std::string output(size + 1, 'a');
    std::sprintf(&output[0], format,
                 currentIteration, currentStepName.c_str(), currentStepTime,
                 totalStepTime, currentStepVertexEliminated);
    output.resize(output.size() - 1);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_FILE_ITERATION_RESULTS);
  }

  // Log result
  {
    auto format = "Iteration %4d [%s], Running time %.4f, Eliminated vertex %lu";
    auto size = std::snprintf(nullptr, 0, format,
                              currentIteration, currentStepName.c_str(), currentStepTime, currentStepVertexEliminated);
    std::string output(size + 1, '0');
    std::sprintf(&output[0], format,
                 currentIteration, currentStepName.c_str(), currentStepTime, currentStepVertexEliminated);
    output.resize(output.size() - 1);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }

  // Save graph
  if (logGraph) {
    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              *this,
                              &UniqueLabelCpu::printActiveGraph,
                              Logger::E_OUTPUT_FILE_GRAPH);
  }

}

void UniqueLabelCpu::printActiveGraph(std::ostream &ostream) const {
  for (vid_t vid = 0; vid < graph->vertex_count; vid++) {
    if (patternmatchingState.vertexActiveList[vid] == 0) continue;

    for (eid_t i = graph->vertices[vid]; i < graph->vertices[vid + 1]; i++) {
      vid_t neighborVertexId = graph->edges[i];

      if (patternmatchingState.vertexActiveList[neighborVertexId] == 0) continue;

      ostream << vid << " " << neighborVertexId << " " << graph->values[vid] << " " << graph->values[neighborVertexId]
              << std::endl;
    }
  }
}

}