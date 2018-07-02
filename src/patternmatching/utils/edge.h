//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_TEMPLATE_EDGE_H
#define PROJECT_MULTIPLE_LABEL_TEMPLATE_EDGE_H

#include "totem_graph.h"
#include <iostream>

namespace patternmatching {

class DirectedEdge {
 public:
  DirectedEdge();
  DirectedEdge(const vid_t vertexFromId, const vid_t vertexToIt);
  void print(std::ostream &ostream = std::cout) const;

  bool operator==(const DirectedEdge &other) const;
  bool operator!=(const DirectedEdge &other) const;
  bool operator<(const DirectedEdge &other) const;

 public:
  vid_t vertexFromId, vertexToIt;
};
class UndirectedEdge : public DirectedEdge{
 public:
  UndirectedEdge();
  UndirectedEdge(const vid_t vertexFromId, const vid_t vertexToIt);
};

struct DirectedEdgeHash {
 public:
  std::size_t operator()(const DirectedEdge &x) const {
    return std::hash<vid_t>()(x.vertexFromId) ^ std::hash<vid_t>()(x.vertexToIt);
  }
};
struct UndirectedEdgeHash : public DirectedEdgeHash {
 public:
  std::size_t operator()(const UndirectedEdge &x) const {
    return DirectedEdgeHash::operator()(x);
  }
};
}

#endif //PROJECT_MULTIPLE_LABEL_TEMPLATE_CONSTRAINT_H
