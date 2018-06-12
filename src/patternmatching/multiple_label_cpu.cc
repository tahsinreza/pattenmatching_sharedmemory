
#include <omp.h>
#include <iostream>
#include <iomanip>

#include "totem.h"
#include "multiple_label_cpu.h"
#include "patternmatching_logger.h"

namespace patternmatching {

error_t parse_vertex_list2(FILE *file_handler, graph_t *graph) {
  const uint32_t MAX_LINE_LENGTH = 100;
  const char delimiters[] = " \t\n:";
  uint64_t line_number = 0;
  char line[MAX_LINE_LENGTH];

  graph->valued = true;
  graph->values = reinterpret_cast<weight_t *>(malloc(graph->vertex_count * sizeof(weight_t)));

  vid_t vertex_index = 0;
  while (vertex_index < graph->vertex_count) {
    if (fgets(line, sizeof(line), file_handler) == NULL) break;
    line_number++;
    if (line[0] == '#') { continue; }

    // Start tokenizing: first, the vertex id.
    char *token;
    char *saveptr;
    CHK((token = strtok_r(line, delimiters, &saveptr)) != NULL, err);
    CHK(is_numeric(token), err);
    uint64_t token_num = atoll(token);
    CHK((token_num < VERTEX_ID_MAX), err_id_overflow);
    vid_t vertex_id = token_num;

    // Second, get the value.
    CHK((token = strtok_r(NULL, delimiters, &saveptr)) != NULL, err);
    // TODO(abdullah): Use isnumeric to verify the value.
    weight_t value = (weight_t) atof(token);

    if (vertex_id != vertex_index) {
      // Vertices must be in increasing order and less than the maximum count.
      CHK(((vertex_id > vertex_index) &&
          (vertex_id < graph->vertex_count)), err);

      // Vertices without values will be assigned a default one.
      while (vertex_index < vertex_id) {
        graph->values[vertex_index++] = DEFAULT_VERTEX_VALUE;
      }
    }
    graph->values[vertex_index++] = value;
  }

  return SUCCESS;

  err_id_overflow:
  fprintf(stderr, "The type used for vertex ids does not support the range of"
                  " values in this file.\n");
  err:
  fprintf(stderr, "parse_vertex_list\n");
  return FAILURE;
}

void MultipleLabelCpu::initialiseTotem() {
  #if 0
  attributeCpu = TOTEM_DEFAULT_ATTR;
  attributeCpu.platform = PLATFORM_CPU;
  attributeCpu.par_algo = PAR_RANDOM;
  attributeCpu.cpu_par_share = 1.0;
  attributeCpu.lambda = 0;
  attributeCpu.gpu_count = 0;
  attributeCpu.gpu_graph_mem = GPU_GRAPH_MEM_DEVICE;
  attributeCpu.gpu_par_randomized = false;
  attributeCpu.sorted = false;
  attributeCpu.edge_sort_by_degree = false;
  attributeCpu.edge_sort_dsc = false;
  attributeCpu.separate_singletons = false;
  attributeCpu.compressed_vertices_supported = false;
  attributeCpu.push_msg_size = MSG_SIZE_WORD;
  attributeCpu.pull_msg_size = MSG_SIZE_ZERO;
  attributeCpu.alloc_func = NULL;
  attributeCpu.free_func = NULL;
  CALL_SAFE(totem_init(graph, &attributeCpu));
    #endif

  // Configure OpenMP.
  omp_set_num_threads(omp_get_max_threads());
  omp_set_schedule(omp_sched_guided, 0);

}

error_t MultipleLabelCpu::allocate(CmdLineOption &cmdLineOption) {
  // Load graph
  Logger::get().log(Logger::E_LEVEL_INFO, "Loading graph");
  graph_initialize(cmdLineOption.getInputGraphFilePath().c_str(), 0, &graph);

  if (!cmdLineOption.getInputVertexMetadataFilePath().empty()) {
    FILE *file_handler = fopen(cmdLineOption.getInputVertexMetadataFilePath().c_str(), "r");
    if (file_handler != NULL) parse_vertex_list2(file_handler, graph);
  }
  //graph_store_binary(graph, (cmdLineOption.getInputGraphFilePath()+".bin").c_str());

  // Load pattern
  Logger::get().log(Logger::E_LEVEL_INFO, "Loading pattern");
  graph_initialize((cmdLineOption.getInputPatternDirectory() + "pattern_edge").c_str(), 0, &pattern);

  if (!cmdLineOption.getInputVertexMetadataFilePath().empty()) {
    FILE *file_handler = fopen((cmdLineOption.getInputPatternDirectory() + "pattern_vertex_data").c_str(), "r");
    if (file_handler != NULL) parse_vertex_list2(file_handler, pattern);
  }

  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize Totem and global state");
  initialiseTotem();

  patternmatchingState.allocate(graph->vertex_count, graph->edge_count, pattern->vertex_count);

  // Initialise algorithms
  Logger::get().log(Logger::E_LEVEL_INFO, "Initialize LCC, CC, PC, and Step algorithms");

  lccCpu.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO, lccCpu, &LccType::printLocalConstraint, Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG, lccCpu, &LccType::printLocalConstraint, Logger::E_OUTPUT_COUT);

