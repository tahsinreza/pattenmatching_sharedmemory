
#include <omp.h>
#include <iostream>
#include <iomanip>

#include "totem.h"
#include "graph_stat_effectiveness_cpu.h"
#include "logger.h"
#include "common_utils.h"

namespace patternmatching {

void GraphStatEffectivenessCpu::initialiseTotem() {
  // Configure OpenMP.
  omp_set_num_threads(omp_get_max_threads());
  omp_set_schedule(omp_sched_guided, 0);

}

error_t GraphStatEffectivenessCpu::allocate(CmdLineOption &cmdLineOption) {
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

  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize Totem and global state");
  initialiseTotem();

  patternmatchingState.allocate(graph->vertex_count, graph->edge_count, pattern->vertex_count);

  // Generate pattern
  Logger::get().log(Logger::E_LEVEL_INFO, "Generate pattern multiple_label");
  Logger::get().log(Logger::E_LEVEL_INFO, "Generate Graph Stat");
  graphStatCpu.preprocessPatern(*pattern);

  Logger::get().log(Logger::E_LEVEL_INFO, "Generate Local constraint");
  generateLocal.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO,
                            generateLocal, &MultipleLabelGenerateConstraintLocal::print, Logger::E_OUTPUT_FILE_LOG);

  Logger::get().log(Logger::E_LEVEL_INFO, "Generate Circular constraint");
  generateCircular.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO,
                            generateCircular, &MultipleLabelGenerateConstraintCircular::print, Logger::E_OUTPUT_FILE_LOG);

  Logger::get().log(Logger::E_LEVEL_INFO, "Generate Path constraint");
  generatePath.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO,
                            generatePath, &MultipleLabelGenerateConstraintPath::print, Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_INFO,
                            generatePath, &MultipleLabelGenerateConstraintPath::print, Logger::E_OUTPUT_COUT);

  Logger::get().log(Logger::E_LEVEL_INFO, "Generate Template constraint");
  generateTemplate.preprocessPatern(*pattern, generateCircular.getConstraintVector(), generatePath.getConstraintVector());
  Logger::get().logFunction(Logger::E_LEVEL_INFO,
                            generateTemplate, &MultipleLabelGenerateConstraintTemplate::print, Logger::E_OUTPUT_FILE_LOG);

  Logger::get().log(Logger::E_LEVEL_INFO, "Generate Enumeration constraint");
  generateEnumeration.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO,
                            generateEnumeration, &MultipleLabelGenerateConstraintEnumeration::print, Logger::E_OUTPUT_FILE_LOG);

  // Initialise unique_label
  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize LCC0, LCC, CC, PC, TDS, and Enumeration");
  lcc0Cpu.setConstraintVector(generateLocal.getConstraintVector());
  lccCpu.setConstraintVector(generateLocal.getConstraintVector());
  ccCpu.setConstraintVector(generateCircular.getConstraintVector());
  ccBacktrackCpu.setConstraintVector(generateCircular.getConstraintVector());
  ccStrictCpu.setConstraintVector(generateCircular.getConstraintVector());
  pcCpu.setConstraintVector(generatePath.getConstraintVector());
  pcBacktrackCpu.setConstraintVector(generatePath.getConstraintVector());
  pcStrictCpu.setConstraintVector(generatePath.getConstraintVector());
  tdsCpu.setConstraintVector(generateTemplate.getConstraintVector());
  tdsBacktrackCpu.setConstraintVector(generateTemplate.getConstraintVector());
  tdsStrictCpu.setConstraintVector(generateTemplate.getConstraintVector());
  enumerationCpu.setConstraint(generateEnumeration.getConstraint());

  currentIteration=0;

  Logger::get().log(Logger::E_LEVEL_INFO, "End of initialisation");
  return SUCCESS;
}

error_t GraphStatEffectivenessCpu::free() {
  patternmatchingState.free();
  graph_finalize(graph);

  return SUCCESS;
}

int GraphStatEffectivenessCpu::runPatternMatching() {
  totalStepTime = 0.;
  currentIteration = 0;

  Logger::get().log(Logger::E_LEVEL_INFO, "Start run");
  // Run LCC0
  {
    currentStepName="LCC0";
    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    algoResults = lcc0Cpu.compute(*graph, &patternmatchingState);
    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    logResults(currentIteration, false);

    currentIteration++;
  }

  Logger::get().log(Logger::E_LEVEL_INFO, "Graph stat");
  // Run Graph Stat
  {
    currentStepName="Graph Stat";
    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    graphStat = graphStatCpu.compute(*graph, &patternmatchingState);

    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    algoResults=AlgoResults();
    logResults(currentIteration, false);

    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              graphStat, &GraphStat::print,
                              Logger::E_OUTPUT_FILE_LOG);
    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              graphStat, &GraphStat::print,
                              Logger::E_OUTPUT_COUT);
  }

  Logger::get().log(Logger::E_LEVEL_INFO, "Effectiveness Approximation");
  runEffectivenessApproximation();
