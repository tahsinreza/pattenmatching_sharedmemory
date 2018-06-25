
#ifndef TOTEM_MULTIPLE_LABEL_COMMON_CPU_H
#define TOTEM_MULTIPLE_LABEL_COMMON_CPU_H

#include "totem_comdef.h"
#include "totem_graph.h"
#include "totem_mem.h"
#include "totem_partition.h"
#include <unordered_map>
#include <unordered_set>
#include "utils/bitmap_fixed.h"
#include "utils/bitmap_dynamic.h"
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
  void resetActiveList();
  void resetModifiedList();
  void resetScheduledList(const bool &value = false);
  void resetPatternMatch();
  void resetPatternAlreadyMatched();
  void resetPatternToUnmatch();

  MultipleLabelGlobalState &operator=(const MultipleLabelGlobalState &);

 public:
  vid_t graphVertexCount;
  vid_t graphActiveVertexCount;
  eid_t graphEdgeCount;
  vid_t graphActiveEdgeCount;
  pvid_t patternVertexCount;

  VisitedType *vertexActiveList;
  VisitedType *vertexModifiedList;
  VisitedType *vertexScheduledList;

  VisitedType *edgeActiveList;

  BitmapType *vertexPatternMatch;
  BitmapType *vertexPatternAlreadyMatched;
  BitmapType *vertexPatternToUnmatch;

};
typedef MultipleLabelGlobalState<uint32_t> MultipleLabelGlobalStateInt;

template<class State>
class MultipleLabelCpuBase {
 protected:
  inline bool isVertexActive(const State &globalState, const vid_t &vertexId) const;
  inline bool isVertexInactive(const State &globalState, const vid_t &vertexId) const;
  inline void deactivateVertex(State *globalState, const vid_t &vertexId) const;

  inline void makeModifiedVertex(State *globalState, const vid_t &vertexId) const;
  inline bool isVertexModified(const State &globalState, const vid_t &vertexId) const;

  inline void scheduleVertex(State *globalState, const vid_t &vertexId) const;
  inline void unscheduleVertex(State *globalState, const vid_t &vertexId) const;
  inline bool isVertexScheduled(const State &globalState, const vid_t &vertexId) const;

  inline bool isEdgeActive(const State &globalState, const eid_t &edgeId) const;
  inline bool isEdgeInactive(const State &globalState, const eid_t &edgeId) const;
  inline void deactivateEdge(State *globalState, const eid_t &edgeId) const;

  inline bool isMatch(const State &globalState, const vid_t &vertexId) const;
  inline void removeMatch(State *globalState, const vid_t &vertexId, const pvid_t &patternVertexId) const;

  inline bool isAlreadyMatchedAtomic(const State &globalState, const vid_t &vertexId, const pvid_t &patternVertexId) const;
  inline void makeAlreadyMatchedAtomic(State *globalState, const vid_t &vertexId, const pvid_t &patternVertexId) const;
  inline void removeAlreadyMatched(State *globalState, const vid_t &vertexId, const pvid_t &patternVertexId) const;
  inline void clearAlreadyMatched(State *globalState, const vid_t &vertexId) const;

  inline void clearToUnmatch(State *globalState, const vid_t &vertexId) const;
  inline void makeToUnmatch(State *globalState, const vid_t &vertexId, const pvid_t &patternVertexId) const;
  inline void removeToUnmatch(State *globalState, const vid_t &vertexId, const pvid_t &patternVertexId) const;

};

}

#include "multiple_label_common_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_COMMON_CPU_H