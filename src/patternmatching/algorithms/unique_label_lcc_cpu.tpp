//
// Created by qiu on 17/05/18.
//
#include <cuda.h>
#include "totem_engine.cuh"
#include "totem_util.h"
#include "unique_label_lcc_cpu.cuh"
#include <iostream>

namespace patternmatching {


template<class State>
__host__ error_t UniqueLabelLccCpu<State>::preprocessPatern(const graph_t &pattern) {
  for (vid_t vertexId = 0; vertexId < pattern.vertex_count; vertexId++) {
    weight_t currentLabel = pattern.values[vertexId];
    auto currentLocalConstraint = &(patternLocalConstraint[currentLabel]);

    // build local constraint map
    for(eid_t patternEdgeId = pattern.vertices[vertexId]; patternEdgeId < pattern.vertices[vertexId+1]; patternEdgeId++) {
      vid_t neighborVertexId = pattern.edges[patternEdgeId];
      weight_t neighborLabel = pattern.values[neighborVertexId];

      if(currentLocalConstraint->find(neighborLabel)!=currentLocalConstraint->end()) {
        ++(*currentLocalConstraint)[neighborLabel];
      } else {
        (*currentLocalConstraint)[neighborLabel]=1;
      }
    }
  }

  return SUCCESS;
}

template<class State>
__host__ void UniqueLabelLccCpu<State>::printLocalConstraint(std::ostream &ostream) const {
  for (const auto& it : patternLocalConstraint) {
    ostream << "Label " << it.first << " : "<< std::endl;
    for (const auto& subit : it.second) {
      ostream << "\t Sublabel " << subit.first << " : "<< subit.second<< std::endl;

    }
  }
}


template<class State>
__host__ size_t UniqueLabelLccCpu<State>::compute(const graph_t &graph, State *globalState) const {


  #pragma omp parallel for
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isActive(*globalState, vertexId)) continue;

    weight_t currentLabel = graph.values[vertexId];

    // test if currentLabel is in the pattern
    if(patternLocalConstraint.find(currentLabel)==patternLocalConstraint.end()) {
      removeMatch(globalState, vertexId);
      continue;
    }

    // build local constraint
    auto currentLocalConstraint = patternLocalConstraint.at(currentLabel);

    for(eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId<graph.vertices[vertexId+1]; neighborEdgeId++) {
      vid_t neighborVertexId = graph.edges[neighborEdgeId];
      if(!BaseClass::isActive(*globalState, neighborVertexId)) continue;

      weight_t neighborLabel = graph.values[neighborVertexId];
      if(currentLocalConstraint.find(neighborLabel)!=currentLocalConstraint.end()) {
        if(currentLocalConstraint[neighborLabel]>0) --currentLocalConstraint[neighborLabel];
      }
    }

    // test local constraint
    for (const auto &it : currentLocalConstraint) {
      if (it.second > 0) {
        removeMatch(globalState, vertexId);
        break;
      }
    }

  }

  size_t vertexEliminatedNumber=0;

  #pragma omp parallel for reduction(+:vertexEliminatedNumber)
  for (vid_t vertexId = 0; vertexId < graph.vertex_count; vertexId++) {
    if (!BaseClass::isActive(*globalState, vertexId)) continue;

    if(!isMatch(*globalState, vertexId)) {
      BaseClass::deactivate(globalState, vertexId);
      vertexEliminatedNumber+=1;
    }
  }

  return vertexEliminatedNumber;
}


template<class State>
bool UniqueLabelLccCpu<State>::isMatch(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexPatternMatchLcc[vertexId] == true;
}

template<class State>
void UniqueLabelLccCpu<State>::removeMatch(State *globalState, const vid_t vertexId) const {
  globalState->vertexPatternMatchLcc[vertexId] = false;
}

}

// get location of current label in pattern
/*vid_t patternIndex=0;
bool patternFound=false;

for (vid_t patternVertexId = 0; patternVertexId < pattern->vertex_count; patternVertexId++) {
  if (pattern->values[patternVertexId] == currentLabel) {
    patternIndex = patternVertexId;
    patternFound=true;
    break;
  }
}

if(!patternFound) {
  globalState->m_vertexActiveList[vertexId]=0;
  continue;
}
 */
