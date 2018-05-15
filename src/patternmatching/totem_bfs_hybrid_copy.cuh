/**
 * This file contains an implementation of the breadth-first search (BFS) graph
 * search algorithm using the totem framework
 *
 *  Created on: 2012-01-30
 *  Author: Abdullah Gharaibeh
 */
 #ifndef TOTEM_BFS_HYBRID_COPY_CUH
 #define TOTEM_BFS_HYBRID_COPY_CUH

#include "totem_bitmap.cuh"
#include "totem_comdef.h"
#include "totem_comkernel.cuh"
#include "totem_graph.h"
#include "totem_mem.h"
#include "totem_partition.h"
#include "totem_engine.cuh"


/**
 * A type for the cost in traversal-based algorithms.
 */
typedef uint16_t cost_t;
const cost_t INF_COST = (cost_t)INFINITE;

/**
 * A type for BFS tree type.
 */
typedef vid_t bfs_tree_t;

/**
 * For traversal-based algorithms, this constant determines the threshold
 * (as percentage of the total number of vertices) below which the frontier
 * is considered sparse. This is used to tune the graph algorithm (for example)
 * to choose between iterating over all the vertices (when the frontier is
 * not sparse), or build a frontier and iterate over only the vertices in the
 * frontier when it is sparse.
 */
const double TRV_FRONTIER_SPARSE_THRESHOLD = .1;

/*
 * For traversal-based algorithms, this constant determines the threshold
 * (as a fraction of the total amount of device memory) that determines the
 * maximum space to be allocated for the frontier array. Since  the GPU has
 * limited memory, this threshold is used by GPU-based partitions to limit
 * the space allocated for the frontier array. Note that if the frontier
 * in a specific level was longer, then the algorithm should not build a
 * frontier array, and should iterate over all the vertices. This value has
 * been determined experimentally.
 */
const double TRV_MAX_FRONTIER_SIZE = .1;


typedef struct frontier_state_s {
  bitmap_t current;         // current frontier bitmap
  bitmap_t visited_last;    // a bitmap of the visited vertices before the
                            // start of the previous round. This is used to
                            // compute the frontier bitmap of the current
                            // round by diffing this bitmap with the visited
                            // bitmap (a bitmap of the visited untill after the
                            // end of the previous round
  vid_t len;                // frontier bitmaps length
  vid_t* list;              // maintains the list of vertices that belong to the
                            // current frontier being processed (GPU only)
  vid_t  list_len;          // maximum number of vertices that the frontier
                            // list can hold (GPU only)
  vid_t* count;             // used to calculate the current number of vertices
                            // in the frontier (GPU only)
  vid_t* boundaries;        // thread scheduling boundaries (GPU only)
} frontier_state_t;

#ifdef FEATURE_SM35
PRIVATE const int FRONTIER_BOUNDARY_COUNT = 6;
#endif /* FEATURE_SM35 */

/**
 * Initializes a frontier data structure internal state
 * @param[in] frontier reference to the frontier data structure
 */

void frontier_init_gpu(frontier_state_t* state, vid_t vertex_count);
void frontier_init_cpu(frontier_state_t* state, vid_t vertex_count);

/**
 * Frees space allocated for a frontier data structure
 * @param[in] frontier reference to the frontier data structure
 */
void frontier_finalize_gpu(frontier_state_t* state);
void frontier_finalize_cpu(frontier_state_t* state);

/**
 * Resets the state of the frontier
 * @param[in] frontier reference to the frontier data structure
 */
void frontier_reset_gpu(frontier_state_t* state);
void frontier_reset_cpu(frontier_state_t* state);

/**
 * Updates the frontier bitmap
 * @param[in] frontier reference to the frontier data structure
 * @param[in] visited a bitmap representing  all the vertices that has been
 * visited untill now
 */
vid_t frontier_update_bitmap_cpu(frontier_state_t* state,
                                 const bitmap_t visited);
vid_t frontier_update_bitmap_gpu(frontier_state_t* state,
                                 const bitmap_t visited,
                                 cudaStream_t stream);

/**
 * Updates the frontier list with the vertex ids of the vertices in the
 * frontier. It also defines the scheduling boundaries in the case
 * the vertices are sorted by degree
 * @param[in] frontier reference to the frontier data structure
 */
void frontier_update_list_gpu(frontier_state_t* state,
                              vid_t level, const cost_t* cost,
                              const cudaStream_t stream);
void frontier_update_list_gpu(frontier_state_t* state,
                              const cudaStream_t stream);

#ifdef FEATURE_SM35
/**
 * Updates the scheduling boundaries if the vertices are sorted by degree
 * @param[in] frontier reference to the frontier data structure
 * @param[in] graph a reference to the graph to be processed
 */
void frontier_update_boundaries_gpu(frontier_state_t* state,
                                    const graph_t* graph,
                                    const cudaStream_t stream);
#endif /* FEATURE_SM35 */

/**
 * Returns the number of vertices in the frontier
 * @param[in] frontier the frontier
 * @return number of vertices in the frontier
 */
inline vid_t frontier_count_cpu(frontier_state_t* state) {
  return bitmap_count_cpu(state->current, state->len);
}
inline vid_t frontier_count_gpu(frontier_state_t* state, cudaStream_t stream) {
  return bitmap_count_gpu(state->current, state->len, state->count, stream);
}

error_t bfs_hybrid(vid_t src_id, cost_t* cost);

#endif