/*
  Logger::get().log(Logger::E_LEVEL_INFO, "Run LCC");
  // Run LCC
  {
    currentStepName="LCC";
    patternMatchingStateTemporary = patternmatchingState;
    bool finished = false;
    totalStepTime = 0;
    while (!finished) {

      totem_timing_reset();
      stopwatch_t stopwatch;
      stopwatch_start(&stopwatch);
      algoResults = lccCpu.compute(*graph, &patternMatchingStateTemporary);
      currentStepTime = stopwatch_elapsed(&stopwatch);
      totalStepTime += currentStepTime;
      logResults(currentIteration, false);

      currentIteration++;
      if (algoResults.isEmpty()) finished = true;
    }
  }*/
/*
  Logger::get().log(Logger::E_LEVEL_INFO, "Run CC");
  currentStepName="CC";
  runTest(generateCircular, ccCpu);*/

  Logger::get().log(Logger::E_LEVEL_INFO, "Run CC Strict");
  currentStepName="CC Strict";
  runTest(generateCircular, ccStrictCpu);
/*
  Logger::get().log(Logger::E_LEVEL_INFO, "Run CC Backtrack");
  currentStepName="CC Backtrack";
  runTest(generateCircular, ccBacktrackCpu);

  Logger::get().log(Logger::E_LEVEL_INFO, "Run PC");
  currentStepName="PC";
  runTest(generatePath, pcCpu);

  Logger::get().log(Logger::E_LEVEL_INFO, "Run PC Strict");
  currentStepName="PC Strict";
  runTest(generatePath, pcStrictCpu);

  Logger::get().log(Logger::E_LEVEL_INFO, "Run PC Backtrack");
  currentStepName="PC Backtrack";
  runTest(generatePath, pcBacktrackCpu);

  Logger::get().log(Logger::E_LEVEL_INFO, "Run TDS");
  currentStepName="TDS";
  runTest(generateTemplate, tdsCpu);

  Logger::get().log(Logger::E_LEVEL_INFO, "Run TDS Strict");
  currentStepName="TDS Strict";
  runTest(generateTemplate, tdsStrictCpu);

  Logger::get().log(Logger::E_LEVEL_INFO, "Run TDS Backtrack");
  currentStepName="TDS Backtrack";
  runTest(generateTemplate, tdsBacktrackCpu);*/

  Logger::get().log(Logger::E_LEVEL_INFO, "Run Enumerate");
  currentStepName="ENUMERATE";
  runTest(generateEnumeration, enumerationCpu);

  return 0;
}

void GraphStatEffectivenessCpu::runEffectivenessApproximation() {
  MultipleLabelConstraintEffectiveness effectiveness;

  for(const auto &it : generateCircular.getConstraintVector()) {
    //effectiveness.compute(*pattern, graphStat, it);
    effectiveness.computeStrict(*pattern, graphStat, it);
    logEffectiveness(currentIteration, effectiveness);
  }

}

template<class Generator, class Algorithm>
void GraphStatEffectivenessCpu::runTest(Generator &generator, Algorithm &algorithm) {
  for(int currentConstraint=0; currentConstraint< generator.getConstraintNumber(); currentConstraint++) {
    patternMatchingStateTemporary = patternmatchingState;

    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);
    algoResults = algorithm.compute(*graph, &patternMatchingStateTemporary);
    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;
    logResults(currentIteration, false);

    currentIteration++;
  }
};

void GraphStatEffectivenessCpu::logEffectiveness(const int currentIteration, const MultipleLabelConstraintEffectiveness &effectiveness) const {
  Logger::get().setCurrentIteration(currentIteration);
  static bool first=true;
  // Print Result file
  if (first) {
    std::string resultHeader = "Iteration; Step; Number pruned; Cost; Effectiveness";
    Logger::get().log(Logger::E_LEVEL_RESULT, resultHeader, Logger::E_OUTPUT_FILE_EFFECTIVENESS_RESUTLS);
    first=false;
  }

  // Print result csv
  {
    auto output=sprintfString("%04d; %.6f ; %.0f ;  %.6f ",
                              currentIteration, currentStepName.c_str(),
                              effectiveness.approximateMatchEliminated, effectiveness.approximateCostEliminated, effectiveness.approximateEffectiveness);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_FILE_EFFECTIVENESS_RESUTLS);
  }

  // Log result
  {
    auto output=sprintfString( "Iteration %4d [%s], "
                               "Number pruned %.6f, Cost %.0f, Effectiveness %.6f",
                               currentIteration, currentStepName.c_str(),
                               effectiveness.approximateMatchEliminated, effectiveness.approximateCostEliminated, effectiveness.approximateEffectiveness);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }
}

