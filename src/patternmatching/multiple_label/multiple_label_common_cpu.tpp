
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
  graphActiveVertexCount = _graphVertexCount;
  graphEdgeCount = _graphEdgeCount;
  graphActiveEdgeCount = _graphEdgeCount;
  patternVertexCount = _patternVertexCount;

  // Vertex and Edge active list
  vertexActiveList = new VisitedType[graphVertexCount];
  edgeActiveList = new VisitedType[graphEdgeCount];
  resetActiveList();

  // Vertex modified list
  vertexModifiedList = new VisitedType[graphVertexCount];
  resetModifiedList();

  // Vertex scheduled list
  vertexScheduledList = new VisitedType[graphVertexCount];
  resetScheduledList(true);


  // Pattern match list
  vertexPatternMatch = new BitmapType[graphVertexCount];
  resetPatternMatch();

  // Pattern matched list
  vertexPatternAlreadyMatched = new BitmapType[graphVertexCount];
  resetPatternAlreadyMatched();

  // To unmatch
  vertexPatternToUnmatch = new BitmapType[graphVertexCount];
  resetPatternToUnmatch();

  return SUCCESS;
}

template<typename VisitedType>
error_t MultipleLabelGlobalState<VisitedType>::free() {
  delete[] vertexActiveList;
  delete[] edgeActiveList;
  delete[] vertexModifiedList;
  delete[] vertexScheduledList;
  delete[] vertexPatternMatch;
  delete[] vertexPatternAlreadyMatched;
  delete[] vertexPatternToUnmatch;

  graphVertexCount = 0;
  graphEdgeCount = 0;
  patternVertexCount = 0;

  return SUCCESS;
}

template<typename VisitedType>
MultipleLabelGlobalState <VisitedType> &MultipleLabelGlobalState<VisitedType>::operator=(const MultipleLabelGlobalState <VisitedType> &other) {
  if (graphVertexCount != other.graphVertexCount
      || graphEdgeCount != other.graphEdgeCount
      || patternVertexCount != other.patternVertexCount) {
    free();
    allocate(other.graphVertexCount, other.graphEdgeCount, other.patternVertexCount);
  }

  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexActiveList[vertexId] = other.vertexActiveList[vertexId];
    vertexModifiedList[vertexId] = other.vertexModifiedList[vertexId];
    vertexScheduledList[vertexId] = other.vertexScheduledList[vertexId];
    vertexPatternMatch[vertexId] = other.vertexPatternMatch[vertexId];
    vertexPatternToUnmatch[vertexId] = other.vertexPatternToUnmatch[vertexId];
  }
  for (eid_t edgeId = 0; edgeId < graphEdgeCount; edgeId++) {
    edgeActiveList[edgeId] = other.edgeActiveList[edgeId];
  }

  return *this;
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetActiveList() {
  totem_memset(vertexActiveList, static_cast<VisitedType>(true), graphVertexCount, TOTEM_MEM_HOST);
  totem_memset(edgeActiveList, static_cast<VisitedType>(true), graphEdgeCount, TOTEM_MEM_HOST);
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
void MultipleLabelGlobalState<VisitedType>::resetPatternMatch() {

  // Temporary list of all possible pattern
  std::vector<pvid_t> tmpPatternMatch;
  for (pvid_t patternVertexId = 0; patternVertexId < patternVertexCount; patternVertexId++) {
    tmpPatternMatch.push_back(patternVertexId);
  }

  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternMatch[vertexId].insert(tmpPatternMatch.cbegin(), tmpPatternMatch.cend());
  }
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternAlreadyMatched() {
  #pragma omp parallel for schedule(static)
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternAlreadyMatched[vertexId].clear();
  }
}

template<typename VisitedType>
void MultipleLabelGlobalState<VisitedType>::resetPatternToUnmatch() {
  #pragma omp parallel for schedule(static)
  for (vid_t vertexId = 0; vertexId < graphVertexCount; vertexId++) {
    vertexPatternToUnmatch[vertexId].clear();
  }
}

template<class State>
bool MultipleLabelCpuBase<State>::isVertexActive(const State &globalState, const vid_t &vertexId) const {
  return globalState.vertexActiveList[vertexId] == true;
}
template<class State>
bool MultipleLabelCpuBase<State>::isVertexInactive(const State &globalState, const vid_t &vertexId) const {
  return globalState.vertexActiveList[vertexId] == false;
}
template<class State>
void MultipleLabelCpuBase<State>::deactivateVertex(State *globalState, const vid_t &vertexId) const {
  globalState->vertexActiveList[vertexId] = false;
}

template<class State>
void MultipleLabelCpuBase<State>::makeModifiedVertex(State *globalState, const vid_t &vertexId) const {
  globalState->vertexModifiedList[vertexId] = true;
}
template<class State>
bool MultipleLabelCpuBase<State>::isVertexModified(const State &globalState, const vid_t &vertexId) const {
  return globalState.vertexModifiedList[vertexId] == true;
}

