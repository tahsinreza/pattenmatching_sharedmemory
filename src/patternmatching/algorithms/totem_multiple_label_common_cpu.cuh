
#ifndef TOTEM_MULTIPLE_LABEL_COMMON_CPU_H
#define TOTEM_MULTIPLE_LABEL_COMMON_CPU_H

#include "totem_bitmap.cuh"
#include "totem_comdef.h"
#include "totem_comkernel.cuh"
#include "totem_graph.h"
#include "totem_mem.h"
#include "totem_partition.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "totem_patternmatching_logger.h"

namespace patternmatching {
typedef uint32_t pvid_t;

#define DEBUG_PRINT(X) std::cout << #X<< "\t" << X <<std::endl;
#define DEBUG_PRINT_SET(X) \
std::cout<< #X << " : ";\
for(const auto &it : X) { std::cout << it << " ";} std::cout << std::endl;

/**
 * state shared between all partitions
 */
template<typename VisitedType>
class MultipleLabelGlobalState {
 public:
  error_t allocate(const vid_t vertexCount, const eid_t edgeCount, const pvid_t patternVertexCount);
  error_t free();
  void resetPatternMatchLcc();
  void resetPatternMatchCc();
  void resetPatternMatchPc();

 public:
  vid_t graphVertexCount;
  vid_t activeVertexCount;
  eid_t graphEdgeCount;
  eid_t activeEdgeCount;
  pvid_t patternVertexCount;
  VisitedType *vertexActiveList;
  VisitedType *edgeActiveList;
  std::unordered_set<pvid_t> *vertexPatternMatch;
  std::unordered_set<pvid_t> *vertexPatternToUnmatchLcc;
  std::unordered_set<pvid_t> *vertexPatternToUnmatchCc;
  uint8_t *vertexPatternOmittedCc;
  std::unordered_set<pvid_t> *vertexPatternToUnmatchPc;

};
typedef MultipleLabelGlobalState<uint32_t> MultipleLabelGlobalStateInt;

template<class State>
class MultipleLabelCpuBase {
 protected:
  inline bool isVertexActive(const State &globalState, const vid_t vertexId) const;
  inline bool isVertexInactive(const State &globalState, const vid_t vertexId) const;
  inline void deactivateVertex(State *globalState, const vid_t vertexId) const;

  inline bool isEdgeActive(const State &globalState, const eid_t edgeId) const;
  inline bool isEdgeInactive(const State &globalState, const eid_t edgeId) const;
  inline void deactivateEdge(State *globalState, const eid_t edgeId) const;

  inline bool isMatch(const State &globalState, const vid_t vertexId) const;
  inline void removeMatch(State *globalState, const vid_t vertexId, const pvid_t patternVertexId) const;

};

}

#include "totem_multiple_label_common_cpu.tpp"

#endif //TOTEM_MULTIPLE_LABEL_COMMON_CPU_H