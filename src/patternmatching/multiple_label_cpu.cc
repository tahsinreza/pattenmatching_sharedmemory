
#include <omp.h>
#include <iostream>
#include <iomanip>

#include "totem.h"
#include "multiple_label_cpu.h"
#include "logger.h"

namespace patternmatching {

void MultipleLabelCpu::initialiseTotem() {
  // Configure OpenMP.
  //omp_set_num_threads(omp_get_max_threads());
  omp_set_num_threads(60);
  omp_set_schedule(omp_sched_guided, 0);

}

error_t MultipleLabelCpu::allocate(CmdLineOption &cmdLineOption) {
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

  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize STEPS");
  algorithmStep.initStepCc(generateCircular.getConstraintNumber());
  algorithmStep.initStepPc(generatePath.getConstraintNumber());
  algorithmStep.initStepTds(generateTemplate.getConstraintNumber());

  Logger::get().log(Logger::E_LEVEL_INFO, "End of initialisation");
  return SUCCESS;
}

error_t MultipleLabelCpu::free() {
  patternmatchingState.free();
  graph_finalize(graph);

  return SUCCESS;
}

int MultipleLabelCpu::runPatternMatching() {
  // run step
  bool finished = 0;
  totalStepTime = 0.;
  Logger::get().log(Logger::E_LEVEL_INFO, "Start run");

  int currentIteration = 0;
  algorithmStep.getNextStep(algoResults, &currentStep, &currentStepName);
  while (!finished && currentIteration < 1000) {
  // reinitialise
    switch (currentStep) {
      case MultipleLabelStep::E_CC :
        ccCpu.resetState(&patternmatchingState);
        break;
      case MultipleLabelStep::E_PC :
        pcCpu.resetState(&patternmatchingState);
        break;
      case MultipleLabelStep::E_TDS :
        tdsCpu.resetState(&patternmatchingState);
        break;
      default:break;
    }
    std::cout << "Start "<< currentStepName << std::endl;

    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);

    switch (currentStep) {
      case MultipleLabelStep::E_LCC0 :
        algoResults = lcc0Cpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_LCC :
        algoResults = lccCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_CC :
        algoResults = ccCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_PC :
        algoResults = pcCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_TDS :
        algoResults = tdsCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_ENUMERATION :
        algoResults = enumerationCpu.compute(*graph, &patternmatchingState);
        break;
      default:break;
    }

    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;

    logResults(currentIteration, false);

    if(currentStep==MultipleLabelStep::E_LCC0 && 0) {
      Logger::get().log(Logger::E_LEVEL_INFO, "Directed run");
      auto directedEdges = undirectedCpu.compute(*graph, &patternmatchingState);
      std::cout << "undirectedEdges = " << directedEdges <<std::endl;
    }

    finished = algorithmStep.getNextStep(algoResults, &currentStep, &currentStepName);
    currentIteration++;
  }

  Logger::get().log(Logger::E_LEVEL_INFO, "End run");
  Logger::get().log(Logger::E_LEVEL_INFO, "Saving pruned graph");
  currentStepTime=0.;
  algoResults=AlgoResults();
  logResults(currentIteration, true);

  return 0;
}

void MultipleLabelCpu::logResults(const int currentIteration, const bool logGraph) const {
  Logger::get().setCurrentIteration(currentIteration);
  // Print Result file
  if (currentIteration == 0) {
    std::string resultHeader = "Iteration; Step; Step time; Cumulative time; "
                               "Eliminated vertex; Active vertex; Total vertex; "
                               "Eliminated edge; Active edge; Total edge; "
                               "Eliminated match; "
                               "Enumeration number";
    Logger::get().log(Logger::E_LEVEL_RESULT, resultHeader, Logger::E_OUTPUT_FILE_ITERATION_RESULTS);
  }

  // Print result csv
  {
    auto output=sprintfString("%04d; %s; %.4f ; %.4f ; "
                              "%zu; %zu; %zu; "
                              "%zu; %zu; %zu; "
                              "%zu; "
                              "%zu",
                              currentIteration, currentStepName.c_str(), currentStepTime, totalStepTime,
                              algoResults.vertexEliminated, patternmatchingState.graphActiveVertexCount, patternmatchingState.graphVertexCount,
                              algoResults.edgeEliminated, patternmatchingState.graphActiveEdgeCount, patternmatchingState.graphEdgeCount,
                              algoResults.matchEliminated,
                              algoResults.enumeration);

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_FILE_ITERATION_RESULTS);
  }

  // Log result
  {
    std::string output;
    switch (currentStep) {
      case MultipleLabelStep::E_LCC0 :
      case MultipleLabelStep::E_LCC :
        output=sprintfString( "Iteration %4d [%s], Running time %.4f/%.4f, "
                              "Eliminated vertex %zu/%zu/%zu, "
                              "Eliminated edge %zu/%zu/%zu, "
                              "Eliminated match %zu",
                              currentIteration, currentStepName.c_str(), currentStepTime, totalStepTime,
                              algoResults.vertexEliminated, patternmatchingState.graphActiveVertexCount, patternmatchingState.graphVertexCount,
                              algoResults.edgeEliminated, patternmatchingState.graphActiveEdgeCount, patternmatchingState.graphEdgeCount,
                              algoResults.matchEliminated);
        break;
      case MultipleLabelStep::E_CC :
      case MultipleLabelStep::E_PC :
      case MultipleLabelStep::E_TDS :
        output=sprintfString( "Iteration %4d [%s], Running time %.4f/%.4f, "
                              "Eliminated vertex %zu/%zu/%zu, "
                              "Eliminated match %zu",
                              currentIteration, currentStepName.c_str(), currentStepTime, totalStepTime,
                              algoResults.vertexEliminated, patternmatchingState.graphActiveVertexCount, patternmatchingState.graphVertexCount,
                              algoResults.matchEliminated);
        break;
      case MultipleLabelStep::E_ENUMERATION :
        output=sprintfString( "Iteration %4d [%s], Running time %.4f/%.4f, "
                              "Enumeration %zu",
                              currentIteration, currentStepName.c_str(), currentStepTime, totalStepTime,
                              algoResults.enumeration);
        break;
      default:break;
    }

    Logger::get().log(Logger::E_LEVEL_RESULT, output, Logger::E_OUTPUT_DEBUG);
  }

  // Save graph
  if (logGraph) {
    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              *this,
                              &MultipleLabelCpu::printActiveGraph,
                              Logger::E_OUTPUT_FILE_GRAPH);
  }

  // Save graph
  if (logGraph) {
    Logger::get().logFunction(Logger::E_LEVEL_RESULT,
                              *this,
                              &MultipleLabelCpu::printActiveVertex,
                              Logger::E_OUTPUT_FILE_VERTEX);
  }

}

void MultipleLabelCpu::printActiveGraph(std::ostream &ostream) const {
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

void MultipleLabelCpu::printActiveVertex(std::ostream &ostream) const {
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