void GraphStatEffectivenessCpu::logResults(const int currentIteration, const bool logGraph) const {
  Logger::get().setCurrentIteration(currentIteration);
  // Print Result file
  if (currentIteration == 0) {
    std::string resultHeader = "Iteration; Step; Step time; Cumulative time; "
                               "Eliminated vertex; Active vertex; Total vertex; "
                               "Eliminated edge; Active edge; Total edge; "
                               "Eliminated match; "
                               "Enumeration number; "
                               "Effectiveness";
    Logger::get().log(Logger::E_LEVEL_RESULT, resultHeader, Logger::E_OUTPUT_FILE_ITERATION_RESULTS);
  }

  // Print result csv
  {
    auto output=sprintfString("%04d; %s; %.4f ; %.4f ; "
                        "%zu; %zu; %zu; "
                        "%zu; %zu; %zu; "
                        "%zu; "
                        "%zu; "
                        "%.6f",
                     currentIteration, currentStepName.c_str(), currentStepTime, totalStepTime,
                        algoResults.vertexEliminated, patternmatchingState.graphActiveVertexCount, patternmatchingState.graphVertexCount,
                        algoResults.edgeEliminated, patternmatchingState.graphActiveEdgeCount, patternmatchingState.graphEdgeCount,
                        algoResults.matchEliminated,
                        algoResults.enumeration,
                        static_cast<double>(algoResults.matchEliminated)/currentStepTime);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_FILE_ITERATION_RESULTS);
  }

  // Log result
  {
    auto output=sprintfString( "Iteration %4d [%s], Running time %.4f/%.4f, "
                               "Eliminated vertex %zu/%zu/%zu, "
                               "Eliminated edge %zu/%zu/%zu, "
                               "Eliminated match %zu,"
                               "Enumeration %zu,"
                               "Effectiveness %.6f",
                               currentIteration, currentStepName.c_str(), currentStepTime, totalStepTime,
                               algoResults.vertexEliminated, patternmatchingState.graphActiveVertexCount, patternmatchingState.graphVertexCount,
                               algoResults.edgeEliminated, patternmatchingState.graphActiveEdgeCount, patternmatchingState.graphEdgeCount,
                               algoResults.matchEliminated,
                               algoResults.enumeration,
                               static_cast<double>(algoResults.matchEliminated)/currentStepTime);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }

  // Save graph
  if (logGraph) {
    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              *this,
                              &GraphStatEffectivenessCpu::printActiveGraph,
                              Logger::E_OUTPUT_FILE_GRAPH);
  }

  // Save graph
  if (logGraph) {
    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              *this,
                              &GraphStatEffectivenessCpu::printActiveVertex,
                              Logger::E_OUTPUT_FILE_VERTEX);
  }

}

void GraphStatEffectivenessCpu::printActiveGraph(std::ostream &ostream) const {
  for (vid_t vertexId = 0; vertexId < graph->vertex_count; vertexId++) {
    if (patternmatchingState.vertexActiveList[vertexId] == 0) continue;

    for (eid_t edgeId = graph->vertices[vertexId]; edgeId < graph->vertices[vertexId + 1]; edgeId++) {
      if (patternmatchingState.edgeActiveList[edgeId] == 0) continue;
      vid_t neighborVertexId = graph->edges[edgeId];

      if (patternmatchingState.vertexActiveList[neighborVertexId] == 0) continue;

      ostream << vertexId << " " << neighborVertexId << " " << graph->values[vertexId] << " " << graph->values[neighborVertexId]
              << std::endl;
    }
  }
}

void GraphStatEffectivenessCpu::printActiveVertex(std::ostream &ostream) const {
  for (vid_t vertexId = 0; vertexId < graph->vertex_count; vertexId++) {
    if (patternmatchingState.vertexActiveList[vertexId] == 0) continue;

    ostream << vertexId << " ";

    for(const auto& it : patternmatchingState.vertexPatternMatch[vertexId]) {
      ostream << it << " ";
    }
    ostream << std::endl;
  }
}

}