  ccCpu.init(*graph, *pattern);
  ccCpu.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO, ccCpu, &CcType::printCircularConstraint, Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG, ccCpu, &CcType::printCircularConstraint, Logger::E_OUTPUT_COUT);

  pcCpu.init(*graph, *pattern);
  pcCpu.preprocessPatern(*pattern);
  Logger::get().logFunction(Logger::E_LEVEL_INFO, pcCpu, &PcType::printPathConstraint, Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG, pcCpu, &PcType::printPathConstraint, Logger::E_OUTPUT_COUT);

  tdsCpu.init(*graph, *pattern);
  tdsCpu.preprocessPatern(*pattern, ccCpu.getCircularConstraintVector(), pcCpu.getPathConstraintVector());
  Logger::get().logFunction(Logger::E_LEVEL_INFO, tdsCpu, &TdsType::printTemplateConstraint, Logger::E_OUTPUT_FILE_LOG);
  Logger::get().logFunction(Logger::E_LEVEL_DEBUG, tdsCpu, &TdsType::printTemplateConstraint, Logger::E_OUTPUT_COUT);

  algorithmStep.initStepCc(ccCpu.getCircularConstraintNumber());
  algorithmStep.initStepPc(pcCpu.getPathConstraintNumber());
  algorithmStep.initStepTds(tdsCpu.getTemplateConstraintNumber());

  Logger::get().log(Logger::E_LEVEL_INFO, "End of initialisation");
  return SUCCESS;
}

error_t MultipleLabelCpu::free() {
  #if 0
  totem_finalize();
    #endif
  patternmatchingState.free();
  graph_finalize(graph);

  return SUCCESS;
}

int MultipleLabelCpu::runPatternMatching() {
  // run step
  bool finished = 0;
  MultipleLabelStep::Step currentStep;
  totalStepTime = 0.;

  Logger::get().log(Logger::E_LEVEL_INFO, "Start run");

  int currentIteration = 0;
  algorithmStep.getNextStep(currentStepVertexEliminated, &currentStep, &currentStepName);
  while (!finished && currentIteration < 200) {
  // reinitialise
    switch (currentStep) {
      case MultipleLabelStep::E_CC :
        ccCpu.resetState(&patternmatchingState);
        break;

      case MultipleLabelStep::E_PC :
        pcCpu.resetState(&patternmatchingState);
        break;
      default:break;
    }

    totem_timing_reset();
    stopwatch_t stopwatch;
    stopwatch_start(&stopwatch);

    switch (currentStep) {
      case MultipleLabelStep::E_LCC :
        currentStepVertexEliminated = lccCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_CC :
        currentStepVertexEliminated = ccCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_PC :
        currentStepVertexEliminated = pcCpu.compute(*graph, &patternmatchingState);
        break;
      case MultipleLabelStep::E_TDS :
        currentStepVertexEliminated = tdsCpu.compute(*graph, &patternmatchingState);
        break;
      default:break;
    }

    currentStepTime = stopwatch_elapsed(&stopwatch);
    totalStepTime += currentStepTime;

    logResults(currentIteration, true);

    finished = algorithmStep.getNextStep(currentStepVertexEliminated, &currentStep, &currentStepName);
    currentIteration++;
  }

  //printActiveGraph(stringStream);
  Logger::get().log(Logger::E_LEVEL_INFO, "End run");
  Logger::get().log(Logger::E_LEVEL_INFO, "Saving pruned graph");
  currentStepTime=0.;
  currentStepVertexEliminated=0;
  logResults(currentIteration, true);

  return 0;
}

void MultipleLabelCpu::logResults(const int currentIteration, const bool logGraph) const {
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