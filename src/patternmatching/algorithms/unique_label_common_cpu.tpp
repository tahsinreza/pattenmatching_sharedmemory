
#ifndef TOTEM_UNIQUE_LABEL_COMMON_CPU_TPP
#define TOTEM_UNIQUE_LABEL_COMMON_CPU_TPP

#include "totem_comdef.h"
#include "totem_graph.h"
#include "totem_mem.h"
#include "totem_partition.h"
#include "unique_label_common_cpu.h"

namespace patternmatching {

/**
 * state shared between all partitions
 */
template<typename VisitedType>
error_t UniqueLabelGlobalState<VisitedType>::allocate(const vid_t _graphVertexCount) {
  graphVertexCount = _graphVertexCount;
  activeVertexCount = _graphVertexCount;

  totem_malloc(graphVertexCount * sizeof(VisitedType), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexActiveList));
  assert(vertexActiveList != nullptr);
  totem_memset(vertexActiveList, static_cast<VisitedType>(true), graphVertexCount, TOTEM_MEM_HOST);

  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternMatchLcc));
  assert(vertexPatternMatchLcc != nullptr);
  resetPatternMatchLcc();

  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternMatchCc));
  assert(vertexPatternMatchCc != nullptr);
  totem_malloc(graphVertexCount * sizeof(uint8_t), TOTEM_MEM_HOST,
               reinterpret_cast<void **>(&vertexPatternOmittedCc));
  assert(vertexPatternOmittedCc != nullptr);
  resetPatternMatchCc();

  return SUCCESS;
}

template<typename VisitedType>
void UniqueLabelGlobalState<VisitedType>::resetPatternMatchLcc() {
  totem_memset(vertexPatternMatchLcc, static_cast<uint8_t>(true), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
void UniqueLabelGlobalState<VisitedType>::resetPatternMatchCc() {
  totem_memset(vertexPatternMatchCc, static_cast<uint8_t>(false), graphVertexCount, TOTEM_MEM_HOST);
  totem_memset(vertexPatternOmittedCc, static_cast<uint8_t>(false), graphVertexCount, TOTEM_MEM_HOST);
}

template<typename VisitedType>
error_t UniqueLabelGlobalState<VisitedType>::free() {
  totem_free(reinterpret_cast<void *>(vertexPatternMatchLcc), TOTEM_MEM_HOST);
  totem_free(reinterpret_cast<void *>(vertexPatternMatchCc), TOTEM_MEM_HOST);
  totem_free(reinterpret_cast<void *>(vertexActiveList), TOTEM_MEM_HOST);

  graphVertexCount = 0;
  activeVertexCount = 0;

  return SUCCESS;
}

template<class State>
bool UniqueLabelCpuBase<State>::isActive(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexActiveList[vertexId] == true;
}
template<class State>
bool UniqueLabelCpuBase<State>::isInactive(const State &globalState, const vid_t vertexId) const {
  return globalState.vertexActiveList[vertexId] == false;
}
template<class State>
void UniqueLabelCpuBase<State>::deactivate(State *globalState, const vid_t vertexId) const {
  globalState->vertexActiveList[vertexId] = false;
}

}

#endif
