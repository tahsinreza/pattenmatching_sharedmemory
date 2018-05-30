
#ifndef TOTEM_UNIQUE_LABEL_COMMON_CPU_H
#define TOTEM_UNIQUE_LABEL_COMMON_CPU_H

#include "totem_bitmap.cuh"
#include "totem_comdef.h"
#include "totem_comkernel.cuh"
#include "totem_graph.h"
#include "totem_mem.h"
#include "totem_partition.h"
#include <unordered_map>
#include "totem_patternmatching_logger.h"

namespace patternmatching {

/**
 * state shared between all partitions
 */
template<typename VisitedType>
class UniqueLabelGlobalState {
 public:
  error_t allocate(const vid_t vertexCount);
  error_t free();
  void resetPatternMatchLcc();
  void resetPatternMatchCc();

 public:
  vid_t graphVertexCount;
  vid_t activeVertexCount;
  VisitedType *vertexActiveList;
  uint8_t *vertexPatternMatchLcc;
  uint8_t *vertexPatternMatchCc;
  uint8_t *vertexPatternOmittedCc;

};
typedef UniqueLabelGlobalState<uint32_t> UniqueLabelGlobalStateInt;

template<class State>
class UniqueLabelCpuBase {
 protected:
  inline bool isActive(const State &globalState, const vid_t vertexId) const;
  inline bool isInactive(const State &globalState, const vid_t vertexId) const;
  inline void deactivate(State *globalState, const vid_t vertexId) const;

};

}

#include "totem_unique_label_common_cpu.tpp"

#endif //TOTEM_UNIQUE_LABEL_COMMON_CPU_H