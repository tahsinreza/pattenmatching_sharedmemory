
#ifndef TOTEM_MULTIPLE_LABEL_COMMON_CPU_TPP
#define TOTEM_MULTIPLE_LABEL_COMMON_CPU_TPP

#include "totem_comdef.h"
#include "totem_graph.h"
#include "totem_mem.h"
#include "multiple_label_common_cpu.h"
#include "patternmatching_logger.h"

namespace patternmatching {


//#define DEBUG_PRINT(X) std::cout << #X << " : " << X <<std::endl;
//#define DEBUG_PRINT(X)

/**
 * state shared between all partitions
 */
template<typename VisitedType>
error_t MultipleLabelGlobalState<VisitedType>::allocate(const vid_t _graphVertexCount,
                                                        const eid_t _graphEdgeCount,
                                                        const vid_t _patternVertexCount) {
  graphVertexCount = _graphVertexCount;
  activeVertexCount = _graphVertexCount;
  graphEdgeCount = _graphEdgeCount;
  activeEdgeCount = _graphEdgeCount;
  patternVertexCount = _patternVertexCount;

  // Temporary list of all possible pattern
  std::vector <pvid_t> tmpPatternMatch;
  for (pvid_t patternVertexId = 0; patternVertexId < patternVertexCount; patternVertexId++) {
    tmpPatternMatch.push_back(patternVertexId);
  }

  // Vertex active list
  totem_malloc(graphVertexCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexActiveList));
  assert(vertexActiveList != nullptr);
  totem_memset(vertexActiveList, static_cast<VisitedType>(true), graphVertexCount, TOTEM_MEM_HOST);

  // Edge active list
  totem_malloc(graphEdgeCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&edgeActiveList));
  assert(edgeActiveList != nullptr);
  totem_memset(edgeActiveList, static_cast<VisitedType>(true), graphEdgeCount, TOTEM_MEM_HOST);

  // Pattern match list
  vertexPatternMatch = new std::unordered_set<pvid_t>[graphVertexCount];

  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternMatch[vertexId].insert(tmpPatternMatch.cbegin(), tmpPatternMatch.cend());
  }


  // LCC

  vertexPatternToUnmatchLcc = new std::unordered_set<pvid_t>[graphVertexCount];
  resetPatternMatchLcc();

  // CC
  vertexPatternToUnmatchCc = new std::unordered_set<pvid_t>[graphVertexCount];
  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternOmittedCc));
  assert(vertexPatternOmittedCc != nullptr);
  resetPatternMatchCc();

  // PC
  vertexPatternToUnmatchPc = new std::unordered_set<pvid_t>[graphVertexCount];
  resetPatternMatchPc();


  // TDS
  vertexPatternToUnmatchTds = new std::unordered_set<pvid_t>[graphVertexCount];
  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternOmittedTds));
  assert(vertexPatternOmittedTds != nullptr);
  resetPatternMatchTds();

  return SUCCESS;
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchLcc() {
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchLcc[vertexId].clear();
  }
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchCc() {
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchCc[vertexId].clear();
  }
  totem_memset(vertexPatternOmittedCc, static_cast<uint8_t>(false), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchPc() {
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchPc[vertexId].clear();
  }
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchTds() {
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchTds[vertexId].clear();
  }
  totem_memset(vertexPatternOmittedTds, static_cast<uint8_t>(false), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
error_t MultipleLabelGlobalState<VisitedType>::free() {
  totem_free(reinterpret_cast<void *>(vertexActiveList), TOTEM_MEM_HOST);
  totem_free(reinterpret_cast<void *>(edgeActiveList), TOTEM_MEM_HOST);
  totem_free(reinterpret_cast<void *>(vertexPatternOmittedCc), TOTEM_MEM_HOST);
  delete[] vertexPatternToUnmatchLcc;
  delete[] vertexPatternToUnmatchCc;

  graphVertexCount = 0;
  activeVertexCount = 0;

  return SUCCESS;
}

template<class State>
bool MultipleLabelCpuBase<State>::isVertexActive(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexActiveList[vertexId] == true;
}
template<class State>
bool MultipleLabelCpuBase<State>::isVertexInactive(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexActiveList[vertexId] == false;
}
template<class State>
void MultipleLabelCpuBase<State>::deactivateVertex(State *globalState, const vid_t vertexId) const {
  globalState->vertexActiveList[vertexId] = false;
}
template<class State>
bool MultipleLabelCpuBase<State>::isEdgeActive(const State &globalState, const eid_t edgeId) const {
  return globalState.edgeActiveList[edgeId] == true;
}
template<class State>
bool MultipleLabelCpuBase<State>::isEdgeInactive(const State &globalState, const eid_t edgeId) const {
  return globalState.edgeActiveList[edgeId] == false;
}
template<class State>
void MultipleLabelCpuBase<State>::deactivateEdge(State *globalState, const eid_t edgeId) const {
  globalState->edgeActiveList[edgeId] = false;
}

template<class State>
bool MultipleLabelCpuBase<State>::isMatch(const State &globalState, const vid_t vertexId) const {
  return !globalState.vertexPatternMatch[vertexId].empty();
}
template<class State>
void MultipleLabelCpuBase<State>::removeMatch(State *globalState,
                                              const vid_t vertexId,
                                              const pvid_t patternVertexId) const {
  globalState->vertexPatternMatch[vertexId].erase(patternVertexId);
}

}

#endif
