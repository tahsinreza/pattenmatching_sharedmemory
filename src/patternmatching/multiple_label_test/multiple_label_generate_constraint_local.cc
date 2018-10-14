//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_graph.h"
#include "totem_util.h"
#include "multiple_label_generate_constraint_local.h"
#include <iostream>

namespace patternmatching {

void MultipleLabelGenerateConstraintLocal::preprocessPatern(const graph_t &pattern) {
  patternLocalConstraint.resize(pattern.vertex_count);
  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];

    auto &currentLocalConstraint = patternLocalConstraint[vertexId];
    currentLocalConstraint.originVertex = vertexId;
    currentLocalConstraint.originLabel = currentLabel;

    // build local constraint map
    for (eid_t patternEdgeId = pattern.vertices[vertexId]; patternEdgeId < pattern.vertices[vertexId + 1];
         patternEdgeId++) {
      vid_t neighborVertexId = pattern.edges[patternEdgeId];
      weight_t neighborLabel = pattern.values[neighborVertexId];

      currentLocalConstraint.localConstraintNeighborVertex.insert(neighborVertexId);
      if (currentLocalConstraint.localConstraint.find(neighborLabel) != currentLocalConstraint.localConstraint.end()) {
        ++(currentLocalConstraint.localConstraint)[neighborLabel];
      } else {
        currentLocalConstraint.localConstraint[neighborLabel] = 1;
      }
    }
  }
}

void MultipleLabelGenerateConstraintLocal::print(std::ostream &ostream) const {
  for (const auto &it : patternLocalConstraint) {
    it.print(ostream);
  }
}

int MultipleLabelGenerateConstraintLocal::getConstraintNumber() const {
  return patternLocalConstraint.size();
}

const std::vector<MultipleLabelConstraintLocal> &MultipleLabelGenerateConstraintLocal::getConstraintVector() const {
  return patternLocalConstraint;
}

}
