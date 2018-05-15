/**
 * Main entry of the benchmark
 *
 *  Created on: 2011-02-28
 *  Author: Abdullah Gharaibeh
 */

// totem includes
#include "totem_mem.h"
#include "totem_patternmatching.h"

// Defines attributes of the algorithms available for benchmarking
PRIVATE void benchmark_bfs(graph_t*, void*, totem_attr_t*);
const benchmark_attr_t BENCHMARKS = 
    {benchmark_bfs, "BFS", sizeof(cost_t), true, false, 1, MSG_SIZE_ZERO, NULL, NULL};
    
    /*
     * 
    bool compressed_vertices_supported;  // Indicates whether the algorithm
    // supports compressed vertices or not.
    size_t push_msg_size;        // Push message size (Totem-based alg.).
    size_t pull_msg_size;        // Pull message size (Totem-based alg.).
    totem_cb_func_t alloc_func;  // Allocation callback function
    // (Totem-based alg.).
    totem_cb_func_t free_func;  // Free callback function (Totem-based alg.).*/

// A reference to the options used to configure the benchmark
PRIVATE benchmark_options_t* options = NULL;
PRIVATE const int SEED = 1985;

// Returns the number of traversed edges used in computing the processing rate
PRIVATE uint64_t get_traversed_edges(graph_t* graph, void* benchmark_output)
{
    uint64_t trv_edges = 0;
#pragma omp parallel for reduction(+ : trv_edges)
    for(vid_t vid = 0; vid < graph->vertex_count; vid++) {
        cost_t* cost = reinterpret_cast<cost_t*>(benchmark_output);
        if(cost[vid] != INF_COST) {
            trv_edges += (graph->vertices[vid + 1] - graph->vertices[vid]);
        }
    }
    return trv_edges;
}

// Randomly picks a random source vertex, and ensures that it is connected to at
// least one other vertex.
PRIVATE vid_t get_random_src(graph_t* graph)
{
    vid_t src = rand() % graph->vertex_count;
    while((graph->vertices[src + 1] - graph->vertices[src] == 0)) {
        src = rand() % graph->vertex_count;
    }
    return src;
}

// Runs the top-down BFS benchmark.
PRIVATE void benchmark_bfs(graph_t* graph, void* cost, totem_attr_t* attr)
{
    vid_t src = get_random_src(graph);
    CALL_SAFE(bfs_hybrid(src, reinterpret_cast<cost_t*>(cost)));
}

// The main execution loop of the benchmark.
PRIVATE void benchmark_run()
{
    assert(options);

    graph_t* graph = NULL;
    CALL_SAFE(graph_initialize(options->graph_file, 0, &graph));
    print_config(graph, options, "BFS");

    void* benchmark_state = NULL;
    totem_malloc(graph->vertex_count * sizeof(cost_t), TOTEM_MEM_HOST,
                 reinterpret_cast<void**>(&benchmark_state));
    assert(benchmark_state || (sizeof(cost_t) == 0));

    totem_attr_t attr = TOTEM_DEFAULT_ATTR;
    attr.par_algo = options->par_algo;
    attr.cpu_par_share = static_cast<float>(options->alpha) / 100.0;
    attr.lambda = static_cast<float>(options->lambda) / 100.0;
    attr.platform = options->platform;
    attr.gpu_count = options->gpu_count;
    attr.gpu_graph_mem = options->gpu_graph_mem;
    attr.gpu_par_randomized = options->gpu_par_randomized;
    attr.sorted = options->sorted;
    attr.edge_sort_by_degree = options->edge_sort_by_degree;
    attr.edge_sort_dsc = options->edge_sort_dsc;
    attr.separate_singletons = options->separate_singletons;
    attr.compressed_vertices_supported = false;
    attr.push_msg_size = 1;
    attr.pull_msg_size = MSG_SIZE_ZERO;
    attr.alloc_func = NULL;
    attr.free_func = NULL;
    CALL_SAFE(totem_init(graph, &attr));

    // Configure OpenMP.
    omp_set_num_threads(options->thread_count);
    omp_set_schedule(options->omp_sched, 0);
    print_header(graph, true);

    srand(SEED);
    for(int s = 0; s < options->repeat; s++) {
        totem_timing_reset();
        stopwatch_t stopwatch;
        stopwatch_start(&stopwatch);
        benchmark_bfs(graph, benchmark_state, &attr);
        double total = stopwatch_elapsed(&stopwatch);
        print_timing(graph, total, get_traversed_edges(graph, benchmark_state), true);
    }

    totem_finalize();
    totem_free(benchmark_state, TOTEM_MEM_HOST);
    CALL_SAFE(graph_finalize(graph));
}

int main(int argc, char** argv)
{
    CALL_SAFE(check_cuda_version());
    options = benchmark_cmdline_parse(argc, argv);
    benchmark_run();
    return 0;
}
