//
// Created by qiu on 01/07/18.
//

#include "common_utils.h"
#include "totem_graph.h"
#include "edge.h"

error_t parseVertexFile(FILE *file_handler, graph_t *graph) {
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
    weight_t value = (weight_t) atoll(token);

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
