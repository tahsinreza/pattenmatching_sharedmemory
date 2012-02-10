/**
 * Implements the core execution engine of Totem
 *
 *  Created on: 2012-02-02
 *  Author: Abdullah Gharaibeh
 */

#include "totem_engine.cuh"

engine_context_t context = {false, NULL, NULL, 0, ENGINE_DEFAULT_CONFIG};

/**
 * Clears allocated state
 */
PRIVATE void engine_finalize() {
  assert(context.pset && context.par_labels);
  if (context.config.finalize_func) {
    context.config.finalize_func(&context.pset->partitions[0]);
    for (int pid = 1; pid < context.pset->partition_count; pid++) {
      CALL_CU_SAFE(cudaSetDevice(context.pset->partitions[pid].processor.id));
      context.config.finalize_func(&context.pset->partitions[pid]);
    }
  }
  CALL_SAFE(partition_set_finalize(context.pset));
  free(context.par_labels);
  free(context.finished);
  // reset the state
  memset(&context, 0, sizeof(engine_context_t));
  // g++ does not allow reinitializing a struct with a predefined value, it 
  // only allows copying from another instance
  engine_config_t config = ENGINE_DEFAULT_CONFIG;
  context.config = config;
}

/**
 * Returns true if all partitions reported a finished state
 */
inline PRIVATE bool superstep_check_finished() {
  bool finished = true;
  for (int pid = 0; pid < context.pset->partition_count; pid++) {
    finished &= context.finished[pid];
  }
  return finished;
}

/**
 * Blocks until all kernels initiated by the client have finished.
 */
inline PRIVATE void superstep_compute_synchronize() {
  for (int pid = 0; pid < context.pset->partition_count; pid++) {
    partition_t* par = &context.pset->partitions[pid];
    if (par->processor.type == PROCESSOR_CPU) continue;
    CALL_CU_SAFE(cudaStreamSynchronize(par->streams[1]));
  }
}

/**
 * Launches the compute kernel on each partition
 */
inline PRIVATE void superstep_compute() {
  // The assumption is that the first partition is the CPU one, and the
  // rest are GPU ones. This is guaranteed by engine_init.
  for (int pid = 1; pid < context.pset->partition_count; pid++) {
    // The kernel for GPU partitions is supposed not to block. The client is 
    // supposedly invoking the GPU kernel asynchronously, and using the compute 
    // "stream" available for each partition
    partition_t* partition = &context.pset->partitions[pid];
    CALL_CU_SAFE(cudaSetDevice(partition->processor.id));
    context.config.kernel_func(partition);
  }
  partition_t* partition = &context.pset->partitions[0];
  context.config.kernel_func(partition);
  superstep_compute_synchronize();
}

/**
 * Triggers grooves to synchronize state across partitions
 */
inline PRIVATE void superstep_communicate() {
  grooves_launch_communications(context.pset);
  grooves_synchronize(context.pset);
  if (!context.config.scatter_func) return;
  // The assumption is that the first partition is the CPU one, and the
  // rest are GPU ones. This is guaranteed by engine_init.
  for (int pid = 1; pid < context.pset->partition_count; pid++) {
    partition_t* partition = &context.pset->partitions[pid];
    CALL_CU_SAFE(cudaSetDevice(partition->processor.id));
    context.config.scatter_func(partition);
  }
  partition_t* partition = &context.pset->partitions[0];
  context.config.scatter_func(partition);  
}

/**
 * Prepares state for the next superstep
 */
inline PRIVATE void superstep_next() {
  context.superstep++;
  memset(context.finished, 0, context.pset->partition_count * sizeof(bool));
}

PRIVATE void engine_aggregate() {
  if (context.config.aggr_func) {
    context.config.aggr_func(&context.pset->partitions[0]);
    for (int pid = 1; pid < context.pset->partition_count; pid++) {
      CALL_CU_SAFE(cudaSetDevice(context.pset->partitions[pid].processor.id));
      context.config.aggr_func(&context.pset->partitions[pid]);
    }
  }
}

error_t engine_start() {
  while (true) {
    superstep_next();                      // prepare state for the next round
    superstep_compute();                   // compute phase
    if (superstep_check_finished()) break; // check for termination
    superstep_communicate();               // communication/synchronize phase
  }

  engine_aggregate();  
  engine_finalize();
  return SUCCESS;
}

error_t engine_init(engine_config_t* config) {
  if (context.initialized) return FAILURE;
  assert(!context.pset && !context.par_labels && !context.finished);
  context.config = *config;

  int pcount;
  CALL_CU_SAFE(cudaGetDeviceCount(&pcount));
  pcount += 1;
  processor_t* processors = (processor_t*)calloc(pcount, sizeof(processor_t));
  assert(processors);
  processors[0].type = PROCESSOR_CPU;
  for (int gpu_id = 0; gpu_id < pcount - 1; gpu_id++) {
    processors[gpu_id + 1].type = PROCESSOR_GPU;
    processors[gpu_id + 1].id = gpu_id;
  }

  // partition the graph
  switch (config->par_algo) {
    case PAR_RANDOM:
      CALL_SAFE(partition_random(config->graph, (uint32_t)pcount, 13, 
                                 &(context.par_labels)));
      break;
    default:
      // TODO(abdullah): Use Lauro's logging library.
      printf("ERROR: Undefined partition algorithm.\n"); fflush(stdout);
      assert(false);
  }
  CALL_SAFE(partition_set_initialize(config->graph, context.par_labels,
                                     processors, pcount, config->msg_size, 
                                     &context.pset));
  free(processors);

  // callback the per-partition initialization function
  if (context.config.init_func) {
    context.config.init_func(&context.pset->partitions[0]);
    for (int pid = 1; pid < context.pset->partition_count; pid++) {
      partition_t* par = &context.pset->partitions[pid];
      CALL_CU_SAFE(cudaSetDevice(par->processor.id));
      context.config.init_func(par);
    }
  }

  // get largest gpu graph
  uint64_t largest = 0;
  for (int pid = 1; pid < context.pset->partition_count; pid++) {
    uint64_t vcount = context.pset->partitions[pid].subgraph.vertex_count;
    largest = vcount > largest ? vcount : largest;
  }
  context.largest_gpu_par = largest;

  context.finished = (bool*)calloc(pcount, sizeof(bool));
  context.initialized = true;
  return SUCCESS;
}