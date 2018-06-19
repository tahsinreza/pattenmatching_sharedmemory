
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
  std::vector<pvid_t> tmpPatternMatch;
  for (pvid_t patternVertexId = 0; patternVertexId < patternVertexCount; patternVertexId++) {
    tmpPatternMatch.push_back(patternVertexId);
  }

  // Vertex active list
  totem_malloc(graphVertexCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexActiveList));
  assert(vertexActiveList != nullptr);
  totem_memset(vertexActiveList, static_cast<VisitedType>(true), graphVertexCount, TOTEM_MEM_HOST);

  // Vertex modified list
  totem_malloc(graphVertexCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexModifiedList));
  assert(vertexModifiedList != nullptr);
  resetModifiedList();

  // Vertex scheduled list
  totem_malloc(graphVertexCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexScheduledList));
  assert(vertexScheduledList != nullptr);
  resetScheduledList(true);

  // Edge active list
  totem_malloc(graphEdgeCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&edgeActiveList));
  assert(edgeActiveList != nullptr);
  totem_memset(edgeActiveList, static_cast<VisitedType>(true), graphEdgeCount, TOTEM_MEM_HOST);

  // Pattern match list
  vertexPatternMatch = new BitmapType[graphVertexCount];

  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternMatch[vertexId].insert(tmpPatternMatch.cbegin(), tmpPatternMatch.cend());
  }


  // LCC
  vertexPatternToUnmatchLcc = new BitmapType[graphVertexCount];
  resetPatternMatchLcc();

  // CC
  vertexPatternToUnmatchCc = new BitmapType[graphVertexCount];
  vertexPatternMatchedCc = new BitmapType[graphVertexCount];
  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternOmittedCc));
  assert(vertexPatternOmittedCc != nullptr);
  resetPatternMatchCc();

  // PC
  vertexPatternToUnmatchPc = new BitmapType[graphVertexCount];
  resetPatternMatchPc();


  // TDS
  vertexPatternToUnmatchTds = new BitmapType[graphVertexCount];
  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternOmittedTds));
  assert(vertexPatternOmittedTds != nullptr);
  resetPatternMatchTds();

  return SUCCESS;
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetModifiedList() {
  totem_memset(vertexModifiedList, static_cast<VisitedType>(false), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetScheduledList(const bool &value) {
  totem_memset(vertexScheduledList, static_cast<VisitedType>(value), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchLcc() {
  #pragma omp parallel for schedule(static)
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchLcc[vertexId].clear();
  }
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchCc() {
  #pragma omp parallel for schedule(static)
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchCc[vertexId].clear();
    vertexPatternMatchedCc[vertexId].clear();
  }
  totem_memset(vertexPatternOmittedCc, static_cast<uint8_t>(false), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchPc() {
  #pragma omp parallel for schedule(static)
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatchPc[vertexId].clear();
  }
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternMatchTds() {
  #pragma omp parallel for schedule(static)
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
void MultipleLabelCpuBase<State>::makeModifiedVertex(State *globalState, const vid_t vertexId) const {
  globalState->vertexModifiedList[vertexId] = true;
}
template<class State>
bool MultipleLabelCpuBase<State>::isVertexModified(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexModifiedList[vertexId] == true;
}

template<class State>
bool MultipleLabelCpuBase<State>::isVertexScheduled(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexScheduledList[vertexId] == true;
}
template<class State>
void MultipleLabelCpuBase<State>::scheduleVertex(State *globalState, const vid_t vertexId) const {
  globalState->vertexScheduledList[vertexId] = true;
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


/*
template<class State>
bool MultipleLabelCpuBase<State>::isVertexScheduledAtomic(const State &globalState, const vid_t vertexId) const {
  auto address = &(globalState.vertexScheduleList[vertexId]);
  bool value;
  #pragma omp atomic read
  value = *address;
  return value;
}*/
/*
template<class State>
void MultipleLabelCpuBase<State>::scheduleVertexAtomic(State *globalState, const vid_t vertexId) const {
  auto address = &(globalState->vertexScheduleList[vertexId]);
  #pragma omp atomic write
  *address = true;
}
template<class State>
void MultipleLabelCpuBase<State>::unscheduleVertex(State *globalState, const vid_t vertexId) const {
  globalState->vertexScheduleList[vertexId] = false;
}

template<class State>
void MultipleLabelCpuBase<State>::scheduleNeighborVertex(const graph_t &graph,
                                                         State *globalState,
                                                         const vid_t vertexId) const {
  for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
       neighborEdgeId++) {
    if (!isEdgeActive(*globalState, neighborEdgeId)) continue;
    vid_t neighborVertexId = graph.edges[neighborEdgeId];
    if (!isVertexActive(*globalState, neighborVertexId)) continue;
    scheduleVertex(globalState, neighborVertexId);
  }
}
template<class State>
void MultipleLabelCpuBase<State>::scheduleNeighborVertexAtomic(const graph_t &graph,
                                                               State *globalState,
                                                               const vid_t vertexId) const {
  for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
       neighborEdgeId++) {
    if (!isEdgeActive(*globalState, neighborEdgeId)) continue;
    vid_t neighborVertexId = graph.edges[neighborEdgeId];
    if (!isVertexActive(*globalState, neighborVertexId)) continue;
    scheduleVertexAtomic(globalState, neighborVertexId);
  }
}*/

#endif
