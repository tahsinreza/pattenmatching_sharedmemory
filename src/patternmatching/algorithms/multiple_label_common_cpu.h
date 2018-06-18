
#ifndef TOTEM_MULTIPLE_LABEL_COMMON_CPU_H
#define TOTEM_MULTIPLE_LABEL_COMMON_CPU_H


#include "totem_comdef.h"
#include "totem_graph.h"
#include "totem_mem.h"
#include "totem_partition.h"
#include <unordered_map>
#include <unordered_set>
#include "bitmap.h"
#include <vector>
#include "patternmatching_logger.h"
#include "multiple_label_common.h"

namespace patternmatching {

/**
 * state shared between all partitions
 */
template<typename VisitedType>
class MultipleLabelGlobalState {
 public:
  using BitmapType = FixedBitmap<C_MAXIMUM_PATTERN_SIZE>;
  //using BitmapType = std::unordered_set<pvid_t >;
 public:
  error_t allocate(const vid_t vertexCount, const eid_t edgeCount, const pvid_t patternVertexCount);
  error_t free();
  void resetModifiedList();
  void resetScheduledList(const bool &value=false);
  void resetPatternMatchLcc();
  void resetPatternMatchCc();
  void resetPatternMatchPc();
  void resetPatternMatchTds();

 public:
  vid_t graphVertexCount;
  vid_t activeVertexCount;
  eid_t graphEdgeCount;
  eid_t activeEdgeCount;
  pvid_t patternVertexCount;

  VisitedType *vertexActiveList;
  VisitedType *vertexModifiedList;
  VisitedType *vertexScheduledList;

  VisitedType *edgeActiveList;

  BitmapType *vertexPatternMatch;

  BitmapType *vertexPatternToUnmatchLcc;
  BitmapType *vertexPatternToUnmatchCc;
  uint8_t *vertexPatternOmittedCc;
  BitmapType *vertexPatternToUnmatchPc;
  BitmapType *vertexPatternToUnmatchTds;
  uint8_t *vertexPatternOmittedTds;

};
typedef MultipleLabelGlobalState<uint32_t> MultipleLabelGlobalStateInt;

template<class State>
class MultipleLabelCpuBase {
 protected:
  inline bool isVertexActive(const State &globalState, const vid_t vertexId) const;
  inline bool isVertexInactive(const State &globalState, const vid_t vertexId) const;
  inline void deactivateVertex(State *globalState, const vid_t vertexId) const;

  inline void makeModifiedVertex(State *globalState, const vid_t vertexId) const;
  inline bool isVertexModified(const State &globalState, const vid_t vertexId) const;

  inline void scheduleVertex(State *globalState, const vid_t vertexId) const;
  inline bool isVertexScheduled(const State &globalState, const vid_t vertexId) const;

  inline bool isEdgeActive(const State &globalState, const eid_t edgeId) const;
  inline bool isEdgeInactive(const State &globalState, const eid_t edgeId) const;
  inline void deactivateEdge(State *globalState, const eid_t edgeId) const;

  inline bool isMatch(const State &globalState, const vid_t vertexId) const;
  inline void removeMatch(State *globalState, const vid_t vertexId, const pvid_t patternVertexId) const;

};

}

#include "multiple_label_common_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_COMMON_CPU_H