template<class State>
bool MultipleLabelCpuBase<State>::isVertexScheduled(const State &globalState, const vid_t &vertexId) const {
  return globalState.vertexScheduledList[vertexId] == true;
}
template<class State>
void MultipleLabelCpuBase<State>::scheduleVertex(State *globalState, const vid_t &vertexId) const {
  globalState->vertexScheduledList[vertexId] = true;
}
template<class State>
void MultipleLabelCpuBase<State>::unscheduleVertex(State *globalState, const vid_t &vertexId) const {
  globalState->vertexScheduledList[vertexId] = false;
}

template<class State>
bool MultipleLabelCpuBase<State>::isEdgeActive(const State &globalState, const eid_t &edgeId) const {
  return globalState.edgeActiveList[edgeId] == true;
}
template<class State>
bool MultipleLabelCpuBase<State>::isEdgeInactive(const State &globalState, const eid_t &edgeId) const {
  return globalState.edgeActiveList[edgeId] == false;
}
template<class State>
void MultipleLabelCpuBase<State>::deactivateEdge(State *globalState, const eid_t &edgeId) const {
  globalState->edgeActiveList[edgeId] = false;
}

template<class State>
bool MultipleLabelCpuBase<State>::isMatch(const State &globalState, const vid_t &vertexId) const {
  return !globalState.vertexPatternMatch[vertexId].empty();
}
template<class State>
void MultipleLabelCpuBase<State>::removeMatch(State *globalState,
                                              const vid_t &vertexId,
                                              const pvid_t &patternVertexId) const {
  globalState->vertexPatternMatch[vertexId].erase(patternVertexId);
}

template<class State>
bool MultipleLabelCpuBase<State>::isAlreadyMatchedAtomic(const State &globalState, const vid_t &vertexId,
                                                         const pvid_t &patternVertexId) const {
  return globalState.vertexPatternAlreadyMatched[vertexId].isInAtomic(patternVertexId);
}
template<class State>
void MultipleLabelCpuBase<State>::makeAlreadyMatchedAtomic(State *globalState,
                                                           const vid_t &vertexId,
                                                           const pvid_t &patternVertexId) const {
  globalState->vertexPatternAlreadyMatched[vertexId].insertAtomic(patternVertexId);
}
template<class State>
void MultipleLabelCpuBase<State>::removeAlreadyMatched(State *globalState,
                                                       const vid_t &vertexId,
                                                       const pvid_t &patternVertexId) const {
  globalState->vertexPatternAlreadyMatched[vertexId].erase(patternVertexId);
}
template<class State>
void MultipleLabelCpuBase<State>::clearAlreadyMatched(State *globalState,
                                                      const vid_t &vertexId) const {
  globalState->vertexPatternAlreadyMatched[vertexId].clear();
}

template<class State>
void MultipleLabelCpuBase<State>::clearToUnmatch(State *globalState,
                                                 const vid_t &vertexId) const {
  globalState->vertexPatternToUnmatch[vertexId].clear();
}

template<class State>
void MultipleLabelCpuBase<State>::makeToUnmatch(State *globalState,
                                                const vid_t &vertexId,
                                                const pvid_t &patternVertexId) const {
  globalState->vertexPatternToUnmatch[vertexId].insert(patternVertexId);
}

template<class State>
void MultipleLabelCpuBase<State>::removeToUnmatch(State *globalState,
                                                  const vid_t &vertexId,
                                                  const pvid_t &patternVertexId) const {
  globalState->vertexPatternToUnmatch[vertexId].erase(patternVertexId);
}

}


/*
template<class State>
bool MultipleLabelCpuBase<State>::isVertexScheduledAtomic(const State &globalState, const vid_t &vertexId) const {
  auto address = &(globalState.vertexScheduleList[vertexId]);
  bool value;
  #pragma omp atomic read
  value = *address;
  return value;
}*/
/*
template<class State>
void MultipleLabelCpuBase<State>::scheduleVertexAtomic(State *globalState, const vid_t &vertexId) const {
  auto address = &(globalState->vertexScheduleList[vertexId]);
  #pragma omp atomic write
  *address = true;
}
template<class State>
void MultipleLabelCpuBase<State>::unscheduleVertex(State *globalState, const vid_t &vertexId) const {
  globalState->vertexScheduleList[vertexId] = false;
}

template<class State>
void MultipleLabelCpuBase<State>::scheduleNeighborVertex(const graph_t &graph,
                                                         State *globalState,
                                                         const vid_t &vertexId) const {
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
                                                               const vid_t &vertexId) const {
  for (eid_t neighborEdgeId = graph.vertices[vertexId]; neighborEdgeId < graph.vertices[vertexId + 1];
       neighborEdgeId++) {
    if (!isEdgeActive(*globalState, neighborEdgeId)) continue;
    vid_t neighborVertexId = graph.edges[neighborEdgeId];
    if (!isVertexActive(*globalState, neighborVertexId)) continue;
    scheduleVertexAtomic(globalState, neighborVertexId);
  }
}*/

#endif
